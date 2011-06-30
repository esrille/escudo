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
    // Location
    std::u16string getHref();
    void setHref(std::u16string href);
    void assign(std::u16string url);
    void replace(std::u16string url);
    void reload();
    std::u16string getProtocol();
    void setProtocol(std::u16string protocol);
    std::u16string getHost();
    void setHost(std::u16string host);
    std::u16string getHostname();
    void setHostname(std::u16string hostname);
    std::u16string getPort();
    void setPort(std::u16string port);
    std::u16string getPathname();
    void setPathname(std::u16string pathname);
    std::u16string getSearch();
    void setSearch(std::u16string search);
    std::u16string getHash();
    void setHash(std::u16string hash);
    std::u16string resolveURL(std::u16string url);

    // Object
    virtual Any message_(uint32_t selector, const char* id, int argc, Any* argv) {
        return html::Location::dispatch(this, selector, id, argc, argv);
    }

    LocationImp(WindowImp* window, std::u16string url);
    LocationImp(const LocationImp& other);
};

}}}}  // org::w3c::dom::bootstrap

#endif  // LOCATION_IMP_H
