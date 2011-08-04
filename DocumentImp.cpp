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

#include "DocumentImp.h"

#include <new>
#include <boost/bind.hpp>

#include <org/w3c/dom/events/MouseEvent.h>

#include "DOMImplementationImp.h"
#include "DocumentTypeImp.h"
#include "CommentImp.h"
#include "ElementImp.h"
#include "css/CSSSerialize.h"
#include "TextImp.h"
#include "html/HTMLAnchorElementImp.h"
#include "html/HTMLAreaElementImp.h"
#include "html/HTMLAudioElementImp.h"
#include "html/HTMLBaseElementImp.h"
#include "html/HTMLBodyElementImp.h"
#include "html/HTMLBRElementImp.h"
#include "html/HTMLButtonElementImp.h"
#include "html/HTMLCanvasElementImp.h"
#include "html/HTMLCommandElementImp.h"
#include "html/HTMLDataListElementImp.h"
#include "html/HTMLDetailsElementImp.h"
#include "html/HTMLDivElementImp.h"
#include "html/HTMLDListElementImp.h"
#include "html/HTMLEmbedElementImp.h"
#include "html/HTMLFieldSetElementImp.h"
#include "html/HTMLFormElementImp.h"
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
#include "html/HTMLTextAreaElementImp.h"
#include "html/HTMLTimeElementImp.h"
#include "html/HTMLTitleElementImp.h"
#include "html/HTMLUListElementImp.h"
#include "html/HTMLUnknownElementImp.h"
#include "html/HTMLVideoElementImp.h"
#include "WindowImp.h"
#include "ObjectArrayImp.h"

#include "js/esjsapi.h"
#include "js/Script.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

DocumentImp::DocumentImp(const std::u16string& url) :
    url(url),
    ObjectMixin(static_cast<DocumentImp*>(0) /*nullptr */),
    doctype(0),
    mode(NoQuirksMode),
    compatMode(u"CSS1Compat"),
    global(0),
    defaultView(0),
    activeElement(0),
    clickListener(boost::bind(&DocumentImp::handleClick, this, _1))
{
    nodeName = u"#document";
    addEventListener(u"click", &clickListener);

    // Set up global
    global = newGlobal();
}

DocumentImp::~DocumentImp()
{
    if (global) {
        ::putGlobal(static_cast<JSObject*>(global));
        global = 0;
    }
}

void DocumentImp::setDefaultView(WindowImp* view) {
    defaultView = view;
    if (global) {
        view->setPrivate(global);
        JS_SetPrivate(jscontext, static_cast<JSObject*>(global), view);
        JS_SetGlobalObject(jscontext, static_cast<JSObject*>(global));
    }
}

void DocumentImp::setURL(const std::u16string& url)
{
    this->url = url;
}

void DocumentImp::setFocus(ElementImp* element)
{
    activeElement = element;
}

void DocumentImp::handleClick(events::Event event)
{
    events::MouseEvent mouse = interface_cast<events::MouseEvent>(event);
    switch (mouse.getButton()) {
    case 7:
        if (defaultView)
            defaultView->getHistory().back();
        break;
    case 8:
        if (defaultView)
            defaultView->getHistory().forward();
        break;
    default:
        break;
    }
}

// Document

DOMImplementation DocumentImp::getImplementation()
{
    return getDOMImplementation();
}

std::u16string DocumentImp::getDocumentURI()
{
    return url;
}

void DocumentImp::setDocumentURI(std::u16string documentURI)
{
    if (defaultView)
        defaultView->setLocation(documentURI);
}

std::u16string DocumentImp::getCompatMode()
{
    return compatMode;
}

std::u16string DocumentImp::getContentType()
{
    // TODO: implement me!
    return u"";
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
    return 0;
}

NodeList DocumentImp::getElementsByTagName(std::u16string qualifiedName)
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

NodeList DocumentImp::getElementsByTagNameNS(std::u16string _namespace, std::u16string localName)
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

NodeList DocumentImp::getElementsByClassName(std::u16string classNames)
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

Element DocumentImp::getElementById(std::u16string elementId)
{
    ElementImp* e = dynamic_cast<ElementImp*>(getDocumentElement().self());
    while (e) {
        Nullable<std::u16string> id = e->getAttribute(u"id");
        if (id.hasValue() && id.value() == elementId)
            return e;
        e = e->getNextElement();
    }
    return 0;
}

Element DocumentImp::createElement(std::u16string localName)
{
    toLower(localName);  // TODO: if html

    // Checked in the order of descriptions in the HTML specification
    if (localName == u"html")
        return new(std::nothrow) HTMLHtmlElementImp(this);
    if (localName == u"head")
        return new(std::nothrow) HTMLHeadElementImp(this);
    if (localName == u"title")
        return new(std::nothrow) HTMLTitleElementImp(this);
    if (localName == u"base")
        return new(std::nothrow) HTMLBaseElementImp(this);
    if (localName == u"link")
        return new(std::nothrow) HTMLLinkElementImp(this);
    if (localName == u"meta")
        return new(std::nothrow) HTMLMetaElementImp(this);
    if (localName == u"style")
        return new(std::nothrow) HTMLStyleElementImp(this);
    if (localName == u"script")
        return new(std::nothrow) HTMLScriptElementImp(this);
    if (localName == u"noscript")
        return new(std::nothrow) HTMLElementImp(this, localName);
    if (localName == u"body")
        return new(std::nothrow) HTMLBodyElementImp(this);
    if (localName == u"section" ||
        localName == u"nav" ||
        localName == u"article" ||
        localName == u"aside")
        return new(std::nothrow) HTMLElementImp(this, localName);
    if (localName == u"h1" ||
        localName == u"h2" ||
        localName == u"h3" ||
        localName == u"h4" ||
        localName == u"h5" ||
        localName == u"h6")
        return new(std::nothrow) HTMLHeadingElementImp(this, localName);
    if (localName == u"hgroup" ||
        localName == u"header" ||
        localName == u"footer" ||
        localName == u"address")
        return new(std::nothrow) HTMLElementImp(this, localName);
    if (localName == u"p")
        return new(std::nothrow) HTMLParagraphElementImp(this);
    if (localName == u"hr")
        return new(std::nothrow) HTMLHRElementImp(this);
    if (localName == u"pre")
        return new(std::nothrow) HTMLPreElementImp(this);
    if (localName == u"blockquote")
        return new(std::nothrow) HTMLQuoteElementImp(this, localName);
    if (localName == u"ol")
        return new(std::nothrow) HTMLOListElementImp(this);
    if (localName == u"ul")
        return new(std::nothrow) HTMLUListElementImp(this);
    if (localName == u"li")
        return new(std::nothrow) HTMLLIElementImp(this);
    if (localName == u"dl")
        return new(std::nothrow) HTMLDListElementImp(this);
    if (localName == u"dt" ||
        localName == u"dd" ||
        localName == u"figure" ||
        localName == u"figcaption")
        return new(std::nothrow) HTMLElementImp(this, localName);
    if (localName == u"div")
        return new(std::nothrow) HTMLDivElementImp(this);
    if (localName == u"a")
        return new(std::nothrow) HTMLAnchorElementImp(this);
    if (localName == u"em" ||
        localName == u"strong" ||
        localName == u"small" ||
        localName == u"s" ||
        localName == u"cite")
        return new(std::nothrow) HTMLElementImp(this, localName);
    if (localName == u"q")
        return new(std::nothrow) HTMLQuoteElementImp(this, localName);
    if (localName == u"dfn" ||
        localName == u"abbr")
        return new(std::nothrow) HTMLElementImp(this, localName);
    if (localName == u"time")
        return new(std::nothrow) HTMLTimeElementImp(this);
    if (localName == u"code" ||
        localName == u"var" ||
        localName == u"samp" ||
        localName == u"kbd" ||
        localName == u"sub" ||
        localName == u"sup" ||
        localName == u"i" ||
        localName == u"b" ||
        localName == u"u" ||
        localName == u"mark" ||
        localName == u"ruby" ||
        localName == u"rt" ||
        localName == u"rp" ||
        localName == u"bdi" ||
        localName == u"bdo")
        return new(std::nothrow) HTMLElementImp(this, localName);
    if (localName == u"span")
        return new(std::nothrow) HTMLSpanElementImp(this);
    if (localName == u"br")
        return new(std::nothrow) HTMLBRElementImp(this);
    if (localName == u"wbr")
        return new(std::nothrow) HTMLElementImp(this, localName);
    if (localName == u"ins" ||
        localName == u"del")
        return new(std::nothrow) HTMLModElementImp(this, localName);
    if (localName == u"img")
        return new(std::nothrow) HTMLImageElementImp(this);
    if (localName == u"iframe")
        return new(std::nothrow) HTMLIFrameElementImp(this);
    if (localName == u"embed")
        return new(std::nothrow) HTMLEmbedElementImp(this);
    if (localName == u"object")
        return new(std::nothrow) HTMLObjectElementImp(this);
    if (localName == u"param")
        return new(std::nothrow) HTMLParamElementImp(this);
    if (localName == u"video")
        return new(std::nothrow) HTMLVideoElementImp(this);
    if (localName == u"audio")
        return new(std::nothrow) HTMLAudioElementImp(this);
    if (localName == u"source")
        return new(std::nothrow) HTMLSourceElementImp(this);
    if (localName == u"canvas")
        return new(std::nothrow) HTMLCanvasElementImp(this);
    if (localName == u"map")
        return new(std::nothrow) HTMLMapElementImp(this);
    if (localName == u"area")
        return new(std::nothrow) HTMLAreaElementImp(this);
    if (localName == u"table")
        return new(std::nothrow) HTMLTableElementImp(this);
    if (localName == u"caption")
        return new(std::nothrow) HTMLTableCaptionElementImp(this);
    if (localName == u"colgroup" ||
        localName == u"col")
        return new(std::nothrow) HTMLTableColElementImp(this, localName);
    if (localName == u"tbody" ||
        localName == u"thead" ||
        localName == u"tfoot")
        return new(std::nothrow) HTMLTableSectionElementImp(this, localName);
    if (localName == u"tr")
        return new(std::nothrow) HTMLTableRowElementImp(this);
    if (localName == u"td")
        return new(std::nothrow) HTMLTableDataCellElementImp(this);
    if (localName == u"th")
        return new(std::nothrow) HTMLTableHeaderCellElementImp(this);
    if (localName == u"form")
        return new(std::nothrow) HTMLFormElementImp(this);
    if (localName == u"fieldset")
        return new(std::nothrow) HTMLFieldSetElementImp(this);
    if (localName == u"legend")
        return new(std::nothrow) HTMLLegendElementImp(this);
    if (localName == u"label")
        return new(std::nothrow) HTMLLabelElementImp(this);
    if (localName == u"input")
        return new(std::nothrow) HTMLInputElementImp(this);
    if (localName == u"button")
        return new(std::nothrow) HTMLButtonElementImp(this);
    if (localName == u"select")
        return new(std::nothrow) HTMLSelectElementImp(this);
    if (localName == u"datalist")
        return new(std::nothrow) HTMLDataListElementImp(this);
    if (localName == u"optgroup")
        return new(std::nothrow) HTMLOptGroupElementImp(this);
    if (localName == u"option")
        return new(std::nothrow) HTMLOptionElementImp(this);
    if (localName == u"textarea")
        return new(std::nothrow) HTMLTextAreaElementImp(this);
    if (localName == u"keygen")
        return new(std::nothrow) HTMLKeygenElementImp(this);
    if (localName == u"output")
        return new(std::nothrow) HTMLOutputElementImp(this);
    if (localName == u"progress")
        return new(std::nothrow) HTMLProgressElementImp(this);
    if (localName == u"meter")
        return new(std::nothrow) HTMLMeterElementImp(this);
    if (localName == u"details")
        return new(std::nothrow) HTMLDetailsElementImp(this);
    if (localName == u"summary")
        return new(std::nothrow) HTMLElementImp(this, localName);
    if (localName == u"command")
        return new(std::nothrow) HTMLCommandElementImp(this);
    if (localName == u"menu")
        return new(std::nothrow) HTMLMenuElementImp(this);

    return new(std::nothrow) HTMLUnknownElementImp(this, localName);
}

Element DocumentImp::createElementNS(std::u16string namespaceURI, std::u16string qualifiedName)
{
    std::u16string prefix;
    std::u16string localName;
    size_t pos = qualifiedName.find(u':');
    if (pos != std::u16string::npos) {
        prefix = qualifiedName.substr(0, pos);
        localName = qualifiedName.substr(pos + 1);
    } else
        localName = qualifiedName;
    // TODO: several more checks
    return new(std::nothrow) ElementImp(this, localName, namespaceURI, prefix);
}

DocumentFragment DocumentImp::createDocumentFragment()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

Text DocumentImp::createTextNode(std::u16string data)
{
    return new(std::nothrow) TextImp(this, data);
}

Comment DocumentImp::createComment(std::u16string data)
{
    return new(std::nothrow) CommentImp(this, data);
}

ProcessingInstruction DocumentImp::createProcessingInstruction(std::u16string target, std::u16string data)
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

Node DocumentImp::importNode(Node node, bool deep)
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

Node DocumentImp::adoptNode(Node node)
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

events::Event DocumentImp::createEvent(std::u16string interface)
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

html::Location DocumentImp::getLocation()
{
    return new(std::nothrow) LocationImp(defaultView, url);
}

void DocumentImp::setLocation(std::u16string href)
{
    if (defaultView)
        defaultView->setLocation(href);
}

std::u16string DocumentImp::getURL()
{
    return url;
}

std::u16string DocumentImp::getDomain()
{
    // TODO: implement me!
    return u"";
}

void DocumentImp::setDomain(std::u16string domain)
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

void DocumentImp::setCookie(std::u16string cookie)
{
    // TODO: implement me!
}

std::u16string DocumentImp::getLastModified()
{
    // TODO: implement me!
    return u"";
}

std::u16string DocumentImp::getCharset()
{
    // TODO: implement me!
    return u"";
}

void DocumentImp::setCharset(std::u16string charset)
{
    // TODO: implement me!
}

std::u16string DocumentImp::getCharacterSet()
{
    // TODO: implement me!
    return u"";
}

std::u16string DocumentImp::getDefaultCharset()
{
    // TODO: implement me!
    return u"";
}

std::u16string DocumentImp::getReadyState()
{
    return readyState;
}

Any DocumentImp::getElement(std::u16string name)
{
    // TODO: implement me!
    return 0;
}

std::u16string DocumentImp::getTitle()
{
    // TODO: implement me!
    return u"";
}

void DocumentImp::setTitle(std::u16string title)
{
    // TODO: implement me!
}

std::u16string DocumentImp::getDir()
{
    // TODO: implement me!
    return u"";
}

void DocumentImp::setDir(std::u16string dir)
{
    // TODO: implement me!
}

html::HTMLElement DocumentImp::getBody()
{
    // TODO: refine.
    Element e = getDocumentElement();
    for (auto i = e.getFirstElementChild(); i; i = i.getNextElementSibling()) {
        if (i.getTagName() == u"body")
            return interface_cast<html::HTMLElement>(i);
    }
    return 0;
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
        if (i.getTagName() == u"head")
            return interface_cast<html::HTMLHeadElement>(i);
    }
    return 0;
}

html::HTMLCollection DocumentImp::getImages()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

html::HTMLCollection DocumentImp::getEmbeds()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

html::HTMLCollection DocumentImp::getPlugins()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

html::HTMLCollection DocumentImp::getLinks()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

html::HTMLCollection DocumentImp::getForms()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

html::HTMLCollection DocumentImp::getScripts()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

NodeList DocumentImp::getElementsByName(std::u16string elementName)
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

DOMElementMap DocumentImp::getCssElementMap()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

std::u16string DocumentImp::getInnerHTML()
{
    // TODO: implement me!
    return u"";
}

void DocumentImp::setInnerHTML(std::u16string innerHTML)
{
    // TODO: implement me!
}

html::HTMLDocument DocumentImp::open(std::u16string type, std::u16string replace)
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

html::Window DocumentImp::open(std::u16string url, std::u16string name, std::u16string features, bool replace)
{
    if (defaultView)
        return defaultView->open(url, name, features, replace);
    return static_cast<Object*>(0);
}

void DocumentImp::close()
{
    // TODO: implement me!
}

void DocumentImp::write(Variadic<std::u16string> text)
{
    // TODO: implement me!
}

void DocumentImp::writeln(Variadic<std::u16string> text)
{
    // TODO: implement me!
}

html::Window DocumentImp::getDefaultView()
{
    return defaultView;
}

Element DocumentImp::getActiveElement()
{
    if (activeElement)
        return activeElement;
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

void DocumentImp::setDesignMode(std::u16string designMode)
{
    // TODO: implement me!
}

bool DocumentImp::execCommand(std::u16string commandId)
{
    // TODO: implement me!
    return 0;
}

bool DocumentImp::execCommand(std::u16string commandId, bool showUI)
{
    // TODO: implement me!
    return 0;
}

bool DocumentImp::execCommand(std::u16string commandId, bool showUI, std::u16string value)
{
    // TODO: implement me!
    return 0;
}

bool DocumentImp::queryCommandEnabled(std::u16string commandId)
{
    // TODO: implement me!
    return 0;
}

bool DocumentImp::queryCommandIndeterm(std::u16string commandId)
{
    // TODO: implement me!
    return 0;
}

bool DocumentImp::queryCommandState(std::u16string commandId)
{
    // TODO: implement me!
    return 0;
}

bool DocumentImp::queryCommandSupported(std::u16string commandId)
{
    // TODO: implement me!
    return 0;
}

std::u16string DocumentImp::queryCommandValue(std::u16string commandId)
{
    // TODO: implement me!
    return u"";
}

html::HTMLCollection DocumentImp::getCommands()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

html::Function DocumentImp::getOnabort()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void DocumentImp::setOnabort(html::Function onabort)
{
    // TODO: implement me!
}

html::Function DocumentImp::getOnblur()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void DocumentImp::setOnblur(html::Function onblur)
{
    // TODO: implement me!
}

html::Function DocumentImp::getOncanplay()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void DocumentImp::setOncanplay(html::Function oncanplay)
{
    // TODO: implement me!
}

html::Function DocumentImp::getOncanplaythrough()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void DocumentImp::setOncanplaythrough(html::Function oncanplaythrough)
{
    // TODO: implement me!
}

html::Function DocumentImp::getOnchange()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void DocumentImp::setOnchange(html::Function onchange)
{
    // TODO: implement me!
}

html::Function DocumentImp::getOnclick()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void DocumentImp::setOnclick(html::Function onclick)
{
    // TODO: implement me!
}

html::Function DocumentImp::getOncontextmenu()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void DocumentImp::setOncontextmenu(html::Function oncontextmenu)
{
    // TODO: implement me!
}

html::Function DocumentImp::getOncuechange()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void DocumentImp::setOncuechange(html::Function oncuechange)
{
    // TODO: implement me!
}

html::Function DocumentImp::getOndblclick()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void DocumentImp::setOndblclick(html::Function ondblclick)
{
    // TODO: implement me!
}

html::Function DocumentImp::getOndrag()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void DocumentImp::setOndrag(html::Function ondrag)
{
    // TODO: implement me!
}

html::Function DocumentImp::getOndragend()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void DocumentImp::setOndragend(html::Function ondragend)
{
    // TODO: implement me!
}

html::Function DocumentImp::getOndragenter()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void DocumentImp::setOndragenter(html::Function ondragenter)
{
    // TODO: implement me!
}

html::Function DocumentImp::getOndragleave()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void DocumentImp::setOndragleave(html::Function ondragleave)
{
    // TODO: implement me!
}

html::Function DocumentImp::getOndragover()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void DocumentImp::setOndragover(html::Function ondragover)
{
    // TODO: implement me!
}

html::Function DocumentImp::getOndragstart()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void DocumentImp::setOndragstart(html::Function ondragstart)
{
    // TODO: implement me!
}

html::Function DocumentImp::getOndrop()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void DocumentImp::setOndrop(html::Function ondrop)
{
    // TODO: implement me!
}

html::Function DocumentImp::getOndurationchange()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void DocumentImp::setOndurationchange(html::Function ondurationchange)
{
    // TODO: implement me!
}

html::Function DocumentImp::getOnemptied()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void DocumentImp::setOnemptied(html::Function onemptied)
{
    // TODO: implement me!
}

html::Function DocumentImp::getOnended()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void DocumentImp::setOnended(html::Function onended)
{
    // TODO: implement me!
}

html::Function DocumentImp::getOnerror()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void DocumentImp::setOnerror(html::Function onerror)
{
    // TODO: implement me!
}

html::Function DocumentImp::getOnfocus()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void DocumentImp::setOnfocus(html::Function onfocus)
{
    // TODO: implement me!
}

html::Function DocumentImp::getOninput()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void DocumentImp::setOninput(html::Function oninput)
{
    // TODO: implement me!
}

html::Function DocumentImp::getOninvalid()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void DocumentImp::setOninvalid(html::Function oninvalid)
{
    // TODO: implement me!
}

html::Function DocumentImp::getOnkeydown()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void DocumentImp::setOnkeydown(html::Function onkeydown)
{
    // TODO: implement me!
}

html::Function DocumentImp::getOnkeypress()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void DocumentImp::setOnkeypress(html::Function onkeypress)
{
    // TODO: implement me!
}

html::Function DocumentImp::getOnkeyup()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void DocumentImp::setOnkeyup(html::Function onkeyup)
{
    // TODO: implement me!
}

html::Function DocumentImp::getOnload()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void DocumentImp::setOnload(html::Function onload)
{
    // TODO: implement me!
}

html::Function DocumentImp::getOnloadeddata()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void DocumentImp::setOnloadeddata(html::Function onloadeddata)
{
    // TODO: implement me!
}

html::Function DocumentImp::getOnloadedmetadata()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void DocumentImp::setOnloadedmetadata(html::Function onloadedmetadata)
{
    // TODO: implement me!
}

html::Function DocumentImp::getOnloadstart()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void DocumentImp::setOnloadstart(html::Function onloadstart)
{
    // TODO: implement me!
}

html::Function DocumentImp::getOnmousedown()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void DocumentImp::setOnmousedown(html::Function onmousedown)
{
    // TODO: implement me!
}

html::Function DocumentImp::getOnmousemove()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void DocumentImp::setOnmousemove(html::Function onmousemove)
{
    // TODO: implement me!
}

html::Function DocumentImp::getOnmouseout()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void DocumentImp::setOnmouseout(html::Function onmouseout)
{
    // TODO: implement me!
}

html::Function DocumentImp::getOnmouseover()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void DocumentImp::setOnmouseover(html::Function onmouseover)
{
    // TODO: implement me!
}

html::Function DocumentImp::getOnmouseup()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void DocumentImp::setOnmouseup(html::Function onmouseup)
{
    // TODO: implement me!
}

html::Function DocumentImp::getOnmousewheel()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void DocumentImp::setOnmousewheel(html::Function onmousewheel)
{
    // TODO: implement me!
}

html::Function DocumentImp::getOnpause()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void DocumentImp::setOnpause(html::Function onpause)
{
    // TODO: implement me!
}

html::Function DocumentImp::getOnplay()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void DocumentImp::setOnplay(html::Function onplay)
{
    // TODO: implement me!
}

html::Function DocumentImp::getOnplaying()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void DocumentImp::setOnplaying(html::Function onplaying)
{
    // TODO: implement me!
}

html::Function DocumentImp::getOnprogress()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void DocumentImp::setOnprogress(html::Function onprogress)
{
    // TODO: implement me!
}

html::Function DocumentImp::getOnratechange()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void DocumentImp::setOnratechange(html::Function onratechange)
{
    // TODO: implement me!
}

html::Function DocumentImp::getOnreadystatechange()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void DocumentImp::setOnreadystatechange(html::Function onreadystatechange)
{
    // TODO: implement me!
}

html::Function DocumentImp::getOnreset()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void DocumentImp::setOnreset(html::Function onreset)
{
    // TODO: implement me!
}

html::Function DocumentImp::getOnscroll()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void DocumentImp::setOnscroll(html::Function onscroll)
{
    // TODO: implement me!
}

html::Function DocumentImp::getOnseeked()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void DocumentImp::setOnseeked(html::Function onseeked)
{
    // TODO: implement me!
}

html::Function DocumentImp::getOnseeking()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void DocumentImp::setOnseeking(html::Function onseeking)
{
    // TODO: implement me!
}

html::Function DocumentImp::getOnselect()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void DocumentImp::setOnselect(html::Function onselect)
{
    // TODO: implement me!
}

html::Function DocumentImp::getOnshow()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void DocumentImp::setOnshow(html::Function onshow)
{
    // TODO: implement me!
}

html::Function DocumentImp::getOnstalled()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void DocumentImp::setOnstalled(html::Function onstalled)
{
    // TODO: implement me!
}

html::Function DocumentImp::getOnsubmit()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void DocumentImp::setOnsubmit(html::Function onsubmit)
{
    // TODO: implement me!
}

html::Function DocumentImp::getOnsuspend()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void DocumentImp::setOnsuspend(html::Function onsuspend)
{
    // TODO: implement me!
}

html::Function DocumentImp::getOntimeupdate()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void DocumentImp::setOntimeupdate(html::Function ontimeupdate)
{
    // TODO: implement me!
}

html::Function DocumentImp::getOnvolumechange()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void DocumentImp::setOnvolumechange(html::Function onvolumechange)
{
    // TODO: implement me!
}

html::Function DocumentImp::getOnwaiting()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void DocumentImp::setOnwaiting(html::Function onwaiting)
{
    // TODO: implement me!
}

std::u16string DocumentImp::getFgColor()
{
    // TODO: implement me!
    return u"";
}

void DocumentImp::setFgColor(std::u16string fgColor)
{
    // TODO: implement me!
}

std::u16string DocumentImp::getBgColor()
{
    // TODO: implement me!
    return u"";
}

void DocumentImp::setBgColor(std::u16string bgColor)
{
    // TODO: implement me!
}

std::u16string DocumentImp::getLinkColor()
{
    // TODO: implement me!
    return u"";
}

void DocumentImp::setLinkColor(std::u16string linkColor)
{
    // TODO: implement me!
}

std::u16string DocumentImp::getVlinkColor()
{
    // TODO: implement me!
    return u"";
}

void DocumentImp::setVlinkColor(std::u16string vlinkColor)
{
    // TODO: implement me!
}

std::u16string DocumentImp::getAlinkColor()
{
    // TODO: implement me!
    return u"";
}

void DocumentImp::setAlinkColor(std::u16string alinkColor)
{
    // TODO: implement me!
}

html::HTMLCollection DocumentImp::getAnchors()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

html::HTMLCollection DocumentImp::getApplets()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void DocumentImp::clear()
{
    // TODO: implement me!
}

html::HTMLAllCollection DocumentImp::getAll()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

css::CSSStyleDeclaration DocumentImp::getOverrideStyle(Element elt, Nullable<std::u16string> pseudoElt)
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

stylesheets::StyleSheetList DocumentImp::getStyleSheets()
{
    return new(std::nothrow) ObjectArrayImp<DocumentImp, stylesheets::StyleSheet, &DocumentImp::styleSheets>(this);
}

Nullable<std::u16string> DocumentImp::getSelectedStyleSheetSet()
{
    // TODO: implement me!
    return u"";
}

void DocumentImp::setSelectedStyleSheetSet(Nullable<std::u16string> selectedStyleSheetSet)
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
    return static_cast<Object*>(0);
}

void DocumentImp::enableStyleSheetsForSet(Nullable<std::u16string> name)
{
    // TODO: implement me!
}

Element DocumentImp::elementFromPoint(float x, float y)
{
    if (!defaultView)
        return 0;
    return defaultView->elementFromPoint(x, y);
}

CaretPosition DocumentImp::caretPositionFromPoint(float x, float y)
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

Element DocumentImp::querySelector(std::u16string selectors)
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

NodeList DocumentImp::querySelectorAll(std::u16string selectors)
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

ranges::Range DocumentImp::createRange()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

traversal::NodeIterator DocumentImp::createNodeIterator(Node root, unsigned int whatToShow, traversal::NodeFilter filter, bool entityReferenceExpansion) throw(DOMException)
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

traversal::TreeWalker DocumentImp::createTreeWalker(Node root, unsigned int whatToShow, traversal::NodeFilter filter, bool entityReferenceExpansion) throw(DOMException)
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

// Node - override

unsigned short DocumentImp::getNodeType()
{
    return Node::DOCUMENT_NODE;
}

Node DocumentImp::appendChild(Node newChild) throw(DOMException)
{
    if (DocumentTypeImp* newDoctype = dynamic_cast<DocumentTypeImp*>(newChild.self())) {
        if (getDoctype())
            throw DOMException(DOMException::HIERARCHY_REQUEST_ERR);
        if (!newChild.getOwnerDocument())
            newDoctype->setOwnerDocument(this);
        NodeImp::appendChild(newChild);
        doctype = newDoctype;
        return newChild;
    }
    // TODO: check second Element
    return NodeImp::appendChild(newChild);
}

}}}}  // org::w3c::dom::bootstrap
