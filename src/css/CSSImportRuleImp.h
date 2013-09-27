/*
 * Copyright 2011-2013 Esrille Inc.
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

#ifndef ORG_W3C_DOM_BOOTSTRAP_CSSIMPORTRULEIMP_H_INCLUDED
#define ORG_W3C_DOM_BOOTSTRAP_CSSIMPORTRULEIMP_H_INCLUDED

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <org/w3c/dom/css/CSSImportRule.h>
#include "CSSRuleImp.h"

#include <org/w3c/dom/stylesheets/MediaList.h>
#include <org/w3c/dom/css/CSSStyleSheet.h>
#include <org/w3c/dom/css/CSSRule.h>

#include "MediaListImp.h"

namespace org
{
namespace w3c
{
namespace dom
{
namespace bootstrap
{

class DocumentImp;
class HttpRequest;

typedef std::shared_ptr<DocumentImp> DocumentPtr;

class CSSImportRuleImp : public ObjectMixin<CSSImportRuleImp, CSSRuleImp>
{
    std::weak_ptr<DocumentImp> document;

    std::u16string href;
    stylesheets::MediaList mediaList;

    std::unique_ptr<HttpRequest> request;
    css::CSSStyleSheet styleSheet;

    void notify();

public:
    CSSImportRuleImp(const std::u16string& href);

    void setDocument(const DocumentPtr& document) {
        this->document = document;
    }

    void setMediaList(MediaListPtr other);

    // CSSRule
    virtual unsigned short getType();
    virtual std::u16string getCssText();

    // CSSImportRule
    std::u16string getHref();
    stylesheets::MediaList getMedia();
    void setMedia(const std::u16string& media);
    css::CSSStyleSheet getStyleSheet();
    // Object
    virtual Any message_(uint32_t selector, const char* id, int argc, Any* argv)
    {
        return css::CSSImportRule::dispatch(this, selector, id, argc, argv);
    }
    static const char* const getMetaData()
    {
        return css::CSSImportRule::getMetaData();
    }
};

typedef std::shared_ptr<CSSImportRuleImp> CSSImportRulePtr;

}
}
}
}

#endif  // ORG_W3C_DOM_BOOTSTRAP_CSSIMPORTRULEIMP_H_INCLUDED
