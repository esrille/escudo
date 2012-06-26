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

#include "NavigatorImp.h"

#include "utf.h"

namespace org
{
namespace w3c
{
namespace dom
{
namespace bootstrap
{

std::u16string NavigatorImp::getAppName()
{
    return u"Escort";
}

std::u16string NavigatorImp::getAppVersion()
{
    return utfconv(PACKAGE_VERSION);
}

std::u16string NavigatorImp::getPlatform()
{
    return u"";
}

std::u16string NavigatorImp::getUserAgent()
{
    // cf. http://tools.ietf.org/html/rfc2616#section-14.43
    return getAppName() + u'/' + getAppVersion();
}

bool NavigatorImp::getOnLine()
{
    // TODO: implement me!
    return 0;
}

void NavigatorImp::registerProtocolHandler(std::u16string scheme, std::u16string url, std::u16string title)
{
    // TODO: implement me!
}

void NavigatorImp::registerContentHandler(std::u16string mimeType, std::u16string url, std::u16string title)
{
    // TODO: implement me!
}

void NavigatorImp::yieldForStorageUpdates()
{
    // TODO: implement me!
}

void NavigatorImp::getUserMedia(std::u16string options, html::NavigatorUserMediaSuccessCallback successCallback)
{
    // TODO: implement me!
}

void NavigatorImp::getUserMedia(std::u16string options, html::NavigatorUserMediaSuccessCallback successCallback, html::NavigatorUserMediaErrorCallback errorCallback)
{
    // TODO: implement me!
}

}
}
}
}
