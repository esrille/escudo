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

#ifndef ES_TEXT_ITERATOR_H
#define ES_TEXT_ITERATOR_H

#include <unicode/ubrk.h>
// cf. http://icu-project.org/apiref/icu4c/ubrk_8h.html

namespace org { namespace w3c { namespace dom { namespace bootstrap {

class TextIterator
{
    UBreakIterator* bi;
    size_t current;
    size_t length;
public:
    TextIterator(UBreakIteratorType type = UBRK_LINE) :
        bi(0),
        current(UBRK_DONE),
        length(0)
    {
        UErrorCode err = U_ZERO_ERROR;
        bi = ubrk_open(UBRK_LINE, 0, 0, 0, &err);
        if (U_FAILURE(err))
            bi = 0;
    }
    ~TextIterator() {
        if (bi)
            ubrk_close(bi);
    }
    void setText(const char16_t* text, size_t length) {
        UErrorCode err = U_ZERO_ERROR;
        ubrk_setText(bi, reinterpret_cast<const UChar*>(text), length, &err);
        if (U_FAILURE(err)) {
            current = UBRK_DONE;
            return;
        }
        current = ubrk_first(bi);
        length = length;
    }
    bool next() {
        if (current != UBRK_DONE)
            current = ubrk_next(bi);
        return current != UBRK_DONE;
    }
    size_t operator*() {
        return current;
    }
    size_t size() const {
        return length;
    }
};

}}}}  // org::w3c::dom::bootstrap

#endif  // ES_TEXT_ITERATOR_H
