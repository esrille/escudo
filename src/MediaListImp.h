/*
 * Copyright 2010-2013 Esrille Inc.
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

#ifndef ORG_W3C_DOM_BOOTSTRAP_MEDIALISTIMP_H_INCLUDED
#define ORG_W3C_DOM_BOOTSTRAP_MEDIALISTIMP_H_INCLUDED

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <org/w3c/dom/stylesheets/MediaList.h>

#include <deque>
#include <list>

namespace org { namespace w3c { namespace dom { namespace bootstrap {

class CSSParserExpr;
class WindowImp;

class MediaListImp : public ObjectMixin<MediaListImp>
{
public:
    // Media query types and operators
    enum {
        All = 0x01ff,
        Not = 0x8000,
        Only = 0x4000,

        Braille = 0x1,
        Embossed = 0x2,
        Handheld = 0x4,
        Print = 0x8,
        Projection = 0x10,
        Screen = 0x20,
        Speech = 0x40,
        Tty = 0x80,
        Tv = 0x100,
    };
    // Media features
    enum {
        Unknown = 0,

        Min                     = 0x2000,
        Max                     = 0x3000,

        Width                   = 0x1000,
        MinWidth                = 0x2000,
        MaxWidth                = 0x3000,
        Height                  = 0x1001,
        MinHeight               = 0x2001,
        MaxHeight               = 0x3001,
        DeviceWidth             = 0x1002,
        MinDeviceWidth          = 0x2002,
        MaxDeviceWidth          = 0x3002,
        DeviceHeight            = 0x1003,
        MinDeviceHeight         = 0x2003,
        MaxDeviceHeight         = 0x3003,
        Orientation             = 0x1004,
        AspectRatio             = 0x1005,
        MinAspectRatio          = 0x2005,
        MaxAspectRatio          = 0x3005,
        DeviceAspectRatio       = 0x1006,
        MinDeviceAspectRatio    = 0x2006,
        MaxDeviceAspectRatio    = 0x3006,
        Color                   = 0x1007,
        MinColor                = 0x2007,
        MaxColor                = 0x3007,
        ColorIndex              = 0x1008,
        MinColorIndex           = 0x2008,
        MaxColorIndex           = 0x3008,
        Monochrome              = 0x1009,
        MinMonochrome           = 0x2009,
        MaxMonochrome           = 0x3009,
        Resolution              = 0x100a,
        MinResolution           = 0x200a,
        MaxResolution           = 0x300a,
        Scan                    = 0x100b,
        Grid                    = 0x100c,

        // Orientation
        Portrait = 1,
        Landscape = 2,

        // Scan
        Progressive = 1,
        Interlace = 2,
    };
    struct MediaFeature {
        int feature;
        float value;
        unsigned short unit;    // or index

        MediaFeature(int feature, float value, unsigned short unit) :
            feature(feature),
            value(value),
            unit(unit)
        {
        }

        unsigned short getIndex() const {
            return unit;
        }
        bool isMin() const {
            return (feature & Max) == Min;
        }
        bool isMax() const {
            return (feature & Max) == Max;
        }
        std::u16string getMediaText() const;
    };
    struct MediaQuery {
        unsigned type;
        std::list<MediaFeature> features;

        MediaQuery(unsigned type, std::list<MediaFeature>&& features);
        MediaQuery(MediaQuery&& from);

        std::u16string getMediaText();
    };

private:
    std::list<MediaFeature> mediaFeatures;  // only used while parsing the text

    std::deque<MediaQuery> mediaQueries;

public:
    MediaListImp()
    {}
    MediaListImp(MediaListImp&& other) :
        mediaQueries(std::move(other.mediaQueries))
    {}

    MediaListImp& operator=(MediaListImp&& other) {
        mediaFeatures.clear();
        mediaQueries = std::move(other.mediaQueries);
        return *this;
    }

    void clear() {
        mediaQueries.clear();
    }
    bool matches(WindowImp* window);

    void appendFeature(int feature, CSSParserExpr* expr);
    void appendMedium(unsigned medium);

    // MediaList
    std::u16string getMediaText();
    void setMediaText(const std::u16string& mediaText);
    unsigned int getLength();
    std::u16string item(unsigned int index);
    void appendMedium(const std::u16string& medium);
    void deleteMedium(const std::u16string& medium);
    // Object
    virtual Any message_(uint32_t selector, const char* id, int argc, Any* argv)
    {
        return stylesheets::MediaList::dispatch(this, selector, id, argc, argv);
    }
    static const char* const getMetaData()
    {
        return stylesheets::MediaList::getMetaData();
    }

    static unsigned getMediaTypeID(const std::u16string& feature);
    static int getFeatureID(const std::u16string& feature);
};

}}}}  // org::w3c::dom::bootstrap

#endif  // ORG_W3C_DOM_BOOTSTRAP_MEDIALISTIMP_H_INCLUDED
