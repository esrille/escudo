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

#ifndef ES_HTTP_UTIL_H
#define ES_HTTP_UTIL_H

#include <string>

namespace org { namespace w3c { namespace dom { namespace bootstrap {

namespace http {

void toLowerCase(std::string& v);
void toUpperCase(std::string& v);

inline bool isLWS(int c)
{
    return c == '\n' || c == '\r' || c == '\t' || c == ' ';
}
void trimLWS(std::string& v);
void trimQuoted(std::string& v);

bool isTokenChar(int c);
bool isValidToken(const char* start, const char* end);

const char* skipSpace(const char* start, const char* const end);
const char* skipTo(const char* start, const char* const end, const char delim = ',');
const char* parseHexDigits(const char* start, const char* const end, unsigned long long& hex);
const char* parseDigits(const char* start, const char* const end, unsigned long long& digits);
const char* parseDigits(const char* start, const char* const end, unsigned& digits);
const char* parseDigits(const char* start, const char* const end, int& digits);
const char* parseMonth(const char* start, const char* const end, int& m);
const char* parseTime(const char* start, const char* const end, unsigned& t);
// parseCRLF(): if start begins with CRLF, the result points to LF.
const char* parseCRLF(const char* start, const char* const end);

}  // http

}}}}  // org::w3c::dom::bootstrap

#endif  // ES_HTTP_UTIL_H