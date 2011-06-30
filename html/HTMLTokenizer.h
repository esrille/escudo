/*
 * Copyright 2010, 2011 Esrille Inc.
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

#ifndef ES_HTMLTOKENIZER_H
#define ES_HTMLTOKENIZER_H

#include <Object.h>
#include <org/w3c/dom/Attr.h>

#include <deque>
#include <iostream>
#include <queue>
#include <set>
#include <stack>
#include <string>

#include "HTMLInputStream.h"

class Attribute
{
    std::u16string name;
    std::u16string value;

public:
    Attribute()
    {
    }

    Attribute(int ch);

    void append(int ch);
    void appendValue(int ch);

    const std::u16string& getName() const
    {
        return name;
    }

    const std::u16string& getValue() const
    {
        return value;
    }

    void clear()
    {
        name.clear();
        value.clear();
    }
};

class Token
{
    typedef org::w3c::dom::Attr Attr;

public:
    enum class Type
    {
        StartTag,
        EndTag,
        Comment,
        Doctype,
        ParseError,
        Character,
        EndOfFile
    };

    enum Flag
    {
        SelfClosing = 0x01,
        ForceQuirks = 0x02,
        HasPublicId = 0x04,
        HasSystemId = 0x08,
        SelfClosingAcknowledged = 0x10
    };

private:
    Type type;
    unsigned flags;

    // Character field
    int ucode;

    // name or data for Comment and Doctype
    std::u16string name;

    // StartTag/EndTag field
    std::set<std::u16string> attrNames;
    std::deque<Attr> attrList;

    // Doctype fields
    std::u16string publicId;
    std::u16string systemId;

public:
    Token(Type type = Type::EndOfFile) :
        type(type),
        flags(0),
        ucode(0)
    {
    }

    Token(int ucode);
    Token(Type type, int ch);
    Token(Type type, const std::u16string& name);

    void append(int ch);
    bool append(Attribute& attribute);

    const std::u16string& getName() const
    {
        return name;
    }

    void setName(std::u16string name)
    {
        this->name = name;
    }

    const std::deque<Attr>& getAttributes() const
    {
        return attrList;
    }

    Nullable<std::u16string> getAttribute(std::u16string name) const;

    bool hasPublicId() const
    {
        return flags & Flag::HasPublicId;
    }

    std::u16string& getPublicId()
    {
        return publicId;
    }

    const std::u16string& getPublicId() const
    {
        return publicId;
    }

    void setPublicId(const std::u16string s)
    {
        setFlag(Flag::HasPublicId);
        publicId = s;
    }

    bool hasSystemId() const
    {
        return flags & Flag::HasSystemId;
    }

    std::u16string& getSystemId()
    {
        return systemId;
    }

    const std::u16string& getSystemId() const
    {
        return systemId;
    }

    void setSystemId(const std::u16string s)
    {
        setFlag(Flag::HasSystemId);
        systemId = s;
    }

    Type getType() const
    {
        return type;
    }

    void setType(Type type)
    {
        this->type = type;
    }

    void setFlag(unsigned flag)
    {
        flags |= flag;
    }

    unsigned getFlags() const
    {
        return flags;
    }

    int getChar() const
    {
        return ucode;
    }

    void acknowledge()
    {
        if (flags & Flag::SelfClosing)
            flags |= SelfClosingAcknowledged;
    }
};

class HTMLTokenizer
{
    class State
    {
    public:
        // Return true if a new token is emitted.
        virtual bool consume(HTMLTokenizer* tokenize, int ch)
        {
            return false;
        }
    };

    class DataState : public State
    {
    public:
        bool consume(HTMLTokenizer* tokenizer, int ch);
    };

    class RcdataState : public State
    {
    public:
        bool consume(HTMLTokenizer* tokenizer, int ch);
    };

    class RawtextState : public State
    {
    public:
        bool consume(HTMLTokenizer* tokenizer, int ch);
    };

    class ScriptDataState : public State
    {
    public:
        bool consume(HTMLTokenizer* tokenizer, int ch);
    };

    class PlaintextState : public State
    {
    public:
        bool consume(HTMLTokenizer* tokenizer, int ch);
    };

    class TagOpenState : public State
    {
    public:
        bool consume(HTMLTokenizer* tokenizer, int ch);
    };

    class EndTagOpenState : public State
    {
    public:
        bool consume(HTMLTokenizer* tokenizer, int ch);
    };

    class TagNameState : public State
    {
    public:
        bool consume(HTMLTokenizer* tokenizer, int ch);
    };

    class RcdataLessThanSignState : public State
    {
    public:
        bool consume(HTMLTokenizer* tokenizer, int ch);
    };

    class RcdataEndTagOpenState : public State
    {
    public:
        bool consume(HTMLTokenizer* tokenizer, int ch);
    };

    class RcdataEndTagNameState : public State
    {
    public:
        bool consume(HTMLTokenizer* tokenizer, int ch);
    };

    class RawtextLessThanSignState : public State
    {
    public:
        bool consume(HTMLTokenizer* tokenizer, int ch);
    };

    class RawtextEndTagOpenState : public State
    {
    public:
        bool consume(HTMLTokenizer* tokenizer, int ch);
    };

    class RawtextEndTagNameState : public State
    {
    public:
        bool consume(HTMLTokenizer* tokenizer, int ch);
    };

    class ScriptDataLessThanSignState : public State
    {
    public:
        bool consume(HTMLTokenizer* tokenizer, int ch);
    };

    class ScriptDataEndTagOpenState : public State
    {
    public:
        bool consume(HTMLTokenizer* tokenizer, int ch);
    };

    class ScriptDataEndTagNameState : public State
    {
    public:
        bool consume(HTMLTokenizer* tokenizer, int ch);
    };

    class ScriptDataEscapeStartState : public State
    {
    public:
        bool consume(HTMLTokenizer* tokenizer, int ch);
    };

    class ScriptDataEscapeStartDashState : public State
    {
    public:
        bool consume(HTMLTokenizer* tokenizer, int ch);
    };

    class ScriptDataEscapedState : public State
    {
    public:
        bool consume(HTMLTokenizer* tokenizer, int ch);
    };

    class ScriptDataEscapedDashState : public State
    {
    public:
        bool consume(HTMLTokenizer* tokenizer, int ch);
    };

    class ScriptDataEscapedDashDashState : public State
    {
    public:
        bool consume(HTMLTokenizer* tokenizer, int ch);
    };

    class ScriptDataEscapedLessThanSignState : public State
    {
    public:
        bool consume(HTMLTokenizer* tokenizer, int ch);
    };

    class ScriptDataEscapedEndTagOpenState : public State
    {
    public:
        bool consume(HTMLTokenizer* tokenizer, int ch);
    };

    class ScriptDataEscapedEndTagNameState : public State
    {
    public:
        bool consume(HTMLTokenizer* tokenizer, int ch);
    };

    class ScriptDataDoubleEscapeStartState : public State
    {
    public:
        bool consume(HTMLTokenizer* tokenizer, int ch);
    };

    class ScriptDataDoubleEscapedState : public State
    {
    public:
        bool consume(HTMLTokenizer* tokenizer, int ch);
    };

    class ScriptDataDoubleEscapedDashState : public State
    {
    public:
        bool consume(HTMLTokenizer* tokenizer, int ch);
    };

    class ScriptDataDoubleEscapedDashDashState : public State
    {
    public:
        bool consume(HTMLTokenizer* tokenizer, int ch);
    };

    class ScriptDataDoubleEscapedLessThanSignState : public State
    {
    public:
        bool consume(HTMLTokenizer* tokenizer, int ch);
    };

    class ScriptDataDoubleEscapeEndState : public State
    {
    public:
        bool consume(HTMLTokenizer* tokenizer, int ch);
    };

    class BeforeAttributeNameState : public State
    {
    public:
        bool consume(HTMLTokenizer* tokenizer, int ch);
    };

    class AttributeNameState : public State
    {
    public:
        bool consume(HTMLTokenizer* tokenizer, int ch);
    };

    class AfterAttributeNameState : public State
    {
    public:
        bool consume(HTMLTokenizer* tokenizer, int ch);
    };

    class BeforeAttributeValueState : public State
    {
    public:
        bool consume(HTMLTokenizer* tokenizer, int ch);
    };

    class AttributeValueDoubleQuotedState : public State
    {
    public:
        bool consume(HTMLTokenizer* tokenizer, int ch);
    };

    class AttributeValueSingleQuotedState : public State
    {
    public:
        bool consume(HTMLTokenizer* tokenizer, int ch);
    };

    class AttributeValueUnquotedState : public State
    {
    public:
        bool consume(HTMLTokenizer* tokenizer, int ch);
    };

    class AfterAttributeValueQuotedState : public State
    {
    public:
        bool consume(HTMLTokenizer* tokenizer, int ch);
    };

    class SelfClosingStartTagState : public State
    {
    public:
        bool consume(HTMLTokenizer* tokenizer, int ch);
    };

    class BogusCommentState : public State
    {
    public:
        bool consume(HTMLTokenizer* tokenizer, int ch);
    };

    class MarkupDeclarationOpenState : public State
    {
    public:
        bool consume(HTMLTokenizer* tokenizer, int ch);
    };

    class CommentStartState : public State
    {
    public:
        bool consume(HTMLTokenizer* tokenizer, int ch);
    };

    class CommentStartDashState : public State
    {
    public:
        bool consume(HTMLTokenizer* tokenizer, int ch);
    };

    class CommentState : public State
    {
    public:
        bool consume(HTMLTokenizer* tokenizer, int ch);
    };

    class CommentEndDashState : public State
    {
    public:
        bool consume(HTMLTokenizer* tokenizer, int ch);
    };

    class CommentEndState : public State
    {
    public:
        bool consume(HTMLTokenizer* tokenizer, int ch);
    };

    class CommentEndBangState : public State
    {
    public:
        bool consume(HTMLTokenizer* tokenizer, int ch);
    };

    class CommentEndSpaceState : public State
    {
    public:
        bool consume(HTMLTokenizer* tokenizer, int ch);
    };

    class DoctypeState : public State
    {
    public:
        bool consume(HTMLTokenizer* tokenizer, int ch);
    };

    class BeforeDoctypeNameState : public State
    {
    public:
        bool consume(HTMLTokenizer* tokenizer, int ch);
    };

    class DoctypeNameState : public State
    {
    public:
        bool consume(HTMLTokenizer* tokenizer, int ch);
    };

    class AfterDoctypeNameState : public State
    {
    public:
        bool consume(HTMLTokenizer* tokenizer, int ch);
    };

    class AfterDoctypePublicKeywordState : public State
    {
    public:
        bool consume(HTMLTokenizer* tokenizer, int ch);
    };

    class BeforeDoctypePublicIdentifierState : public State
    {
    public:
        bool consume(HTMLTokenizer* tokenizer, int ch);
    };

    class DoctypePublicIdentifierDoubleQuotedState : public State
    {
    public:
        bool consume(HTMLTokenizer* tokenizer, int ch);
    };

    class DoctypePublicIdentifierSingleQuotedState : public State
    {
    public:
        bool consume(HTMLTokenizer* tokenizer, int ch);
    };

    class AfterDoctypePublicIdentifierState : public State
    {
    public:
        bool consume(HTMLTokenizer* tokenizer, int ch);
    };

    class BetweenDoctypePublicAndSystemIdentifiersState : public State
    {
    public:
        bool consume(HTMLTokenizer* tokenizer, int ch);
    };

    class AfterDoctypeSystemKeywordState : public State
    {
    public:
        bool consume(HTMLTokenizer* tokenizer, int ch);
    };

    class BeforeDoctypeSystemIdentifierState : public State
    {
    public:
        bool consume(HTMLTokenizer* tokenizer, int ch);
    };

    class DoctypeSystemIdentifierDoubleQuotedState : public State
    {
    public:
        bool consume(HTMLTokenizer* tokenizer, int ch);
    };

    class DoctypeSystemIdentifierSingleQuotedState : public State
    {
    public:
        bool consume(HTMLTokenizer* tokenizer, int ch);
    };

    class AfterDoctypeSystemIdentifierState : public State
    {
    public:
        bool consume(HTMLTokenizer* tokenizer, int ch);
    };

    class BogusDoctypeState : public State
    {
    public:
        bool consume(HTMLTokenizer* tokenizer, int ch);
    };

    class CdataSectionState : public State
    {
    public:
        bool consume(HTMLTokenizer* tokenizer, int ch);
    };

    static DataState dataState;
    static RcdataState rcdataState;
    static RawtextState rawtextState;
    static ScriptDataState scriptDataState;
    static PlaintextState plaintextState;
    static TagOpenState tagOpenState;
    static EndTagOpenState endTagOpenState;
    static TagNameState tagNameState;
    static RcdataLessThanSignState rcdataLessThanSignState;
    static RcdataEndTagOpenState rcdataEndTagOpenState;
    static RcdataEndTagNameState rcdataEndTagNameState;
    static RawtextLessThanSignState rawtextLessThanSignState;
    static RawtextEndTagOpenState rawtextEndTagOpenState;
    static RawtextEndTagNameState rawtextEndTagNameState;
    static ScriptDataLessThanSignState scriptDataLessThanSignState;
    static ScriptDataEndTagOpenState scriptDataEndTagOpenState;
    static ScriptDataEndTagNameState scriptDataEndTagNameState;
    static ScriptDataEscapeStartState scriptDataEscapeStartState;
    static ScriptDataEscapeStartDashState scriptDataEscapeStartDashState;
    static ScriptDataEscapedState scriptDataEscapedState;
    static ScriptDataEscapedDashState scriptDataEscapedDashState;
    static ScriptDataEscapedDashDashState scriptDataEscapedDashDashState;
    static ScriptDataEscapedLessThanSignState scriptDataEscapedLessThanSignState;
    static ScriptDataEscapedEndTagOpenState scriptDataEscapedEndTagOpenState;
    static ScriptDataEscapedEndTagNameState scriptDataEscapedEndTagNameState;
    static ScriptDataDoubleEscapeStartState scriptDataDoubleEscapeStartState;
    static ScriptDataDoubleEscapedState scriptDataDoubleEscapedState;
    static ScriptDataDoubleEscapedDashState scriptDataDoubleEscapedDashState;
    static ScriptDataDoubleEscapedDashDashState scriptDataDoubleEscapedDashDashState;
    static ScriptDataDoubleEscapedLessThanSignState scriptDataDoubleEscapedLessThanSignState;
    static ScriptDataDoubleEscapeEndState scriptDataDoubleEscapeEndState;
    static BeforeAttributeNameState beforeAttributeNameState;
    static AttributeNameState attributeNameState;
    static AfterAttributeNameState afterAttributeNameState;
    static BeforeAttributeValueState beforeAttributeValueState;
    static AttributeValueDoubleQuotedState attributeValueDoubleQuotedState;
    static AttributeValueSingleQuotedState attributeValueSingleQuotedState;
    static AttributeValueUnquotedState attributeValueUnquotedState;
    static AfterAttributeValueQuotedState afterAttributeValueQuotedState;
    static SelfClosingStartTagState selfClosingStartTagState;
    static BogusCommentState bogusCommentState;
    static MarkupDeclarationOpenState markupDeclarationOpenState;
    static CommentStartState commentStartState;
    static CommentStartDashState commentStartDashState;
    static CommentState commentState;
    static CommentEndDashState commentEndDashState;
    static CommentEndState commentEndState;
    static CommentEndBangState commentEndBangState;
    static CommentEndSpaceState commentEndSpaceState;
    static DoctypeState doctypeState;
    static BeforeDoctypeNameState beforeDoctypeNameState;
    static DoctypeNameState doctypeNameState;
    static AfterDoctypeNameState afterDoctypeNameState;
    static AfterDoctypePublicKeywordState afterDoctypePublicKeywordState;
    static BeforeDoctypePublicIdentifierState beforeDoctypePublicIdentifierState;
    static DoctypePublicIdentifierDoubleQuotedState doctypePublicIdentifierDoubleQuotedState;
    static DoctypePublicIdentifierSingleQuotedState doctypePublicIdentifierSingleQuotedState;
    static AfterDoctypePublicIdentifierState afterDoctypePublicIdentifierState;
    static BetweenDoctypePublicAndSystemIdentifiersState betweenDoctypePublicAndSystemIdentifiersState;
    static AfterDoctypeSystemKeywordState afterDoctypeSystemKeywordState;
    static BeforeDoctypeSystemIdentifierState beforeDoctypeSystemIdentifierState;
    static DoctypeSystemIdentifierDoubleQuotedState doctypeSystemIdentifierDoubleQuotedState;
    static DoctypeSystemIdentifierSingleQuotedState doctypeSystemIdentifierSingleQuotedState;
    static AfterDoctypeSystemIdentifierState afterDoctypeSystemIdentifierState;
    static BogusDoctypeState bogusDoctypeState;
    static CdataSectionState cdataSectionState;

    Token currentToken;
    Attribute currentAttribute;
    std::u16string temporaryBuffer;
    std::u16string appropriateTagName;

    HTMLInputStream* stream;
    bool fromAttribute;
    State* state;
    std::stack<char16_t> charStack;

    std::queue<Token> tokenQueue;

    char32_t replaceCharacter(char32_t number);
    int consumeCharacterReference(int additionalAllowedCharacter = EOF);

    void ungetChar(int ch)
    {
        if (ch != EOF)
            charStack.push(ch);
    }

    void ungetString(const std::string& s)
    {
        std::string::const_reverse_iterator i;
        for (i = s.rbegin(); i < s.rend(); ++i)
            ungetChar(*i);
    }

    int getChar()
    {
        if (!charStack.empty()) {
            char16_t ch = charStack.top();
            charStack.pop();
            return ch;
        }
        return stream->getChar();
    }

    int peekChar()
    {
        if (!charStack.empty())
            return charStack.top();
        return stream->peekChar();
    }

    void setState(State* state)
    {
        this->state = state;
    }

    // Set state and reconsume the character in the new state
    void setState(State* state, int ch)
    {
        this->state = state;
        state->consume(this, ch);
    }

    void outputString(const std::u16string& s);

    void parseError();

    bool emit(int ch);
    bool emit(const std::u16string& s);
    bool emit(const Token& tag);

    bool isAppropriate(const std::u16string& name) {
        return name == appropriateTagName;
    }

public:
    HTMLTokenizer(HTMLInputStream* stream) :
        stream(stream),
        fromAttribute(false),
        state(&dataState)
    {
    }

    Token peekToken();
    Token getToken();

    friend class HTMLParser;
};

#endif  // ES_HTMLTOKENIZER_H
