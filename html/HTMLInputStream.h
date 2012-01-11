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

#ifndef ES_HTMLINPUTSTREAM_H
#define ES_HTMLINPUTSTREAM_H

#include "U16InputStream.h"

// cf. 10.2.2 The input stream
class HTMLInputStream : public U16InputStream
{
    static const char* getAttr(const char* p, std::string& name, std::string& value);
    static const char* handleTag(const char* p);
    static std::string handleContentType(const char* p);
    const char* handleMeta(const char* p);
    virtual void detect(const char* p);
public:
    HTMLInputStream(std::istream& stream, const std::string& optionalEncoding = "");
};

#endif  // ES_HTMLINPUTSTREAM_H
