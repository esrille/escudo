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

#ifndef ORG_W3C_DOM_BOOTSTRAP_NAVIGATORIMP_H_INCLUDED
#define ORG_W3C_DOM_BOOTSTRAP_NAVIGATORIMP_H_INCLUDED

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <org/w3c/dom/html/Navigator.h>

#include <org/w3c/dom/html/NavigatorUserMediaSuccessCallback.h>
#include <org/w3c/dom/html/NavigatorUserMediaErrorCallback.h>

namespace org
{
namespace w3c
{
namespace dom
{
namespace bootstrap
{
class NavigatorImp : public ObjectMixin<NavigatorImp>
{
public:
    // Navigator
    // NavigatorID
    std::u16string getAppName();
    std::u16string getAppVersion();
    std::u16string getPlatform();
    std::u16string getUserAgent();
    // NavigatorOnLine
    bool getOnLine();
    // NavigatorContentUtils
    void registerProtocolHandler(std::u16string scheme, std::u16string url, std::u16string title);
    void registerContentHandler(std::u16string mimeType, std::u16string url, std::u16string title);
    // NavigatorStorageUtils
    void yieldForStorageUpdates();
    // NavigatorUserMedia
    void getUserMedia(std::u16string options, html::NavigatorUserMediaSuccessCallback successCallback);
    void getUserMedia(std::u16string options, html::NavigatorUserMediaSuccessCallback successCallback, html::NavigatorUserMediaErrorCallback errorCallback);
    // Object
    virtual Any message_(uint32_t selector, const char* id, int argc, Any* argv)
    {
        return html::Navigator::dispatch(this, selector, id, argc, argv);
    }
    static const char* const getMetaData()
    {
        return html::Navigator::getMetaData();
    }
};

}
}
}
}

#endif  // ORG_W3C_DOM_BOOTSTRAP_NAVIGATORIMP_H_INCLUDED
