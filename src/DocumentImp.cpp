/*
 * Copyright 2010-2015 Esrille Inc.
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

#include "DocumentImp.h"

#include <time.h>

#include <algorithm>
#include <new>
#include <vector>
#include <boost/bind.hpp>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>

#include <org/w3c/dom/events/MouseEvent.h>

#include "CommentImp.h"
#include "DOMImplementationImp.h"
#include "DocumentTypeImp.h"
#include "ElementImp.h"
#include "EventImp.h"
#include "NodeListImp.h"
#include "ObjectArrayImp.h"
#include "RangeImp.h"
#include "TextImp.h"
#include "WindowProxy.h"
#include "XMLDocumentImp.h"
#include "css/CSSSerialize.h"
#include "html/HTMLAnchorElementImp.h"
#include "html/HTMLAppletElementImp.h"
#include "html/HTMLAreaElementImp.h"
#include "html/HTMLAudioElementImp.h"
#include "html/HTMLBaseElementImp.h"
#include "html/HTMLBindingElementImp.h"
#include "html/HTMLBodyElementImp.h"
#include "html/HTMLBRElementImp.h"
#include "html/HTMLButtonElementImp.h"
#include "html/HTMLCanvasElementImp.h"
#include "html/HTMLCollectionImp.h"
#include "html/HTMLCommandElementImp.h"
#include "html/HTMLDataListElementImp.h"
#include "html/HTMLDetailsElementImp.h"
#include "html/HTMLDivElementImp.h"
#include "html/HTMLDListElementImp.h"
#include "html/HTMLEmbedElementImp.h"
#include "html/HTMLFieldSetElementImp.h"
#include "html/HTMLFormElementImp.h"
#include "html/HTMLFontElementImp.h"
#include "html/HTMLHeadElementImp.h"
#include "html/HTMLHeadingElementImp.h"
#include "html/HTMLHRElementImp.h"
#include "html/HTMLHtmlElementImp.h"
#include "html/HTMLIFrameElementImp.h"
#include "html/HTMLImageElementImp.h"
#include "html/HTMLInputElementImp.h"
#include "html/HTMLKeygenElementImp.h"
#include "html/HTMLLabelElementImp.h"
#include "html/HTMLLegendElementImp.h"
#include "html/HTMLLIElementImp.h"
#include "html/HTMLLinkElementImp.h"
#include "html/HTMLMapElementImp.h"
#include "html/HTMLMarqueeElementImp.h"
#include "html/HTMLMenuElementImp.h"
#include "html/HTMLMetaElementImp.h"
#include "html/HTMLMeterElementImp.h"
#include "html/HTMLModElementImp.h"
#include "html/HTMLObjectElementImp.h"
#include "html/HTMLOListElementImp.h"
#include "html/HTMLOptGroupElementImp.h"
#include "html/HTMLOptionElementImp.h"
#include "html/HTMLOutputElementImp.h"
#include "html/HTMLParagraphElementImp.h"
#include "html/HTMLParamElementImp.h"
#include "html/HTMLPreElementImp.h"
#include "html/HTMLProgressElementImp.h"
#include "html/HTMLQuoteElementImp.h"
#include "html/HTMLScriptElementImp.h"
#include "html/HTMLSelectElementImp.h"
#include "html/HTMLSpanElementImp.h"
#include "html/HTMLSourceElementImp.h"
#include "html/HTMLStyleElementImp.h"
#include "html/HTMLTableCaptionElementImp.h"
#include "html/HTMLTableColElementImp.h"
#include "html/HTMLTableDataCellElementImp.h"
#include "html/HTMLTableElementImp.h"
#include "html/HTMLTableHeaderCellElementImp.h"
#include "html/HTMLTableRowElementImp.h"
#include "html/HTMLTableSectionElementImp.h"
#include "html/HTMLTemplateElementImp.h"
#include "html/HTMLTextAreaElementImp.h"
#include "html/HTMLTimeElementImp.h"
#include "html/HTMLTitleElementImp.h"
#include "html/HTMLUListElementImp.h"
#include "html/HTMLUnknownElementImp.h"
#include "html/HTMLVideoElementImp.h"

#include "Test.util.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

DocumentImp::DocumentImp(const std::u16string& url) :
    ObjectMixin(nullptr),
    url(url),
    mode(NoQuirksMode),
    readyState(u"loading"),
    compatMode(u"CSS1Compat"),
    loadEventDelayCount(1),
    contentLoaded(false),
    insertionPoint(0),
    lastModified(0),
    defaultView(0),
    error(0)
{
    nodeName = u"#document";
}

DocumentImp::~DocumentImp()
{
}

void DocumentImp::enter()
{
    if (defaultView)
        defaultView->enter();
}

void DocumentImp::exit()
{
    if (defaultView)
        defaultView->exit();
}

ECMAScriptContext* DocumentImp::getContext() const
{
    if (defaultView)
        return defaultView->getWindowPtr()->getContext();
    return 0;
}

void DocumentImp::setEventHandler(const std::u16string& type, Object handler)
{
    if (!defaultView)
        return;

    EventListenerPtr listener = getEventHandlerListener(type);
    if (listener) {
        listener->setEventHandler(handler);
        return;
    }
    listener = std::make_shared<EventListenerImp>(boost::bind(&ECMAScriptContext::dispatchEvent, getContext(), _1, _2));
    if (listener) {
        listener->setEventHandler(handler);
        addEventListener(type, listener, false, EventTargetImp::UseEventHandler);
    }
}

bool DocumentImp::processScripts(std::list<html::HTMLScriptElement>& scripts)
{
    while (!scripts.empty()) {
        auto script = std::dynamic_pointer_cast<HTMLScriptElementImp>(scripts.front().self());
        if (!script->isReadyToBeParserExecuted())
            return false;
        script->execute();
        scripts.pop_front();
        decrementLoadEventDelayCount(script->getSrc());
    }
    return true;
}

void DocumentImp::setCharacterSet(const std::u16string& charset)
{
    if (this->charset.empty()) {
        this->charset = charset;
        return;
    }
    // TODO: Change the character encoding.
}

void DocumentImp::setReadyState(const std::u16string& readyState)
{
    if (this->readyState == readyState)
        return;
    this->readyState = readyState;
    if (readyState == u"complete") {
        if (defaultView) {
            events::Event event = std::make_shared<EventImp>();
            event.initEvent(u"load", false, false);
            defaultView->dispatchEvent(event);
            if (auto frame = std::dynamic_pointer_cast<HTMLIFrameElementImp>(defaultView->getFrameElementImp()))
                frame->notify(getError());
            if (defaultView->isBindingDocumentWindow()) {
                if (auto parent = defaultView->getParentProxy())
                    parent->setViewFlags(Box::NEED_SELECTOR_MATCHING);
            }
        }
    }
}

void DocumentImp::resetStyleSheets()
{
    clearStyleSheets();
    html::HTMLHeadElement head = getHead();
    if (!head)
        return;
    for (auto element = head.getFirstElementChild(); element; element = element.getNextElementSibling()) {
        if (html::HTMLStyleElement::hasInstance(element)) {
            html::HTMLStyleElement styleElement = interface_cast<html::HTMLStyleElement>(element);
            stylesheets::StyleSheet styleSheet = styleElement.getSheet();
            addStyleSheet(styleSheet);
        } else if (html::HTMLLinkElement::hasInstance(element)) {
            html::HTMLLinkElement linkElement = interface_cast<html::HTMLLinkElement>(element);
            stylesheets::StyleSheet styleSheet = linkElement.getSheet();
            addStyleSheet(styleSheet);
        }
    }
}

void DocumentImp::setURL(const std::u16string& url)
{
    this->url = url;
}

void DocumentImp::setContentType(const std::u16string& type)
{
    contentType = type;
}

void DocumentImp::setDoctype(DocumentType type)
{
    doctype = type;
}

void DocumentImp::setFocus(const ElementPtr& element)
{
    ElementPtr active = getFocus();
    if (active == element)
        return;
    if (active) {
        events::Event event = std::make_shared<EventImp>();
        event.initEvent(u"blur", false, false);
        active->dispatchEvent(event);
    }
    activeElement = element;
    if (element) {
        events::Event event = std::make_shared<EventImp>();
        event.initEvent(u"focus", false, false);
        element->dispatchEvent(event);
    }
}

unsigned DocumentImp::incrementLoadEventDelayCount(const std::u16string& href) {
    unsigned count = ++loadEventDelayCount;
    if (1 <= getLogLevel()) {
        recordTime("incrementLoadEventDelayCount(%u): %s", count, utfconv(href).c_str());
        loadingList.push_back(href);
    }
    return count;
}

unsigned DocumentImp::decrementLoadEventDelayCount(const std::u16string& href) {
    assert(0 < loadEventDelayCount);
    unsigned count = --loadEventDelayCount;
    if (count == 0)
        setReadyState(u"complete");

    if (1 <= getLogLevel()) {
        recordTime("decrementLoadEventDelayCount(%u): %s", count, utfconv(href).c_str());
        if (count == 0)
            loadingList.clear();
        else {
            for (auto i = loadingList.begin(); i != loadingList.end(); ++i) {
                if (*i == href) {
                    loadingList.erase(i);
                    break;
                }
            }
            if (count < 3) {
                for (auto i = loadingList.begin(); i != loadingList.end(); ++i)
                    std::cout << "  * " << *i << '\n';
            }
        }
    }

    return count;
}

void DocumentImp::appendRange(const RangePtr& range)
{
    rangeList.push_back(range);
}

void DocumentImp::eraseRange(const RangePtr& range)
{
    for (auto i = rangeList.begin(); i != rangeList.end(); ++i) {
        if (i->lock() == range) {
            rangeList.erase(i);
            break;
        }
    }
}


// Document

DOMImplementation DocumentImp::getImplementation()
{
    return getDOMImplementation();
}

std::u16string DocumentImp::getURL()
{
    return url;
}

std::u16string DocumentImp::getDocumentURI()
{
    return url;
}

void DocumentImp::setDocumentURI(const std::u16string& documentURI)
{
    if (defaultView)
        defaultView->setLocation(documentURI);
}

std::u16string DocumentImp::getCompatMode()
{
    return compatMode;
}

std::u16string DocumentImp::getCharacterSet()
{
    return charset;
}

std::u16string DocumentImp::getContentType()
{
    return contentType;
}

DocumentType DocumentImp::getDoctype()
{
    return doctype;
}

Element DocumentImp::getDocumentElement()
{
    for (auto i = getFirstChild(); i; i = i.getNextSibling())
        if (i.getNodeType() == Node::ELEMENT_NODE)
            return interface_cast<Element>(i);
    return nullptr;
}

html::HTMLCollection DocumentImp::getElementsByTagName(const std::u16string& localName)
{
    return ElementImp::getElementsByTagName(std::dynamic_pointer_cast<ElementImp>(getDocumentElement().self()), localName);
}

html::HTMLCollection DocumentImp::getElementsByTagNameNS(const Nullable<std::u16string>& _namespace, const std::u16string& localName)
{
    // TODO: implement me!
    return nullptr;
}

html::HTMLCollection DocumentImp::getElementsByClassName(const std::u16string& classNames)
{
    return ElementImp::getElementsByClassName(std::dynamic_pointer_cast<ElementImp>(getDocumentElement().self()), classNames);
}

Element DocumentImp::getElementById(const std::u16string& elementId)
{
    auto e = std::dynamic_pointer_cast<ElementImp>(getDocumentElement().self());
    while (e) {
        Nullable<std::u16string> id = e->getAttribute(u"id");
        if (id.hasValue() && id.value() == elementId)
            return e;
        e = e->getNextElement();
    }
    return nullptr;
}

Element DocumentImp::createElement(const std::u16string& localName)
{
    std::u16string name(localName);
    if (!dynamic_cast<XMLDocumentImp*>(this))
        toLower(name);

    // Checked in the order of descriptions in the HTML specification
    if (name == u"html")
        return std::make_shared<HTMLHtmlElementImp>(this);
    if (name == u"head")
        return std::make_shared<HTMLHeadElementImp>(this);
    if (name == u"title")
        return std::make_shared<HTMLTitleElementImp>(this);
    if (name == u"base")
        return std::make_shared<HTMLBaseElementImp>(this);
    if (name == u"link")
        return std::make_shared<HTMLLinkElementImp>(this);
    if (name == u"meta")
        return std::make_shared<HTMLMetaElementImp>(this);
    if (name == u"style")
        return std::make_shared<HTMLStyleElementImp>(this);
    if (name == u"script")
        return std::make_shared<HTMLScriptElementImp>(this);
    if (name == u"noscript")
        return std::make_shared<HTMLElementImp>(this, name);
    if (name == u"body")
        return std::make_shared<HTMLBodyElementImp>(this);
    if (name == u"section" ||
        name == u"nav" ||
        name == u"article" ||
        name == u"aside")
        return std::make_shared<HTMLElementImp>(this, name);
    if (name == u"h1" ||
        name == u"h2" ||
        name == u"h3" ||
        name == u"h4" ||
        name == u"h5" ||
        name == u"h6")
        return std::make_shared<HTMLHeadingElementImp>(this, name);
    if (name == u"hgroup" ||
        name == u"header" ||
        name == u"footer" ||
        name == u"address")
        return std::make_shared<HTMLElementImp>(this, name);
    if (name == u"p")
        return std::make_shared<HTMLParagraphElementImp>(this);
    if (name == u"hr")
        return std::make_shared<HTMLHRElementImp>(this);
    if (name == u"pre")
        return std::make_shared<HTMLPreElementImp>(this);
    if (name == u"blockquote")
        return std::make_shared<HTMLQuoteElementImp>(this, name);
    if (name == u"ol")
        return std::make_shared<HTMLOListElementImp>(this);
    if (name == u"ul")
        return std::make_shared<HTMLUListElementImp>(this);
    if (name == u"li")
        return std::make_shared<HTMLLIElementImp>(this);
    if (name == u"dl")
        return std::make_shared<HTMLDListElementImp>(this);
    if (name == u"dt" ||
        name == u"dd" ||
        name == u"figure" ||
        name == u"figcaption")
        return std::make_shared<HTMLElementImp>(this, name);
    if (name == u"div")
        return std::make_shared<HTMLDivElementImp>(this);
    if (name == u"a")
        return std::make_shared<HTMLAnchorElementImp>(this);
    if (name == u"em" ||
        name == u"strong" ||
        name == u"small" ||
        name == u"s" ||
        name == u"cite")
        return std::make_shared<HTMLElementImp>(this, name);
    if (name == u"q")
        return std::make_shared<HTMLQuoteElementImp>(this, name);
    if (name == u"dfn" ||
        name == u"abbr")
        return std::make_shared<HTMLElementImp>(this, name);
    if (name == u"time")
        return std::make_shared<HTMLTimeElementImp>(this);
    if (name == u"code" ||
        name == u"var" ||
        name == u"samp" ||
        name == u"kbd" ||
        name == u"sub" ||
        name == u"sup" ||
        name == u"i" ||
        name == u"b" ||
        name == u"u" ||
        name == u"mark" ||
        name == u"ruby" ||
        name == u"rt" ||
        name == u"rp" ||
        name == u"bdi" ||
        name == u"bdo")
        return std::make_shared<HTMLElementImp>(this, name);
    if (name == u"span")
        return std::make_shared<HTMLSpanElementImp>(this);
    if (name == u"br")
        return std::make_shared<HTMLBRElementImp>(this);
    if (name == u"wbr")
        return std::make_shared<HTMLElementImp>(this, name);
    if (name == u"ins" ||
        name == u"del")
        return std::make_shared<HTMLModElementImp>(this, name);
    if (name == u"img")
        return std::make_shared<HTMLImageElementImp>(this);
    if (name == u"iframe") {
        auto context = getDefaultWindow();
        assert(context);
        auto iframe = std::make_shared<HTMLIFrameElementImp>(this);
        iframe->open(u"about:blank", context->isDeskTop() ? WindowProxy::TopLevel : 0);
        return iframe;
    }
    if (name == u"embed")
        return std::make_shared<HTMLEmbedElementImp>(this);
    if (name == u"object")
        return std::make_shared<HTMLObjectElementImp>(this);
    if (name == u"param")
        return std::make_shared<HTMLParamElementImp>(this);
    if (name == u"video")
        return std::make_shared<HTMLVideoElementImp>(this);
    if (name == u"audio")
        return std::make_shared<HTMLAudioElementImp>(this);
    if (name == u"source")
        return std::make_shared<HTMLSourceElementImp>(this);
    if (name == u"canvas")
        return std::make_shared<HTMLCanvasElementImp>(this);
    if (name == u"map")
        return std::make_shared<HTMLMapElementImp>(this);
    if (name == u"area")
        return std::make_shared<HTMLAreaElementImp>(this);
    if (name == u"table")
        return std::make_shared<HTMLTableElementImp>(this);
    if (name == u"caption")
        return std::make_shared<HTMLTableCaptionElementImp>(this);
    if (name == u"colgroup" ||
        name == u"col")
        return std::make_shared<HTMLTableColElementImp>(this, name);
    if (name == u"tbody" ||
        name == u"thead" ||
        name == u"tfoot")
        return std::make_shared<HTMLTableSectionElementImp>(this, name);
    if (name == u"tr")
        return std::make_shared<HTMLTableRowElementImp>(this);
    if (name == u"td")
        return std::make_shared<HTMLTableDataCellElementImp>(this);
    if (name == u"th")
        return std::make_shared<HTMLTableHeaderCellElementImp>(this);
    if (name == u"form")
        return std::make_shared<HTMLFormElementImp>(this);
    if (name == u"fieldset")
        return std::make_shared<HTMLFieldSetElementImp>(this);
    if (name == u"legend")
        return std::make_shared<HTMLLegendElementImp>(this);
    if (name == u"label")
        return std::make_shared<HTMLLabelElementImp>(this);
    if (name == u"input")
        return std::make_shared<HTMLInputElementImp>(this);
    if (name == u"button")
        return std::make_shared<HTMLButtonElementImp>(this);
    if (name == u"select")
        return std::make_shared<HTMLSelectElementImp>(this);
    if (name == u"datalist")
        return std::make_shared<HTMLDataListElementImp>(this);
    if (name == u"optgroup")
        return std::make_shared<HTMLOptGroupElementImp>(this);
    if (name == u"option")
        return std::make_shared<HTMLOptionElementImp>(this);
    if (name == u"textarea")
        return std::make_shared<HTMLTextAreaElementImp>(this);
    if (name == u"keygen")
        return std::make_shared<HTMLKeygenElementImp>(this);
    if (name == u"output")
        return std::make_shared<HTMLOutputElementImp>(this);
    if (name == u"progress")
        return std::make_shared<HTMLProgressElementImp>(this);
    if (name == u"meter")
        return std::make_shared<HTMLMeterElementImp>(this);
    if (name == u"details")
        return std::make_shared<HTMLDetailsElementImp>(this);
    if (name == u"summary")
        return std::make_shared<HTMLElementImp>(this, name);
    if (name == u"command")
        return std::make_shared<HTMLCommandElementImp>(this);
    if (name == u"menu")
        return std::make_shared<HTMLMenuElementImp>(this);

    if (name == u"binding")
        return std::make_shared<HTMLBindingElementImp>(this);
    if (name == u"template")
        return std::make_shared<HTMLTemplateElementImp>(this);
    if (name == u"implementation")
        return std::make_shared<HTMLScriptElementImp>(this, name);

    // Deprecated elements
    if (name == u"applet")
        return std::make_shared<HTMLAppletElementImp>(this);
    if (name == u"center")   // shorthand for DIV align=center
        return std::make_shared<HTMLDivElementImp>(this, name);
    if (name == u"font")
        return std::make_shared<HTMLFontElementImp>(this);
    if (name == u"marquee")
        return std::make_shared<HTMLMarqueeElementImp>(this);

    return std::make_shared<HTMLUnknownElementImp>(this, name);
}

Element DocumentImp::createElementNS(const Nullable<std::u16string>& namespaceURI, const std::u16string& qualifiedName)
{
    // TODO: Check name productions
    std::u16string prefix;
    std::u16string localName;
    size_t pos = qualifiedName.find(u':');
    if (pos != std::u16string::npos) {
        prefix = qualifiedName.substr(0, pos);
        localName = qualifiedName.substr(pos + 1);
    } else
        localName = qualifiedName;
    if (!prefix.empty() && !namespaceURI.hasValue())
        throw DOMException{DOMException::NAMESPACE_ERR};
    if (prefix == u"xml" && namespaceURI != u"http://www.w3.org/XML/1998/namespace")
        throw DOMException{DOMException::NAMESPACE_ERR};
    if (namespaceURI == u"http://www.w3.org/2000/xmlns/" && prefix != u"xmlns" && qualifiedName != u"xmlns")
        throw DOMException{DOMException::NAMESPACE_ERR};

    if (namespaceURI == u"http://www.w3.org/1999/xhtml" && prefix.empty())  // TODO: Check prefix
        return createElement(localName);

    return std::make_shared<ElementImp>(this, localName, namespaceURI, prefix);
}

DocumentFragment DocumentImp::createDocumentFragment()
{
    // TODO: implement me!
    return nullptr;
}

Text DocumentImp::createTextNode(const std::u16string& data)
{
    return std::make_shared<TextImp>(this, data);
}

Comment DocumentImp::createComment(const std::u16string& data)
{
    return std::make_shared<CommentImp>(this, data);
}

ProcessingInstruction DocumentImp::createProcessingInstruction(const std::u16string& target, const std::u16string& data)
{
    // TODO: implement me!
    return nullptr;
}

Node DocumentImp::importNode(Node node, bool deep)
{
    // TODO: implement me!
    return nullptr;
}

Node DocumentImp::adoptNode(Node node)
{
    auto n = std::dynamic_pointer_cast<NodeImp>(node.self());
    if (!n)
        return nullptr;
    if (n->getNodeType() == Node::DOCUMENT_NODE)
        return nullptr;   // TODO: throw NotSupportedError
    if (auto parent = n->getParent())
        parent->removeChild(node);
    n->setOwnerDocument(std::static_pointer_cast<DocumentImp>(self()));
    // TODO: Change base URL for elements
    return node;
}

events::Event DocumentImp::createEvent(const std::u16string& eventInterfaceName)
{
    // TODO: implement me!
    return nullptr;
}

css::CSSStyleDeclaration DocumentImp::getOverrideStyle(Element elt, const Nullable<std::u16string>& pseudoElt)
{
    // TODO: implement me!
    return nullptr;
}

stylesheets::StyleSheetList DocumentImp::getStyleSheets()
{
    return std::make_shared<ObjectArrayImp<DocumentImp, stylesheets::StyleSheet, &DocumentImp::styleSheets>>(std::static_pointer_cast<DocumentImp>(self()));
}

Nullable<std::u16string> DocumentImp::getSelectedStyleSheetSet()
{
    // TODO: implement me!
    return u"";
}

void DocumentImp::setSelectedStyleSheetSet(const Nullable<std::u16string>& selectedStyleSheetSet)
{
    // TODO: implement me!
}

Nullable<std::u16string> DocumentImp::getLastStyleSheetSet()
{
    // TODO: implement me!
    return u"";
}

Nullable<std::u16string> DocumentImp::getPreferredStyleSheetSet()
{
    // TODO: implement me!
    return u"";
}

DOMStringList DocumentImp::getStyleSheetSets()
{
    // TODO: implement me!
    return nullptr;
}

void DocumentImp::enableStyleSheetsForSet(const Nullable<std::u16string>& name)
{
    // TODO: implement me!
}

Element DocumentImp::elementFromPoint(float x, float y)
{
    if (!defaultView)
        return nullptr;
    return defaultView->elementFromPoint(x, y);
}

CaretPosition DocumentImp::caretPositionFromPoint(float x, float y)
{
    // TODO: implement me!
    return nullptr;
}

html::Location DocumentImp::getLocation()
{
    if (!defaultView)
        return nullptr;
    return std::make_shared<LocationImp>(defaultView, url);
}

void DocumentImp::setLocation(const std::u16string& href)
{
    if (defaultView)
        defaultView->setLocation(href);
}

std::u16string DocumentImp::getDomain()
{
    // TODO: implement me!
    return u"";
}

void DocumentImp::setDomain(const std::u16string& domain)
{
    // TODO: implement me!
}

std::u16string DocumentImp::getReferrer()
{
    // TODO: implement me!
    return u"";
}

std::u16string DocumentImp::getCookie()
{
    // TODO: implement me!
    return u"";
}

void DocumentImp::setCookie(const std::u16string& cookie)
{
    // TODO: implement me!
}

std::u16string DocumentImp::getLastModified()
{
    time_t t = lastModified;
    if (t <= 0)
        time(&t);
    tm lm;
    localtime_r(&t, &lm);
    // MM/DD/YYYY hh:mm:ss
    boost::format fmt("%02u/%02u/%04u %2u:%02u:%02u");
    return utfconv((fmt % (lm.tm_mon + 1) % lm.tm_mday % (1900 + lm.tm_year) % lm.tm_hour % lm.tm_min % lm.tm_sec).str());
}

std::u16string DocumentImp::getReadyState()
{
    return readyState;
}

Any DocumentImp::getElement(const std::u16string& name)
{
    Element e = getDocumentElement();
    if (!e)
        return 0;
    HTMLCollectionPtr collection = std::make_shared<HTMLCollectionImp>();
    if (!collection)
        return 0;
    std::u16string tag;
    for (auto i = std::dynamic_pointer_cast<ElementImp>(e.self()); i; i = i->getNextElement()) {
        if (auto e = std::dynamic_pointer_cast<HTMLElementImp>(i)) {
            tag = e->getLocalName();
            // TODO: check applet, embed, and object, too.
            if (tag == u"form" || tag == u"iframe" || tag == u"img") {
                Nullable<std::u16string> n = i->getAttribute(u"name");
                if (n.hasValue() && name == n.value())
                    collection->addItem(i, name);
            }
        }
    }
    switch (collection->getLength()) {
    case 0:
        return Any();   // Return 'undefined' so that JSAPI bridge can call the default property stub.
    case 1:
        e = collection->item(0);
        if (tag == u"iframe")
            return interface_cast<html::HTMLIFrameElement>(e).getContentWindow();
        else
            return e;
    default:
        return collection;
    }
}

std::u16string DocumentImp::getTitle()
{
    html::HTMLHeadElement head = getHead();
    if (!head)
        return u"";
    for (auto i = head.getFirstElementChild(); i; i = i.getNextElementSibling()) {
        if (auto t = std::dynamic_pointer_cast<HTMLTitleElementImp>(i.self())) {
            std::u16string title = t->getText();
            size_t spacePos;
            size_t spaceLen = 0;
            for (size_t i = 0; i < title.length(); ++i) {
                char16_t c = title[i];
                if (isSpace(c)) {
                    c = title[i] = ' ';
                    if (spaceLen == 0)
                        spacePos = i;
                    ++spaceLen;
                } else if (0 < spaceLen) {
                    if (1 < spaceLen) {
                        ++spacePos;
                        --spaceLen;
                        title.erase(spacePos, spaceLen);
                        i = spacePos;
                    }
                    spaceLen = 0;
                }
            }
            return title;
        }
    }
    return u"";
}

void DocumentImp::setTitle(const std::u16string& title)
{
    html::HTMLHeadElement head = getHead();
    if (!head)
        return;
    for (auto i = head.getFirstElementChild(); i; i = i.getNextElementSibling()) {
        if (auto t = std::dynamic_pointer_cast<HTMLTitleElementImp>(i.self())) {
            t->setText(title);
            return;
        }
    }
    if (html::HTMLTitleElement t = interface_cast<html::HTMLTitleElement>(createElement(u"title"))) {
        t.setText(title);
        head.appendChild(t);
    }
}

std::u16string DocumentImp::getDir()
{
    // TODO: implement me!
    return u"";
}

void DocumentImp::setDir(const std::u16string& dir)
{
    // TODO: implement me!
}

html::HTMLElement DocumentImp::getBody()
{
    // TODO: refine.
    Element e = getDocumentElement();
    for (auto i = e.getFirstElementChild(); i; i = i.getNextElementSibling()) {
        if (std::dynamic_pointer_cast<HTMLBodyElementImp>(i.self()))
            return interface_cast<html::HTMLElement>(i);
    }
    return nullptr;
}

void DocumentImp::setBody(html::HTMLElement body)
{
    // TODO: implement me!
}

html::HTMLHeadElement DocumentImp::getHead()
{
    // TODO: refine.
    Element e = getDocumentElement();
    for (auto i = e.getFirstElementChild(); i; i = i.getNextElementSibling()) {
        if (std::dynamic_pointer_cast<HTMLHeadElementImp>(i.self()))
            return interface_cast<html::HTMLHeadElement>(i);
    }
    return nullptr;
}

html::HTMLCollection DocumentImp::getImages()
{
    // TODO: implement me!
    return nullptr;
}

html::HTMLCollection DocumentImp::getEmbeds()
{
    // TODO: implement me!
    return nullptr;
}

html::HTMLCollection DocumentImp::getPlugins()
{
    // TODO: implement me!
    return nullptr;
}

html::HTMLCollection DocumentImp::getLinks()
{
    // TODO: implement me!
    return nullptr;
}

html::HTMLCollection DocumentImp::getForms()
{
    // TODO: implement me!
    return nullptr;
}

html::HTMLCollection DocumentImp::getScripts()
{
    // TODO: implement me!
    return nullptr;
}

NodeList DocumentImp::getElementsByName(const std::u16string& elementName)
{
    // TODO: implement me!
    return nullptr;
}

Document DocumentImp::open(const std::u16string& type, const std::u16string& replace)
{
    // TODO: implement me!
    return nullptr;
}

html::Window DocumentImp::open(const std::u16string& url, const std::u16string& name, const std::u16string& features, bool replace)
{
    if (defaultView)
        return defaultView->open(url, name, features, replace);
    return nullptr;
}

void DocumentImp::close()
{
    // TODO: implement me!
}

void DocumentImp::write(const Variadic<std::u16string>& text, bool linefeed)
{
    if (dynamic_cast<XMLDocumentImp*>(this))
        throw DOMException{DOMException::INVALID_STATE_ERR};
    HTMLTokenizer* insertionPoint = getInsertionPoint();
    if (!insertionPoint) {
        // TODO: more items...
        return;
    }
    if (linefeed)
        insertionPoint->insertString(u"\n");
    for (size_t i = text.size(); 0 < i; )
        insertionPoint->insertString(text[--i]);
    // TODO: more items...
}

void DocumentImp::write(Variadic<std::u16string> text)
{
    write(text, false);
}

void DocumentImp::writeln(Variadic<std::u16string> text)
{
    write(text, true);
}

html::Window DocumentImp::getDefaultView()
{
    return defaultView;
}

Element DocumentImp::getActiveElement()
{
    if (auto active = getFocus())
        return active ;
    return getBody();
}

bool DocumentImp::hasFocus()
{
    // TODO: implement me!
    return 0;
}

std::u16string DocumentImp::getDesignMode()
{
    // TODO: implement me!
    return u"";
}

void DocumentImp::setDesignMode(const std::u16string& designMode)
{
    // TODO: implement me!
}

bool DocumentImp::execCommand(const std::u16string& commandId)
{
    // TODO: implement me!
    return 0;
}

bool DocumentImp::execCommand(const std::u16string& commandId, bool showUI)
{
    // TODO: implement me!
    return 0;
}

bool DocumentImp::execCommand(const std::u16string& commandId, bool showUI, const std::u16string& value)
{
    // TODO: implement me!
    return 0;
}

bool DocumentImp::queryCommandEnabled(const std::u16string& commandId)
{
    // TODO: implement me!
    return 0;
}

bool DocumentImp::queryCommandIndeterm(const std::u16string& commandId)
{
    // TODO: implement me!
    return 0;
}

bool DocumentImp::queryCommandState(const std::u16string& commandId)
{
    // TODO: implement me!
    return 0;
}

bool DocumentImp::queryCommandSupported(const std::u16string& commandId)
{
    // TODO: implement me!
    return 0;
}

std::u16string DocumentImp::queryCommandValue(const std::u16string& commandId)
{
    // TODO: implement me!
    return u"";
}

html::HTMLCollection DocumentImp::getCommands()
{
    // TODO: implement me!
    return nullptr;
}

events::EventHandlerNonNull DocumentImp::getOnabort()
{
    return interface_cast<events::EventHandlerNonNull>(getEventHandler(u"abort"));
}

void DocumentImp::setOnabort(events::EventHandlerNonNull onabort)
{
    setEventHandler(u"abort", onabort);
}

events::EventHandlerNonNull DocumentImp::getOnblur()
{
    return interface_cast<events::EventHandlerNonNull>(getEventHandler(u"blur"));
}

void DocumentImp::setOnblur(events::EventHandlerNonNull onblur)
{
    setEventHandler(u"blur", onblur);
}

events::EventHandlerNonNull DocumentImp::getOncancel()
{
    return interface_cast<events::EventHandlerNonNull>(getEventHandler(u"cancel"));
}

void DocumentImp::setOncancel(events::EventHandlerNonNull oncancel)
{
    setEventHandler(u"cancel", oncancel);
}

events::EventHandlerNonNull DocumentImp::getOncanplay()
{
    return interface_cast<events::EventHandlerNonNull>(getEventHandler(u"canplay"));
}

void DocumentImp::setOncanplay(events::EventHandlerNonNull oncanplay)
{
    setEventHandler(u"canplay", oncanplay);
}

events::EventHandlerNonNull DocumentImp::getOncanplaythrough()
{
    return interface_cast<events::EventHandlerNonNull>(getEventHandler(u"canplaythrough"));
}

void DocumentImp::setOncanplaythrough(events::EventHandlerNonNull oncanplaythrough)
{
    setEventHandler(u"canplaythrough", oncanplaythrough);
}

events::EventHandlerNonNull DocumentImp::getOnchange()
{
    return interface_cast<events::EventHandlerNonNull>(getEventHandler(u"change"));
}

void DocumentImp::setOnchange(events::EventHandlerNonNull onchange)
{
    setEventHandler(u"change", onchange);
}

events::EventHandlerNonNull DocumentImp::getOnclick()
{
    return interface_cast<events::EventHandlerNonNull>(getEventHandler(u"click"));
}

void DocumentImp::setOnclick(events::EventHandlerNonNull onclick)
{
    setEventHandler(u"click", onclick);
}

events::EventHandlerNonNull DocumentImp::getOnclose()
{
    return interface_cast<events::EventHandlerNonNull>(getEventHandler(u"close"));
}

void DocumentImp::setOnclose(events::EventHandlerNonNull onclose)
{
    setEventHandler(u"close", onclose);
}

events::EventHandlerNonNull DocumentImp::getOncontextmenu()
{
    return interface_cast<events::EventHandlerNonNull>(getEventHandler(u"contextmenu"));
}

void DocumentImp::setOncontextmenu(events::EventHandlerNonNull oncontextmenu)
{
    setEventHandler(u"contextmenu", oncontextmenu);
}

events::EventHandlerNonNull DocumentImp::getOncuechange()
{
    return interface_cast<events::EventHandlerNonNull>(getEventHandler(u"cuechange"));
}

void DocumentImp::setOncuechange(events::EventHandlerNonNull oncuechange)
{
    setEventHandler(u"cuechange", oncuechange);
}

events::EventHandlerNonNull DocumentImp::getOndblclick()
{
    return interface_cast<events::EventHandlerNonNull>(getEventHandler(u"dblclick"));
}

void DocumentImp::setOndblclick(events::EventHandlerNonNull ondblclick)
{
    setEventHandler(u"dblclick", ondblclick);
}

events::EventHandlerNonNull DocumentImp::getOndrag()
{
    return interface_cast<events::EventHandlerNonNull>(getEventHandler(u"drag"));
}

void DocumentImp::setOndrag(events::EventHandlerNonNull ondrag)
{
    setEventHandler(u"drag", ondrag);
}

events::EventHandlerNonNull DocumentImp::getOndragend()
{
    return interface_cast<events::EventHandlerNonNull>(getEventHandler(u"dragend"));
}

void DocumentImp::setOndragend(events::EventHandlerNonNull ondragend)
{
    setEventHandler(u"dragend", ondragend);
}

events::EventHandlerNonNull DocumentImp::getOndragenter()
{
    return interface_cast<events::EventHandlerNonNull>(getEventHandler(u"dragenter"));
}

void DocumentImp::setOndragenter(events::EventHandlerNonNull ondragenter)
{
    setEventHandler(u"dragenter", ondragenter);
}

events::EventHandlerNonNull DocumentImp::getOndragleave()
{
    return interface_cast<events::EventHandlerNonNull>(getEventHandler(u"dragleave"));
}

void DocumentImp::setOndragleave(events::EventHandlerNonNull ondragleave)
{
    setEventHandler(u"dragleave", ondragleave);
}

events::EventHandlerNonNull DocumentImp::getOndragover()
{
    return interface_cast<events::EventHandlerNonNull>(getEventHandler(u"dragover"));
}

void DocumentImp::setOndragover(events::EventHandlerNonNull ondragover)
{
    setEventHandler(u"dragover", ondragover);
}

events::EventHandlerNonNull DocumentImp::getOndragstart()
{
    return interface_cast<events::EventHandlerNonNull>(getEventHandler(u"dragstart"));
}

void DocumentImp::setOndragstart(events::EventHandlerNonNull ondragstart)
{
    setEventHandler(u"dragstart", ondragstart);
}

events::EventHandlerNonNull DocumentImp::getOndrop()
{
    return interface_cast<events::EventHandlerNonNull>(getEventHandler(u"drop"));
}

void DocumentImp::setOndrop(events::EventHandlerNonNull ondrop)
{
    setEventHandler(u"drop", ondrop);
}

events::EventHandlerNonNull DocumentImp::getOndurationchange()
{
    return interface_cast<events::EventHandlerNonNull>(getEventHandler(u"durationchange"));
}

void DocumentImp::setOndurationchange(events::EventHandlerNonNull ondurationchange)
{
    setEventHandler(u"durationchange", ondurationchange);
}

events::EventHandlerNonNull DocumentImp::getOnemptied()
{
    return interface_cast<events::EventHandlerNonNull>(getEventHandler(u"emptied"));
}

void DocumentImp::setOnemptied(events::EventHandlerNonNull onemptied)
{
    setEventHandler(u"emptied", onemptied);
}

events::EventHandlerNonNull DocumentImp::getOnended()
{
    return interface_cast<events::EventHandlerNonNull>(getEventHandler(u"ended"));
}

void DocumentImp::setOnended(events::EventHandlerNonNull onended)
{
    setEventHandler(u"ended", onended);
}

events::OnErrorEventHandlerNonNull DocumentImp::getOnerror()
{
    return interface_cast<events::OnErrorEventHandlerNonNull>(getEventHandler(u"error"));
}

void DocumentImp::setOnerror(events::OnErrorEventHandlerNonNull onerror)
{
    setEventHandler(u"error", onerror);
}

events::EventHandlerNonNull DocumentImp::getOnfocus()
{
    return interface_cast<events::EventHandlerNonNull>(getEventHandler(u"focus"));
}

void DocumentImp::setOnfocus(events::EventHandlerNonNull onfocus)
{
    setEventHandler(u"focus", onfocus);
}

events::EventHandlerNonNull DocumentImp::getOninput()
{
    return interface_cast<events::EventHandlerNonNull>(getEventHandler(u"input"));
}

void DocumentImp::setOninput(events::EventHandlerNonNull oninput)
{
    setEventHandler(u"input", oninput);
}

events::EventHandlerNonNull DocumentImp::getOninvalid()
{
    return interface_cast<events::EventHandlerNonNull>(getEventHandler(u"invalid"));
}

void DocumentImp::setOninvalid(events::EventHandlerNonNull oninvalid)
{
    setEventHandler(u"invalid", oninvalid);
}

events::EventHandlerNonNull DocumentImp::getOnkeydown()
{
    return interface_cast<events::EventHandlerNonNull>(getEventHandler(u"keydown"));
}

void DocumentImp::setOnkeydown(events::EventHandlerNonNull onkeydown)
{
    setEventHandler(u"keydown", onkeydown);
}

events::EventHandlerNonNull DocumentImp::getOnkeypress()
{
    return interface_cast<events::EventHandlerNonNull>(getEventHandler(u"keypress"));
}

void DocumentImp::setOnkeypress(events::EventHandlerNonNull onkeypress)
{
    setEventHandler(u"keypress", onkeypress);
}

events::EventHandlerNonNull DocumentImp::getOnkeyup()
{
    return interface_cast<events::EventHandlerNonNull>(getEventHandler(u"keyup"));
}

void DocumentImp::setOnkeyup(events::EventHandlerNonNull onkeyup)
{
    setEventHandler(u"keyup", onkeyup);
}

events::EventHandlerNonNull DocumentImp::getOnload()
{
    return interface_cast<events::EventHandlerNonNull>(getEventHandler(u"load"));
}

void DocumentImp::setOnload(events::EventHandlerNonNull onload)
{
    setEventHandler(u"load", onload);
}

events::EventHandlerNonNull DocumentImp::getOnloadeddata()
{
    return interface_cast<events::EventHandlerNonNull>(getEventHandler(u"loadeddata"));
}

void DocumentImp::setOnloadeddata(events::EventHandlerNonNull onloadeddata)
{
    setEventHandler(u"loadeddata", onloadeddata);
}

events::EventHandlerNonNull DocumentImp::getOnloadedmetadata()
{
    return interface_cast<events::EventHandlerNonNull>(getEventHandler(u"loadedmetadata"));
}

void DocumentImp::setOnloadedmetadata(events::EventHandlerNonNull onloadedmetadata)
{
    setEventHandler(u"loadedmetadata", onloadedmetadata);
}

events::EventHandlerNonNull DocumentImp::getOnloadstart()
{
    return interface_cast<events::EventHandlerNonNull>(getEventHandler(u"loadstart"));
}

void DocumentImp::setOnloadstart(events::EventHandlerNonNull onloadstart)
{
    setEventHandler(u"loadstart", onloadstart);
}

events::EventHandlerNonNull DocumentImp::getOnmousedown()
{
    return interface_cast<events::EventHandlerNonNull>(getEventHandler(u"mousedown"));
}

void DocumentImp::setOnmousedown(events::EventHandlerNonNull onmousedown)
{
    setEventHandler(u"mousedown", onmousedown);
}

events::EventHandlerNonNull DocumentImp::getOnmousemove()
{
    return interface_cast<events::EventHandlerNonNull>(getEventHandler(u"mousemove"));
}

void DocumentImp::setOnmousemove(events::EventHandlerNonNull onmousemove)
{
    setEventHandler(u"mousemove", onmousemove);
}

events::EventHandlerNonNull DocumentImp::getOnmouseout()
{
    return interface_cast<events::EventHandlerNonNull>(getEventHandler(u"mouseout"));
}

void DocumentImp::setOnmouseout(events::EventHandlerNonNull onmouseout)
{
    setEventHandler(u"mouseout", onmouseout);
}

events::EventHandlerNonNull DocumentImp::getOnmouseover()
{
    return interface_cast<events::EventHandlerNonNull>(getEventHandler(u"mouseover"));
}

void DocumentImp::setOnmouseover(events::EventHandlerNonNull onmouseover)
{
    setEventHandler(u"mouseover", onmouseover);
}

events::EventHandlerNonNull DocumentImp::getOnmouseup()
{
    return interface_cast<events::EventHandlerNonNull>(getEventHandler(u"mouseup"));
}

void DocumentImp::setOnmouseup(events::EventHandlerNonNull onmouseup)
{
    setEventHandler(u"mouseup", onmouseup);
}

events::EventHandlerNonNull DocumentImp::getOnmousewheel()
{
    return interface_cast<events::EventHandlerNonNull>(getEventHandler(u"mousewheel"));
}

void DocumentImp::setOnmousewheel(events::EventHandlerNonNull onmousewheel)
{
    setEventHandler(u"mousewheel", onmousewheel);
}

events::EventHandlerNonNull DocumentImp::getOnpause()
{
    return interface_cast<events::EventHandlerNonNull>(getEventHandler(u"pause"));
}

void DocumentImp::setOnpause(events::EventHandlerNonNull onpause)
{
    setEventHandler(u"pause", onpause);
}

events::EventHandlerNonNull DocumentImp::getOnplay()
{
    return interface_cast<events::EventHandlerNonNull>(getEventHandler(u"play"));
}

void DocumentImp::setOnplay(events::EventHandlerNonNull onplay)
{
    setEventHandler(u"play", onplay);
}

events::EventHandlerNonNull DocumentImp::getOnplaying()
{
    return interface_cast<events::EventHandlerNonNull>(getEventHandler(u"playing"));
}

void DocumentImp::setOnplaying(events::EventHandlerNonNull onplaying)
{
    setEventHandler(u"playing", onplaying);
}

events::EventHandlerNonNull DocumentImp::getOnprogress()
{
    return interface_cast<events::EventHandlerNonNull>(getEventHandler(u"progress"));
}

void DocumentImp::setOnprogress(events::EventHandlerNonNull onprogress)
{
    setEventHandler(u"progress", onprogress);
}

events::EventHandlerNonNull DocumentImp::getOnratechange()
{
    return interface_cast<events::EventHandlerNonNull>(getEventHandler(u"ratechange"));
}

void DocumentImp::setOnratechange(events::EventHandlerNonNull onratechange)
{
    setEventHandler(u"ratechange", onratechange);
}

events::EventHandlerNonNull DocumentImp::getOnreset()
{
    return interface_cast<events::EventHandlerNonNull>(getEventHandler(u"reset"));
}

void DocumentImp::setOnreset(events::EventHandlerNonNull onreset)
{
    setEventHandler(u"reset", onreset);
}

events::EventHandlerNonNull DocumentImp::getOnscroll()
{
    return interface_cast<events::EventHandlerNonNull>(getEventHandler(u"scroll"));
}

void DocumentImp::setOnscroll(events::EventHandlerNonNull onscroll)
{
    setEventHandler(u"scroll", onscroll);
}

events::EventHandlerNonNull DocumentImp::getOnseeked()
{
    return interface_cast<events::EventHandlerNonNull>(getEventHandler(u"seeked"));
}

void DocumentImp::setOnseeked(events::EventHandlerNonNull onseeked)
{
    setEventHandler(u"seeked", onseeked);
}

events::EventHandlerNonNull DocumentImp::getOnseeking()
{
    return interface_cast<events::EventHandlerNonNull>(getEventHandler(u"seeking"));
}

void DocumentImp::setOnseeking(events::EventHandlerNonNull onseeking)
{
    setEventHandler(u"seeking", onseeking);
}

events::EventHandlerNonNull DocumentImp::getOnselect()
{
    return interface_cast<events::EventHandlerNonNull>(getEventHandler(u"select"));
}

void DocumentImp::setOnselect(events::EventHandlerNonNull onselect)
{
    setEventHandler(u"select", onselect);
}

events::EventHandlerNonNull DocumentImp::getOnshow()
{
    return interface_cast<events::EventHandlerNonNull>(getEventHandler(u"show"));
}

void DocumentImp::setOnshow(events::EventHandlerNonNull onshow)
{
    setEventHandler(u"show", onshow);
}

events::EventHandlerNonNull DocumentImp::getOnstalled()
{
    return interface_cast<events::EventHandlerNonNull>(getEventHandler(u"stalled"));
}

void DocumentImp::setOnstalled(events::EventHandlerNonNull onstalled)
{
    setEventHandler(u"stalled", onstalled);
}

events::EventHandlerNonNull DocumentImp::getOnsubmit()
{
    return interface_cast<events::EventHandlerNonNull>(getEventHandler(u"submit"));
}

void DocumentImp::setOnsubmit(events::EventHandlerNonNull onsubmit)
{
    setEventHandler(u"submit", onsubmit);
}

events::EventHandlerNonNull DocumentImp::getOnsuspend()
{
    return interface_cast<events::EventHandlerNonNull>(getEventHandler(u"suspend"));
}

void DocumentImp::setOnsuspend(events::EventHandlerNonNull onsuspend)
{
    setEventHandler(u"suspend", onsuspend);
}

events::EventHandlerNonNull DocumentImp::getOntimeupdate()
{
    return interface_cast<events::EventHandlerNonNull>(getEventHandler(u"timeupdate"));
}

void DocumentImp::setOntimeupdate(events::EventHandlerNonNull ontimeupdate)
{
    setEventHandler(u"timeupdate", ontimeupdate);
}

events::EventHandlerNonNull DocumentImp::getOnvolumechange()
{
    return interface_cast<events::EventHandlerNonNull>(getEventHandler(u"volumechange"));
}

void DocumentImp::setOnvolumechange(events::EventHandlerNonNull onvolumechange)
{
    setEventHandler(u"volumechange", onvolumechange);
}

events::EventHandlerNonNull DocumentImp::getOnwaiting()
{
    return interface_cast<events::EventHandlerNonNull>(getEventHandler(u"waiting"));
}

void DocumentImp::setOnwaiting(events::EventHandlerNonNull onwaiting)
{
    setEventHandler(u"waiting", onwaiting);
}

events::EventHandlerNonNull DocumentImp::getOnreadystatechange()
{
    return interface_cast<events::EventHandlerNonNull>(getEventHandler(u"readystatechange"));
}

void DocumentImp::setOnreadystatechange(events::EventHandlerNonNull onreadystatechange)
{
    setEventHandler(u"readystatechange", onreadystatechange);
}

std::u16string DocumentImp::getFgColor()
{
    // TODO: implement me!
    return u"";
}

void DocumentImp::setFgColor(const std::u16string& fgColor)
{
    // TODO: implement me!
}

std::u16string DocumentImp::getLinkColor()
{
    // TODO: implement me!
    return u"";
}

void DocumentImp::setLinkColor(const std::u16string& linkColor)
{
    // TODO: implement me!
}

std::u16string DocumentImp::getVlinkColor()
{
    // TODO: implement me!
    return u"";
}

void DocumentImp::setVlinkColor(const std::u16string& vlinkColor)
{
    // TODO: implement me!
}

std::u16string DocumentImp::getAlinkColor()
{
    // TODO: implement me!
    return u"";
}

void DocumentImp::setAlinkColor(const std::u16string& alinkColor)
{
    // TODO: implement me!
}

std::u16string DocumentImp::getBgColor()
{
    // TODO: implement me!
    return u"";
}

void DocumentImp::setBgColor(const std::u16string& bgColor)
{
    // TODO: implement me!
}

html::HTMLCollection DocumentImp::getAnchors()
{
    // TODO: implement me!
    return nullptr;
}

html::HTMLCollection DocumentImp::getApplets()
{
    // TODO: implement me!
    return nullptr;
}

void DocumentImp::clear()
{
    // TODO: implement me!
}

html::HTMLAllCollection DocumentImp::getAll()
{
    // TODO: implement me!
    return nullptr;
}

Element DocumentImp::querySelector(const std::u16string& selectors)
{
    if (auto e = std::dynamic_pointer_cast<ElementImp>(getDocumentElement().self()))
        return e->querySelector(selectors);
    return nullptr;
}

NodeList DocumentImp::querySelectorAll(const std::u16string& selectors)
{
    if (auto e = std::dynamic_pointer_cast<ElementImp>(getDocumentElement().self()))
        return e->querySelectorAll(selectors);
    return std::make_shared<NodeListImp>();
}

ranges::Range DocumentImp::createRange()
{
    try {
        RangePtr range = std::make_shared<RangeImp>(std::static_pointer_cast<DocumentImp>(self()));
        appendRange(range);
        return range;
    } catch (...) {
        return nullptr;
    }
}

traversal::NodeIterator DocumentImp::createNodeIterator(Node root, unsigned int whatToShow, traversal::NodeFilter filter)
{
    // TODO: implement me!
    return nullptr;
}

traversal::TreeWalker DocumentImp::createTreeWalker(Node root, unsigned int whatToShow, traversal::NodeFilter filter)
{
    // TODO: implement me!
    return nullptr;
}

html::HTMLCollection DocumentImp::getBindingDocuments()
{
    // TODO: implement me!
    return nullptr;
}

Document DocumentImp::loadBindingDocument(const std::u16string& documentURI)
{
    auto found = bindingDocuments.find(documentURI);
    if (found != bindingDocuments.end())
        return found->second->getDocument();

    WindowProxyPtr window = defaultView->createChildProxy();
    if (!window)
        return nullptr;
    window->setBase(defaultView->getLocation().getHref());
    bindingDocuments.insert(std::pair<std::u16string, WindowProxyPtr>(documentURI, window));
    window->open(documentURI, u"_self", u"", true);
    return window->getDocument();
}

bool DocumentImp::isBindingDocumentWindow(const WindowProxyPtr& window) const
{
    for (auto i = bindingDocuments.begin(); i != bindingDocuments.end(); ++i) {
        if (i->second == window)
            return true;
    }
    return false;
}

// Node - override

unsigned short DocumentImp::getNodeType()
{
    return Node::DOCUMENT_NODE;
}

Node DocumentImp::appendChild(Node newChild)
{
    if (auto newDoctype = std::dynamic_pointer_cast<DocumentTypeImp>(newChild.self())) {
        if (getDoctype())
            throw DOMException{DOMException::HIERARCHY_REQUEST_ERR};
        if (!newChild.getOwnerDocument())
            newDoctype->setOwnerDocument(std::static_pointer_cast<DocumentImp>(self()));
        NodeImp::appendChild(newChild);
        doctype = newDoctype;
        return newChild;
    }
    // TODO: check second Element
    return NodeImp::appendChild(newChild);
}

}  // org::w3c::dom::bootstrap

namespace {

class Constructor : public Object
{
public:
    // Object
    virtual Any message_(uint32_t selector, const char* id, int argc, Any* argv) {
        bootstrap::DocumentPtr doc;
        switch (argc) {
        case 0:
            doc = std::make_shared<bootstrap::DocumentImp>();
            break;
        default:
            break;
        }
        return doc;
    }
    Constructor() :
        Object(this) {
    }
};

}  // namespace

Object Document::getConstructor()
{
    static Constructor constructor;
    return constructor.self();
}

}}}  // org::w3c::dom::bootstrap

