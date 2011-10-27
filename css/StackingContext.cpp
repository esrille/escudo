/*
 * Copyright 2011 Esrille Inc.
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
    item->parent = 0;
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

StackingContext::StackingContext(bool auto_, int zIndex) :
    auto_(auto_),
    zIndex(zIndex),
    parent(0),
    firstChild(0),
    lastChild(0),
    previousSibling(0),
    nextSibling(0),
    childCount(0),
    firstBase(0),
    lastBase(0),
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

StackingContext* StackingContext::addContext(bool auto_, int zIndex)
{
    if (isAuto())
        return parent->addContext(auto_, zIndex);

    StackingContext* after = 0;
    for (auto i = getFirstChild(); i; i = i->getNextSibling()) {
        if (zIndex < i->zIndex) {
            after = i;
            break;
        }
    }
    StackingContext* item = new(std::nothrow) StackingContext(auto_, zIndex);
    if (item)
        insertBefore(item, after);
    return item;
}

void StackingContext::render(ViewCSSImp* view)
{
    currentFloat = 0;
    for (Box* base = firstBase; base; base = base->nextBase) {
        if (base->clipBox) {
            // TODO: Support the cumulative clip intersection.
            BlockLevelBox* clip = base->clipBox;
            // TODO: if (base->isAbsolutelyPositioned()) ... else ...
            float left = clip->x + clip->marginLeft + clip->borderLeft;
            float top = clip->y + clip->marginTop + clip->borderTop;
            float right = left + clip->getPaddingWidth();
            float bottom = top + clip->getPaddingHeight();
            glViewport(left, view->getInitialContainingBlock()->getHeight() - bottom, right - left, bottom - top);
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            glOrtho(left, right, bottom, top, -1000.0, 1.0);
            glMatrixMode(GL_MODELVIEW);
        }

        BlockLevelBox* block = dynamic_cast<BlockLevelBox*>(base);
        unsigned overflow = CSSOverflowValueImp::Visible;
        if (block)
            overflow = block->renderBegin(view);
        StackingContext* childContext = getFirstChild();
        for (; childContext && childContext->zIndex < 0; childContext = childContext->getNextSibling())
            childContext->render(view);
        if (!block)
            base->render(view, this);
        else
            block->renderContent(view, this);
        for (currentFloat = firstFloat; currentFloat; currentFloat = currentFloat->nextBase)
            currentFloat ->render(view, this);
        for (; childContext; childContext = childContext->getNextSibling())
            childContext->render(view);
        if (block)
            block->renderEnd(view, overflow);

        if (base->clipBox) {
            float w = view->getInitialContainingBlock()->getWidth();
            float h = view->getInitialContainingBlock()->getHeight();
            glViewport(0, 0, w, h);
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            glOrtho(0, w, h, 0, -1000.0, 1.0);
            glMatrixMode(GL_MODELVIEW);
        }
    }
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