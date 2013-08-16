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

#include "MediaListImp.h"

#include <assert.h>
#include <math.h>

#include <utility>

#include <org/w3c/dom/css/CSSPrimitiveValue.h>

#include "css/CSSParser.h"
#include "css/CSSPropertyValueImp.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

using namespace stylesheets;

namespace {

const char16_t* const allType = u"all";

const char16_t* const mediaTypes[] = {
    u"braille",
    u"embossed",
    u"handheld",
    u"print",
    u"projection",
    u"screen",
    u"speech",
    u"tty",
    u"tv"
};

const char16_t* const featureTypes[] = {
    u"width",
    u"height",
    u"device-width",
    u"device-height",
    u"orientation",
    u"aspect-ratio",
    u"device-aspect-ratio",
    u"color",
    u"color-index",
    u"monochrome",
    u"resolution",
    u"scan",
    u"grid",
};

const char16_t* const orientationTypes[] = {
    u"",
    u"portrait",
    u"landscape",
};

const char16_t* const scanTypes[] = {
    u"",
    u"progressive",
    u"interlace",
};

const size_t mediaTypesCount = sizeof mediaTypes / sizeof mediaTypes[0];

}

std::u16string MediaListImp::
MediaFeature::getMediaText() const
{
    std::u16string text;

    if (isMax())
        text += u"max-";
    else if (isMin())
        text += u"min-";
    text += featureTypes[feature & 0xfff];

    switch (feature) {
    case AspectRatio:
    case MinAspectRatio:
    case MaxAspectRatio:
    case DeviceAspectRatio:
    case MinDeviceAspectRatio:
    case MaxDeviceAspectRatio:
        if (!isnan(value))
            text += u": " + CSSSerializeNumber(value) + u'/' + CSSSerializeNumber(unit);
        break;
    case Orientation:
        if (0 < getIndex()) {
            text += u": ";
            text += orientationTypes[getIndex()];
        }
        break;
    case Scan:
        if (0 < getIndex())
            text += u": ";
            text += scanTypes[getIndex()];
        break;
    default:
        if (!isnan(value)) {
            text += u": " + CSSSerializeNumber(value);
            if (css::CSSPrimitiveValue::CSS_PERCENTAGE <= unit && unit <= css::CSSPrimitiveValue::CSS_KHZ)
                text += CSSNumericValue::Units[unit - css::CSSPrimitiveValue::CSS_PERCENTAGE];
            else if (css::CSSPrimitiveValue::CSS_DPPX <= unit && unit <= css::CSSPrimitiveValue::CSS_DPCM)
                text += CSSNumericValue::ResolutionUnits[unit - css::CSSPrimitiveValue::CSS_DPPX];
        }
        break;
    }

    return text;
}

MediaListImp::
MediaQuery::MediaQuery(unsigned type, std::list<MediaFeature>&& features) :
    type(type),
    features(std::move(features))
{
}

MediaListImp::
MediaQuery::MediaQuery(MediaQuery&& from) :
    type(from.type),
    features(std::move(from.features))
{
    from.type = 0;
    assert(from.features.empty());
}

std::u16string MediaListImp::
MediaQuery::getMediaText()
{
    std::u16string text;

    if (type & Only)
        text += u"only ";
    if (type & Not)
        text += u"not ";
    if (type == All)
        text += allType;
    else {
        for (unsigned i = 0; i < mediaTypesCount; ++i) {
            if (type & (1u << i)) {
                text += mediaTypes[i];
                break;
            }
        }
    }
    for (auto i = features.begin(); i != features.end(); ++i)
        text += u" and (" + (*i).getMediaText() + u')';
    return text;
}

// MediaList

bool MediaListImp::hasMedium(unsigned bit)
{
    // TODO: Process features
    if (mediaQueries.empty())
        return true;
    for (auto i = mediaQueries.begin(); i != mediaQueries.end(); ++i) {
        if ((*i).type & bit)
            return true;
    }
    return false;
}

void MediaListImp::appendMedium(unsigned medium)
{
    mediaQueries.emplace_back(medium, std::move(mediaFeatures));
    assert(mediaFeatures.empty());
}

void MediaListImp::appendFeature(int feature, CSSParserExpr* expr)
{
    float value(NAN);
    unsigned short unit(css::CSSPrimitiveValue::CSS_UNKNOWN);    // or index
    if (expr) {
        CSSValueParser parser(feature);
        if (!parser.isValid(expr))
            feature = Unknown;
        CSSParserTerm* term = parser.getStack().back();
        switch (feature) {
        case AspectRatio:
        case MinAspectRatio:
        case MaxAspectRatio:
        case DeviceAspectRatio:
        case MinDeviceAspectRatio:
        case MaxDeviceAspectRatio:
            assert(parser.getStack().size() == 3);
            value = parser.getStack()[0]->getNumber();
            unit = parser.getStack()[2]->getNumber();
            break;
        case Orientation:
        case Scan:
            unit = (term->getIndex() < 0) ? 0 : term->getIndex();
            break;
        default:
            value = term->getNumber();
            unit = term->unit;
            break;
        }
        delete expr;
    }
    mediaFeatures.emplace_back(feature, value, unit);
}

std::u16string MediaListImp::getMediaText()
{
    if (mediaQueries.empty())
        return allType;
    std::u16string text;
    for (auto i = mediaQueries.begin(); i != mediaQueries.end(); ++i) {
        if (i != mediaQueries.begin())
            text += u", ";
        text += (*i).getMediaText();
    }
    return text;
}

void MediaListImp::setMediaText(const std::u16string& mediaText)
{
    clear();
    if (!mediaText.empty()) {
        CSSParser parser;
        *this = std::move(*parser.parseMediaList(mediaText));
    }
}

unsigned int MediaListImp::getLength()
{
    return mediaQueries.empty() ? 1 : mediaQueries.size();
}

std::u16string MediaListImp::item(unsigned int index)
{
    if (getLength() <= index)
        return u"";
    if (mediaQueries.empty())
        return allType;
    return mediaQueries[index].getMediaText();
}

void MediaListImp::appendMedium(const std::u16string& medium)
{
    if (medium.empty())
        return;
    CSSParser parser;
    parser.parseMediaList(medium);
    // TODO: append
}

void MediaListImp::deleteMedium(const std::u16string& medium)
{
    if (medium.empty())
        return;
    CSSParser parser;
    parser.parseMediaList(medium);
    // TODO: delete
}

}  // org::w3c::dom::bootstrap

namespace stylesheets {

namespace {

class Constructor : public Object
{
public:
    // Object
    virtual Any message_(uint32_t selector, const char* id, int argc, Any* argv) {
        bootstrap::MediaListImp* imp = 0;
        switch (argc) {
        case 1:
            imp = new(std::nothrow) bootstrap::MediaListImp;
            if (imp)
                imp->setMediaText(argv[0].toString());
            break;
        default:
            break;
        }
        return imp;
    }
    Constructor() :
        Object(this) {
    }
};

}  // namespace

Object MediaList::getConstructor()
{
    static Constructor constructor;
    return constructor.self();
}

}

}}}  // org::w3c::dom::bootstrap
