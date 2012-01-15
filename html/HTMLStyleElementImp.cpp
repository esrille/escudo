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

#include "HTMLStyleElementImp.h"

#include "css/CSSParser.h"
#include "TextImp.h"
#include "DocumentImp.h"
#include "WindowImp.h"

#include "Test.util.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

void HTMLStyleElementImp::eval()
{
    // TODO: update type, media, and scoped. Then check type.

    std::u16string content;
    for (Node node = getFirstChild(); node; node = node.getNextSibling()) {
        if (TextImp* text = dynamic_cast<TextImp*>(node.self()))  // TODO better to avoid imp call?
            content += text->getData();
    }
    CSSParser parser;
    styleSheet = parser.parse(content);

    dumpStyleSheet(std::cerr, styleSheet.self());

    if (DocumentImp* document = getOwnerDocumentImp()) {
        if (WindowImp* view = document->getDefaultWindow())
            view->setFlagsToBoxTree(1);
    }
}

// Node
Node HTMLStyleElementImp::cloneNode(bool deep)
{
    return new(std::nothrow) HTMLStyleElementImp(this, deep);
}

// HTMLStyleElement
bool HTMLStyleElementImp::getDisabled()
{
    if (!styleSheet)
        return false;
    return styleSheet.getDisabled();
}

void HTMLStyleElementImp::setDisabled(bool disabled)
{
    if (styleSheet)
        styleSheet.setDisabled(disabled);
}

std::u16string HTMLStyleElementImp::getMedia()
{
    return media;
}

void HTMLStyleElementImp::setMedia(std::u16string media)
{
    this->media = media;
}

std::u16string HTMLStyleElementImp::getType()
{
    return type;
}

void HTMLStyleElementImp::setType(std::u16string type)
{
    this->type = type;
}

bool HTMLStyleElementImp::getScoped()
{
    return scoped;
}

void HTMLStyleElementImp::setScoped(bool scoped)
{
    this->scoped = scoped;
}

// LinkStyle
stylesheets::StyleSheet HTMLStyleElementImp::getSheet()
{
    return styleSheet;
}

}}}}  // org::w3c::dom::bootstrap
