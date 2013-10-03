/*
 * Copyright 2011-2013 Esrille Inc.
 * Copyright 2010 Google Inc.
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

#ifndef ES_SEQUENCE_H
#define ES_SEQUENCE_H

#include <initializer_list>
#include <new>
#include <org/w3c/dom/ObjectArray.h>

// The sequence type for Web IDL
// TODO: This implementation is not multi-thread safe.
template <typename T>
class SequenceImp : public Imp
{
    unsigned int length;
    T* sequence;

    void init(unsigned int size) {
        if (0 < size) {
            sequence = new(std::nothrow) T[size];
            length = sequence ? size : 0;
        } else {
            sequence = nullptr;
            length = 0;
        }
    }

    void copy(const T* array, unsigned int size) {
        for (unsigned int i = 0; i < length; ++i)
            sequence[i] = array[i];
    }

    // Prevent copying
    SequenceImp(const SequenceImp&) = delete;
    SequenceImp& operator=(const SequenceImp& value) = delete;

public:
    SequenceImp() : length{0}, sequence{nullptr} {}

    SequenceImp(const T* sequence, unsigned int length) {
        init(length);
        copy(sequence, length);
    }

    explicit SequenceImp(unsigned int size) {
        init(size);
    }

    SequenceImp(std::initializer_list<T> list) {
        init(list.size());
        if (sequence) {
            unsigned int index = 0;
            for (const T* p = list.begin(); p != list.end(); ++p, ++index)
                sequence[index] = *p;
        }
    }

    ~SequenceImp() {
        if (sequence)
            delete[] sequence;
    }

    T getElement(unsigned int index) const {
        // TODO: Check range
        return sequence[index];
    }

    void setElement(unsigned int index, const T value) {
        // TODO: Check range
        sequence[index] = value;
    }

    unsigned int getLength() const {
        return length;
    }

    void setLength(unsigned int length) {
        // TODO: Raise an exception
    }
};

template<typename E> using Sequence = org::w3c::dom::ObjectArray<E>;

#endif  // ES_SEQUENCE_H
