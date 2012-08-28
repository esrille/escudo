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

#ifndef ES_BOX_IMAGE_H
#define ES_BOX_IMAGE_H

#include <cstdio>
#include <vector>
#include <stdint.h>

namespace org { namespace w3c { namespace dom {

namespace bootstrap {

class ViewCSSImp;

class ViewCSSImp;
class BoxImage
{
public:
    static const short Unavailable = 0;
    static const short Sent = 1;
    static const short PartiallyAvailable = 2;
    static const short CompletelyAvailable = 3;
    static const short Broken = 4;

    static const unsigned RepeatS = 1;
    static const unsigned RepeatT = 2;
    static const unsigned Clamp = 4;

private:
    static const short Rendered = 1;

    short state;
    unsigned short flags;
    unsigned char* pixels;  // in argb32 format
    unsigned naturalWidth;
    unsigned naturalHeight;
    unsigned repeat;
    unsigned format;
    unsigned frameCount;
    unsigned loop;
    std::vector<uint16_t> delays;
    unsigned total;

public:
    BoxImage(unsigned repeat = Clamp);
    ~BoxImage();

    void open(std::FILE* file);

    short getState() const {
        return state;
    }
    void setState(short value) {
        state = value;
    }
    unsigned getNaturalWidth() const {
        return naturalWidth;
    }
    unsigned getNaturalHeight() const {
        return naturalHeight;
    }
    unsigned char* getPixels() const {
        return pixels;
    }
    unsigned getCurrentFrame(unsigned t, unsigned& delay, unsigned start);
    unsigned render(ViewCSSImp* view, float x, float y, float width, float height, float left, float top, unsigned start);
};

}}}}  // org::w3c::dom::bootstrap

#endif  // ES_BOX_IMAGE_H
