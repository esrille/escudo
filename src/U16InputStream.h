/*
 * Copyright 2010-2013 Esrille Inc.
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

#include <sstream>
#include <string>

struct UConverter;

class U16InputStream
{
public:
    virtual ~U16InputStream() {}

    virtual explicit operator bool( ) const = 0;
    virtual bool operator!() const = 0;
    virtual int peek() = 0;
    virtual U16InputStream& get(char16_t& c) = 0;

    int get() {
        char16_t c;
        get(c);
        return !*this ? -1 : c;
    }
    operator std::u16string()
    {
        std::u16string text;
        char16_t c;
        while (get(c))
            text += c;
        return text;
    }
};

class U16TrivialInputStream : public U16InputStream
{
    std::basic_istream<char16_t>& stream;

public:
    U16TrivialInputStream(std::basic_istream<char16_t>& stream) :
        stream(stream)
    {}
    virtual explicit operator bool( ) const {
        return stream;
    }
    virtual bool operator! () const {
        return !stream;
    }
    virtual int peek() {
        return stream.peek();
    }
    virtual U16TrivialInputStream& get(char16_t& c) {
        stream.get(c);
        return *this;
    }
};

class U16ConverterInputStream : public U16InputStream
{
public:
    static const size_t ChunkSize = 512;
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
    void setEncoding(std::string value, bool useDefault);

    virtual bool detect(const char* p);
    std::string beToAscii(const char* p) {
        std::string s;
        while (p < sourceBuffer + ChunkSize) {
            if (*p++)
                return "";
            char u = *p++;
            if (!u)
                return "";
            s += u;
            if (u == ';')
                break;
        }
        return s;
    }
    std::string leToAscii(const char* p) {
        std::string s;
        while (p < sourceBuffer + ChunkSize) {
            char u = *p++;
            if (!u)
                return "";
            if (*p++)
                return "";
            s += u;
            if (u == ';')
                break;
        }
        return s;
    }

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
    U16ConverterInputStream(std::istream& stream, const std::string& optionalEncoding = "");
    virtual ~U16ConverterInputStream();

    virtual explicit operator bool( ) const {
        return !eof;
    }
    virtual bool operator! () const {
        return eof;
    }
    virtual int peek() {
        int c;
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
                return c;
            }
            if (!flush)
                readChunk();
            else
                eof = true;
        }
        return -1;
    }
    virtual U16ConverterInputStream& get(char16_t& c)
    {
        int ch = peek();
        if (!eof) {
            lastChar = *nextChar;
            ++nextChar;
            c = static_cast<char16_t>(ch);
        }
        return *this;
    }

    enum Confidence getConfidence() const {
        return confidence;
    }

    const std::string& getEncoding() {
        if (!converter)
            peek();
        return encoding;
    }

    static std::string checkEncoding(std::string value);
};

#endif  // ES_U16INPUTSTREAM_H
