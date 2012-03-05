/*
 * Copyright 2012 Esrille Inc.
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

#ifndef HTML_REPLACED_ELEMENT_IMP_H
#define HTML_REPLACED_ELEMENT_IMP_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "HTMLElementImp.h"

#include "http/HTTPRequest.h"
#include "css/Box.h"

namespace org
{
namespace w3c
{
namespace dom
{
namespace bootstrap
{

class HTMLReplacedElementImp : public HTMLElementImp
{
protected:
    HttpRequest* request;
    bool active;
    BoxImage* image;

public:
    HTMLReplacedElementImp(DocumentImp* ownerDocument, const std::u16string& localName) :
        HTMLElementImp(ownerDocument, localName),
        request(0),
        active(true),
        image(0)
    {
    }
    HTMLReplacedElementImp(HTMLReplacedElementImp* org, bool deep) :
        HTMLElementImp(org, deep),
        request(0), // TODO
        active(org->active),
        image(0)    // TODO
    {
    }
    ~HTMLReplacedElementImp()
    {
        delete request;
        delete image;
    }

    bool getIntrinsicSize(float& w, float& h) const {
        if (!active)
            return false;
        if (!image)
            w = h = 0.0f;
        else {
            w = image->getNaturalWidth();
            h = image->getNaturalHeight();
        }
        return true;
    }
    BoxImage* getImage() const {
        return image;
    }
};

}
}
}
}

#endif  // HTML_REPLACED_ELEMENT_IMP_H
