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

#ifndef ES_CSSSTACKINGCONTEXT_H
#define ES_CSSSTACKINGCONTEXT_H

#include <atomic>
#include <string>

#include <boost/intrusive_ptr.hpp>

namespace org { namespace w3c { namespace dom { namespace bootstrap {

class Box;
class Block;
class ViewCSSImp;
class CSSStyleDeclarationImp;

class StackingContext
{
    std::atomic_uint count;

    CSSStyleDeclarationImp* style;
    bool needStaticPosition;
    bool auto_;
    int zIndex;
    StackingContext* parent;
    StackingContext* firstChild;
    StackingContext* lastChild;
    StackingContext* previousSibling;
    StackingContext* nextSibling;
    unsigned int childCount;

    StackingContext* positioned;

    // The top level boxes in this StackingContext. Note a single, positioned
    // inline element can generate multiple inline boxes.
    Box* firstBase;
    Box* lastBase;
    Block* clipBox;

    // render
    Box* firstFloat;
    Box* lastFloat;
    Box* currentFloat;

    float relativeX;
    float relativeY;

    float clipLeft;
    float clipTop;
    float clipWidth;
    float clipHeight;

    StackingContext* removeChild(StackingContext* item);
    StackingContext* insertBefore(StackingContext* item, StackingContext* after);
    StackingContext* appendChild(StackingContext* item);

    StackingContext* getParent() const {
        return parent;
    }
    bool hasChildren() const {
        return firstChild;
    }
    StackingContext* getFirstChild() const {
        return firstChild;
    }
    StackingContext* getLastChild() const {
        return lastChild;
    }
    StackingContext* getPreviousSibling() const {
        return previousSibling;
    }
    StackingContext* getNextSibling() const {
        return nextSibling;
    }

    bool isAuto() const {
        return auto_;
    }

    void insertContext(StackingContext* item);
    void reparent(StackingContext* target);

    void updateClipBox(StackingContext* s);

public:
    StackingContext(bool auto_, int zIndex, CSSStyleDeclarationImp* style);
    ~StackingContext();

    unsigned int count_() const {
        return count;
    }
    unsigned int retain_() {
        return ++count;
    }
    unsigned int release_() {
        if (0 < count)
            --count;
        if (count == 0) {
            delete this;
            return 0;
        }
        return count;
    }

    StackingContext* getAuto(CSSStyleDeclarationImp* style) {
        return addContext(true, 0, style);
    }
    StackingContext* addContext(int zIndex, CSSStyleDeclarationImp* style) {
        return addContext(false, zIndex, style);
    }
    StackingContext* addContext(bool auto_, int zIndex, CSSStyleDeclarationImp* style);

    void setZIndex(bool auto_, int index = 0);

    void clearBase() {
        firstBase = lastBase = 0;
        firstFloat = lastFloat = 0;
        for (auto i = getFirstChild(); i; i = i->getNextSibling())
            i->clearBase();
    }
    Box* getBase() const {
        return firstBase;
    }
    void addBase(Box* box);
    void addBox(Box* box, Box* parentBox);

    Box* getLastFloat() const {
        return lastFloat;
    }
    void addFloat(Box* box);

    void removeBox(Box* box);

    void setClipBox(Block* box) {
        clipBox = box;
    }

    void layOutAbsolute(ViewCSSImp* view);

    void renderFloats(ViewCSSImp* view, Box* last, Box* current);

    void resetScrollSize();
    void resolveScrollSize(ViewCSSImp* view);

    bool hasClipBox();
    void render(ViewCSSImp* view);

    float getRelativeX() const {
        return relativeX;
    }
    float getRelativeY() const {
        return relativeY;
    }

    Box* boxFromPoint(int x, int y);

    void dump(std::string indent = "");
};

inline void intrusive_ptr_add_ref(StackingContext* context)
{
    context->retain_();
}

inline void intrusive_ptr_release(StackingContext* context)
{
    context->release_();
}

typedef boost::intrusive_ptr<StackingContext> StackingContextPtr;

}}}}  // org::w3c::dom::bootstrap

#endif  // ES_CSSSTACKINGCONTEXT_H
