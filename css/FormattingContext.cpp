/*
 * Copyright 2010, 2011 Esrille Inc.
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

#include "Box.h"

#include <assert.h>

#include "css/CSSStyleDeclarationImp.h"
#include "css/ViewCSSImp.h"
#include "CSSPropertyValueImp.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

float FormattingContext::getLeftEdge() const {
    if (left.empty())
        return 0.0f;
    return left.back()->edge;
}

float FormattingContext::getRightEdge() const {
    if (right.empty())
        return 0.0f;
    return right.front()->edge;
}

float FormattingContext::getLeftRemainingHeight() const {
    if (left.empty())
        return 0.0f;
    return left.back()->remainingHeight;
}

float FormattingContext::getRightRemainingHeight() const {
    if (right.empty())
        return 0.0f;
    return right.front()->remainingHeight;
}

LineBox* FormattingContext::addLineBox(ViewCSSImp* view, BlockLevelBox* parentBox) {
    assert(!lineBox);
    assert(parentBox);
    lineBox = new(std::nothrow) LineBox(parentBox->getStyle());
    if (lineBox) {
        parentBox->appendChild(lineBox);

        // Set marginLeft and marginRight to the current values. Note these
        // margins do not contain the widths of the float boxes to be added
        // below.
        lineBox->marginLeft = getLeftEdge();
        lineBox->marginRight = getRightEdge();

        x = lineBox->marginLeft;
        leftover = parentBox->width - x - lineBox->marginRight;

        tryAddFloat(view);

        x = getLeftEdge();
        leftover = parentBox->width - x - getRightEdge();
    }
    return lineBox;
}

// if floatNodes is not empty, append float boxes as much as possible.
void FormattingContext::tryAddFloat(ViewCSSImp* view)
{
    while (!floatNodes.empty()) {
        BlockLevelBox* floatBox = view->getFloatBox(floatNodes.front());
        unsigned clear = floatBox->style->clear.getValue();
        if ((clear & CSSClearValueImp::Left) && !left.empty() ||
            (clear & CSSClearValueImp::Right) && !right.empty()) {
            break;
        }
        float w = floatBox->getEffectiveTotalWidth();
        if (leftover < w) {
            if (left.empty() && right.empty() && !lineBox->hasChildBoxes()) {
                addFloat(floatBox, w);
                floatNodes.pop_front();
            }
            break;
        }
        addFloat(floatBox, w);
        floatNodes.pop_front();
    }
}

void FormattingContext::updateRemainingHeight(float h)
{
    for (auto i = left.begin(); i != left.end();) {
        if (((*i)->remainingHeight -= h) <= 0.0f)
            i = left.erase(i);
        else
            ++i;
    }
    for (auto i = right.begin(); i != right.end();) {
        if (((*i)->remainingHeight -= h) <= 0.0f)
            i = right.erase(i);
        else
            ++i;
    }
}

float FormattingContext::shiftDown()
{
    assert(lineBox);
    float h = 0.0f;
    float w = 0.0f;
    do {
        float lh = getLeftRemainingHeight();
        float rh = getRightRemainingHeight();
        if (0.0f < lh && (lh < rh || rh <= 0.0f)) {
            // Shift down to left
            w = left.back()->getEffectiveTotalWidth();
            x -= w;
            leftover += w;
            h += lh;
        } else if (0.0f < rh && (rh < lh || lh <= 0.0f)) {
            // Shift down to right
            w = right.front()->getEffectiveTotalWidth();
            leftover += w;
            h += rh;
        } else if (0.0f < lh) {
            // Shift down to both
            float l = left.back()->getEffectiveTotalWidth();
            w = l + right.front()->getEffectiveTotalWidth();
            x -= l;
            leftover += w;
            h += lh;
        } else
            break;
    } while (w == 0.0f);
    return h;
}

// If there's a float, shiftDownLineBox() will expand leftover by extending
// marginTop in lineBox down to the bottom of the nearest float box.
bool FormattingContext::shiftDownLineBox()
{
    assert(lineBox);
    if (float h = shiftDown()) {
        updateRemainingHeight(h);
        lineBox->marginTop += h;
        return true;
    }
    return false;  // no floats
}

// Complete the current lineBox by adding float boxes if any.
// Then update remainingHeight.
void FormattingContext::nextLine(ViewCSSImp* view, BlockLevelBox* parentBox, unsigned clearValue)
{
    assert(lineBox);
    assert(lineBox == parentBox->lastChild);

    if (InlineLevelBox* inlineLevelBox = dynamic_cast<InlineLevelBox*>(lineBox->getLastChild())) {
        float w = inlineLevelBox->atEndOfLine();
        if (w < 0.0f) {
            lineBox->width += w;
            leftover -= w;
            tryAddFloat(view);
        }
    }

    for (auto i = left.rbegin(); i != left.rend(); ++i) {
        BlockLevelBox* floatBox = *i;
        if (!floatBox->inserted) {
            floatBox->inserted = true;
            lineBox->insertBefore(floatBox, lineBox->getFirstChild());
        }
    }
    bool first = true;
    for (auto i = right.begin(); i != right.end(); ++i) {
        BlockLevelBox* floatBox = *i;
        if (!floatBox->inserted) {
            floatBox->inserted = true;
            // We would need a margin before the 1st float box to be added.
            // TODO: We should use another measure for adjusting the left edge of the float box.
            if (first) {
                first = false;
                floatBox->marginLeft += parentBox->width - lineBox->getTotalWidth() - getLeftEdge() - getRightEdge();
            }
            lineBox->appendChild(*i);
        }
    }
    float height = lineBox->getTotalHeight();
    if (height != 0.0f)
        updateRemainingHeight(height);
    else if (clearValue)
        lineBox->marginBottom += clear(clearValue);
    lineBox = 0;
    x = leftover = 0.0f;
}

void FormattingContext::addFloat(BlockLevelBox* floatBox, float totalWidth)
{
    if (floatBox->style->float_.getValue() == CSSFloatValueImp::Left) {
        if (left.empty())
            floatBox->edge = totalWidth;
        else
            floatBox->edge = left.back()->edge + totalWidth;
        left.push_back(floatBox);
        x += totalWidth;
    } else {
        if (right.empty())
            floatBox->edge = totalWidth;
        else
            floatBox->edge = right.front()->edge + totalWidth;
        right.push_front(floatBox);
    }
    leftover -= totalWidth;
}

float FormattingContext::clear(unsigned value)
{
    if (!value)
        return 0.0f;
    float h = 0.0f;
    if (value & 1) {  // clear left
        for (auto i = left.begin(); i != left.end(); ++i) {
            float w = (*i)->getEffectiveTotalWidth();
            x -= w;
            leftover += w;
            h = std::max(h, (*i)->remainingHeight);
        }
    }
    if (value & 2) {  // clear right
        for (auto i = right.begin(); i != right.end(); ++i) {
            float w = (*i)->getEffectiveTotalWidth();
            leftover += w;
            h = std::max(h, (*i)->remainingHeight);
        }
    }
    updateRemainingHeight(h);
    assert(!(value & 1) || left.empty());
    assert(!(value & 2) || right.empty());
    return h;
}

}}}}  // org::w3c::dom::bootstrap
