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

        // if floatNodes is not empty, append float boxes as much as possible.
        while (!floatNodes.empty()) {
            BlockLevelBox* floatBox = view->getFloatBox(floatNodes.front());
            float w = floatBox->getTotalWidth();
            if (leftover < w) {
                if (left.empty() && right.empty()) {
                    addFloat(floatBox, w);
                    floatNodes.pop_front();
                }
                break;
            }
            addFloat(floatBox, w);
            floatNodes.pop_front();
        }

        x = getLeftEdge();
        leftover = parentBox->width - x - getRightEdge();
    }
    return lineBox;
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

// If there's a float, shiftDownLineBox() will expand leftover by extending
// marginTop in lineBox down to the bottom of the nearest float box.
bool FormattingContext::shiftDownLineBox()
{
    assert(lineBox);
    float lh = getLeftRemainingHeight();
    float rh = getRightRemainingHeight();
    if (0.0f < lh && lh < rh) {
        // Shift down to left
        float w = left.back()->getTotalWidth();
        lineBox->marginTop += lh;
        x -= w;
        leftover += w;
        updateRemainingHeight(lh);
    } else if (0.0f < rh && rh < lh) {
        // Shift down to right
        float w = right.front()->getTotalWidth();
        lineBox->marginTop += rh;
        leftover += w;
        updateRemainingHeight(rh);
    } else if (0.0f < lh) {
        // Shift down to both
        float l = left.back()->getTotalWidth();
        float w = l + right.front()->getTotalWidth();
        lineBox->marginTop += lh;
        x -= l;
        leftover += w;
        updateRemainingHeight(lh);
    } else
        return false;  // no float boxes
    return true;
}

// Complete the current lineBox by adding float boxes if any.
// Then update remainingHeight.
void FormattingContext::nextLine(BlockLevelBox* parentBox, bool moreFloats)
{
    assert(lineBox);
    assert(lineBox == parentBox->lastChild);
    for (auto i = left.rbegin(); i != left.rend(); ++i) {
        if ((*i)->edge <= lineBox->marginLeft)
            break;
        lineBox->insertBefore(*i, lineBox->getFirstChild());
    }
    bool first = true;
    for (auto i = right.begin(); i != right.end(); ++i) {
        if ((*i)->edge <= lineBox->marginRight)
            break;
        // We would need a margin before the 1st float box to be added.
        // TODO: We should use another measure for adjusting the left edge of the float box.
        if (first) {
            (*i)->marginLeft += parentBox->width - lineBox->getTotalWidth() - getLeftEdge() - getRightEdge();
            first = false;
        }
        lineBox->appendChild(*i);
    }
    float height = lineBox->getTotalHeight();
    if (height != 0.0f)
        updateRemainingHeight(height);
    else if (moreFloats)
        lineBox->marginTop += clear(3);
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
            float w = (*i)->getTotalWidth();
            x -= w;
            leftover += w;
            h = std::max(h, (*i)->remainingHeight);
        }
    }
    if (value & 2) {  // clear right
        for (auto i = right.begin(); i != right.end(); ++i) {
            float w = (*i)->getTotalWidth();
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
