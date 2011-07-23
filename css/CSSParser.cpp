/*
 * Copyright 2010, 2011 Esrille Inc.
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

#include <Object.h>
#include <org/w3c/dom/css/CSSPrimitiveValue.h>

#include "CSSSelector.h"
#include "CSSStyleSheetImp.h"
#include "CSSStyleDeclarationImp.h"

using namespace org::w3c::dom::bootstrap;
using namespace org::w3c::dom::css;

CSSStyleSheet CSSParser::parse(const std::u16string& cssText)
{
    styleSheet = new(std::nothrow) CSSStyleSheetImp;
    if (!styleSheet)
        return 0;
    tokenizer.reset(cssText);
    CSSparse(this);
    return styleSheet;
}

CSSStyleDeclaration CSSParser::parseDeclarations(const std::u16string& cssDecl)
{
    styleDeclaration = new(std::nothrow) CSSStyleDeclarationImp;
    if (!styleDeclaration)
        return 0;
    tokenizer.reset(cssDecl, CSSTokenizer::StartDeclarationList);
    CSSparse(this);
    return styleDeclaration;
}

CSSParserExpr* CSSParser::parseExpression(const std::u16string& cssExpr)
{
    tokenizer.reset(cssExpr, CSSTokenizer::StartExpression);
    CSSparse(this);
    return getExpression();
}

std::u16string CSSParserExpr::getCssText()
{
    std::u16string cssText;
    for (auto i = list.begin(); i != list.end(); ++i)
        cssText += (*i).getCssText();
    return cssText;
}

std::u16string CSSParserTerm::getCssText()
{
    std::u16string cssText;

    if (unit == CSS_TERM_OPERATOR) {
        cssText += static_cast<char16_t>(op);
        return cssText;
    }

    switch (op) {
    case '/':
        cssText += u" / ";
        break;
    case ',':
        cssText += u", ";
        break;
    default:
        break;
    }

    switch (unit) {
    case CSSPrimitiveValue::CSS_NUMBER:
        cssText += CSSSerializeNumber(number);
        break;
    case CSSPrimitiveValue::CSS_PERCENTAGE:
        cssText += CSSSerializeNumber(number) + u'%';
        break;
    case CSSPrimitiveValue::CSS_EMS:
        cssText += CSSSerializeNumber(number) + u"em";
        break;
    case CSSPrimitiveValue::CSS_EXS:
        cssText += CSSSerializeNumber(number) + u"ex";
        break;
    case CSSPrimitiveValue::CSS_PX:
        cssText += CSSSerializeNumber(number) + u"px";
        break;
    case CSSPrimitiveValue::CSS_CM:
        cssText += CSSSerializeNumber(number) + u"cm";
        break;
    case CSSPrimitiveValue::CSS_MM:
        cssText += CSSSerializeNumber(number) + u"mm";
        break;
    case CSSPrimitiveValue::CSS_IN:
        cssText += CSSSerializeNumber(number) + u"in";
        break;
    case CSSPrimitiveValue::CSS_PT:
        cssText += CSSSerializeNumber(number) + u"pt";
        break;
    case CSSPrimitiveValue::CSS_PC:
        cssText += CSSSerializeNumber(number) + u"pc";
        break;
    case CSSPrimitiveValue::CSS_DEG:
        cssText += CSSSerializeNumber(number) + u"deg";
        break;
    case CSSPrimitiveValue::CSS_RAD:
        cssText += CSSSerializeNumber(number) + u"rad";
        break;
    case CSSPrimitiveValue::CSS_GRAD:
        cssText += CSSSerializeNumber(number) + u"grad";
        break;
    case CSSPrimitiveValue::CSS_MS:
        cssText += CSSSerializeNumber(number) + u"ms";
        break;
    case CSSPrimitiveValue::CSS_S:
        cssText += CSSSerializeNumber(number) + u"s";
        break;
    case CSSPrimitiveValue::CSS_HZ:
        cssText += CSSSerializeNumber(number) + u"Hz";
        break;
    case CSSPrimitiveValue::CSS_KHZ:
        cssText += CSSSerializeNumber(number) + u"kHz";
        break;
    case CSSPrimitiveValue::CSS_IDENT:
        cssText += CSSSerializeIdentifier(text);
        break;
    case CSSPrimitiveValue::CSS_STRING:
        cssText += CSSSerializeString(text);
        break;
    case CSSPrimitiveValue::CSS_URI:
        cssText += u"url(" + CSSSerializeString(text) + u")";
        break;
    case CSSPrimitiveValue::CSS_UNICODE_RANGE:
        cssText += text;
        break;
    case CSSPrimitiveValue::CSS_RGBCOLOR:
        cssText += CSSSerializeRGB(rgb);
        break;
    case CSS_TERM_FUNCTION:
        cssText += CSSSerializeIdentifier(text) + u'(' + expr->getCssText() + u')';
        break;
    case CSSPrimitiveValue::CSS_DIMENSION:
        cssText += CSSSerializeNumber(number) + CSSSerializeIdentifier(text);
        break;
    case CSSPrimitiveValue::CSS_ATTR:
    case CSSPrimitiveValue::CSS_COUNTER:
    case CSSPrimitiveValue::CSS_RECT:
    case CSSPrimitiveValue::CSS_UNKNOWN:
    default:
        break;
    }
    return cssText;
}
