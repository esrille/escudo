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

#include "HTMLParser.h"

#include <org/w3c/dom/Comment.h>
#include <org/w3c/dom/DocumentType.h>
#include <org/w3c/dom/Element.h>
#include <org/w3c/dom/Text.h>
#include <org/w3c/dom/html/HTMLDocument.h>
#include <org/w3c/dom/html/HTMLHtmlElement.h>

#include <algorithm>

#include <iostream>   // TODO: only for debugging

#include "utf.h"
#include "css/CSSSerialize.h"
#include "DocumentImp.h"
#include "DOMImplementationImp.h"
#include "ElementImp.h"
#include "HTMLScriptElementImp.h"

using namespace org::w3c::dom::bootstrap;

namespace
{

// TODO: rename this one
bool isSetTo(const std::u16string& s, const std::u16string& t)
{
    if (s.length() != t.length())
        return false;
    std::u16string::const_iterator i;
    std::u16string::const_iterator j;
    for (i = s.begin(), j = t.begin(); i < s.end(); ++i, ++j) {
        if (toLower(*i) != toLower(*j))
            return false;
    }
    return true;
}

bool doesStartWith(const std::u16string& s, const std::u16string& t)
{
    if (s.length() < t.length())
        return false;
    for (int pos = t.length() - 1; 0 < pos; --pos) {
        if (toLower(s[pos]) != toLower(t[pos]))
            return false;
    }
    return true;
}

bool isOneOf(const std::u16string& s, std::initializer_list<const char16_t*> list)
{
    for (auto i = list.begin(); i != list.end(); ++i) {
        if (s.compare(*i) == 0)
            return true;
    }
    return false;
}

bool isOneOf(const std::u16string& s, const char16_t** const list, size_t size)
{
    for (auto i = list; i < list + size; ++i) {
        if (s.compare(*i) == 0)
            return true;
    }
    return false;
}

inline bool isOneOf(const char16_t* t, std::initializer_list<const char16_t*> list)
{
    const std::u16string s(t);
    return isOneOf(s, list);
}

inline bool isOneOf(const char16_t* t, const char16_t** const list, size_t size)
{
    const std::u16string s(t);
    return isOneOf(s, list, size);
}

const char16_t* specialElements[] = {
    u"address", u"applet", u"area", u"article", u"aside",
    u"base", u"basefont", u"bgsound", u"binding", u"blockquote", u"body", u"br", u"button",
    u"caption", u"center", u"col", u"colgroup", u"command",
    u"dd", u"details", u"dir", u"div", u"dl", u"dt",
    u"embed",
    u"fieldset", u"figcaption", u"figure", u"footer", u"form", u"frame", u"frameset",
    u"h1", u"h2", u"h3", u"h4", u"h5", u"h6", u"head", u"header", u"hgroup", u"hr", u"html",
    u"iframe", u"img", u"implementation",
    u"input", u"isindex",
    u"li", u"link", u"listing",
    u"marquee", u"menu", u"meta",
    u"nav", u"noembed", u"noframes", u"noscript",
    u"object", u"ol",
    u"p", u"param", u"plaintext", u"pre",
    u"script", u"section", u"select", u"style", u"summary",
    u"table", u"tbody", u"td", u"template", u"textarea", u"tfoot", u"th", u"thead", u"title", u"tr",
    u"ul",
    u"wbr",
    u"xmp",
    u"foreignObject" /* SVG */
};
const size_t specialElementCount = sizeof specialElements / sizeof specialElements[0];

template <typename T>
bool isSpecial(const T& t)
{
    return isOneOf(t, specialElements, specialElementCount);
}

const char16_t* formattinglElements[] = {
    u"a", u"b", u"big", u"code", u"em", u"font", u"i", u"nobr", u"s", u"small", u"strike", u"strong", u"tt", u"u"
};
const size_t formattinglElementCount = sizeof formattinglElements / sizeof formattinglElements[0];

const char16_t* scopingElements[] = {
    u"applet", u"caption", u"html", u"marquee", u"object", u"table", u"td", u"th",
    u"foreignObject" /* SVG */
};
const size_t scopingElementCount = sizeof scopingElements / sizeof scopingElements[0];

const char16_t* listScopingElements[] = {
    u"applet", u"caption", u"html", u"marquee", u"object", u"table", u"td", u"th",
    u"foreignObject" /* SVG */,
    u"ol", u"ul"
};
const size_t listScopingElementCount = sizeof listScopingElements / sizeof listScopingElements[0];

const char16_t* buttonScopingElements[] = {
    u"applet", u"caption", u"html", u"marquee", u"object", u"table", u"td", u"th",
    u"foreignObject" /* SVG */,
    u"button"
};
const size_t buttonScopingElementCount = sizeof buttonScopingElements / sizeof buttonScopingElements[0];

const char16_t* tableScopingElements[] = {
    u"html", u"table"
};
const size_t tableScopingElementCount = sizeof tableScopingElements / sizeof tableScopingElements[0];

const char16_t* selectScopingElements[] = {
    u"optgroup", u"option"
};
const size_t selectScopingElementCount = sizeof selectScopingElements / sizeof selectScopingElements[0];

void dumpElementStack(std::deque<Element>& stack)
{
    for (auto i = stack.begin(); i != stack.end(); ++i)
        std::cout << (*i).getLocalName() << ' ';
    std::cout << '\n';
}

}  // namespace

HTMLParser::Initial HTMLParser::initial;
HTMLParser::BeforeHtml HTMLParser::beforeHtml;
HTMLParser::BeforeHead HTMLParser::beforeHead;
HTMLParser::InHead HTMLParser::inHead;
HTMLParser::InHeadNoscript HTMLParser::inHeadNoscript;
HTMLParser::AfterHead HTMLParser::afterHead;
HTMLParser::InBody HTMLParser::inBody;
HTMLParser::Text HTMLParser::text;
HTMLParser::InTable HTMLParser::inTable;
HTMLParser::InTableText HTMLParser::inTableText;
HTMLParser::InCaption HTMLParser::inCaption;
HTMLParser::InColumnGroup HTMLParser::inColumnGroup;
HTMLParser::InTableBody HTMLParser::inTableBody;
HTMLParser::InRow HTMLParser::inRow;
HTMLParser::InCell HTMLParser::inCell;
HTMLParser::InSelect HTMLParser::inSelect;
HTMLParser::InSelectInTable HTMLParser::inSelectInTable;
HTMLParser::InForeignContent HTMLParser::inForeignContent;
HTMLParser::AfterBody HTMLParser::afterBody;
HTMLParser::InFrameset HTMLParser::inFrameset;
HTMLParser::AfterFrameset HTMLParser::afterFrameset;
HTMLParser::AfterAfterBody HTMLParser::afterAfterBody;
HTMLParser::AfterAfterFrameset HTMLParser::afterAfterFrameset;
HTMLParser::InBinding HTMLParser::inBinding;

Element HTMLParser::currentTable()
{
    // TODO: make this method faster if necessary.
    for (auto i = openElementStack.rbegin(); i != openElementStack.rend(); ++i) {
        if ((*i).getLocalName() == u"table")
            return *i;
    }
    return openElementStack.front();
}

Element HTMLParser::insertHtmlElement(const std::u16string& name)
{
    Element element = document.createElement(name);
    if (element) {
        if (!insertFromTable)
            currentNode().appendChild(element);
        else
            fosterNode(element);
        pushElement(element);  // Note the fostered element must also be pushed into the stack.
    }
    return element;
}

Element HTMLParser::insertHtmlElement(Token& token)
{
    Element element = insertHtmlElement(token.getName());
    if (!element)
        return 0;
    if (ElementImp* imp = dynamic_cast<ElementImp*>(element.self()))
        imp->setAttributes(token.getAttributes());
    return element;
}

void HTMLParser::insertCharacter(Node node, const std::u16string& data)
{
    Node last = node.getLastChild();
    if (last && last.getNodeType() == Node::TEXT_NODE) {
        org::w3c::dom::Text text = interface_cast<org::w3c::dom::Text>(last);
        text.appendData(data);
    } else {
        org::w3c::dom::Text text = document.createTextNode(data);
        node.appendChild(text);
    }
}

void HTMLParser::insertCharacter(const std::u16string& data)
{
    if (!insertFromTable)
        insertCharacter(currentNode(), data);
    else {
        Element table = currentTable();
        Element fosterParent(0);
        if (table == openElementStack.front())
            fosterParent = table;
        else {
            fosterParent = table.getParentElement();
            if (fosterParent) {
                Node prev = table.getPreviousSibling();
                if (prev) {
                    if (prev.getNodeType() == Node::TEXT_NODE) {
                        org::w3c::dom::Text text = interface_cast<org::w3c::dom::Text>(prev);
                        text.appendData(data);
                        return;
                    }
                    if (prev.getNodeType() == Node::ELEMENT_NODE) {
                        Element e = interface_cast<Element>(prev);
                        if (!isOneOf(e.getLocalName(), { u"table", u"tbody", u"tfoot", u"thead", u"tr" })) {
                            insertCharacter(prev, data);
                            return;
                        }
                    }
                }
                org::w3c::dom::Text text = document.createTextNode(data);
                fosterParent.insertBefore(text, table);
                return;
            }
            auto it = find(openElementStack.begin(), openElementStack.end(), table);
            fosterParent = *--it;
        }
        insertCharacter(fosterParent, data);
    }
}

void HTMLParser::insertCharacter(Token& token)
{
    std::u16string data(1, token.getChar());
    insertCharacter(data);
}

void HTMLParser::fosterNode(Node node)
{
    Element table = currentTable();
    Element fosterParent(0);
    if (table == openElementStack.front())
        fosterParent = table;
    else {
        fosterParent = table.getParentElement();
        if (fosterParent) {
            fosterParent.insertBefore(node, table);
            return;
        }
        auto it = find(openElementStack.begin(), openElementStack.end(), table);
        fosterParent = *--it;
    }
    fosterParent.appendChild(node);
}

bool HTMLParser::stopParsing()
{
    DocumentImp* documentImp = dynamic_cast<DocumentImp*>(document.self());
    assert(documentImp);

    // Step 1
    documentImp->setReadyState(u"interactive");
    // TODO: set the insertion point to undefined.
    // Step 2
    while (!openElementStack.empty())
        popElement();
    // TODO: implement me from step 3
    return true;
}

void HTMLParser::parseRawtext(Token& token, HTMLTokenizer::State* state)
{
    insertHtmlElement(token);
    tokenizer->setState(state);
    originalInsertionMode = insertionMode;
    setInsertionMode(&text);
}

void HTMLParser::parseError(const char* message)
{
    // TODO: implement me
}

void HTMLParser::resetInsertionMode()
{
    bool last = false;
    for (auto i = openElementStack.rbegin(); i != openElementStack.rend(); ++i) {
        Element node = *i;
        if (node == openElementStack.front()) {
            last = true;
            // TODO:  set node to the context element
        }
        if (node.getLocalName() == u"select") {
            setInsertionMode(&inSelect);
            break;
        }
        if (isOneOf(node.getLocalName(), { u"td", u"th" }) && !last) {
            setInsertionMode(&inCell);
            break;
        }
        if (node.getLocalName() == u"tr") {
            setInsertionMode(&inRow);
            break;
        }
        if (isOneOf(node.getLocalName(), { u"tbody", u"thead", u"tfoot" }) && !last) {
            setInsertionMode(&inTableBody);
            break;
        }
        if (node.getLocalName() == u"caption") {
            setInsertionMode(&inColumnGroup);
            break;
        }
        if (node.getLocalName() == u"table") {
            setInsertionMode(&inTable);
            break;
        }
        if (node.getLocalName() == u"head") {
            setInsertionMode(&inBody);
            break;
        }
        if (node.getLocalName() == u"body") {
            setInsertionMode(&inBody);
            break;
        }
        if (node.getLocalName() == u"frameset") {
            setInsertionMode(&inFrameset);
            break;
        }
        if (node.getLocalName() == u"html") {
            setInsertionMode(&beforeHead);
            break;
        }
        // TODO: MathML or SVG
        if (last) {
            setInsertionMode(&inBody);
            break;
        }

        if (enableXBL) {
            if (node.getLocalName() == u"binding") {
                setInsertionMode(&inBinding);
                break;
            }
        }
    }
}

std::list<Element>::iterator HTMLParser::elementInActiveFormattingElements(const std::u16string name)
{
    auto i = activeFormattingElements.end();
    if (activeFormattingElements.empty())
        return i;
    do {
        Element item = *--i;
        if (!item)  // Marker
            break;
        if (item.getLocalName() == name)
            return i;
    } while (i != activeFormattingElements.begin());
    return activeFormattingElements.end();
}

void HTMLParser::reconstructActiveFormattingElements()
{
    if (insertFromTable)
        return;

    // Step 1
    if (activeFormattingElements.empty())
        return;
    // Step 2, 3
    auto entry = activeFormattingElements.end();
    --entry;
    if (!*entry)
        return;
    if (find(openElementStack.begin(), openElementStack.end(), *entry) != openElementStack.end())
        return;
    // Step 4
    while (activeFormattingElements.begin() != entry) {
        // Step 5
        --entry;
        // Step 6
        if (!*entry || find(openElementStack.begin(), openElementStack.end(), *entry) != openElementStack.end()) {
            // Step 7
            ++entry;
            break;
        }
    }
    do {
        // Step 8
        Element newElement = interface_cast<Element>((*entry).cloneNode(false));
        // Step 9
        currentNode().appendChild(newElement);
        pushElement(newElement);
        // Step 10
        *entry = newElement;
        ++entry;
    } while (entry != activeFormattingElements.end());  // Step 11
}

void HTMLParser::clearActiveFormattingElements()
{
    // TODO: review this
    Element entry(0);
    do {
        entry = activeFormattingElements.back();
        activeFormattingElements.pop_back();
    } while (entry);
}

void HTMLParser::generateImpliedEndTags(std::u16string exclude)
{
    for (;;) {
        Element current = currentNode();
        if (!isOneOf(current.getLocalName(), { u"dd", u"dt", u"li", u"option", u"optgroup", u"p", u"rp", u"rt" }) ||
            current.getLocalName() == exclude)
            break;
        popElement();
    }
}

bool HTMLParser::elementInSpecificScope(Element target, const char16_t** const list, size_t size, bool except) {
    for (auto i = openElementStack.rbegin(); i != openElementStack.rend(); ++i) {
        Element node = *i;
        if (node == target)
            return true;
        if (isOneOf(node.getLocalName(), list, size) != except)
            return false;
    }
    return false;
}

bool HTMLParser::elementInSpecificScope(const std::u16string& tagName, const char16_t** const list, size_t size, bool except) {
    for (auto i = openElementStack.rbegin(); i != openElementStack.rend(); ++i) {
        Element node = *i;
        if (node.getLocalName() == tagName)
            return true;
        if (isOneOf(node.getLocalName(), list, size) != except)
            return false;
    }
    return false;
}

bool HTMLParser::elementInSpecificScope(std::initializer_list<const char16_t*> targets, const char16_t** const list, size_t size, bool except) {
    for (auto i = openElementStack.rbegin(); i != openElementStack.rend(); ++i) {
        Element node = *i;
        if (isOneOf(node.getLocalName(), targets))
            return true;
        if (isOneOf(node.getLocalName(), list, size) != except)
            return false;
    }
    return false;
}

template <typename T>
bool HTMLParser::elementInScope(T target)
{
    return elementInSpecificScope(target, scopingElements, scopingElementCount);
}

template <typename T>
bool HTMLParser::elementInListItemScope(T target)
{
    return elementInSpecificScope(target, listScopingElements, listScopingElementCount);
}

template <typename T>
bool HTMLParser::elementInButtonScope(T target)
{
    return elementInSpecificScope(target, buttonScopingElements, buttonScopingElementCount);
}

template <typename T>
bool HTMLParser::elementInTableScope(T target)
{
    return elementInSpecificScope(target, tableScopingElements, tableScopingElementCount);
}

template <typename T>
bool HTMLParser::elementInTableScope(std::initializer_list<T> list)
{
    return elementInSpecificScope(list, tableScopingElements, tableScopingElementCount);
}

template <typename T>
bool HTMLParser::elementInSelectScope(T target)
{
    return elementInSpecificScope(target, selectScopingElements, selectScopingElementCount, true);
}

Element HTMLParser::removeOpenElement(Element element)
{
    for (auto i = openElementStack.begin(); i != openElementStack.end(); ++i) {
        if (*i == element) {
            openElementStack.erase(i);
            return element;
        }
    }
    return 0;
}

//
// HTMLParser::Initial
//

bool HTMLParser::Initial::anythingElse(HTMLParser* parser, Token& token)
{
    // TODO: Check parse error
    return parser->setInsertionMode(&parser->beforeHtml, token);
}

bool HTMLParser::Initial::processEOF(HTMLParser* parser, Token& token)
{
    return anythingElse(parser, token);
}

bool HTMLParser::Initial::processComment(HTMLParser* parser, Token& token)
{
    Comment comment = parser->document.createComment(token.getName());
    parser->document.appendChild(comment);
    return true;
}

bool HTMLParser::Initial::processDoctype(HTMLParser* parser, Token& token)
{
    bool parseError = true;
    if (token.getName() == u"html") {
        if (!token.hasPublicId()) {
            if  (!token.hasSystemId() || token.getSystemId() == u"about:legacy-compat")
                parseError = false;
        } else if (token.getPublicId() == u"-//W3C//DTD HTML 4.0//EN" &&
                   (!token.hasSystemId() || token.getSystemId() == u"http://www.w3.org/TR/REC-html40/strict.dtd") ||
                   token.getPublicId() == u"-//W3C//DTD HTML 4.01//EN" &&
                   (!token.hasSystemId() || token.getSystemId() == u"http://www.w3.org/TR/html4/strict.dtd") ||
                   token.getPublicId() == u"-//W3C//DTD XHTML 1.0 Strict//EN" &&
                   (!token.hasSystemId() || token.getSystemId() == u"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd") ||
                   token.getPublicId() == u"-//W3C//DTD XHTML 1.1//EN" &&
                   (!token.hasSystemId() || token.getSystemId() == u"http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd")) {
            parseError = false;
        }
    }
    if (parseError)
        parser->parseError("unknown-doctype");

    DocumentType doctype = getDOMImplementation()->createDocumentType(token.getName(),
                                                                      token.hasPublicId() ? token.getPublicId() : u"",
                                                                      token.hasSystemId() ? token.getSystemId() : u"");
    parser->document.appendChild(doctype);

    int mode = DocumentImp::NoQuirksMode;
    if ((token.getFlags() & Token::ForceQuirks) || token.getName() != u"html")
        mode = DocumentImp::QuirksMode;
    if (token.hasPublicId()) {
        if (doesStartWith(token.getPublicId(), u"+//Silmaril//dtd html Pro v0r11 19970101//") ||
            doesStartWith(token.getPublicId(), u"-//AdvaSoft Ltd//DTD HTML 3.0 asWedit + extensions//") ||
            doesStartWith(token.getPublicId(), u"-//AS//DTD HTML 3.0 asWedit + extensions//") ||
            doesStartWith(token.getPublicId(), u"-//IETF//DTD HTML 2.0 Level 1//") ||
            doesStartWith(token.getPublicId(), u"-//IETF//DTD HTML 2.0 Level 2//") ||
            doesStartWith(token.getPublicId(), u"-//IETF//DTD HTML 2.0 Strict Level 1//") ||
            doesStartWith(token.getPublicId(), u"-//IETF//DTD HTML 2.0 Strict Level 2//") ||
            doesStartWith(token.getPublicId(), u"-//IETF//DTD HTML 2.0 Strict//") ||
            doesStartWith(token.getPublicId(), u"-//IETF//DTD HTML 2.0//") ||
            doesStartWith(token.getPublicId(), u"-//IETF//DTD HTML 2.1E//") ||
            doesStartWith(token.getPublicId(), u"-//IETF//DTD HTML 3.0//") ||
            doesStartWith(token.getPublicId(), u"-//IETF//DTD HTML 3.2 Final//") ||
            doesStartWith(token.getPublicId(), u"-//IETF//DTD HTML 3.2//") ||
            doesStartWith(token.getPublicId(), u"-//IETF//DTD HTML 3//") ||
            doesStartWith(token.getPublicId(), u"-//IETF//DTD HTML Level 0//") ||
            doesStartWith(token.getPublicId(), u"-//IETF//DTD HTML Level 1//") ||
            doesStartWith(token.getPublicId(), u"-//IETF//DTD HTML Level 2//") ||
            doesStartWith(token.getPublicId(), u"-//IETF//DTD HTML Level 3//") ||
            doesStartWith(token.getPublicId(), u"-//IETF//DTD HTML Strict Level 0//") ||
            doesStartWith(token.getPublicId(), u"-//IETF//DTD HTML Strict Level 1//") ||
            doesStartWith(token.getPublicId(), u"-//IETF//DTD HTML Strict Level 2//") ||
            doesStartWith(token.getPublicId(), u"-//IETF//DTD HTML Strict Level 3//") ||
            doesStartWith(token.getPublicId(), u"-//IETF//DTD HTML Strict//") ||
            doesStartWith(token.getPublicId(), u"-//IETF//DTD HTML//") ||
            doesStartWith(token.getPublicId(), u"-//Metrius//DTD Metrius Presentational//") ||
            doesStartWith(token.getPublicId(), u"-//Microsoft//DTD Internet Explorer 2.0 HTML Strict//") ||
            doesStartWith(token.getPublicId(), u"-//Microsoft//DTD Internet Explorer 2.0 HTML//") ||
            doesStartWith(token.getPublicId(), u"-//Microsoft//DTD Internet Explorer 2.0 Tables//") ||
            doesStartWith(token.getPublicId(), u"-//Microsoft//DTD Internet Explorer 3.0 HTML Strict//") ||
            doesStartWith(token.getPublicId(), u"-//Microsoft//DTD Internet Explorer 3.0 HTML//") ||
            doesStartWith(token.getPublicId(), u"-//Microsoft//DTD Internet Explorer 3.0 Tables//") ||
            doesStartWith(token.getPublicId(), u"-//Netscape Comm. Corp.//DTD HTML//") ||
            doesStartWith(token.getPublicId(), u"-//Netscape Comm. Corp.//DTD Strict HTML//") ||
            doesStartWith(token.getPublicId(), u"-//O'Reilly and Associates//DTD HTML 2.0//") ||
            doesStartWith(token.getPublicId(), u"-//O'Reilly and Associates//DTD HTML Extended 1.0//") ||
            doesStartWith(token.getPublicId(), u"-//O'Reilly and Associates//DTD HTML Extended Relaxed 1.0//") ||
            doesStartWith(token.getPublicId(), u"-//SoftQuad Software//DTD HoTMetaL PRO 6.0::19990601::extensions to HTML 4.0//") ||
            doesStartWith(token.getPublicId(), u"-//SoftQuad//DTD HoTMetaL PRO 4.0::19971010::extensions to HTML 4.0//") ||
            doesStartWith(token.getPublicId(), u"-//Spyglass//DTD HTML 2.0 Extended//") ||
            doesStartWith(token.getPublicId(), u"-//SQ//DTD HTML 2.0 HoTMetaL + extensions//") ||
            doesStartWith(token.getPublicId(), u"-//Sun Microsystems Corp.//DTD HotJava HTML//") ||
            doesStartWith(token.getPublicId(), u"-//Sun Microsystems Corp.//DTD HotJava Strict HTML//") ||
            doesStartWith(token.getPublicId(), u"-//W3C//DTD HTML 3 1995-03-24//") ||
            doesStartWith(token.getPublicId(), u"-//W3C//DTD HTML 3.2 Draft//") ||
            doesStartWith(token.getPublicId(), u"-//W3C//DTD HTML 3.2 Final//") ||
            doesStartWith(token.getPublicId(), u"-//W3C//DTD HTML 3.2//") ||
            doesStartWith(token.getPublicId(), u"-//W3C//DTD HTML 3.2S Draft//") ||
            doesStartWith(token.getPublicId(), u"-//W3C//DTD HTML 4.0 Frameset//") ||
            doesStartWith(token.getPublicId(), u"-//W3C//DTD HTML 4.0 Transitional//") ||
            doesStartWith(token.getPublicId(), u"-//W3C//DTD HTML Experimental 19960712//") ||
            doesStartWith(token.getPublicId(), u"-//W3C//DTD HTML Experimental 970421//") ||
            doesStartWith(token.getPublicId(), u"-//W3C//DTD W3 HTML//") ||
            doesStartWith(token.getPublicId(), u"-//W3O//DTD W3 HTML 3.0//") ||
            isSetTo(token.getPublicId(), u"-//W3O//DTD W3 HTML Strict 3.0//EN//") ||
            doesStartWith(token.getPublicId(), u"-//WebTechs//DTD Mozilla HTML 2.0//") ||
            doesStartWith(token.getPublicId(), u"-//WebTechs//DTD Mozilla HTML//") ||
            isSetTo(token.getPublicId(), u"-/W3C/DTD HTML 4.0 Transitional/EN") ||
            isSetTo(token.getPublicId(), u"HTML"))
            mode = DocumentImp::QuirksMode;
        else if (doesStartWith(token.getPublicId(), u"-//W3C//DTD XHTML 1.0 Frameset//") ||
                 doesStartWith(token.getPublicId(), u"-//W3C//DTD XHTML 1.0 Transitional//"))
            mode = DocumentImp::LimitedQuirksMode;
        else if (doesStartWith(token.getPublicId(), u"-//W3C//DTD HTML 4.01 Frameset//") ||
                 doesStartWith(token.getPublicId(), u"-//W3C//DTD HTML 4.01 Transitional//"))
            mode = token.hasSystemId() ? DocumentImp::LimitedQuirksMode : DocumentImp::QuirksMode;
    }
    if (token.hasSystemId() && isSetTo(token.getSystemId(), u"http://www.ibm.com/data/dtd/v11/ibmxhtml1-transitional.dtd"))
        mode = DocumentImp::QuirksMode;
    if (mode != DocumentImp::NoQuirksMode) {
        if (DocumentImp* documentImp = dynamic_cast<DocumentImp*>(parser->document.self()))
            documentImp->setMode(mode);
    }
    parser->setInsertionMode(&parser->beforeHtml);
    return true;
}

bool HTMLParser::Initial::processCharacter(HTMLParser* parser, Token& token)
{
    if (isSpace(token.getChar()))
        return false;
    return anythingElse(parser, token);
}

bool HTMLParser::Initial::processStartTag(HTMLParser* parser, Token& token)
{
    return anythingElse(parser, token);
}

bool HTMLParser::Initial::processEndTag(HTMLParser* parser, Token& token)
{
    return anythingElse(parser, token);
}

//
// HTMLParser::BeforeHtml
//

bool HTMLParser::BeforeHtml::anythingElse(HTMLParser* parser, Token& token)
{
    insertHtmlElement(parser);
    // TODO: misc.
    return parser->setInsertionMode(&parser->beforeHead, token);
}

Element HTMLParser::BeforeHtml::insertHtmlElement(HTMLParser* parser)
{
    Document document = parser->document;
    Element element = document.createElement(u"html");
    document.appendChild(element);
    parser->pushElement(element);
    return element;
}

bool HTMLParser::BeforeHtml::processEOF(HTMLParser* parser, Token& token)
{
    return anythingElse(parser, token);
}

bool HTMLParser::BeforeHtml::processComment(HTMLParser* parser, Token& token)
{
    Comment comment = parser->document.createComment(token.getName());
    parser->document.appendChild(comment);
    return true;
}

bool HTMLParser::BeforeHtml::processDoctype(HTMLParser* parser, Token& token)
{
    parser->parseError("unexpected-doctype");
    return false;
}

bool HTMLParser::BeforeHtml::processCharacter(HTMLParser* parser, Token& token)
{
    if (isSpace(token.getChar()))
        return false;
    return anythingElse(parser, token);
}

bool HTMLParser::BeforeHtml::processStartTag(HTMLParser* parser, Token& token)
{
    Element element = insertHtmlElement(parser);
    if (token.getName() == u"html") {
        if (ElementImp* imp = dynamic_cast<ElementImp*>(element.self()))
            imp->setAttributes(token.getAttributes());
        parser->setInsertionMode(&parser->beforeHead);
    } else
        parser->setInsertionMode(&parser->beforeHead, token);
    return true;
}

bool HTMLParser::BeforeHtml::processEndTag(HTMLParser* parser, Token& token)
{
    if (isOneOf(token.getName(), { u"head", u"body", u"html", u"br" })) {
        insertHtmlElement(parser);
        return parser->setInsertionMode(&parser->beforeHead, token);
    }
    parser->parseError("unexpected-end-tag-before-html");
    return false;
}

//
// HTMLParser::BeforeHead
//

bool HTMLParser::BeforeHead::anythingElse(HTMLParser* parser, Token& token)
{
    parser->headElement = parser->insertHtmlElement(u"head");
    return parser->setInsertionMode(&parser->inHead, token);
}

bool HTMLParser::BeforeHead::processEOF(HTMLParser* parser, Token& token)
{
    return anythingElse(parser, token);
}

bool HTMLParser::BeforeHead::processComment(HTMLParser* parser, Token& token)
{
    Comment comment = parser->document.createComment(token.getName());
    parser->currentNode().appendChild(comment);
    return true;
}

bool HTMLParser::BeforeHead::processDoctype(HTMLParser* parser, Token& token)
{
    parser->parseError("unexpected-doctype");
    return false;
}

bool HTMLParser::BeforeHead::processCharacter(HTMLParser* parser, Token& token)
{
    if (isSpace(token.getChar()))
        return false;
    return anythingElse(parser, token);
}

bool HTMLParser::BeforeHead::processStartTag(HTMLParser* parser, Token& token)
{
    if (token.getName() == u"html")
        return parser->inBody.processStartTag(parser, token);
    if (token.getName() == u"head") {
        parser->headElement = parser->insertHtmlElement(token);
        parser->setInsertionMode(&parser->inHead);
        return true;
    }
    return anythingElse(parser, token);
}

bool HTMLParser::BeforeHead::processEndTag(HTMLParser* parser, Token& token)
{
    if (isOneOf(token.getName(), { u"head", u"body", u"html", u"br" })) {
        parser->headElement = parser->insertHtmlElement(u"head");
        return parser->setInsertionMode(&parser->inHead, token);
    }
    parser->parseError("end-tag-after-implied-root");
    return false;
}

//
// HTMLParser::InHead
//

bool HTMLParser::InHead::anythingElse(HTMLParser* parser, Token& token)
{
    parser->popElement();
    return parser->setInsertionMode(&parser->afterHead, token);
}

bool HTMLParser::InHead::processEOF(HTMLParser* parser, Token& token)
{
    return anythingElse(parser, token);
}

bool HTMLParser::InHead::processComment(HTMLParser* parser, Token& token)
{
    Comment comment = parser->document.createComment(token.getName());
    parser->currentNode().appendChild(comment);
    return true;
}

bool HTMLParser::InHead::processDoctype(HTMLParser* parser, Token& token)
{
    parser->parseError("unexpected-doctype");
    return false;
}

bool HTMLParser::InHead::processCharacter(HTMLParser* parser, Token& token)
{
    if (isSpace(token.getChar())) {
        parser->insertCharacter(token);
        return true;
    }
    return anythingElse(parser, token);
}

bool HTMLParser::InHead::processStartTag(HTMLParser* parser, Token& token)
{
    if (token.getName() == u"html")
        return parser->inBody.processStartTag(parser, token);
    if (isOneOf(token.getName(), { u"base", u"basefont", u"bgsound", u"command", u"link" })) {
        parser->insertHtmlElement(token);
        parser->popElement();
        token.acknowledge();
        return true;
    }
    if (token.getName() == u"meta") {
        parser->insertHtmlElement(token);
        parser->popElement();
        token.acknowledge();
        // TODO: check encoding
        return true;
    }
    if (token.getName() == u"title") {
        parser->parseRawtext(token, &HTMLTokenizer::rcdataState);
        return true;
    }
    if ((token.getName() == u"noscript" && parser->scriptingFlag) ||
        token.getName() == u"noframes" || token.getName() == u"style") {
        parser->parseRawtext(token, &HTMLTokenizer::rawtextState);
        return true;
    }
    if (token.getName() == u"noscript" && !parser->scriptingFlag) {
        parser->insertHtmlElement(token);
        parser->setInsertionMode(&parser->inHeadNoscript);
        return true;
    }
    if (token.getName() == u"script") {
        parser->insertHtmlElement(token);
        parser->tokenizer->setState(&HTMLTokenizer::scriptDataState);
        parser->originalInsertionMode = parser->insertionMode;
        parser->setInsertionMode(&text);
        // TODO: misc.
        return false;
    }
    if (token.getName() == u"head") {
        parser->parseError("two-heads-are-not-better-than-one");
        return false;
    }

    if (parser->enableXBL) {
        if (token.getName() == u"binding") {
            parser->insertHtmlElement(token);
            parser->setInsertionMode(&parser->inBinding);
            return true;
        }
    }

    return anythingElse(parser, token);
}

bool HTMLParser::InHead::processEndTag(HTMLParser* parser, Token& token)
{
    if (token.getName() == u"head") {
        parser->popElement();
        parser->setInsertionMode(&parser->afterHead);
        return true;
    }
    if (isOneOf(token.getName(), { u"body", u"html", u"br" })) {
        parser->popElement();
        return parser->setInsertionMode(&parser->afterHead, token);
    }
    parser->parseError("unexpected-end-tag");
    return false;
}

//
// HTMLParser::InHeadNoscript
//

bool HTMLParser::InHeadNoscript::anythingElse(HTMLParser* parser, Token& token)
{
    parser->popElement();
    return parser->setInsertionMode(&parser->inHead, token);
}

bool HTMLParser::InHeadNoscript::processEOF(HTMLParser* parser, Token& token)
{
    parser->parseError();
    return anythingElse(parser, token);
}

bool HTMLParser::InHeadNoscript::processComment(HTMLParser* parser, Token& token)
{
    return parser->inHead.processComment(parser, token);
}

bool HTMLParser::InHeadNoscript::processDoctype(HTMLParser* parser, Token& token)
{
    parser->parseError("unexpected-doctype");
    return false;
}

bool HTMLParser::InHeadNoscript::processCharacter(HTMLParser* parser, Token& token)
{
    if (isSpace(token.getChar()))
        return parser->inHead.processCharacter(parser, token);
    parser->parseError();
    return anythingElse(parser, token);
}

bool HTMLParser::InHeadNoscript::processStartTag(HTMLParser* parser, Token& token)
{
    if (token.getName() == u"html")
        return parser->inBody.processStartTag(parser, token);
    if (isOneOf(token.getName(), { u"basefont", u"bgsound", u"binding", u"link",  u"meta", u"noframes", u"style" }))
        return parser->inHead.processStartTag(parser, token);
    if (token.getName() == u"head" || token.getName() == u"noscript") {
        parser->parseError("unexpected-start-tag");
        return false;
    }
    parser->parseError("unexpected-start-tag");
    return anythingElse(parser, token);
}

bool HTMLParser::InHeadNoscript::processEndTag(HTMLParser* parser, Token& token)
{
    if (token.getName() == u"noscript") {
        parser->popElement();
        parser->setInsertionMode(&parser->inHead);
        return true;
    }
    if (token.getName() == u"br") {
        parser->parseError("unexpected-end-tag");
        parser->popElement();
        return parser->setInsertionMode(&parser->inHead, token);
    }
    parser->parseError("unexpected-end-tag");
    return false;
}

//
// HTMLParser::AfterHead
//

bool HTMLParser::AfterHead::anythingElse(HTMLParser* parser, Token& token)
{
    static Token startTagBody(Token::Type::StartTag, u"body");

    processStartTag(parser, startTagBody);
    parser->framesetOkFlag = true;
    return parser->processToken(token);
}

bool HTMLParser::AfterHead::processEOF(HTMLParser* parser, Token& token)
{
    return anythingElse(parser, token);
}

bool HTMLParser::AfterHead::processComment(HTMLParser* parser, Token& token)
{
    Comment comment = parser->document.createComment(token.getName());
    parser->currentNode().appendChild(comment);
    return true;
}

bool HTMLParser::AfterHead::processDoctype(HTMLParser* parser, Token& token)
{
    parser->parseError("unexpected-doctype");
    return false;
}

bool HTMLParser::AfterHead::processCharacter(HTMLParser* parser, Token& token)
{
    if (isSpace(token.getChar())) {
        parser->insertCharacter(token);
        return true;
    }
    return anythingElse(parser, token);
}

bool HTMLParser::AfterHead::processStartTag(HTMLParser* parser, Token& token)
{
    if (token.getName() == u"html")
        return parser->inBody.processStartTag(parser, token);
    if (token.getName() == u"body") {
        parser->insertHtmlElement(token);
        parser->framesetOkFlag = false;
        parser->setInsertionMode(&parser->inBody);
        return true;
    }
    if (token.getName() == u"frameset") {
        parser->insertHtmlElement(token);
        parser->setInsertionMode(&parser->inFrameset);
        return true;
    }
    if (isOneOf(token.getName(), { u"base", u"basefont", u"bgsound", u"binding", u"link",  u"meta", u"noframes", u"script", u"style", u"title" })) {
        assert(parser->headElement);
        parser->parseError("unexpected-start-tag");
        parser->pushElement(parser->headElement);
        parser->inHead.processStartTag(parser, token);
        parser->removeOpenElement(parser->headElement);
        return true;
    }
    if (token.getName() == u"head") {
        parser->parseError();
        return false;
    }
    return anythingElse(parser, token);
}

bool HTMLParser::AfterHead::processEndTag(HTMLParser* parser, Token& token)
{
    if (isOneOf(token.getName(), { u"body", u"html", u"br"}))
        return anythingElse(parser, token);
    parser->parseError();
    return false;
}

//
// HTMLParser::InBody
//

bool HTMLParser::InBody::processEOF(HTMLParser* parser, Token& token)
{
    for (std::deque<Element>::iterator i = parser->openElementStack.begin(); i != parser->openElementStack.end(); ++i) {
        Element node = *i;
        if (isOneOf(node.getLocalName(), { u"dd", u"dt", u"li", u"p", u"tbody", u"td", u"tfoot", u"th", u"thead", u"tr", u"body", u"html" }))
            parser->parseError();
    }
    return true;  // stop parsing
}

bool HTMLParser::InBody::processComment(HTMLParser* parser, Token& token)
{
    Comment comment = parser->document.createComment(token.getName());
    parser->currentNode().appendChild(comment);
    return true;
}

bool HTMLParser::InBody::processDoctype(HTMLParser* parser, Token& token)
{
    parser->parseError("unexpected-doctype");
    return false;
}

bool HTMLParser::InBody::processCharacter(HTMLParser* parser, Token& token)
{
    if (token.getChar() == 0) {
        parser->parseError();
        return false;
    }
    parser->reconstructActiveFormattingElements();
    parser->insertCharacter(token);
    if (!isSpace(token.getChar()))
        parser->framesetOkFlag = false;
    return true;
}

bool HTMLParser::InBody::processStartTag(HTMLParser* parser, Token& token)
{
    static Token endTagA(Token::Type::EndTag, u"a");
    static Token endTagLi(Token::Type::EndTag, u"li");
    static Token endTagNobr(Token::Type::EndTag, u"nobr");
    static Token endTagOption(Token::Type::EndTag, u"option");
    static Token endTagP(Token::Type::EndTag, u"p");

    if (token.getName() == u"html") {
        parser->parseError("non-html-root");
        // TODO: add the attribute
        return true;
    }
    if (isOneOf(token.getName(), { u"base", u"basefont", u"bgsound", u"binding", u"command", u"link", u"meta", u"noframes", u"script", u"style", u"title" }))
        return parser->inHead.processStartTag(parser, token);
    if (token.getName() == u"body") {
        parser->parseError("unexpected-start-tag");
        if (parser->openElementStack.size() < 2 || parser->openElementStack[1].getLocalName() != u"body")
            return false;
        // TODO: add the attribute
        return true;
    }
    if (token.getName() == u"frameset") {
        parser->parseError("unexpected-start-tag");
        if (parser->openElementStack.size() < 2 ||  parser->openElementStack[1].getLocalName() != u"body")
            return false;
        if (!parser->framesetOkFlag)
            return false;
        // TODO: GC issues
        if (Node parent = parser->openElementStack[1].getParentNode())
            parent.removeChild(parser->openElementStack[1]);
        while (parser->currentNode().getLocalName() != u"html")
            parser->popElement();
        parser->insertHtmlElement(token);
        parser->setInsertionMode(&parser->inFrameset);
        return true;
    }
    if (isOneOf(token.getName(), { u"address", u"article", u"aside", u"blockquote", u"center", u"details", u"dir",
                                   u"div", u"dl", u"fieldset", u"figcaption", u"figure", u"footer", u"header",
                                   u"hgroup", u"menu", u"nav", u"ol", u"p", u"section", u"summary", u"ul" })) {
        if (parser->elementInButtonScope(u"p"))
            processEndTag(parser, endTagP);
        parser->insertHtmlElement(token);
        return true;
    }
    if (isOneOf(token.getName(), { u"h1", u"h2", u"h3", u"h4", u"h5", u"h6" })) {
        if (parser->elementInButtonScope(u"p"))
            processEndTag(parser, endTagP);
        if (isOneOf(parser->currentNode().getLocalName(), { u"h1", u"h2", u"h3", u"h4", u"h5", u"h6" })) {
            parser->parseError();
            parser->popElement();
        }
        parser->insertHtmlElement(token);
        return true;
    }
    if (isOneOf(token.getName(), { u"pre", u"listing" })) {
        if (parser->elementInButtonScope(u"p"))
            processEndTag(parser, endTagP);
        parser->insertHtmlElement(token);
        parser->framesetOkFlag = false;
        Token token = parser->tokenizer->getToken();
        if (token.getType() != Token::Type::Character || token.getChar() != '\n')
            parser->processToken(token);
        return true;
    }
    if (token.getName() == u"form") {
        if (parser->formElement) {
            parser->parseError();
            return false;
        }
        if (parser->elementInButtonScope(u"p"))
            processEndTag(parser, endTagP);
        parser->formElement = parser->insertHtmlElement(token);
        return true;
    }
    if (token.getName() == u"li") {
        parser->framesetOkFlag = false;
        for (auto i = parser->openElementStack.rbegin(); i != parser->openElementStack.rend(); ++i) {
            Element node = *i;
            if (node.getLocalName() == u"li") {
                processEndTag(parser, endTagLi);
                break;
            }
            if (isSpecial(node.getLocalName()) && !isOneOf(node.getLocalName(), { u"address", u"div", u"p" }))
                break;
        }
        if (parser->elementInButtonScope(u"p"))
            processEndTag(parser, endTagP);
        parser->insertHtmlElement(token);
        return true;
    }
    if (isOneOf(token.getName(), { u"dd", u"dt" })) {
        parser->framesetOkFlag = false;
        for (auto i = parser->openElementStack.rbegin(); i != parser->openElementStack.rend(); ++i) {
            Element node = *i;
            if (isOneOf(node.getLocalName(), { u"dd", u"dt" })) {
                Token endTag(Token::Type::EndTag, node.getLocalName());
                processEndTag(parser, endTag);
                break;
            }
            if (isSpecial(node.getLocalName()) && !isOneOf(node.getLocalName(), { u"address", u"div", u"p" }))
                break;
        }
        if (parser->elementInButtonScope(u"p"))
            processEndTag(parser, endTagP);
        parser->insertHtmlElement(token);
        return true;
    }
    if (token.getName() == u"plaintext") {
        if (parser->elementInButtonScope(u"p"))
            processEndTag(parser, endTagP);
        parser->insertHtmlElement(token);
        parser->tokenizer->setState(&HTMLTokenizer::plaintextState);
        return true;
    }
    if (token.getName() == u"button") {
        if (parser->elementInButtonScope(u"button")) {
            parser->parseError();
            Token endTag(Token::Type::EndTag, u"button");
            processEndTag(parser, endTag);
        }
        parser->reconstructActiveFormattingElements();
        parser->insertHtmlElement(token);
        parser->framesetOkFlag = false;
        return true;
    }
    if (token.getName() == u"a") {
        auto it = parser->elementInActiveFormattingElements(u"a");
        if (it != parser->activeFormattingElements.end()) {
            Element element = *it;
            parser->parseError();
            processEndTag(parser, endTagA);
            parser->activeFormattingElements.remove(element);
            parser->removeOpenElement(element);
        }
        parser->reconstructActiveFormattingElements();
        parser->addFormattingElement(parser->insertHtmlElement(token));
        return true;
    }
    if (isOneOf(token.getName(), { u"b", u"big", u"code", u"em", u"font", u"i", u"s", u"small", u"strike", u"strong",
                                   u"tt", u"u" })) {
        parser->reconstructActiveFormattingElements();
        parser->addFormattingElement(parser->insertHtmlElement(token));
        return true;
    }
    if (token.getName() == u"nobr") {
        parser->reconstructActiveFormattingElements();
        if (parser->elementInScope(u"nobr")) {
            parser->parseError();
            processEndTag(parser, endTagNobr);
            parser->reconstructActiveFormattingElements();
        }
        parser->addFormattingElement(parser->insertHtmlElement(token));
        return true;
    }
    if (isOneOf(token.getName(), { u"applet", u"marquee", u"object" })) {
        parser->reconstructActiveFormattingElements();
        parser->insertHtmlElement(token);
        parser->addFormattingElement(0);
        parser->framesetOkFlag = false;
        return true;
    }
    if (token.getName() == u"table") {
        if (parser->document.getCompatMode() != u"BackCompat" && parser->elementInButtonScope(u"p"))
            processEndTag(parser, endTagP);
        parser->insertHtmlElement(token);
        parser->framesetOkFlag = false;
        parser->setInsertionMode(&parser->inTable);
        return true;
    }
    if (isOneOf(token.getName(), { u"area", u"br", u"embed", u"img", u"input", u"keygen", u"wbr" })) {
        parser->reconstructActiveFormattingElements();
        parser->insertHtmlElement(token);
        parser->popElement();
        token.acknowledge();
        parser->framesetOkFlag = false;
        return true;
    }
    if (isOneOf(token.getName(), { u"param", u"source", u"track" })) {
        parser->insertHtmlElement(token);
        parser->popElement();
        token.acknowledge();
        return true;
    }
    if (token.getName() == u"hr") {
        if (parser->elementInButtonScope(u"p"))
            processEndTag(parser, endTagP);
        parser->insertHtmlElement(token);
        parser->popElement();
        token.acknowledge();
        parser->framesetOkFlag = false;
        return true;
    }
    if (token.getName() == u"image") {
        parser->parseError();
        token.setName(u"img");
        return processStartTag(parser, token);
    }
    if (token.getName() == u"isindex") {
        parser->parseError();
        if (!parser->formElement)
            return false;
        token.acknowledge();
        Token startTag(Token::Type::StartTag, u"form");
        processStartTag(parser, startTag);
        // TODO: If the token has an attribute called "action", set the action attribute on the resulting form element to the value of the "action" attribute of the token.
        startTag.setName(u"hr");
        processStartTag(parser, startTag);
        startTag.setName(u"label");
        processStartTag(parser, startTag);
        // TODO:
        startTag.setName(u"input");
        processStartTag(parser, startTag);
        // TODO:
        Token endTag(Token::Type::EndTag, u"lable");
        processEndTag(parser, endTag);
        endTag.setName(u"hr");
        processEndTag(parser, endTag);
        endTag.setName(u"form");
        processEndTag(parser, endTag);
        return true;
    }
    if (token.getName() == u"textarea") {
        parser->insertHtmlElement(token);
        parser->tokenizer->setState(&HTMLTokenizer::rcdataState);
        Token nextToken = parser->tokenizer->peekToken();
        if (nextToken.getType() == Token::Type::Character && nextToken.getChar() == '\n')
            parser->tokenizer->getToken();
        parser->originalInsertionMode = parser->insertionMode;
        parser->framesetOkFlag = false;
        parser->setInsertionMode(&parser->text);
        return true;
    }
    if (token.getName() == u"xmp") {
        if (parser->elementInButtonScope(u"p"))
            processEndTag(parser, endTagP);
        parser->reconstructActiveFormattingElements();
        parser->framesetOkFlag = false;
        parser->parseRawtext(token, &HTMLTokenizer::rawtextState);
        return true;
    }
    if (token.getName() == u"iframe") {
        parser->framesetOkFlag = false;
        parser->parseRawtext(token, &HTMLTokenizer::rawtextState);
        return true;
    }
    if (token.getName() == u"noembed" || parser->scriptingFlag && token.getName() == u"noscript") {
        parser->parseRawtext(token, &HTMLTokenizer::rawtextState);
        return true;
    }
    if (token.getName() == u"select") {
        parser->reconstructActiveFormattingElements();
        parser->insertHtmlElement(token);
        parser->framesetOkFlag = false;
        if (parser->insertionMode == &parser->inTable ||
            parser->insertionMode == &parser->inCaption ||
            parser->insertionMode == &parser->inTableBody ||
            parser->insertionMode == &parser->inRow ||
            parser->insertionMode == &parser->inCell)
            parser->setInsertionMode(&parser->inSelectInTable);
        else
            parser->setInsertionMode(&parser->inSelect);
        return true;
    }
    if (isOneOf(token.getName(), { u"optgroup", u"option" })) {
        if (parser->currentNode().getLocalName() == u"option")
            processEndTag(parser, endTagOption);
        parser->reconstructActiveFormattingElements();
        parser->insertHtmlElement(token);
        return true;
    }
    if (isOneOf(token.getName(), { u"rp", u"rt" })) {
        if (parser->elementInScope(u"ruby"))
            parser->generateImpliedEndTags();
        if (parser->currentNode().getLocalName() != u"ruby") {
            parser->parseError();
            do {
                parser->popElement();
            } while (parser->currentNode().getLocalName() != u"ruby");
        }
        parser->insertHtmlElement(token);
        return true;
    }
    if (token.getName() == u"math") {
        parser->reconstructActiveFormattingElements();
        // TODO:
        return false;
    }
    if (token.getName() == u"svg") {
        parser->reconstructActiveFormattingElements();
        // TODO:
        return false;
    }
    if (isOneOf(token.getName(), { u"caption", u"col", u"colgroup", u"frame", u"head",
                                   u"tbody", u"td", u"tfoot", u"th", u"thead", u"tr" })) {
        parser->parseError();
        return true;
    }
    // Any other start tag
    parser->reconstructActiveFormattingElements();
    parser->insertHtmlElement(token);
    return true;
}

bool HTMLParser::InBody::processEndTag(HTMLParser* parser, Token& token)
{
    static Token startTagBr(Token::Type::StartTag, u"br");
    static Token startTagP(Token::Type::StartTag, u"p");
    static Token endTagBody(Token::Type::EndTag, u"body");

    if (token.getName() == u"body") {
        if (!parser->elementInScope(token.getName())) {
            parser->parseError();
            return false;
        }
        for (auto i = parser->openElementStack.rbegin(); i != parser->openElementStack.rend(); ++i) {
            if (!isOneOf(token.getName(), { u"dd",  u"dt",  u"li",  u"optgroup",  u"option",  u"p",  u"rp",  u"rt",
                                            u"tbody",  u"td",  u"tfoot",  u"th",  u"thead",  u"tr",  u"body",  u"html" })) {
                parser->parseError();
                break;
            }
        }
        parser->setInsertionMode(&parser->afterBody);
        return true;
    }
    if (token.getName() == u"html") {
        if (processEndTag(parser, endTagBody))
            return parser->processToken(token);
        return false;
    }
    if (isOneOf(token.getName(), { u"address", u"article", u"aside", u"blockquote", u"button", u"center",
                                   u"details", u"dir", u"div", u"dl",
                                   u"fieldset", u"figcaption", u"figure", u"footer", u"header", u"hgroup",
                                   u"listing", u"menu", u"nav", u"ol", u"pre", u"section", u"summary", u"ul" })) {
        if (!parser->elementInScope(token.getName())) {
            parser->parseError();
            return false;
        }
        parser->generateImpliedEndTags();
        if (parser->currentNode().getLocalName() != token.getName())
            parser->parseError();
        while (parser->popElement().getLocalName() != token.getName())
            ;
        return true;
    }
    if (token.getName() == u"form") {
        Element node = parser->formElement;
        parser->formElement = 0;
        if (!node || !parser->elementInScope(node)) {
            parser->parseError();
            return false;
        }
        parser->generateImpliedEndTags();
        if (parser->currentNode() == node)
            parser->popElement();
        else {
            parser->parseError();
            for (auto i = parser->openElementStack.begin(); i != parser->openElementStack.end(); ++i) {
                if (*i == node) {
                    parser->openElementStack.erase(i);
                    break;
                }
            }
        }
        return true;
    }
    if (token.getName() == u"p") {
        if (!parser->elementInButtonScope(token.getName())) {
            parser->parseError();
            processStartTag(parser, startTagP);
            return parser->processToken(token);
        }
        parser->generateImpliedEndTags(token.getName());
        if (parser->currentNode().getLocalName() != token.getName())
            parser->parseError();
        while (parser->popElement().getLocalName() != token.getName())
            ;
        return true;
    }
    if (token.getName() == u"li") {
        if (!parser->elementInListItemScope(token.getName())) {
            parser->parseError();
            return false;
        }
        parser->generateImpliedEndTags(token.getName());
        if (parser->currentNode().getLocalName() != token.getName())
            parser->parseError();
        while (parser->popElement().getLocalName() != token.getName())
            ;
        return true;
    }
    if (isOneOf(token.getName(), { u"dd", u"dt" })) {
        if (!parser->elementInScope(token.getName())) {
            parser->parseError();
            return false;
        }
        parser->generateImpliedEndTags(token.getName());
        if (parser->currentNode().getLocalName() != token.getName())
            parser->parseError();
        while (parser->popElement().getLocalName() != token.getName())
            ;
        return true;
    }
    if (isOneOf(token.getName(), { u"h1", u"h2", u"h3", u"h4", u"h5", u"h6" })) {
        if (!parser->elementInScope(token.getName())) {
            parser->parseError();
            return false;
        }
        parser->generateImpliedEndTags();
        if (parser->currentNode().getLocalName() != token.getName())
            parser->parseError();
        while (!isOneOf(parser->popElement().getLocalName(), { u"h1", u"h2", u"h3", u"h4", u"h5", u"h6" }))
            ;
        return true;

    }
    if (token.getName() == u"sarcasm") {
        // Take a deep breath, then
        return processAnyOtherEndTag(parser, token);
    }
    if (isOneOf(token.getName(), { u"a", u"b", u"big", u"code", u"em", u"font", u"i", u"nobr",
                                   u"s", u"small", u"strike", u"strong", u"tt", u"u" })) {
        for (int outerLoopCounter = 0; outerLoopCounter < 8; ++outerLoopCounter ) {
            // Step 4 paragraph 1
            auto bookmark = parser->elementInActiveFormattingElements(token.getName());
            if (bookmark == parser->activeFormattingElements.end())
                return processAnyOtherEndTag(parser, token);
            Element formattingElement = *bookmark;
            // Step 4 paragraph 2
            auto it = find(parser->openElementStack.begin(), parser->openElementStack.end(), formattingElement);
            if (it == parser->openElementStack.end()) {
                parser->parseError();
                parser->activeFormattingElements.remove(formattingElement);
                return false;
            }
            // Step 4 paragraph 3
            if (!parser->elementInScope(formattingElement)) {
                parser->parseError();
                return false;
            }
            // Step 4 paragraph 4
            if (formattingElement != parser->currentNode())
                parser->parseError();
            // Step 5
            auto furthestBlock = it;
            for (; furthestBlock != parser->openElementStack.end(); ++furthestBlock) {
                if (isSpecial((*furthestBlock).getLocalName()))
                    break;
            }
            // Step 6
            if (furthestBlock == parser->openElementStack.end()) {
                while (parser->popElement() != formattingElement)
                    ;
                parser->activeFormattingElements.erase(bookmark);
                return false;
            }
            // Step 7
            Element commonAncestor = *(it - 1);
            // Step 8
            // Step 9
            auto node = furthestBlock;
            auto lastNode = furthestBlock;
            auto aboveNode = node - 1;
            for (int innerLoopCount = 0; innerLoopCount < 3; ++innerLoopCount) {
                // Step 9-4
                node = aboveNode;
                aboveNode = node - 1;
                // Step 9-5
                auto itafe = find(parser->activeFormattingElements.begin(), parser->activeFormattingElements.end(), *node);
                if (itafe == parser->activeFormattingElements.end()) {
                    parser->openElementStack.erase(node);
                    continue;
                }
                // Step 9-6
                if (*node == formattingElement)
                    break;
                // Step 9-7
                Element clone = interface_cast<Element>((*node).cloneNode(false));
                *itafe = clone;
                *node = clone;
                // Step 9-8
                if (lastNode == furthestBlock)
                    bookmark = ++itafe;
                // Step 9-9
                if (Node parent = (*lastNode).getParentNode())
                    parent.removeChild(*lastNode);
                (*node).appendChild(*lastNode);
                // Step 9-10
                lastNode = node;
            }
            // Step 10
            if (isOneOf(commonAncestor.getLocalName(), { u"table", u"tbody", u"tfoot", u"thead", u"tr" }))
                parser->fosterNode(*lastNode);
            else {
                if (Node parent = (*lastNode).getParentNode())
                    parent.removeChild(*lastNode);
                commonAncestor.appendChild(*lastNode);
            }
            // Step 11
            Element clone = interface_cast<Element>(formattingElement.cloneNode(false));
            // Step 12
            while ((*furthestBlock).hasChildNodes()) {
                Node child = (*furthestBlock).getFirstChild();
                (*furthestBlock).removeChild(child);
                clone.appendChild(child);
            }
            // Step 13
            (*furthestBlock).appendChild(clone);
            // Step 14
            parser->activeFormattingElements.insert(bookmark, clone);
            parser->activeFormattingElements.remove(formattingElement);
            // Step 15
            parser->openElementStack.insert(furthestBlock + 1, clone);
            parser->openElementStack.erase(it);
        }
   }
    if (isOneOf(token.getName(), { u"applet", u"marquee", u"object" })) {
        if (!parser->elementInScope(token.getName())) {
            parser->parseError();
            return false;
        }
        parser->generateImpliedEndTags();
        if (parser->currentNode().getLocalName() != token.getName())
            parser->parseError();
        while (parser->popElement().getLocalName() != token.getName())
            ;
        parser->clearActiveFormattingElements();
        return true;
    }
    if (token.getName() == u"br") {
        parser->parseError();
        return processStartTag(parser, startTagBr);
    }
    return processAnyOtherEndTag(parser, token);
}

bool HTMLParser::InBody::processAnyOtherEndTag(HTMLParser* parser, Token& token)
{
    for (auto i = parser->openElementStack.rbegin(); i != parser->openElementStack.rend(); ++i) {
        Element node = *i;
        if (node.getLocalName() == token.getName()) {
            parser->generateImpliedEndTags(token.getName());
            if (token.getName() != parser->currentNode().getLocalName())
                parser->parseError();
            while (parser->popElement() != node)
                ;
            return true;
        } else if (isSpecial(node.getLocalName())) {
            parser->parseError();
            break;
        }
    }
    return false;
}

//
// HTMLParser::Text
//

bool HTMLParser::Text::processEOF(HTMLParser* parser, Token& token)
{
    parser->parseError();
    if (isOneOf(parser->currentNode().getLocalName(), { u"implementation", u"script" })) {
        // TODO: mark the script element as "already started".
    }
    parser->popElement();
    return parser->setInsertionMode(parser->originalInsertionMode, token);
}

bool HTMLParser::Text::processComment(HTMLParser* parser, Token& token)
{
    return false;
}

bool HTMLParser::Text::processDoctype(HTMLParser* parser, Token& token)
{
    return false;
}

bool HTMLParser::Text::processCharacter(HTMLParser* parser, Token& token)
{
    assert(token.getChar() != 0);
    parser->insertCharacter(token);
    return true;
}

bool HTMLParser::Text::processStartTag(HTMLParser* parser, Token& token)
{
    return false;
}

bool HTMLParser::Text::processEndTag(HTMLParser* parser, Token& token)
{
    if (isOneOf(token.getName(), { u"implementation", u"script" })) {
        Element script = parser->currentNode();
        parser->popElement();
        parser->setInsertionMode(parser->originalInsertionMode);
        // TODO: Let the old insertion point have the same value as the current insertion point.
        //       Let the insertion point be just before the next input character.
        ++(parser->scriptNestingLevel);
        // TODO: Prepare the script.
        if (HTMLScriptElementImp* imp = dynamic_cast<HTMLScriptElementImp*>(script.self()))
            imp->prepare();
        if (--(parser->scriptNestingLevel) == 0)
            parser->pauseFlag = false;
        // TODO: Let the insertion point have the value of the old insertion point.
        // TODO: if there is a pending parsing-blocking script
            // more TODOs
        return true;
    }
    // Any other end tag
    parser->popElement();
    parser->setInsertionMode(parser->originalInsertionMode);
    return true;
}

//
// HTMLParser::InTable
//

bool HTMLParser::InTable::anythingElse(HTMLParser* parser, Token& token)
{
    parser->parseError();
    if (isOneOf(parser->currentNode().getLocalName(), { u"table", u"tbody", u"tfoot", u"thead", u"tr" }))
        parser->insertFromTable = true;
    bool result = parser->inBody.processToken(parser, token);
    parser->insertFromTable = false;
    return result;
}

void HTMLParser::InTable::clearStackBackToTableContext(HTMLParser* parser)
{
    while (!isOneOf(parser->currentNode().getLocalName(), { u"table", u"html" }))
        parser->popElement();
}

bool HTMLParser::InTable::processEOF(HTMLParser* parser, Token& token)
{
    if (parser->currentNode().getLocalName() != u"html")
        parser->parseError();
    return parser->stopParsing();
}

bool HTMLParser::InTable::processComment(HTMLParser* parser, Token& token)
{
    Comment comment = parser->document.createComment(token.getName());
    parser->currentNode().appendChild(comment);
    return true;
}

bool HTMLParser::InTable::processDoctype(HTMLParser* parser, Token& token)
{
    parser->parseError();
    return false;
}

bool HTMLParser::InTable::processCharacter(HTMLParser* parser, Token& token)
{
    parser->pendingTableCharacters = u"";
    parser->spaceInPendingTableCharacters = false;
    parser->originalInsertionMode = parser->insertionMode;
    return parser->setInsertionMode(&parser->inTableText, token);
}

bool HTMLParser::InTable::processStartTag(HTMLParser* parser, Token& token)
{
    static Token startTagColgroup(Token::Type::StartTag, u"colgroup");
    static Token startTagTbody(Token::Type::StartTag, u"tbody");
    static Token endTagTable(Token::Type::EndTag, u"table");

    if (token.getName() == u"caption") {
        clearStackBackToTableContext(parser);
        parser->activeFormattingElements.push_back(0);
        parser->insertHtmlElement(token);
        parser->setInsertionMode(&parser->inCaption);
        return true;
    }
    if (token.getName() == u"colgroup") {
        clearStackBackToTableContext(parser);
        parser->insertHtmlElement(token);
        parser->setInsertionMode(&parser->inColumnGroup);
        return true;
    }
    if (token.getName() == u"col") {
        processStartTag(parser, startTagColgroup);
        return parser->processToken(token);
    }
    if (isOneOf(token.getName(), { u"tbody", u"tfoot", u"thead" })) {
        clearStackBackToTableContext(parser);
        parser->insertHtmlElement(token);
        parser->setInsertionMode(&parser->inTableBody);
        return true;
    }
    if (isOneOf(token.getName(), { u"td", u"th", u"tr" })) {
        processStartTag(parser, startTagTbody);
        return parser->processToken(token);
    }
    if (token.getName() == u"table") {
        parser->parseError();
        if (processEndTag(parser, endTagTable))
            return parser->processToken(token);
        return false;
    }
    if (isOneOf(token.getName(), { u"style", u"script" }))
        return parser->inHead.processStartTag(parser, token);
    if (token.getName() == u"input") {
        Nullable<std::u16string> value = token.getAttribute(u"type");
        if (!value.hasValue() || !isSetTo(value.value(), u"hidden"))
            return anythingElse(parser, token);
        parser->parseError();
        parser->insertHtmlElement(token);
        parser->popElement();
        return true;
    }
    if (token.getName() == u"form") {
        parser->parseError();
        if (parser->formElement)
            return false;
        parser->formElement = parser->insertHtmlElement(token);
        parser->popElement();
        return true;
    }
    return anythingElse(parser, token);
}

bool HTMLParser::InTable::processEndTag(HTMLParser* parser, Token& token)
{
    if (token.getName() == u"table") {
        if (!parser->elementInTableScope(token.getName())) {
            parser->parseError();
            return false;
        }
        while (parser->popElement().getLocalName() != u"table")
            ;
        parser->resetInsertionMode();
        return true;
    }
    if (isOneOf(token.getName(), { u"body", u"caption", u"col", u"colgroup", u"html", u"tbody", u"td", u"tfoot", u"th", u"thead", u"tr" })) {
        parser->parseError();
        return false;
    }
    return anythingElse(parser, token);
}

//
// HTMLParser::InTableText
//

bool HTMLParser::InTableText::anythingElse(HTMLParser* parser, Token& token)
{
    if (!parser->spaceInPendingTableCharacters) {
        for (auto i = parser->pendingTableCharacters.begin(); i < parser->pendingTableCharacters.end(); ++i) {
            Token c(*i);
            parser->inTable.anythingElse(parser, c);
        }
    } else
        parser->insertCharacter(parser->pendingTableCharacters);
    return parser->setInsertionMode(parser->originalInsertionMode, token);
}

bool HTMLParser::InTableText::processEOF(HTMLParser* parser, Token& token)
{
    return anythingElse(parser, token);
}

bool HTMLParser::InTableText::processComment(HTMLParser* parser, Token& token)
{
    return anythingElse(parser, token);
}

bool HTMLParser::InTableText::processDoctype(HTMLParser* parser, Token& token)
{
    return anythingElse(parser, token);
}

bool HTMLParser::InTableText::processCharacter(HTMLParser* parser, Token& token)
{
    parser->pendingTableCharacters += token.getChar();
    if (isSpace(token.getChar()))
        parser->spaceInPendingTableCharacters = true;
    return true;
}

bool HTMLParser::InTableText::processStartTag(HTMLParser* parser, Token& token)
{
    return anythingElse(parser, token);
}

bool HTMLParser::InTableText::processEndTag(HTMLParser* parser, Token& token)
{
    return anythingElse(parser, token);
}

//
// HTMLParser::InCaption
//

bool HTMLParser::InCaption::anythingElse(HTMLParser* parser, Token& token)
{
    return parser->inBody.processToken(parser, token);
}

bool HTMLParser::InCaption::processEOF(HTMLParser* parser, Token& token)
{
    return anythingElse(parser, token);
}

bool HTMLParser::InCaption::processComment(HTMLParser* parser, Token& token)
{
    return anythingElse(parser, token);
}

bool HTMLParser::InCaption::processDoctype(HTMLParser* parser, Token& token)
{
    return anythingElse(parser, token);
}

bool HTMLParser::InCaption::processCharacter(HTMLParser* parser, Token& token)
{
    return anythingElse(parser, token);
}

bool HTMLParser::InCaption::processStartTag(HTMLParser* parser, Token& token)
{
    static Token endTagCaption(Token::Type::EndTag, u"caption");

    if (isOneOf(token.getName(), { u"caption", u"col", u"colgroup", u"tbody", u"td", u"tfoot", u"th", u"thead", u"tr" })) {
        parser->parseError();
        if (processEndTag(parser, endTagCaption))
            return parser->processToken(token);
        return false;
    }
    return anythingElse(parser, token);
}

bool HTMLParser::InCaption::processEndTag(HTMLParser* parser, Token& token)
{
    static Token endTagCaption(Token::Type::EndTag, u"caption");

    if (token.getName() == u"caption") {
        if (!parser->elementInTableScope(token.getName())) {
            parser->parseError();
            return false;
        }
        parser->generateImpliedEndTags();
        if (parser->currentNode().getLocalName() != u"caption")
            parser->parseError();
        while (parser->popElement().getLocalName() != u"caption")
            ;
        parser->clearActiveFormattingElements();
        parser->setInsertionMode(&parser->inTable);
        return true;
    }
    if (token.getName() == u"table") {
        parser->parseError();
        if (processEndTag(parser, endTagCaption))
            return parser->processToken(token);
        return false;
    }
    if (isOneOf(token.getName(), { u"body", u"col", u"colgroup", u"html", u"tbody", u"td", u"tfoot", u"th", u"thead", u"tr" })) {
        parser->parseError();
        return false;
    }
    return anythingElse(parser, token);
}

//
// HTMLParser::InColumnGroup
//

bool HTMLParser::InColumnGroup::anythingElse(HTMLParser* parser, Token& token)
{
    static Token endTagColgroup(Token::Type::EndTag, u"colgroup");
    if (processEndTag(parser, endTagColgroup))
        return parser->processToken(token);
    return false;
}

bool HTMLParser::InColumnGroup::processEOF(HTMLParser* parser, Token& token)
{
    if (parser->currentNode() == parser->openElementStack.front())
        return parser->stopParsing();
    return anythingElse(parser, token);
}

bool HTMLParser::InColumnGroup::processComment(HTMLParser* parser, Token& token)
{
    Comment comment = parser->document.createComment(token.getName());
    parser->currentNode().appendChild(comment);
    return true;
}

bool HTMLParser::InColumnGroup::processDoctype(HTMLParser* parser, Token& token)
{
    parser->parseError();
    return false;
}

bool HTMLParser::InColumnGroup::processCharacter(HTMLParser* parser, Token& token)
{
    if (isSpace(token.getChar())) {
        parser->insertCharacter(token);
        return true;
    }
    return anythingElse(parser, token);
}

bool HTMLParser::InColumnGroup::processStartTag(HTMLParser* parser, Token& token)
{
    if (token.getName() == u"html")
        return parser->inBody.processStartTag(parser, token);
    if (token.getName() == u"col") {
        parser->insertHtmlElement(token);
        parser->popElement();
        token.acknowledge();
        return true;
    }
    return anythingElse(parser, token);
}

bool HTMLParser::InColumnGroup::processEndTag(HTMLParser* parser, Token& token)
{
    if (token.getName() == u"colgroup") {
        if (parser->currentNode() == parser->openElementStack.front()) {
            parser->parseError();
            return false;
        }
        parser->popElement();
        parser->setInsertionMode(&parser->inTable);
        return true;
    }
    if (token.getName() == u"col") {
        parser->parseError();
        return false;
    }
    return anythingElse(parser, token);
}

//
// HTMLParser::InTableBody
//

void HTMLParser::InTableBody::clearStackBackToTableBodyContext(HTMLParser* parser)
{
    while (!isOneOf(parser->currentNode().getLocalName(), { u"tbody", u"tfoot", u"thead", u"html" }))
        parser->popElement();
}

bool HTMLParser::InTableBody::anythingElse(HTMLParser* parser, Token& token)
{
    return parser->inTable.processToken(parser, token);
}

bool HTMLParser::InTableBody::processEOF(HTMLParser* parser, Token& token)
{
    return anythingElse(parser, token);
}

bool HTMLParser::InTableBody::processComment(HTMLParser* parser, Token& token)
{
    return anythingElse(parser, token);
}

bool HTMLParser::InTableBody::processDoctype(HTMLParser* parser, Token& token)
{
    return anythingElse(parser, token);
}

bool HTMLParser::InTableBody::processCharacter(HTMLParser* parser, Token& token)
{
    return anythingElse(parser, token);
}

bool HTMLParser::InTableBody::processStartTag(HTMLParser* parser, Token& token)
{
    static Token startTagTr(Token::Type::StartTag, u"tr");

    if (token.getName() == u"tr") {
        clearStackBackToTableBodyContext(parser);
        parser->insertHtmlElement(token);
        parser->setInsertionMode(&parser->inRow);
        return true;
    }
    if (isOneOf(token.getName(), { u"th", u"td" })) {
        parser->parseError();
        processStartTag(parser, startTagTr);
        return parser->processToken(token);
    }
    if (isOneOf(token.getName(), { u"caption", u"col", u"colgroup", u"tbody", u"tfoot", u"thead", u"tr" })) {
        if (!parser->elementInTableScope({ u"tbody", u"thead", u"tfoot"})) {
            parser->parseError();
            return false;
        }
        clearStackBackToTableBodyContext(parser);
        Token endTag(Token::Type::EndTag, parser->currentNode().getLocalName());
        processEndTag(parser, endTag);
        return parser->processToken(token);
    }
    return anythingElse(parser, token);
}

bool HTMLParser::InTableBody::processEndTag(HTMLParser* parser, Token& token)
{
    if (isOneOf(token.getName(), { u"tbody", u"tfoot", u"thead" })) {
        if (!parser->elementInTableScope(token.getName())) {
            parser->parseError();
            return false;
        }
        clearStackBackToTableBodyContext(parser);
        parser->popElement();
        parser->setInsertionMode(&parser->inTable);
        return true;
    }
    if (token.getName() == u"table") {
        if (!parser->elementInTableScope({ u"tbody", u"thead", u"tfoot"})) {
            parser->parseError();
            return false;
        }
        clearStackBackToTableBodyContext(parser);
        Token endTag(Token::Type::EndTag, parser->currentNode().getLocalName());
        processEndTag(parser, endTag);
        return parser->processToken(token);
    }
    if (isOneOf(token.getName(), { u"body", u"caption", u"col", u"colgroup", u"html", u"td", u"th", u"tr" })) {
        parser->parseError();
        return false;
    }
    return anythingElse(parser, token);
}

//
// HTMLParser::InRow
//

void HTMLParser::InRow::clearStackBackToTableRowContext(HTMLParser* parser)
{
    while (!isOneOf(parser->currentNode().getLocalName(), { u"tr", u"html" }))
        parser->popElement();
}

bool HTMLParser::InRow::anythingElse(HTMLParser* parser, Token& token)
{
    return parser->inTable.processToken(parser, token);
}

bool HTMLParser::InRow::processEOF(HTMLParser* parser, Token& token)
{
    return anythingElse(parser, token);
}

bool HTMLParser::InRow::processComment(HTMLParser* parser, Token& token)
{
    return anythingElse(parser, token);
}

bool HTMLParser::InRow::processDoctype(HTMLParser* parser, Token& token)
{
    return anythingElse(parser, token);
}

bool HTMLParser::InRow::processCharacter(HTMLParser* parser, Token& token)
{
    return anythingElse(parser, token);
}

bool HTMLParser::InRow::processStartTag(HTMLParser* parser, Token& token)
{
    static Token endTagTr(Token::Type::EndTag, u"tr");

    if (isOneOf(token.getName(), { u"th", u"td" })) {
        clearStackBackToTableRowContext(parser);
        parser->insertHtmlElement(token);
        parser->setInsertionMode(&parser->inCell);
        parser->activeFormattingElements.push_back(0);
        return true;
    }
    if (isOneOf(token.getName(), { u"caption", u"col", u"colgroup", u"tbody", u"tfoot", u"thead", u"tr" })) {
        if (processEndTag(parser, endTagTr))
            return parser->processToken(token);
        return false;
    }
    return anythingElse(parser, token);
}

bool HTMLParser::InRow::processEndTag(HTMLParser* parser, Token& token)
{
    static Token endTagTr(Token::Type::EndTag, u"tr");

    if (token.getName() == u"tr") {
        if (!parser->elementInTableScope(token.getName())) {
            parser->parseError();
            return false;
        }
        clearStackBackToTableRowContext(parser);
        parser->popElement();
        parser->setInsertionMode(&parser->inTableBody);
        return true;
    }
    if (token.getName() == u"table") {
        if (processEndTag(parser, endTagTr))
            return parser->processToken(token);
        return false;
    }
    if (isOneOf(token.getName(), { u"tbody", u"tfoot", u"thead" })) {
        if (!parser->elementInTableScope(token.getName())) {
            parser->parseError();
            return false;
        }
        processEndTag(parser, endTagTr);
        return parser->processToken(token);
    }
    if (isOneOf(token.getName(), { u"body", u"caption", u"col", u"colgroup", u"html", u"td", u"th" })) {
        parser->parseError();
        return false;
    }
    return anythingElse(parser, token);
}

//
// HTMLParser::InCell
//

void HTMLParser::InCell::closeCell(HTMLParser* parser)
{
    static Token endTagTd(Token::Type::EndTag, u"td");
    static Token endTagTh(Token::Type::EndTag, u"th");

    if (parser->elementInTableScope(u"td"))
        processEndTag(parser, endTagTd);
    else
        processEndTag(parser, endTagTh);
}

bool HTMLParser::InCell::anythingElse(HTMLParser* parser, Token& token)
{
    return parser->inBody.processToken(parser, token);
}

bool HTMLParser::InCell::processEOF(HTMLParser* parser, Token& token)
{
    return anythingElse(parser, token);
}

bool HTMLParser::InCell::processComment(HTMLParser* parser, Token& token)
{
    return anythingElse(parser, token);
}

bool HTMLParser::InCell::processDoctype(HTMLParser* parser, Token& token)
{
    return anythingElse(parser, token);
}

bool HTMLParser::InCell::processCharacter(HTMLParser* parser, Token& token)
{
    return anythingElse(parser, token);
}

bool HTMLParser::InCell::processStartTag(HTMLParser* parser, Token& token)
{
    if (isOneOf(token.getName(), { u"caption", u"col", u"colgroup", u"tbody", u"td", u"tfoot", u"th", u"thead", u"tr" })) {
        if (!parser->elementInTableScope( { u"td", u"th" } )) {
            parser->parseError();
            return false;
        }
        closeCell(parser);
        return parser->processToken(token);
    }
    return anythingElse(parser, token);
}

bool HTMLParser::InCell::processEndTag(HTMLParser* parser, Token& token)
{
    if (isOneOf(token.getName(), { u"th", u"td" })) {
        if (!parser->elementInTableScope(token.getName())) {
            parser->parseError();
            return false;
        }
        parser->generateImpliedEndTags();
        if (parser->currentNode().getLocalName() != token.getName())
            parser->parseError();
        while (parser->popElement().getLocalName() != token.getName())
            ;
        parser->clearActiveFormattingElements();
        parser->setInsertionMode(&parser->inRow);
        return true;
    }
    if (isOneOf(token.getName(), { u"body", u"caption", u"col", u"colgroup", u"html" })) {
        parser->parseError();
        return false;
    }
    if (isOneOf(token.getName(), { u"table", u"tbody", u"tfoot", u"thead", u"tr" })) {
        if (!parser->elementInTableScope(token.getName())) {
            parser->parseError();
            return false;
        }
        closeCell(parser);
        return parser->processToken(token);
    }
    return anythingElse(parser, token);
}

//
// HTMLParser::InSelect
//

bool HTMLParser::InSelect::anythingElse(HTMLParser* parser, Token& token)
{
    parser->parseError();
    return false;
}

bool HTMLParser::InSelect::processEOF(HTMLParser* parser, Token& token)
{
    if (parser->currentNode().getLocalName() != u"html")
        parser->parseError();
    return parser->stopParsing();
}

bool HTMLParser::InSelect::processComment(HTMLParser* parser, Token& token)
{
    Comment comment = parser->document.createComment(token.getName());
    parser->currentNode().appendChild(comment);
    return true;
}

bool HTMLParser::InSelect::processDoctype(HTMLParser* parser, Token& token)
{
    parser->parseError();
    return false;
}

bool HTMLParser::InSelect::processCharacter(HTMLParser* parser, Token& token)
{
    if (token.getChar() == 0) {
        parser->parseError();
        return false;
    }
    parser->insertCharacter(token);
    return false;
}

bool HTMLParser::InSelect::processStartTag(HTMLParser* parser, Token& token)
{
    static Token endTagOption(Token::Type::EndTag, u"option");
    static Token endTagOptgroup(Token::Type::EndTag, u"optgroup");
    static Token endTagSelect(Token::Type::EndTag, u"select");

    if (token.getName() == u"html")
        return parser->inBody.processStartTag(parser, token);
    if (token.getName() == u"option") {
        if (parser->currentNode().getLocalName() == u"option")
            processEndTag(parser, endTagOption);
        parser->insertHtmlElement(token);
        return true;
    }
    if (token.getName() == u"optgroup") {
        if (parser->currentNode().getLocalName() == u"option")
            processEndTag(parser, endTagOption);
        if (parser->currentNode().getLocalName() == u"optgroup")
            processEndTag(parser, endTagOptgroup);
        parser->insertHtmlElement(token);
        return true;
    }
    if (token.getName() == u"select") {
        parser->parseError();
        token.setType(Token::Type::EndTag);
        return processEndTag(parser, token);
    }
    if (isOneOf(token.getName(), { u"input", u"keygen", u"textarea" })) {
        parser->parseError();
        if (!parser->elementInSelectScope(u"select")) {
            parser->parseError();
            return false;
        }
        processEndTag(parser, endTagSelect);
        return parser->processToken(token);
    }
    if (isOneOf(token.getName(), { u"script" }))
        return parser->inHead.processStartTag(parser, token);
    return anythingElse(parser, token);
}

bool HTMLParser::InSelect::processEndTag(HTMLParser* parser, Token& token)
{
    static Token endTagOption(Token::Type::EndTag, u"option");

    if (token.getName() == u"optgroup") {
        if (parser->currentNode().getLocalName() == u"option" &&
            parser->openElementStack[parser->openElementStack.size() - 2].getLocalName() == u"optgroup")
            processEndTag(parser, endTagOption);
        if (parser->currentNode().getLocalName() == u"optgroup")
            parser->popElement();
        else {
            parser->parseError();
            return false;
        }
        return true;
    }
    if (token.getName() == u"option") {
        if (parser->currentNode().getLocalName() == u"option")
            parser->popElement();
        else {
            parser->parseError();
            return false;
        }
        return true;
    }
    if (token.getName() == u"select") {
        if (!parser->elementInSelectScope(token.getName())) {
            parser->parseError();
            return false;
        }
        while (parser->popElement().getLocalName() != u"select")
            ;
        parser->resetInsertionMode();
        return true;
    }
    return anythingElse(parser, token);
}

//
// HTMLParser::InSelectInTable
//

bool HTMLParser::InSelectInTable::anythingElse(HTMLParser* parser, Token& token)
{
    return parser->inSelect.processToken(parser, token);
}

bool HTMLParser::InSelectInTable::processEOF(HTMLParser* parser, Token& token)
{
    return anythingElse(parser, token);
}

bool HTMLParser::InSelectInTable::processComment(HTMLParser* parser, Token& token)
{
    return anythingElse(parser, token);
}

bool HTMLParser::InSelectInTable::processDoctype(HTMLParser* parser, Token& token)
{
    return anythingElse(parser, token);
}

bool HTMLParser::InSelectInTable::processCharacter(HTMLParser* parser, Token& token)
{
    return anythingElse(parser, token);
}

bool HTMLParser::InSelectInTable::processStartTag(HTMLParser* parser, Token& token)
{
    static Token endTagSelect(Token::Type::EndTag, u"select");

    if (isOneOf(token.getName(), { u"caption", u"table", u"tbody", u"tfoot", u"thead", u"tr", u"td", u"th" })) {
        parser->parseError();
        processEndTag(parser, endTagSelect);
        return parser->processToken(token);
    }
    return anythingElse(parser, token);
}

bool HTMLParser::InSelectInTable::processEndTag(HTMLParser* parser, Token& token)
{
    static Token endTagSelect(Token::Type::EndTag, u"select");

    if (isOneOf(token.getName(), { u"caption", u"table", u"tbody", u"tfoot", u"thead", u"tr", u"td", u"th" })) {
        parser->parseError();
        if (!parser->elementInTableScope(token.getName())) {
            processEndTag(parser, endTagSelect);
            return parser->processToken(token);
        }
        return false;
    }
    return anythingElse(parser, token);
}

//
// HTMLParser::InForeignContent
//

bool HTMLParser::InForeignContent::processEOF(HTMLParser* parser, Token& token)
{
    // TODO: imeplement me!
}

bool HTMLParser::InForeignContent::processComment(HTMLParser* parser, Token& token)
{
    // TODO: imeplement me!
}

bool HTMLParser::InForeignContent::processDoctype(HTMLParser* parser, Token& token)
{
    // TODO: imeplement me!
}

bool HTMLParser::InForeignContent::processCharacter(HTMLParser* parser, Token& token)
{
    // TODO: imeplement me!
}

bool HTMLParser::InForeignContent::processStartTag(HTMLParser* parser, Token& token)
{
    // TODO: imeplement me!
}

bool HTMLParser::InForeignContent::processEndTag(HTMLParser* parser, Token& token)
{
    // TODO: imeplement me!
}

//
// HTMLParser::AfterBody
//

bool HTMLParser::AfterBody::anythingElse(HTMLParser* parser, Token& token)
{
    parser->parseError();
    return parser->setInsertionMode(&parser->inBody, token);
}

bool HTMLParser::AfterBody::processEOF(HTMLParser* parser, Token& token)
{
    return parser->stopParsing();
}

bool HTMLParser::AfterBody::processComment(HTMLParser* parser, Token& token)
{
    Comment comment = parser->document.createComment(token.getName());
    parser->openElementStack.front().appendChild(comment);
    return true;
}

bool HTMLParser::AfterBody::processDoctype(HTMLParser* parser, Token& token)
{
    parser->parseError();
    return false;
}

bool HTMLParser::AfterBody::processCharacter(HTMLParser* parser, Token& token)
{
    if (isSpace(token.getChar()))
        return parser->inBody.processCharacter(parser, token);
    return anythingElse(parser, token);
}

bool HTMLParser::AfterBody::processStartTag(HTMLParser* parser, Token& token)
{
    if (token.getName() == u"html")
        return parser->inBody.processCharacter(parser, token);
    return anythingElse(parser, token);
}

bool HTMLParser::AfterBody::processEndTag(HTMLParser* parser, Token& token)
{
    if (token.getName() == u"html") {
        if (parser->innerHTML) {
            parser->parseError();
            return false;
        }
        parser->setInsertionMode(&parser->afterAfterBody);
        return true;
    }
    return anythingElse(parser, token);
}

//
// HTMLParser::InFrameset
//

bool HTMLParser::InFrameset::anythingElse(HTMLParser* parser, Token& token)
{
    parser->parseError();
    return false;
}

bool HTMLParser::InFrameset::processEOF(HTMLParser* parser, Token& token)
{
    if (parser->currentNode().getLocalName() != u"html")
        parser->parseError();
    return parser->stopParsing();
}

bool HTMLParser::InFrameset::processComment(HTMLParser* parser, Token& token)
{
    Comment comment = parser->document.createComment(token.getName());
    parser->currentNode().appendChild(comment);
    return true;
}

bool HTMLParser::InFrameset::processDoctype(HTMLParser* parser, Token& token)
{
    parser->parseError();
    return false;
}

bool HTMLParser::InFrameset::processCharacter(HTMLParser* parser, Token& token)
{
    if (isSpace(token.getChar()))
        parser->insertCharacter(token);
    return anythingElse(parser, token);
}

bool HTMLParser::InFrameset::processStartTag(HTMLParser* parser, Token& token)
{
    if (token.getName() == u"html")
        return parser->inBody.processCharacter(parser, token);
    if (token.getName() == u"frameset") {
        parser->insertHtmlElement(token);
        return true;
    }
    if (token.getName() == u"frame") {
        parser->insertHtmlElement(token);
        parser->popElement();
        token.acknowledge();
        return true;
    }
    if (token.getName() == u"noframes")
        return parser->inHead.processStartTag(parser, token);
    return anythingElse(parser, token);
}

bool HTMLParser::InFrameset::processEndTag(HTMLParser* parser, Token& token)
{
    if (token.getName() == u"frameset") {
        if (parser->currentNode().getLocalName() == u"html") {
            parser->parseError();
            return false;
        }
        parser->popElement();
        if (!parser->innerHTML && parser->currentNode().getLocalName() != u"frameset")
            parser->setInsertionMode(&parser->afterFrameset);
        return true;
    }
    return anythingElse(parser, token);
}

//
// HTMLParser::AfterFrameset
//

bool HTMLParser::AfterFrameset::anythingElse(HTMLParser* parser, Token& token)
{
    parser->parseError();
    return false;
}

bool HTMLParser::AfterFrameset::processEOF(HTMLParser* parser, Token& token)
{
    return parser->stopParsing();
}

bool HTMLParser::AfterFrameset::processComment(HTMLParser* parser, Token& token)
{
    Comment comment = parser->document.createComment(token.getName());
    parser->currentNode().appendChild(comment);
    return true;
}

bool HTMLParser::AfterFrameset::processDoctype(HTMLParser* parser, Token& token)
{
    parser->parseError();
    return false;
}

bool HTMLParser::AfterFrameset::processCharacter(HTMLParser* parser, Token& token)
{
    if (isSpace(token.getChar()))
        parser->insertCharacter(token);
    return anythingElse(parser, token);
}

bool HTMLParser::AfterFrameset::processStartTag(HTMLParser* parser, Token& token)
{
    if (token.getName() == u"html")
        return parser->inBody.processStartTag(parser, token);
    if (token.getName() == u"noframes")
        return parser->inHead.processStartTag(parser, token);
    return anythingElse(parser, token);
}

bool HTMLParser::AfterFrameset::processEndTag(HTMLParser* parser, Token& token)
{
    if (token.getName() == u"html") {
        parser->setInsertionMode(&parser->afterAfterFrameset);
        return true;
    }
    return anythingElse(parser, token);
}

//
// HTMLParser::AfterAfterBody
//

bool HTMLParser::AfterAfterBody::anythingElse(HTMLParser* parser, Token& token)
{
    parser->parseError();
    parser->setInsertionMode(&parser->inBody, token);
    return false;
}

bool HTMLParser::AfterAfterBody::processEOF(HTMLParser* parser, Token& token)
{
    return parser->stopParsing();
}

bool HTMLParser::AfterAfterBody::processComment(HTMLParser* parser, Token& token)
{
    Comment comment = parser->document.createComment(token.getName());
    parser->document.appendChild(comment);
    return true;
}

bool HTMLParser::AfterAfterBody::processDoctype(HTMLParser* parser, Token& token)
{
    return parser->inBody.processDoctype(parser, token);
}

bool HTMLParser::AfterAfterBody::processCharacter(HTMLParser* parser, Token& token)
{
    if (isSpace(token.getChar()))
        return parser->inBody.processCharacter(parser, token);
    return anythingElse(parser, token);
}

bool HTMLParser::AfterAfterBody::processStartTag(HTMLParser* parser, Token& token)
{
    if (token.getName() == u"html")
        return parser->inBody.processStartTag(parser, token);
    return anythingElse(parser, token);
}

bool HTMLParser::AfterAfterBody::processEndTag(HTMLParser* parser, Token& token)
{
    return anythingElse(parser, token);
}

//
// HTMLParser::AfterAfterFrameset
//

bool HTMLParser::AfterAfterFrameset::anythingElse(HTMLParser* parser, Token& token)
{
    parser->parseError();
    parser->setInsertionMode(&parser->inBody);
    return false;
}

bool HTMLParser::AfterAfterFrameset::processEOF(HTMLParser* parser, Token& token)
{
    return parser->stopParsing();
}

bool HTMLParser::AfterAfterFrameset::processComment(HTMLParser* parser, Token& token)
{
    Document document = parser->document;
    Comment comment = document.createComment(token.getName());
    document.appendChild(comment);
    return true;
}

bool HTMLParser::AfterAfterFrameset::processDoctype(HTMLParser* parser, Token& token)
{
    return parser->inBody.processDoctype(parser, token);
}

bool HTMLParser::AfterAfterFrameset::processCharacter(HTMLParser* parser, Token& token)
{
    if (isSpace(token.getChar()))
        return parser->inBody.processCharacter(parser, token);
    return anythingElse(parser, token);
}

bool HTMLParser::AfterAfterFrameset::processStartTag(HTMLParser* parser, Token& token)
{
    if (token.getName() == u"html")
        return parser->inBody.processStartTag(parser, token);
    if (token.getName() == u"noframes")
        return parser->inHead.processStartTag(parser, token);
    return anythingElse(parser, token);
}

bool HTMLParser::AfterAfterFrameset::processEndTag(HTMLParser* parser, Token& token)
{
    return anythingElse(parser, token);
}

//
// HTMLParser::InBinding
//

bool HTMLParser::InBinding::anythingElse(HTMLParser* parser, Token& token)
{
    return parser->inBody.processToken(parser, token);
}

bool HTMLParser::InBinding::processEOF(HTMLParser* parser, Token& token)
{
    return anythingElse(parser, token);
}

bool HTMLParser::InBinding::processComment(HTMLParser* parser, Token& token)
{
    return anythingElse(parser, token);
}

bool HTMLParser::InBinding::processDoctype(HTMLParser* parser, Token& token)
{
    return anythingElse(parser, token);
}

bool HTMLParser::InBinding::processCharacter(HTMLParser* parser, Token& token)
{
    return anythingElse(parser, token);
}

bool HTMLParser::InBinding::processStartTag(HTMLParser* parser, Token& token)
{
    if (token.getName() == u"implementation") {
        parser->insertHtmlElement(token);
        parser->tokenizer->setState(&HTMLTokenizer::scriptDataState);
        parser->originalInsertionMode = parser->insertionMode;
        parser->setInsertionMode(&text);
        // TODO: misc.
        return false;
    }
    return anythingElse(parser, token);
}

bool HTMLParser::InBinding::processEndTag(HTMLParser* parser, Token& token)
{
    static Token endTagBinding(Token::Type::EndTag, u"binding");

    if (token.getName() == u"binding") {
        if (!parser->elementInScope(token.getName())) {
            parser->parseError();
            return false;
        }
        while (parser->popElement().getLocalName() != u"binding")
            ;
        parser->setInsertionMode(&parser->inHead);
        return true;
    }
    if (token.getName() == u"head") {
        if (processEndTag(parser, endTagBinding))
            return parser->processToken(token);
        return false;
    }
    return anythingElse(parser, token);
}

HTMLParser::HTMLParser(Document document, HTMLTokenizer* tokenizer, bool enableXBL) :
    document(document),
    tokenizer(tokenizer),
    insertionMode(&initial),
    originalInsertionMode(0),
    secondaryInsertionMode(0),
    scriptNestingLevel(0),
    pauseFlag(false),
    headElement(0),
    formElement(0),
    scriptingFlag(true),
    framesetOkFlag(false),
    insertFromTable(false),
    innerHTML(false),
    enableXBL(enableXBL)
{
}

bool HTMLParser::processToken(Token& token)
{
    return insertionMode->processToken(this, token);
}

bool HTMLParser::mainLoop()
{
    Token token;
    do {
        token = tokenizer->getToken();
        processToken(token);
    } while (token.getType() != Token::Type::EndOfFile);
}