/*
 * Copyright 2011 Esrille Inc.
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

#include <Any.h>

// The sequence type for Web IDL
// TODO: This implementation is not multi-thread safe.
template <typename T>
class Sequence
{
    class Rep
    {
        friend class Sequence;
        // reference count
        unsigned count;
        // stub
        unsigned int length;
        T* sequence;

        void init(unsigned int size)
        {
            length = size;
            sequence = (0 < length) ? new T[length] : 0;
            count = 1;
        }

        void copy(const T* array, unsigned int size)
        {
            for (unsigned int i = 0; i < length; ++i)
            {
                sequence[i] = array[i];
            }
        }

        // Prevent copying
        Rep(const Rep& value);
        Rep& operator=(const Rep& value);

    public:
        Rep()
        {
            length = 0;
            sequence = 0;
            count = 1;
        }

        Rep(const T* sequence, unsigned int length)
        {
            init(length);
            copy(sequence, length);
        }

        Rep(const char** array, unsigned int length)
        {
            init(length);
            for (unsigned int i = 0; i < length; ++i)
            {
                sequence[i] = array[i];
            }
        }

        explicit Rep(unsigned int size)
        {
            init(size);
        }

        ~Rep()
        {
            if (sequence)
            {
                delete[] sequence;
            }
        }

        virtual T& getElement(unsigned int index)
        {
            return sequence[index];
        }

        virtual void setElement(unsigned int index, const T value)
        {
            sequence[index] = value;
        }

        virtual unsigned int getLength()
        {
            return length;
        }

        virtual void setLength(unsigned int length)
        {
            // TODO: Raise an exception
        }
    };

    Rep* rep;

public:
    Sequence()
    {
        rep = new Rep();
    }

    Sequence(const T* array, unsigned int size)
    {
        rep = new Rep(array, size);
    }

    // For std::string
    Sequence(const char** array, unsigned int size)
    {
        rep = new Rep(array, size);
    }

    explicit Sequence(unsigned int size)
    {
        rep = new Rep(size);
    }

#ifdef __GXX_EXPERIMENTAL_CXX0X__
    Sequence(std::initializer_list<T> list)
    {
        rep = new Rep(list.size());
        unsigned int index = 0;
        for (const T* p = list.begin(); p != list.end(); ++p, ++index)
        {
            rep->setElement(index, *p);
        }
    }
#endif

    // Copy-constructor
    Sequence(const Sequence& value)
    {
        ++value.rep->count;
        rep = value.rep;
    }

    ~Sequence()
    {
        if (--rep->count == 0)
        {
            delete rep;
        }
    }

    Sequence& operator=(const Sequence& value)
    {
        ++value.rep->count;
        if (--rep->count == 0)
        {
            delete rep;
        }
        rep = value.rep;
        return *this;
    }

    T getElement(unsigned int index) const
    {
        return rep->getElement(index);
    }

    T& getElement(unsigned int index)
    {
        return rep->getElement(index);
    }

    void setElement(unsigned int index, const T value)
    {
        rep->setElement(index, value);
    }

    T operator[](int index) const
    {
        return rep->getElement(index);
    }

    T& operator[](int index)
    {
        return rep->getElement(index);
    }

    T at(unsigned int index) const
    {
        if (getLength() <= index)
        {
            // TODO: raise an exception;
        }
        return rep->getElement(index);
    }

    T& at(unsigned int index)
    {
        if (getLength() <= index)
        {
            // TODO: raise an exception;
        }
        return rep->getElement(index);
    }

    unsigned int getLength() const
    {
        return rep->getLength();
    }

    Sequence(const Any& any)
    {
        const Sequence& value = any.as<Sequence<T> >();
        ++value.rep->count;
        rep = value.rep;
    }
};

#endif  // ES_SEQUENCE_H
