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

#ifndef ES_HTMLPARSER_H
#define ES_HTMLPARSER_H

#include <initializer_list>
#include <deque>
#include <list>
#include <string>

#include <Object.h>
#include <org/w3c/dom/DOMImplementation.h>
#include <org/w3c/dom/Element.h>
#include <org/w3c/dom/Document.h>

#include "HTMLTokenizer.h"

using namespace org::w3c::dom;

class HTMLParser
{
    Element insertHtmlElement(Token& token);
    Element insertHtmlElement(const std::u16string& name);
    void insertCharacter(Node node, const std::u16string& data);
    void insertCharacter(const std::u16string& data);
    void insertCharacter(Token& token);
    void parseRawtext(Token& token, HTMLTokenizer::State* state);
    void generateImpliedEndTags(std::u16string exclude = u"");

    class InsertionMode
    {
    public:
        virtual bool processEOF(HTMLParser* parser, Token& token) = 0;
        virtual bool processComment(HTMLParser* parser, Token& token) = 0;
        virtual bool processDoctype(HTMLParser* parser, Token& token) = 0;
        virtual bool processCharacter(HTMLParser* parser, Token& token) = 0;
        virtual bool processStartTag(HTMLParser* parser, Token& token) = 0;
        virtual bool processEndTag(HTMLParser* parser, Token& token) = 0;

        bool processToken(HTMLParser* parser, Token& token)
        {
            bool result;
            switch (token.getType()) {
            case Token::Type::StartTag:
                result = processStartTag(parser, token);
                break;
            case Token::Type::EndTag:
                result = processEndTag(parser, token);
                break;
            case Token::Type::Comment:
                result = processComment(parser, token);
                break;
            case Token::Type::Doctype:
                result = processDoctype(parser, token);
                break;
            case Token::Type::Character:
                result = processCharacter(parser, token);
                break;
            case Token::Type::EndOfFile:
                result = processEOF(parser, token);
                break;
            case Token::Type::ParseError:
            default:
                result = true;
                break;
            }
            return result;
        }
    };

    class Initial : public InsertionMode
    {
        bool anythingElse(HTMLParser* parser, Token& token);
    public:
        virtual bool processEOF(HTMLParser* parser, Token& token);
        virtual bool processComment(HTMLParser* parser, Token& token);
        virtual bool processDoctype(HTMLParser* parser, Token& token);
        virtual bool processCharacter(HTMLParser* parser, Token& token);
        virtual bool processStartTag(HTMLParser* parser, Token& token);
        virtual bool processEndTag(HTMLParser* parser, Token& token);
    };

    class BeforeHtml : public InsertionMode
    {
        bool anythingElse(HTMLParser* parser, Token& token);
        Element insertHtmlElement(HTMLParser* parser);
    public:
        virtual bool processEOF(HTMLParser* parser, Token& token);
        virtual bool processComment(HTMLParser* parser, Token& token);
        virtual bool processDoctype(HTMLParser* parser, Token& token);
        virtual bool processCharacter(HTMLParser* parser, Token& token);
        virtual bool processStartTag(HTMLParser* parser, Token& token);
        virtual bool processEndTag(HTMLParser* parser, Token& token);
    };

    class BeforeHead : public InsertionMode
    {
        bool anythingElse(HTMLParser* parser, Token& token);
    public:
        virtual bool processEOF(HTMLParser* parser, Token& token);
        virtual bool processComment(HTMLParser* parser, Token& token);
        virtual bool processDoctype(HTMLParser* parser, Token& token);
        virtual bool processCharacter(HTMLParser* parser, Token& token);
        virtual bool processStartTag(HTMLParser* parser, Token& token);
        virtual bool processEndTag(HTMLParser* parser, Token& token);
    };

    class InHead : public InsertionMode
    {
        bool anythingElse(HTMLParser* parser, Token& token);
    public:
        virtual bool processEOF(HTMLParser* parser, Token& token);
        virtual bool processComment(HTMLParser* parser, Token& token);
        virtual bool processDoctype(HTMLParser* parser, Token& token);
        virtual bool processCharacter(HTMLParser* parser, Token& token);
        virtual bool processStartTag(HTMLParser* parser, Token& token);
        virtual bool processEndTag(HTMLParser* parser, Token& token);
    };

    class InHeadNoscript : public InsertionMode
    {
        bool anythingElse(HTMLParser* parser, Token& token);
    public:
        virtual bool processEOF(HTMLParser* parser, Token& token);
        virtual bool processComment(HTMLParser* parser, Token& token);
        virtual bool processDoctype(HTMLParser* parser, Token& token);
        virtual bool processCharacter(HTMLParser* parser, Token& token);
        virtual bool processStartTag(HTMLParser* parser, Token& token);
        virtual bool processEndTag(HTMLParser* parser, Token& token);
    };

    class AfterHead : public InsertionMode
    {
        bool anythingElse(HTMLParser* parser, Token& token);
    public:
        virtual bool processEOF(HTMLParser* parser, Token& token);
        virtual bool processComment(HTMLParser* parser, Token& token);
        virtual bool processDoctype(HTMLParser* parser, Token& token);
        virtual bool processCharacter(HTMLParser* parser, Token& token);
        virtual bool processStartTag(HTMLParser* parser, Token& token);
        virtual bool processEndTag(HTMLParser* parser, Token& token);
    };

    class InBody : public InsertionMode
    {
        bool processAnyOtherEndTag(HTMLParser* parser, Token& token);
    public:
        virtual bool processEOF(HTMLParser* parser, Token& token);
        virtual bool processComment(HTMLParser* parser, Token& token);
        virtual bool processDoctype(HTMLParser* parser, Token& token);
        virtual bool processCharacter(HTMLParser* parser, Token& token);
        virtual bool processStartTag(HTMLParser* parser, Token& token);
        virtual bool processEndTag(HTMLParser* parser, Token& token);
    };

    class Text : public InsertionMode
    {
    public:
        virtual bool processEOF(HTMLParser* parser, Token& token);
        virtual bool processComment(HTMLParser* parser, Token& token);
        virtual bool processDoctype(HTMLParser* parser, Token& token);
        virtual bool processCharacter(HTMLParser* parser, Token& token);
        virtual bool processStartTag(HTMLParser* parser, Token& token);
        virtual bool processEndTag(HTMLParser* parser, Token& token);
    };

    class InTable : public InsertionMode
    {
        void clearStackBackToTableContext(HTMLParser* parser);
    public:
        bool anythingElse(HTMLParser* parser, Token& token);  // TODO: should be private. just for InTableText
        virtual bool processEOF(HTMLParser* parser, Token& token);
        virtual bool processComment(HTMLParser* parser, Token& token);
        virtual bool processDoctype(HTMLParser* parser, Token& token);
        virtual bool processCharacter(HTMLParser* parser, Token& token);
        virtual bool processStartTag(HTMLParser* parser, Token& token);
        virtual bool processEndTag(HTMLParser* parser, Token& token);
    };

    class InTableText : public InsertionMode
    {
        bool anythingElse(HTMLParser* parser, Token& token);
    public:
        virtual bool processEOF(HTMLParser* parser, Token& token);
        virtual bool processComment(HTMLParser* parser, Token& token);
        virtual bool processDoctype(HTMLParser* parser, Token& token);
        virtual bool processCharacter(HTMLParser* parser, Token& token);
        virtual bool processStartTag(HTMLParser* parser, Token& token);
        virtual bool processEndTag(HTMLParser* parser, Token& token);
    };

    class InCaption : public InsertionMode
    {
        bool anythingElse(HTMLParser* parser, Token& token);
    public:
        virtual bool processEOF(HTMLParser* parser, Token& token);
        virtual bool processComment(HTMLParser* parser, Token& token);
        virtual bool processDoctype(HTMLParser* parser, Token& token);
        virtual bool processCharacter(HTMLParser* parser, Token& token);
        virtual bool processStartTag(HTMLParser* parser, Token& token);
        virtual bool processEndTag(HTMLParser* parser, Token& token);
    };

    class InColumnGroup : public InsertionMode
    {
        bool anythingElse(HTMLParser* parser, Token& token);
    public:
        virtual bool processEOF(HTMLParser* parser, Token& token);
        virtual bool processComment(HTMLParser* parser, Token& token);
        virtual bool processDoctype(HTMLParser* parser, Token& token);
        virtual bool processCharacter(HTMLParser* parser, Token& token);
        virtual bool processStartTag(HTMLParser* parser, Token& token);
        virtual bool processEndTag(HTMLParser* parser, Token& token);
    };

    class InTableBody : public InsertionMode
    {
        void clearStackBackToTableBodyContext(HTMLParser* parser);
        bool anythingElse(HTMLParser* parser, Token& token);
    public:
        virtual bool processEOF(HTMLParser* parser, Token& token);
        virtual bool processComment(HTMLParser* parser, Token& token);
        virtual bool processDoctype(HTMLParser* parser, Token& token);
        virtual bool processCharacter(HTMLParser* parser, Token& token);
        virtual bool processStartTag(HTMLParser* parser, Token& token);
        virtual bool processEndTag(HTMLParser* parser, Token& token);
    };

    class InRow : public InsertionMode
    {
        void clearStackBackToTableRowContext(HTMLParser* parser);
        bool anythingElse(HTMLParser* parser, Token& token);
    public:
        virtual bool processEOF(HTMLParser* parser, Token& token);
        virtual bool processComment(HTMLParser* parser, Token& token);
        virtual bool processDoctype(HTMLParser* parser, Token& token);
        virtual bool processCharacter(HTMLParser* parser, Token& token);
        virtual bool processStartTag(HTMLParser* parser, Token& token);
        virtual bool processEndTag(HTMLParser* parser, Token& token);
    };

    class InCell : public InsertionMode
    {
        void closeCell(HTMLParser* parser);
        bool anythingElse(HTMLParser* parser, Token& token);
    public:
        virtual bool processEOF(HTMLParser* parser, Token& token);
        virtual bool processComment(HTMLParser* parser, Token& token);
        virtual bool processDoctype(HTMLParser* parser, Token& token);
        virtual bool processCharacter(HTMLParser* parser, Token& token);
        virtual bool processStartTag(HTMLParser* parser, Token& token);
        virtual bool processEndTag(HTMLParser* parser, Token& token);
    };

    class InSelect : public InsertionMode
    {
        bool anythingElse(HTMLParser* parser, Token& token);
    public:
        virtual bool processEOF(HTMLParser* parser, Token& token);
        virtual bool processComment(HTMLParser* parser, Token& token);
        virtual bool processDoctype(HTMLParser* parser, Token& token);
        virtual bool processCharacter(HTMLParser* parser, Token& token);
        virtual bool processStartTag(HTMLParser* parser, Token& token);
        virtual bool processEndTag(HTMLParser* parser, Token& token);
    };

    class InSelectInTable : public InsertionMode
    {
        bool anythingElse(HTMLParser* parser, Token& token);
    public:
        virtual bool processEOF(HTMLParser* parser, Token& token);
        virtual bool processComment(HTMLParser* parser, Token& token);
        virtual bool processDoctype(HTMLParser* parser, Token& token);
        virtual bool processCharacter(HTMLParser* parser, Token& token);
        virtual bool processStartTag(HTMLParser* parser, Token& token);
        virtual bool processEndTag(HTMLParser* parser, Token& token);
    };

    class InForeignContent : public InsertionMode
    {
    public:
        virtual bool processEOF(HTMLParser* parser, Token& token);
        virtual bool processComment(HTMLParser* parser, Token& token);
        virtual bool processDoctype(HTMLParser* parser, Token& token);
        virtual bool processCharacter(HTMLParser* parser, Token& token);
        virtual bool processStartTag(HTMLParser* parser, Token& token);
        virtual bool processEndTag(HTMLParser* parser, Token& token);
    };

    class AfterBody : public InsertionMode
    {
        bool anythingElse(HTMLParser* parser, Token& token);
    public:
        virtual bool processEOF(HTMLParser* parser, Token& token);
        virtual bool processComment(HTMLParser* parser, Token& token);
        virtual bool processDoctype(HTMLParser* parser, Token& token);
        virtual bool processCharacter(HTMLParser* parser, Token& token);
        virtual bool processStartTag(HTMLParser* parser, Token& token);
        virtual bool processEndTag(HTMLParser* parser, Token& token);
    };

    class InFrameset : public InsertionMode
    {
        bool anythingElse(HTMLParser* parser, Token& token);
    public:
        virtual bool processEOF(HTMLParser* parser, Token& token);
        virtual bool processComment(HTMLParser* parser, Token& token);
        virtual bool processDoctype(HTMLParser* parser, Token& token);
        virtual bool processCharacter(HTMLParser* parser, Token& token);
        virtual bool processStartTag(HTMLParser* parser, Token& token);
        virtual bool processEndTag(HTMLParser* parser, Token& token);
    };

    class AfterFrameset : public InsertionMode
    {
        bool anythingElse(HTMLParser* parser, Token& token);
    public:
        virtual bool processEOF(HTMLParser* parser, Token& token);
        virtual bool processComment(HTMLParser* parser, Token& token);
        virtual bool processDoctype(HTMLParser* parser, Token& token);
        virtual bool processCharacter(HTMLParser* parser, Token& token);
        virtual bool processStartTag(HTMLParser* parser, Token& token);
        virtual bool processEndTag(HTMLParser* parser, Token& token);
    };

    class AfterAfterBody : public InsertionMode
    {
        bool anythingElse(HTMLParser* parser, Token& token);
    public:
        virtual bool processEOF(HTMLParser* parser, Token& token);
        virtual bool processComment(HTMLParser* parser, Token& token);
        virtual bool processDoctype(HTMLParser* parser, Token& token);
        virtual bool processCharacter(HTMLParser* parser, Token& token);
        virtual bool processStartTag(HTMLParser* parser, Token& token);
        virtual bool processEndTag(HTMLParser* parser, Token& token);
    };

    class AfterAfterFrameset : public InsertionMode
    {
        bool anythingElse(HTMLParser* parser, Token& token);
    public:
        virtual bool processEOF(HTMLParser* parser, Token& token);
        virtual bool processComment(HTMLParser* parser, Token& token);
        virtual bool processDoctype(HTMLParser* parser, Token& token);
        virtual bool processCharacter(HTMLParser* parser, Token& token);
        virtual bool processStartTag(HTMLParser* parser, Token& token);
        virtual bool processEndTag(HTMLParser* parser, Token& token);
    };

    class InBinding: public InsertionMode
    {
        void closeBinding(HTMLParser* parser);
        bool anythingElse(HTMLParser* parser, Token& token);
    public:
        virtual bool processEOF(HTMLParser* parser, Token& token);
        virtual bool processComment(HTMLParser* parser, Token& token);
        virtual bool processDoctype(HTMLParser* parser, Token& token);
        virtual bool processCharacter(HTMLParser* parser, Token& token);
        virtual bool processStartTag(HTMLParser* parser, Token& token);
        virtual bool processEndTag(HTMLParser* parser, Token& token);
    };

    static Initial initial;
    static BeforeHtml beforeHtml;
    static BeforeHead beforeHead;
    static InHead inHead;
    static InHeadNoscript inHeadNoscript;
    static AfterHead afterHead;
    static InBody inBody;
    static Text text;
    static InTable inTable;
    static InTableText inTableText;
    static InCaption inCaption;
    static InColumnGroup inColumnGroup;
    static InTableBody inTableBody;
    static InRow inRow;
    static InCell inCell;
    static InSelect inSelect;
    static InSelectInTable inSelectInTable;
    static InForeignContent inForeignContent;
    static AfterBody afterBody;
    static InFrameset inFrameset;
    static AfterFrameset afterFrameset;
    static AfterAfterBody afterAfterBody;
    static AfterAfterFrameset afterAfterFrameset;

    static InBinding inBinding;

    Document document;
    HTMLTokenizer* tokenizer;

    InsertionMode* insertionMode;
    InsertionMode* originalInsertionMode;
    InsertionMode* secondaryInsertionMode;

    // 10.2.1 Overview of the parsing model
    int scriptNestingLevel;
    bool pauseFlag;

    void parseError(const char* message = "");

    void setInsertionMode(InsertionMode* mode)
    {
        insertionMode = mode;
    }

    // Switch to the new mode, then reprocess the current token
    bool setInsertionMode(InsertionMode* mode, Token& token)
    {
        setInsertionMode(mode);
        return processToken(token);
    }

    void resetInsertionMode();

    bool processToken(Token& token);

    //
    // open element stack
    //
    std::deque<Element> openElementStack;

    Element currentNode() {
        return openElementStack.back();
    }
    Element currentTable();
    Element pushElement(Element element) {
        if (element)
            openElementStack.push_back(element);
        return element;
    }
    Element popElement() {
        assert(!openElementStack.empty());
        if (openElementStack.empty())
            return 0;
        Element top = openElementStack.back();
        openElementStack.pop_back();
        return top;
    }
    Element removeOpenElement(Element element);
    bool elementInSpecificScope(Element target, const char16_t** const list, size_t size, bool except = false);
    bool elementInSpecificScope(const std::u16string& tagName, const char16_t** const list, size_t size, bool excpet = false);
    bool elementInSpecificScope(std::initializer_list<const char16_t*> targets, const char16_t** const list, size_t size, bool except = false);
    template <typename T>
    bool elementInScope(T target);
    template <typename T>
    bool elementInListItemScope(T target);
    template <typename T>
    bool elementInButtonScope(T target);
    template <typename T>
    bool elementInTableScope(T target);
    template <typename T>
    bool elementInSelectScope(T target);

    //
    // active formatting elements
    //
    std::list<Element> activeFormattingElements;

    void reconstructActiveFormattingElements();
    void clearActiveFormattingElements();
    std::list<Element>::iterator elementInActiveFormattingElements(const std::u16string name);
    Element addFormattingElement(Element element) {
        activeFormattingElements.push_back(element);
        return element;
    }

    void fosterNode(Node node);

    std::u16string pendingTableCharacters;
    bool spaceInPendingTableCharacters;

    Element headElement;
    Element formElement;

    bool scriptingFlag;
    bool framesetOkFlag;
    bool insertFromTable;

    bool innerHTML;   // true if this parser was originally created as part of the HTML fragment parsing algorithm

    bool enableXBL;   // true if parse elements defined in XBL 2.0.

    bool stopParsing();

public:
    HTMLParser(Document document, HTMLTokenizer* tokenizer, bool enableXBL = true);
    bool mainLoop();
};

#endif  // ES_HTMLPARSER_H
