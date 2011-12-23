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

#include "CSSImportRuleImp.h"

#include <boost/bind.hpp>
#include <boost/version.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/file_descriptor.hpp>

#include "DocumentImp.h"
#include "css/CSSInputStream.h"
#include "css/CSSParser.h"

#include "http/HTTPRequest.h"

namespace org
{
namespace w3c
{
namespace dom
{
namespace bootstrap
{

using namespace css;

CSSImportRuleImp::CSSImportRuleImp(const std::u16string& href) :
    href(href),
    mediaList(MediaListImp::All),
    request(0),
    styleSheet(0)
{
}

CSSImportRuleImp::~CSSImportRuleImp()
{
    delete request;
}

// CSSRule
unsigned short CSSImportRuleImp::getType()
{
    return CSSRule::IMPORT_RULE;
}

std::u16string CSSImportRuleImp::getCssText()
{
    std::u16string text = u"@import url(" + href + u")";
    std::u16string media = mediaList.getMediaText();
    if (!media.empty())
        text += u" " + media;
    return text;
}

// CSSImportRule
std::u16string CSSImportRuleImp::getHref()
{
    return href;
}

stylesheets::MediaList CSSImportRuleImp::getMedia()
{
    return &mediaList;
}

void CSSImportRuleImp::setMedia(std::u16string media)
{
    mediaList.setMediaText(media);
}

css::CSSStyleSheet CSSImportRuleImp::getStyleSheet()
{
    if (!styleSheet && !href.empty() && !request) {  // TODO: deal with ins. mem
        request = new(std::nothrow) HttpRequest(document->getDocumentURI());
        if (request) {
            request->open(u"GET", href);
            request->setHanndler(boost::bind(&CSSImportRuleImp::notify, this));
            document->incrementLoadEventDelayCount();
            request->send();
        }
    }
    return styleSheet;
}

void CSSImportRuleImp::notify()
{
    if (request->getStatus() == 200) {
#if 104400 <= BOOST_VERSION
        boost::iostreams::stream<boost::iostreams::file_descriptor_source> stream(request->getContentDescriptor(), boost::iostreams::never_close_handle);
#else
        boost::iostreams::stream<boost::iostreams::file_descriptor_source> stream(request->getContentDescriptor(), false);
#endif
        CSSParser parser;
        CSSInputStream cssStream(stream);  // TODO detect encode
        styleSheet = parser.parse(cssStream);
    }
    document->decrementLoadEventDelayCount();
}

}
}
}
}
