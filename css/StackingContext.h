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

#ifndef ES_CSSSTACKINGCONTEXT_H
#define ES_CSSSTACKINGCONTEXT_H

#include <string>

namespace org { namespace w3c { namespace dom { namespace bootstrap {

class Box;
class ViewCSSImp;

class StackingContext
{
    bool auto_;
    int zIndex;
    StackingContext* parent;
    StackingContext* firstChild;
    StackingContext* lastChild;
    StackingContext* previousSibling;
    StackingContext* nextSibling;
    unsigned int childCount;

    // The top level boxes in this StackingContext. Note a single, positioned
    // inline element can generate multiple inline boxes.
    Box* firstBase;
    Box* lastBase;

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

public:
    StackingContext(bool auto_, int zIndex = 0);
    ~StackingContext();

    StackingContext* getAuto() {
        return addContext(true, 0);
    }
    StackingContext* addContext(int zIndex) {
        return addContext(false, zIndex);
    }
    StackingContext* addContext(bool auto_, int zIndex);

    void clearBase() {
        firstBase = lastBase = 0;
        for (auto i = getFirstChild(); i; i = i->getNextSibling())
            i->clearBase();
    }
    Box* getBase() const {
        return firstBase;
    }
    void addBase(Box* box);

    void render(ViewCSSImp* view);

    void dump(std::string indent = "");
};

}}}}  // org::w3c::dom::bootstrap

#endif  // ES_CSSSTACKINGCONTEXT_H