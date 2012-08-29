/*
 * Copyright 2010-2012 Esrille Inc.
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

#ifndef ES_FORMATTING_CONTEXT_H
#define ES_FORMATTING_CONTEXT_H

#include <Object.h>
#include <org/w3c/dom/Text.h>

#include <algorithm>
#include <list>
#include <string>

#include <boost/intrusive_ptr.hpp>

#include "TextIterator.h"
#include "http/HTTPRequest.h"
#include "css/CSSStyleDeclarationImp.h"

class FontGlyph;
class FontTexture;

namespace org { namespace w3c { namespace dom {

class Element;

namespace bootstrap {

class Box;
class ContainingBlock;
class LineBox;
class BlockLevelBox;
class StackingContext;
class ViewCSSImp;
class WindowImp;

class FormattingContext
{
    friend class BlockLevelBox;

    TextIterator textIterator;
    size_t textLength;

    bool breakable;
    bool isFirstLine;
    LineBox* lineBox;
    float x;
    float leftover;
    char16_t prevChar;
    float blankLeft;
    float blankRight;
    std::list<BlockLevelBox*> left;   // active floating boxes on the left side
    std::list<BlockLevelBox*> right;  // active floating boxes on the right side
    std::list<Node> floatNodes;       // floating boxes to be layed out

    float clearance;  // The clearance introduced by the previous collapsed through boxes.

    float usedMargin;

    // Adjoining margins
    float positiveMargin;
    float negativeMargin;
    float previousMargin;
    bool withClearance;

    // Previous height and baseline of the current lineBox used in nextLine()
    float baseline;
    float lineHeight;
    bool atLineHead;

    void shiftDownLeft();
    void shiftDownRight();

public:
    FormattingContext();

    void updateBlanks(Box* box);
    void restoreBlanks(Box* box);

    LineBox* addLineBox(ViewCSSImp* view, BlockLevelBox* parentBox);
    void addFloat(BlockLevelBox* floatBox, float totalWidth);

    float hasLeft() const {
        return !left.empty();
    }
    float hasRight() const {
        return !right.empty();
    }
    float getLeftoverForFloat(Box* block, unsigned floatValue) const;
    float getLeftEdge() const;
    float getRightEdge() const;
    float getLeftRemainingHeight() const;
    float getRightRemainingHeight() const;
    float shiftDown();
    bool shiftDownLineBox(ViewCSSImp* view);
    bool hasNewFloats() const;
    void appendInlineBox(ViewCSSImp* view, InlineLevelBox* inlineBox, CSSStyleDeclarationImp* activeStyle);
    void dontWrap();
    void nextLine(ViewCSSImp* view, BlockLevelBox* parentBox, bool linefeed);
    void tryAddFloat(ViewCSSImp* view);
    float adjustRemainingHeight(float height);

    // Use the positive margin stored in context to consume the remaining height of floating boxes.
    void useMargin(BlockLevelBox* block);

    float updateRemainingHeight(float height);
    float clear(unsigned value);

    float collapseMargins(float margin);
    float undoCollapseMargins();
    float fixMargin();
    float getMargin() const {
        return positiveMargin + negativeMargin;
    }
    void clearMargin() {
        clearance = 0.0f;
        usedMargin = 0.0f;
        positiveMargin = negativeMargin = 0.0f;
        previousMargin = NAN;
        withClearance = false;
    }
    void inheritMarginContext(FormattingContext* from) {
        if (from) {
            positiveMargin = from->positiveMargin;
            negativeMargin = from->negativeMargin;
            previousMargin = from->previousMargin;
            withClearance = from->withClearance;
        }
    }
    bool hasClearance() const {
        return withClearance;
    }
    void setClearance() {
        withClearance = true;
    }

    //
    // Text
    //
    void setText(const char16_t* text, size_t length) {
        textLength = length;
        textIterator.setText(text, length);
    }
    size_t getNextTextBoundary() {
        return textIterator.next() ? *textIterator : textIterator.size();
    }
    bool isFirstCharacter(const std::u16string& text);
    InlineLevelBox* getWrapBox(const std::u16string& text);
};

}}}}  // org::w3c::dom::bootstrap

#endif  // ES_FORMATTING_CONTEXT_H
