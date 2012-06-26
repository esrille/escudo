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
    style(style),
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
    firstRenderBase(0),
    lastRenderBase(0),
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
    if (isAuto()) {
        StackingContext* item = parent->addContext(auto_, zIndex, style);
        if (item)
            item->positioned = this;
        return item;
    }

    StackingContext* after = 0;
    for (auto i = getFirstChild(); i; i = i->getNextSibling()) {
        if (zIndex < i->zIndex) {
            after = i;
            break;
        }
    }
    StackingContext* item = new(std::nothrow) StackingContext(auto_, zIndex, style);
    if (item) {
        insertBefore(item, after);
        item->positioned = this;
    }
    return item;
}

void StackingContext::clip(StackingContext* s, float relativeX, float relativeY)
{
    for (BlockLevelBox* clip = s->clipBox; clip && clip != s->positioned->clipBox; clip = clip->clipBox) {
        if (clip->stackingContext == s)
            continue;
        if (clip->style->overflow.getValue() != CSSOverflowValueImp::Visible) {  // TODO: check this
            Element element = interface_cast<Element>(clip->node);
            glTranslatef(-element.getScrollLeft(), -element.getScrollTop(), 0.0f);
            if (clip != s->clipBox) {
                clipLeft -= element.getScrollLeft();
                clipTop -= element.getScrollTop();
            }
        }
        if (clipWidth == HUGE_VALF) {
            clipLeft = clip->x + clip->marginLeft + clip->borderLeft - relativeX;
            clipTop = clip->y + clip->marginTop + clip->borderTop - relativeY;
            clipWidth = clip->getPaddingWidth();
            clipHeight = clip->getPaddingHeight();
        } else {
            Box::unionRect(clipLeft, clipTop, clipWidth, clipHeight,
                           clip->x + clip->marginLeft + clip->borderLeft - relativeX,
                           clip->y + clip->marginTop + clip->borderTop - relativeY,
                           clip->getPaddingWidth(), clip->getPaddingHeight());
        }
    }
}

bool StackingContext::resolveOffset(float& x, float &y)
{
    bool result = false;
    for (StackingContext* s = this; s != parent; s = s->positioned) {
        assert(s->style);
        result |= s->style->resolveOffset(x, y);
        clip(s, x, y);
    }
    return result;
}

void StackingContext::render(ViewCSSImp* view)
{
    clipLeft = clipTop = 0.0f;
    clipWidth = clipHeight = HUGE_VALF;
    glPushMatrix();
    float relativeX = 0.0f;
    float relativeY = 0.0f;
    if (resolveOffset(relativeX, relativeY))
        glTranslatef(relativeX, relativeY, 0.0f);
    if (clipWidth != HUGE_VALF && clipHeight != HUGE_VALF)
        view->clip(clipLeft, clipTop, clipWidth, clipHeight);

    currentFloat = firstFloat = lastFloat = 0;
    for (Box* base = firstRenderBase; base; base = base->nextBase) {
        glPushMatrix();
        BlockLevelBox* block = dynamic_cast<BlockLevelBox*>(base);
        unsigned overflow = CSSOverflowValueImp::Visible;
        if (block)
            overflow = block->renderBegin(view);
        StackingContext* childContext = getFirstChild();
        for (; childContext && childContext->zIndex < 0; childContext = childContext->getNextSibling())
            childContext->render(view);
        if (block)
            block->renderNonInline(view, this);
        else
            base->render(view, this);
        for (currentFloat = firstFloat; currentFloat; currentFloat = currentFloat->nextBase)
            currentFloat->render(view, this);
        if (block)
            block->renderInline(view, this);
        for (; childContext; childContext = childContext->getNextSibling())
            childContext->render(view);
        if (block) {
            if (0.0f < block->getOutlineWidth())
                block->renderOutline(view, block->x, block->y + block->getTopBorderEdge());
            block->renderEnd(view, overflow);
        }
        glPopMatrix();
    }

    if (clipWidth != HUGE_VALF && clipHeight != HUGE_VALF)
        view->unclip(clipLeft, clipTop, clipWidth, clipHeight);
    glPopMatrix();
}

void StackingContext::addBase(Box* box)
{
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
    if (currentFloat) {
        box->nextBase = currentFloat->nextBase;
        currentFloat->nextBase = box;
        return;
    }
    if (!firstFloat)
        firstFloat = lastFloat = box;
    else {
        lastFloat->nextBase = box;
        lastFloat = box;
    }
    box->nextBase = 0;
}

void StackingContext::dump(std::string indent)
{
    std::cout << indent << "z-index: " << zIndex << '\n';
    indent += "  ";
    for (auto child = getFirstChild(); child; child = child->getNextSibling())
        child->dump(indent);
}

}}}}  // org::w3c::dom::bootstrap