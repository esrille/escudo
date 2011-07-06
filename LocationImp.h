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

#ifndef LOCATION_IMP_H
#define LOCATION_IMP_H

#include <Object.h>
#include <org/w3c/dom/html/Location.h>

#include "url/URL.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

class WindowImp;

class LocationImp : public ObjectMixin<LocationImp>
{
    WindowImp* window;
    URL url;

public:
    LocationImp(WindowImp* window, std::u16string url);
    LocationImp(const LocationImp& other);

    // Location
    virtual std::u16string getHref() __attribute__((weak));
    virtual void setHref(std::u16string href) __attribute__((weak));
    virtual void assign(std::u16string url) __attribute__((weak));
    virtual void replace(std::u16string url) __attribute__((weak));
    virtual void reload() __attribute__((weak));
    virtual std::u16string getProtocol() __attribute__((weak));
    virtual void setProtocol(std::u16string protocol) __attribute__((weak));
    virtual std::u16string getHost() __attribute__((weak));
    virtual void setHost(std::u16string host) __attribute__((weak));
    virtual std::u16string getHostname() __attribute__((weak));
    virtual void setHostname(std::u16string hostname) __attribute__((weak));
    virtual std::u16string getPort() __attribute__((weak));
    virtual void setPort(std::u16string port) __attribute__((weak));
    virtual std::u16string getPathname() __attribute__((weak));
    virtual void setPathname(std::u16string pathname) __attribute__((weak));
    virtual std::u16string getSearch() __attribute__((weak));
    virtual void setSearch(std::u16string search) __attribute__((weak));
    virtual std::u16string getHash() __attribute__((weak));
    virtual void setHash(std::u16string hash) __attribute__((weak));
    virtual std::u16string resolveURL(std::u16string url) __attribute__((weak));
    // Object
    virtual Any message_(uint32_t selector, const char* id, int argc, Any* argv)
    {
        return html::Location::dispatch(this, selector, id, argc, argv);
    }
    static const char* const getMetaData()
    {
        return html::Location::getMetaData();
    }
};

}}}}  // org::w3c::dom::bootstrap

#endif  // LOCATION_IMP_H
