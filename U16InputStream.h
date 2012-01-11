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

#ifndef ES_U16INPUTSTREAM_H
#define ES_U16INPUTSTREAM_H

#include <iostream>
#include <string>

struct UConverter;

class U16InputStream
{
    static const size_t ChunkSize = 512;

public:
    static const char* DefaultEncoding;  // "utf-8"
    enum Confidence
    {
        Certain,
        Tentative,
        Irrelevant
    };

protected:
    enum Confidence confidence;
    std::string encoding;

    static const char* skipSpace(const char* p);
    static const char* skipOver(const char* p, const char* target, size_t length);
    void setEncoding(std::string value);

    virtual void detect(const char* p);

private:
    UConverter* converter;

    std::istream& stream;

    bool eof;
    bool flush;
    char sourceBuffer[ChunkSize + 1];
    char* source;
    char* sourceLimit;
    char16_t targetBuffer[ChunkSize];
    char16_t* target;
    char16_t* nextChar;
    char16_t lastChar;

    void initializeConverter();
    void updateSource();
    void readChunk();

public:
    U16InputStream(std::istream& stream, const std::string& optionalEncoding = "");
    virtual ~U16InputStream();

    enum Confidence getConfidence() const {
        return confidence;
    }

    const std::string& getEncoding()
    {
        if (!converter) {
            char16_t c;
            peekChar(c);
        }
        return encoding;
    }

    operator void* ( ) const
    {
        return eof ? 0 : const_cast<U16InputStream*>(this);
    }

    bool operator! () const
    {
        return eof;
    }

    U16InputStream& peekChar(char16_t& c)
    {
        while (!eof) {
            while (nextChar < target) {
                c = *nextChar;
                if (lastChar == '\r' && c == '\n') {
                    lastChar = '\n';
                    ++nextChar;
                    continue;
                }
                switch (c) {
                case '\r':
                    c = '\n';
                    break;
                case '\0':
                    c = u'\xfffd';
                    break;
                case 0xFEFF:  // BOM
                    ++nextChar;
                    continue;
                    break;
                default:
                    break;
                }
                return *this;
            }
            if (!flush)
                readChunk();
            else
                eof = true;
        }
        return *this;
    }

    U16InputStream& getChar(char16_t& c)
    {
        if (peekChar(c)) {
            lastChar = *nextChar;
            ++nextChar;
        }
        return *this;
    }

    int getChar()
    {
        char16_t c;
        return (getChar(c)) ? c : EOF;
    }

    int peekChar()
    {
        char16_t c;
        return (peekChar(c)) ? c : EOF;
    }

    operator std::u16string();

    static std::string checkEncoding(std::string value);
};

#endif  // ES_U16INPUTSTREAM_H
