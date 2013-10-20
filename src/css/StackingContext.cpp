/*
 * Copyright 2011-2013 Esrille Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "StackingContext.h"

// TODO: Do not inlcude GL in this file
#include <GL/gl.h>
#include <GL/glu.h>

#include <new>
#include <iostream>

#include "Box.h"
#include "ViewCSSImp.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

namespace {

std::atomic_uint aid;

}

StackingContext* StackingContext::removeChild(StackingContext* item)
{
    StackingContext* next = item->nextSibling;
    StackingContext* prev = item->previousSibling;
    if (!next)
        lastChild = prev;
    else
        next->previousSibling = prev;
    if (!prev)
        firstChild = next;
    else
        prev->nextSibling = next;
    item->parent = item->nextSibling = item->previousSibling = 0;
    --childCount;
    return item;
}

StackingContext* StackingContext::insertBefore(StackingContext* item, StackingContext* after)
{
    if (!after)
        return appendChild(item);
    item->previousSibling = after->previousSibling;
    item->nextSibling = after;
    after->previousSibling = item;
    if (!item->previousSibling)
        firstChild = item;
    else
        item->previousSibling->nextSibling = item;
    item->parent = this;
    ++childCount;
    return item;
}

StackingContext* StackingContext::appendChild(StackingContext* item)
{
    StackingContext* prev = lastChild;
    if (!prev)
        firstChild = item;
    else
        prev->nextSibling = item;
    item->previousSibling = prev;
    item->nextSibling = 0;
    lastChild = item;
    item->parent = this;
    ++childCount;
    return item;
}

StackingContext::StackingContext(bool auto_, int zIndex, const CSSStyleDeclarationPtr& style) :
    count(0),
    style(style),
    needStaticPosition(false),
    auto_(auto_),
    zIndex(zIndex),
    parent(0),
    firstChild(0),
    lastChild(0),
    previousSibling(0),
    nextSibling(0),
    childCount(0),
    positioned(0),
    relativeX(0.0f),
    relativeY(0.0f),
    uid(++aid)
{
}

StackingContext::~StackingContext()
{
    detach();
}

StackingContext* StackingContext::addContext(bool auto_, int zIndex, const CSSStyleDeclarationPtr& style)
{
    StackingContext* item = new(std::nothrow) StackingContext(auto_, zIndex, style);
    if (item)
        insertContext(item);
    return item;
}

void StackingContext::insertContext(StackingContext* item)
{
    assert(item);
    if (isAuto()) {
        assert(!parent->isAuto());
        parent->insertContext(item);
        item->positioned = this;
        return;
    }
    // TODO: Preserve the order in the DOM tree
    StackingContext* after = 0;
    for (auto i = getFirstChild(); i; i = i->getNextSibling()) {
        if (item->zIndex < i->zIndex) {
            after = i;
            break;
        }
    }
    insertBefore(item, after);
    item->positioned = this;
}

void StackingContext::reparent(StackingContext* target)
{
    auto child = parent->getFirstChild();
    while (child) {
        auto next = child->getNextSibling();
        if (child->positioned == target) {
            parent->removeChild(child);
            target->insertContext(child);
            if (child->isAuto())
                reparent(child);
        }
        child = next;
    }
}

void StackingContext::setZIndex(bool auto_, int index)
{
    if (isAuto() == auto_ && zIndex == index)
        return;
    bool updateChildren = (this->auto_ != auto_);
    this->auto_ = auto_;
    zIndex = index;
    if (parent) {
        parent->removeChild(this);
        positioned->insertContext(this);
        if (updateChildren) {
            if (isAuto()) {
                while (StackingContext* child = getFirstChild()) {
                    removeChild(child);
                    child->positioned->insertContext(child);
                }
            } else
                reparent(this);
        }
    }
}

void StackingContext::detach()
{
    while (hasChildren())
        getFirstChild()->detach();
    if (parent) {
        StackingContext* i;
        do {
            for (i = parent->getFirstChild(); i; i = i->getNextSibling()) {
                if (i->positioned == this) {
                    i->detach();
                    break;
                }
            }
        } while (i);
        parent->removeChild(this);
        parent = 0;
    }
    if (auto style = getStyle()) {
        if (style->getStackingContext().get() == this)
            style->clearStackingContext();
    }
    positioned = 0;
}

void StackingContext::resetScrollSize()
{
    for (auto i = baseList.begin(); i != baseList.end(); i->expired() ? (i = baseList.erase(i)) : ++i) {
        if (auto base = i->lock())
            base->resetScrollSize();
    }
    for (StackingContext* childContext = getFirstChild(); childContext; childContext = childContext->getNextSibling())
        childContext->resetScrollSize();
}

void StackingContext::resolveScrollSize(ViewCSSImp* view)
{
    relativeX = relativeY = 0.0f;
    for (StackingContext* s = this; s != parent; s = s->positioned) {
        assert(s->getStyle());
        CSSStyleDeclarationPtr style = s->getStyle();
        if (!style->isResolved()) {
            if (BoxPtr b = style->getBox()) {
                if (auto c = b->getContainingBlock(view)) {
                    auto imp = style.get();
                    imp->left.resolve(view, imp, c->width);
                    imp->right.resolve(view, imp, c->width);
                    imp->top.resolve(view, imp, c->height);
                    imp->bottom.resolve(view, imp, c->height);
                }
            }
        }
        style->resolveRelativeOffset(relativeX, relativeY);
    }
    if (parent) {
        relativeX += parent->relativeX;
        relativeY += parent->relativeY;
    }
    for (auto i = baseList.begin(); i != baseList.end(); i->expired() ? (i = baseList.erase(i)) : ++i) {
        if (auto base = i->lock())
            base->updateScrollSize();
    }
    for (StackingContext* childContext = getFirstChild(); childContext; childContext = childContext->getNextSibling())
        childContext->resolveScrollSize(view);
}

void StackingContext::updateClipBox(StackingContext* s)
{
    float scrollLeft = 0.0f;
    float scrollTop = 0.0f;
    for (BlockPtr clip = s->getClipBox(); clip && (!s->positioned || clip != s->positioned->getClipBox()); clip = clip->getClipBox()) {
        if (clip->stackingContext == s || !clip->getParentBox())
            continue;
        assert(clip->stackingContext);
        Element element = interface_cast<Element>(clip->node);
        scrollLeft += element.getScrollLeft();
        scrollTop += element.getScrollTop();
        if (clipWidth == HUGE_VALF) {
            clipLeft = scrollLeft + clip->x + clip->marginLeft + clip->borderLeft + clip->stackingContext->relativeX;
            clipTop = scrollTop + clip->y + clip->marginTop + clip->borderTop + clip->stackingContext->relativeY;
            clipWidth = clip->getPaddingWidth();
            clipHeight = clip->getPaddingHeight();
        } else {
            Box::unionRect(clipLeft, clipTop, clipWidth, clipHeight,
                           scrollLeft + clip->x + clip->marginLeft + clip->borderLeft + clip->stackingContext->relativeX,
                           scrollTop + clip->y + clip->marginTop + clip->borderTop + clip->stackingContext->relativeY,
                           clip->getPaddingWidth(), clip->getPaddingHeight());
        }
    }
    glTranslatef(-scrollLeft, -scrollTop, 0.0f);
}

bool StackingContext::hasClipBox()
{
    clipLeft = clipTop = 0.0f;
    clipWidth = clipHeight = HUGE_VALF;
    for (StackingContext* s = this; s != parent; s = s->positioned)
        updateClipBox(s);
    return clipWidth != HUGE_VALF && clipHeight != HUGE_VALF;
}

void StackingContext::render(ViewCSSImp* view)
{
    auto style = getStyle();
    assert(style);
    if (hasClipBox())
        view->clip(clipLeft, clipTop, clipWidth, clipHeight);
    glPushMatrix();
    if (parent)
        glTranslatef(relativeX - parent->relativeX, relativeY - parent->relativeY, 0.0f);
    else
        glTranslatef(relativeX, relativeY, 0.0f);
    if (style->opacity.getValue() < 1.0f)
        view->beginTranslucent();

    if (getFirstBase()) {
        firstFloat = lastFloat = currentFloat = nullptr;
        GLfloat mtx[16];
        glGetFloatv(GL_MODELVIEW_MATRIX, mtx);
        for (auto i = baseList.begin(); i != baseList.end(); i->expired() ? (i = baseList.erase(i)) : ++i) {
            BoxPtr base = i->lock();
            if (!base)
                continue;
            glPushMatrix();
            auto block = std::dynamic_pointer_cast<Block>(base);
            unsigned overflow = CSSOverflowValueImp::Visible;
            if (block)
                overflow = block->renderBegin(view);

            StackingContext* childContext = getFirstChild();
            for (; childContext && childContext->zIndex < 0; childContext = childContext->getNextSibling()) {
                glPushMatrix();
                glLoadMatrixf(mtx);
                childContext->render(view);
                glPopMatrix();
            }

            if (block) {
                block->renderNonInline(view, this);
                renderFloats(view, 0, 0);
                block->renderInline(view, this);
            } else
                base->render(view, this);

            for (; childContext; childContext = childContext->getNextSibling()) {
                glPushMatrix();
                glLoadMatrixf(mtx);
                childContext->render(view);
                glPopMatrix();
            }

            if (block) {
                if (0.0f < block->getOutlineWidth())
                    block->renderOutline(view, block->x, block->y + block->getTopBorderEdge());
                block->renderEnd(view, overflow);
            }
            glPopMatrix();
        }
    } else {
        glPushMatrix();
        for (StackingContext* childContext = getFirstChild(); childContext; childContext = childContext->getNextSibling())
            childContext->render(view);
        glPopMatrix();
    }

    if (style->opacity.getValue() < 1.0f)
        view->endTranslucent(style->opacity.getValue());
    glPopMatrix();
    if (clipWidth != HUGE_VALF && clipHeight != HUGE_VALF)
        view->unclip(clipLeft, clipTop, clipWidth, clipHeight);
}

void StackingContext::renderFloats(ViewCSSImp* view, const BoxPtr& last, const BoxPtr& current)
{
    if (current && current == currentFloat)
        return;

    BoxPtr saved = currentFloat;
    if (!current || !last)
        currentFloat = firstFloat;
    else if (last->nextFloat)
        currentFloat = last->nextFloat;
    else
        return;
    while (currentFloat) {
        currentFloat->render(view, this);
        assert(currentFloat);
        currentFloat = currentFloat->nextFloat;
    }
    if (!current || !last)
        firstFloat = lastFloat = 0;
    else {
        last->nextFloat = 0;
        lastFloat = last;
    }
    currentFloat = saved;
}

BoxPtr StackingContext::getFirstBase() const
{
    for (auto i = baseList.begin(); i != baseList.end(); i->expired() ? (i = baseList.erase(i)) : ++i) {
        if (auto box = i->lock())
            return box;
    }
    return nullptr;
}

BoxPtr StackingContext::getLastBase() const
{
    for (auto i = baseList.rbegin(); i != baseList.rend(); ++i) {
        if (auto box = i->lock())
            return box;
    }
    return nullptr;
}

void StackingContext::addBase(const BoxPtr& box)
{
    for (auto i = baseList.begin(); i != baseList.end(); i->expired() ? (i = baseList.erase(i)) : ++i) {
        if (auto base = i->lock()) {
            if (base->getParentBox() == box) {  // This happens with positioned inline-level boxes
                baseList.insert(i, box);
                baseList.erase(i);
                return;
            }
        }
    }
    baseList.push_back(box);
}

void StackingContext::addBox(const BoxPtr& box, const BoxPtr& parentBox)
{
    assert(parentBox);
    box->stackingContext = this;
    if (parentBox->stackingContext != this)
        addBase(box);
}

void StackingContext::addFloat(const BoxPtr& box)
{
#ifndef NDEBUG
    for (BoxPtr i = firstFloat; i; i = i->nextFloat) {
        assert(box != i);
    }
#endif
    if (currentFloat) {
        box->nextFloat = currentFloat->nextFloat;
        currentFloat->nextFloat = box;
        return;
    }
    if (!firstFloat)
        firstFloat = lastFloat = box;
    else {
        lastFloat->nextFloat = box;
        lastFloat = box;
    }
    box->nextFloat = 0;
}

void StackingContext::removeBox(const BoxPtr& box)
{
    for (auto i = baseList.begin(); i != baseList.end(); i->expired() ? (i = baseList.erase(i)) : ++i) {
        if (auto b = i->lock()) {
            if (b == box) {
                baseList.erase(i);
                return;
            }
        }
    }
}

void StackingContext::layOutAbsolute(ViewCSSImp* view)
{
    for (auto i = baseList.begin(); i != baseList.end(); i->expired() ? (i = baseList.erase(i)) : ++i) {
        auto base = i->lock();
        if (!base)
            continue;
        auto block = std::dynamic_pointer_cast<Block>(base);
        if (!block || !block->isAbsolutelyPositioned())
            continue;
        block->layOutAbsolute(view);
        block->resolveXY(view, block->x, block->y, block->getClipBox() ? block->getClipBox() : ((view->getTree() == block) ? 0 : view->getTree()));
    }
    needStaticPosition = false;

    for (StackingContext* childContext = getFirstChild(); childContext; childContext = childContext->getNextSibling())
        childContext->needStaticPosition = true;
    bool repeat;
    do {
        repeat = false;
        for (StackingContext* childContext = getFirstChild(); childContext; childContext = childContext->getNextSibling()) {
            if (childContext->needStaticPosition) {
                if (!childContext->positioned->needStaticPosition)
                    childContext->layOutAbsolute(view);
                else
                    repeat = true;
            }
        }
    } while (repeat);
}

BoxPtr StackingContext::boxFromPoint(int x, int y)
{
    StackingContext* childContext;
    for (childContext = getLastChild(); childContext && 0 <= childContext->zIndex; childContext = childContext->getPreviousSibling()) {
        if (BoxPtr target = childContext->boxFromPoint(x, y))
            return target;
    }
    if (getFirstBase()) {
        int s = x - relativeX;
        int t = y - relativeY;
        for (auto i = baseList.begin(); i != baseList.end(); i->expired() ? (i = baseList.erase(i)) : ++i) {
            auto base = i->lock();
            if (!base)
                continue;
            if (BoxPtr target = base->boxFromPoint(s, t, this))
                return target;
        }
    }
    for (; childContext; childContext = childContext->getPreviousSibling()) {
        if (BoxPtr target = childContext->boxFromPoint(x, y))
            return target;
    }
    return nullptr;
}

void StackingContext::dump(std::string indent)
{
    std::cout << indent << "z-index: ";
    if (isAuto())
        std::cout << "auto";
    else
        std::cout << zIndex;

    std::cout << ": " << uid << '(' << (void*) this << ')';
    if (positioned)
        std::cout << ' ' << positioned->uid << '(' << (void*) positioned << ')';

    std::cout << '\n';
    indent += "  ";
    for (auto child = getFirstChild(); child; child = child->getNextSibling())
        child->dump(indent);
}

}}}}  // org::w3c::dom::bootstrap
