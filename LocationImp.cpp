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

#include "LocationImp.h"

#include "WindowImp.h"
#include "http/HTTPConnection.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

// Location
std::u16string LocationImp::getHref()
{
    return static_cast<std::u16string>(url);
}

void LocationImp::setHref(std::u16string href)
{
    // TODO: still need to refine a lot.
    URL resolved(url, href);
    window->open(resolved);
}

void LocationImp::assign(std::u16string url)
{
}

void LocationImp::replace(std::u16string url)
{
    // TODO: implement me!
}

void LocationImp::reload()
{
    // TODO: implement me!
}

std::u16string LocationImp::getProtocol()
{
    return url.getProtocol();
}

void LocationImp::setProtocol(std::u16string protocol)
{
    // TODO: implement me!
}

std::u16string LocationImp::getHost()
{
    return url.getHost();
}

void LocationImp::setHost(std::u16string host)
{
    // TODO: implement me!
}

std::u16string LocationImp::getHostname()
{
    return url.getHostname();
}

void LocationImp::setHostname(std::u16string hostname)
{
    // TODO: implement me!
}

std::u16string LocationImp::getPort()
{
    return url.getPort();
}

void LocationImp::setPort(std::u16string port)
{
    // TODO: implement me!
}

std::u16string LocationImp::getPathname()
{
    return url.getPathname();
}

void LocationImp::setPathname(std::u16string pathname)
{
    // TODO: implement me!
}

std::u16string LocationImp::getSearch()
{
    return url.getSearch();
}

void LocationImp::setSearch(std::u16string search)
{
    // TODO: implement me!
}

std::u16string LocationImp::getHash()
{
    return url.getHash();
}

void LocationImp::setHash(std::u16string hash)
{
    // TODO: implement me!
}

std::u16string LocationImp::resolveURL(std::u16string url)
{
    URL resolved(this->url, url);  // TODO: base might not be this->url
    return static_cast<std::u16string>(resolved);  // TODO: raise an exception upon error
}

LocationImp::LocationImp(WindowImp* window, std::u16string url) :
    window(window),
    url(url)
{
}

LocationImp::LocationImp(const LocationImp& other) :
    window(other.window),
    url(other.url)
{
}

}}}}  // org::w3c::dom::bootstrap
