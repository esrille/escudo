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
class Block;
class StackingContext;
class ViewCSSImp;
class WindowImp;

// SavedFormattingContext stores the initial FormattingContext state for
// laying out a block-level box.
struct SavedFormattingContext
{
    struct FloatingBoxContext {
        Block* floatingBox;
        float remainingHeight;

        FloatingBoxContext(Block* floatingBox, float remainingHeight) :
            floatingBox(floatingBox),
            remainingHeight(remainingHeight)
        {}
    };

    struct MarginContext {
        float clearance;  // The clearance introduced by the previous collapsed through boxes.
        float usedMargin;
        // Adjoining margins
        float positiveMargin;
        float negativeMargin;
        float previousMargin;
        bool withClearance;
    };

    bool saved;

    // Saved context for floating boxes
    float blankLeft;
    float blankRight;
    std::list<FloatingBoxContext> left;
    std::list<FloatingBoxContext> right;

    // Saved context for margin collapse
    float clearance;
    float marginTop;
    float marginRight;
    float marginBottom;
    float marginLeft;
    float topBorderEdge;
    float consumed;
    MarginContext marginContext;

    SavedFormattingContext() :
        saved(false)
    {}

    void reset() {
        saved = false;
    }
};

class FormattingContext
{
    friend class Block;
    friend class TableWrapperBox;

    TextIterator textIterator;
    size_t textLength;

    bool breakable;
    bool isFirstLine;
    LineBox* lineBox;
    float x;
    float leftover;
    char16_t prevChar;

    // Context for floating boxes
    float blankLeft;
    float blankRight;
    std::list<Block*> left;           // active floating boxes on the left side
    std::list<Block*> right;          // active floating boxes on the right side
    std::list<Block*> floatingBoxes;  // floating boxes to be added

    // Context for margin collapse
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

    float shiftDownLeft();
    float shiftDownRight();

    void saveContext(SavedFormattingContext::MarginContext& context);
    void restoreContext(const SavedFormattingContext::MarginContext& context);
    bool hasChanged(const SavedFormattingContext::MarginContext& context);

public:
    FormattingContext();

    void updateBlanks(Box* box);
    void restoreBlanks(Box* box);

    void saveContext(Block* block);
    void restoreContext(Block* block);
    bool hasChanged(const Block* block);

    LineBox* addLineBox(ViewCSSImp* view, Block* parentBox);
    void addFloat(Block* floatBox, float totalWidth);

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
    float shiftDown(float* e = 0);
    bool shiftDownLineBox(ViewCSSImp* view);
    bool hasNewFloats() const;
    void appendInlineBox(ViewCSSImp* view, InlineBox* inlineBox, CSSStyleDeclarationImp* activeStyle);
    void dontWrap();
    void nextLine(ViewCSSImp* view, Block* parentBox, bool linefeed);
    void tryAddFloat(ViewCSSImp* view);
    float adjustRemainingHeight(float height, Block* from = 0);

    // Use the positive margin stored in context to consume the remaining height of floating boxes.
    void useMargin(Block* block);

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
    InlineBox* getWrapBox(const std::u16string& text);
};

}}}}  // org::w3c::dom::bootstrap

#endif  // ES_FORMATTING_CONTEXT_H
