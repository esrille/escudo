/*
 * Copyright 2011, 2012 Esrille Inc.
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

StackingContext::StackingContext(bool auto_, int zIndex, CSSStyleDeclarationImp* style) :
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
    firstBase(0),
    lastBase(0),
    clipBox(0),
    firstFloat(0),
    lastFloat(0),
    currentFloat(0)
{
}

StackingContext::~StackingContext()
{
    if (parent)
        parent->removeChild(this);
    while (0 < childCount) {
        StackingContext* child = removeChild(firstChild);
        delete child;
    }
}

StackingContext* StackingContext::addContext(bool auto_, int zIndex, CSSStyleDeclarationImp* style)
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

void StackingContext::clip(StackingContext* s)
{
    float scrollLeft = 0.0f;
    float scrollTop = 0.0f;
    for (Block* clip = s->clipBox; clip && clip != s->positioned->clipBox; clip = clip->clipBox) {
        if (clip->stackingContext == s)
            continue;
        Element element = interface_cast<Element>(clip->node);
        scrollLeft += element.getScrollLeft();
        scrollTop += element.getScrollTop();
        if (clipWidth == HUGE_VALF) {
            clipLeft = scrollLeft + clip->x + clip->marginLeft + clip->borderLeft;
            clipTop = scrollTop + clip->y + clip->marginTop + clip->borderTop;
            clipWidth = clip->getPaddingWidth();
            clipHeight = clip->getPaddingHeight();
        } else {
            Box::unionRect(clipLeft, clipTop, clipWidth, clipHeight,
                           scrollLeft + clip->x + clip->marginLeft + clip->borderLeft,
                           scrollTop + clip->y + clip->marginTop + clip->borderTop,
                           clip->getPaddingWidth(), clip->getPaddingHeight());
        }
    }
    glTranslatef(-scrollLeft, -scrollTop, 0.0f);
}

bool StackingContext::resolveRelativeOffset(ViewCSSImp* view, float& x, float &y)
{
    bool result = false;
    for (StackingContext* s = this; s != parent; s = s->positioned) {
        assert(s->style);
        CSSStyleDeclarationImp* style = s->style;
        if (!style->isResolved()) {
            if (Box* b = style->getBox()) {
                if (auto c = b->getContainingBlock(view)) {
                    style->left.resolve(view, style, c->width);
                    style->right.resolve(view, style, c->width);
                    style->top.resolve(view, style, c->height);
                    style->bottom.resolve(view, style, c->height);
                }
            }
        }
        result |= style->resolveRelativeOffset(x, y);
        clip(s);
        clipLeft -= x;
        clipTop -= y;
    }
    return result;
}

void StackingContext::renderFloats(ViewCSSImp* view, Box* last, Box* current)
{
    if (current && current == currentFloat)
        return;
    if (!last)
        currentFloat = firstFloat;
    else if (last->nextFloat)
        currentFloat = last->nextFloat;
    else
        return;
    while (currentFloat) {
        currentFloat->render(view, this);;
        currentFloat = currentFloat->nextFloat;
    }
    if (!last)
        firstFloat = lastFloat = 0;
    else {
        last->nextFloat = 0;
        lastFloat = last;
    }
    currentFloat = 0;
}

void StackingContext::render(ViewCSSImp* view)
{
    clipLeft = clipTop = 0.0f;
    clipWidth = clipHeight = HUGE_VALF;
    glPushMatrix();
    float relativeX = 0.0f;
    float relativeY = 0.0f;
    if (resolveRelativeOffset(view, relativeX, relativeY))
        glTranslatef(relativeX, relativeY, 0.0f);
    if (clipWidth != HUGE_VALF && clipHeight != HUGE_VALF)
        view->clip(clipLeft, clipTop, clipWidth, clipHeight);

    if (firstBase) {
        currentFloat = firstFloat = lastFloat = 0;
        for (Box* base = firstBase; base; base = base->nextBase) {
            glPushMatrix();
            Block* block = dynamic_cast<Block*>(base);
            unsigned overflow = CSSOverflowValueImp::Visible;
            if (block) {
                overflow = block->renderBegin(view);
            }

            GLfloat mtx[16];
            glGetFloatv(GL_MODELVIEW_MATRIX, mtx);
            glPopMatrix();
            glPushMatrix();
            StackingContext* childContext = getFirstChild();
            for (; childContext && childContext->zIndex < 0; childContext = childContext->getNextSibling()) {
                childContext->render(view);
            }
            glLoadMatrixf(mtx);

            if (block) {
                block->renderNonInline(view, this);
                renderFloats(view, 0, 0);
                block->renderInline(view, this);
            } else
                base->render(view, this);

            glPopMatrix();
            glPushMatrix();
            for (; childContext; childContext = childContext->getNextSibling()) {
                    childContext->render(view);
            }
            glLoadMatrixf(mtx);

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

    if (clipWidth != HUGE_VALF && clipHeight != HUGE_VALF)
        view->unclip(clipLeft, clipTop, clipWidth, clipHeight);
    glPopMatrix();
}

void StackingContext::addBase(Box* box)
{
#ifndef NDEBUG
    for (Box* i = firstBase; i; i = i->nextBase)
        assert(box != i);
#endif
    if (!firstBase)
        firstBase = lastBase = box;
    else {
        lastBase->nextBase = box;
        lastBase = box;
    }
    box->nextBase = 0;
}

void StackingContext::addBox(Box* box, Box* parentBox)
{
    assert(parentBox);
    if (parentBox->stackingContext != this)
        addBase(box);
}

void StackingContext::addFloat(Box* box)
{
#ifndef NDEBUG
    for (Box* i = firstFloat; i; i = i->nextFloat) {
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

void StackingContext::removeBox(Box* box)
{
    Box* p = 0;
    for (Box* i = firstBase; i ; p = i, i = i->nextBase) {
        if (i == box) {
            if (!p)
                firstBase = i->nextBase;
            else
                p->nextBase = i->nextBase;
            if (lastBase == box)
                lastBase = p;
            break;
        }
    }
}

void StackingContext::layOutAbsolute(ViewCSSImp* view)
{
    for (Box* base = firstBase; base; base = base->nextBase) {
        Block* block = dynamic_cast<Block*>(base);
        if (!block || !block->isAbsolutelyPositioned())
            continue;
        block->layOutAbsolute(view);
        block->resolveXY(view, block->x, block->y, block->clipBox);
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

void StackingContext::dump(std::string indent)
{
    std::cout << indent << "z-index: ";
    if (isAuto())
        std::cout << "auto";
    else
        std::cout << zIndex;
    std::cout << '\n';
    indent += "  ";
    for (auto child = getFirstChild(); child; child = child->getNextSibling())
        child->dump(indent);
}

}}}}  // org::w3c::dom::bootstrap
