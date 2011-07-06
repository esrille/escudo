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

#ifndef DOCUMENT_IMP_H
#define DOCUMENT_IMP_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <org/w3c/dom/Document.h>
#include "NodeImp.h"

#include <org/w3c/dom/stylesheets/StyleSheet.h>
#include <org/w3c/dom/css/CSSStyleDeclaration.h>
#include <org/w3c/dom/html/HTMLElement.h>
#include <org/w3c/dom/CaretPosition.h>
#include <org/w3c/dom/ranges/Range.h>
#include <org/w3c/dom/traversal/NodeIterator.h>
#include <org/w3c/dom/traversal/NodeFilter.h>
#include <org/w3c/dom/traversal/TreeWalker.h>
#include <org/w3c/dom/DOMException.h>
#include <org/w3c/dom/events/Event.h>
#include <org/w3c/dom/Node.h>
#include <org/w3c/dom/DocumentFragment.h>
#include <org/w3c/dom/DOMImplementation.h>
#include <org/w3c/dom/Element.h>
#include <org/w3c/dom/DocumentType.h>
#include <org/w3c/dom/ProcessingInstruction.h>
#include <org/w3c/dom/Text.h>
#include <org/w3c/dom/Comment.h>
#include <org/w3c/dom/NodeList.h>
#include <org/w3c/dom/html/HTMLCollection.h>
#include <org/w3c/dom/DOMStringList.h>
#include <org/w3c/dom/DOMElementMap.h>
#include <org/w3c/dom/html/HTMLAllCollection.h>
#include <org/w3c/dom/html/HTMLDocument.h>
#include <org/w3c/dom/html/HTMLHeadElement.h>
#include <org/w3c/dom/html/Window.h>
#include <org/w3c/dom/html/Location.h>
#include <org/w3c/dom/html/Function.h>

#include <deque>

#include "NodeImp.h"
#include "EventListenerImp.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

class WindowImp;
class ElementImp;

class DocumentImp : public ObjectMixin<DocumentImp, NodeImp>
{
    std::u16string url;
    DocumentType doctype;
    int mode;
    std::u16string readyState;
    std::u16string compatMode;
    std::deque<stylesheets::StyleSheet> styleSheets;

    void* global;
    WindowImp* defaultView;

    ElementImp* activeElement;

    Retained<EventListenerImp> clickListener;
    void handleClick(events::Event event);

public:
    DocumentImp(const std::u16string& url = u"about:blank");
    ~DocumentImp();

    enum
    {
         NoQuirksMode,  // default
         QuirksMode,
         LimitedQuirksMode
    };

    void* getGlobal() {
        return global;
    }
    void setGlobal(void* g) {
        global = g;
    }

    void setDefaultView(WindowImp* view);

    int getMode() const {
        return mode;
    }
    void setMode(int mode) {
        this->mode = mode;
    }

    void setReadyState(const std::u16string readyState) {
        this->readyState = readyState;
    }

    void addStyleSheet(stylesheets::StyleSheet sheet) {
        styleSheets.push_back(sheet);
    }

    void setURL(const std::u16string& url);

    ElementImp* getFocus() const {
        return activeElement;
    }
    void setFocus(ElementImp* element);

    // Node - override
    virtual unsigned short getNodeType();
    virtual Node appendChild(Node newChild) throw(DOMException);

    // Document
    DOMImplementation getImplementation() __attribute__((weak));
    std::u16string getDocumentURI() __attribute__((weak));
    void setDocumentURI(std::u16string documentURI) __attribute__((weak));
    std::u16string getCompatMode() __attribute__((weak));
    std::u16string getCharset() __attribute__((weak));
    void setCharset(std::u16string charset) __attribute__((weak));
    std::u16string getCharacterSet() __attribute__((weak));
    std::u16string getDefaultCharset() __attribute__((weak));
    std::u16string getContentType() __attribute__((weak));
    DocumentType getDoctype() __attribute__((weak));
    Element getDocumentElement() __attribute__((weak));
    NodeList getElementsByTagName(std::u16string qualifiedName) __attribute__((weak));
    NodeList getElementsByTagNameNS(std::u16string _namespace, std::u16string localName) __attribute__((weak));
    NodeList getElementsByClassName(std::u16string classNames) __attribute__((weak));
    Element getElementById(std::u16string elementId) __attribute__((weak));
    Element createElement(std::u16string localName) __attribute__((weak));
    Element createElementNS(std::u16string _namespace, std::u16string qualifiedName) __attribute__((weak));
    DocumentFragment createDocumentFragment() __attribute__((weak));
    Text createTextNode(std::u16string data) __attribute__((weak));
    Comment createComment(std::u16string data) __attribute__((weak));
    ProcessingInstruction createProcessingInstruction(std::u16string target, std::u16string data) __attribute__((weak));
    Node importNode(Node node, bool deep) __attribute__((weak));
    Node adoptNode(Node node) __attribute__((weak));
    events::Event createEvent(std::u16string eventInterfaceName) __attribute__((weak));
    // DocumentCSS
    css::CSSStyleDeclaration getOverrideStyle(Element elt, Nullable<std::u16string> pseudoElt) __attribute__((weak));
    // Document-1
    stylesheets::StyleSheetList getStyleSheets() __attribute__((weak));
    Nullable<std::u16string> getSelectedStyleSheetSet() __attribute__((weak));
    void setSelectedStyleSheetSet(Nullable<std::u16string> selectedStyleSheetSet) __attribute__((weak));
    Nullable<std::u16string> getLastStyleSheetSet() __attribute__((weak));
    Nullable<std::u16string> getPreferredStyleSheetSet() __attribute__((weak));
    DOMStringList getStyleSheetSets() __attribute__((weak));
    void enableStyleSheetsForSet(Nullable<std::u16string> name) __attribute__((weak));
    // Document-2
    Element elementFromPoint(float x, float y) __attribute__((weak));
    CaretPosition caretPositionFromPoint(float x, float y) __attribute__((weak));
    // NodeSelector
    Element querySelector(std::u16string selectors) __attribute__((weak));
    NodeList querySelectorAll(std::u16string selectors) __attribute__((weak));
    // HTMLDocument
    html::Location getLocation() __attribute__((weak));
    void setLocation(std::u16string location) __attribute__((weak));
    std::u16string getURL() __attribute__((weak));
    std::u16string getDomain() __attribute__((weak));
    void setDomain(std::u16string domain) __attribute__((weak));
    std::u16string getReferrer() __attribute__((weak));
    std::u16string getCookie() __attribute__((weak));
    void setCookie(std::u16string cookie) __attribute__((weak));
    std::u16string getLastModified() __attribute__((weak));
    std::u16string getReadyState() __attribute__((weak));
    Any getElement(std::u16string name) __attribute__((weak));
    std::u16string getTitle() __attribute__((weak));
    void setTitle(std::u16string title) __attribute__((weak));
    std::u16string getDir() __attribute__((weak));
    void setDir(std::u16string dir) __attribute__((weak));
    html::HTMLElement getBody() __attribute__((weak));
    void setBody(html::HTMLElement body) __attribute__((weak));
    html::HTMLHeadElement getHead() __attribute__((weak));
    html::HTMLCollection getImages() __attribute__((weak));
    html::HTMLCollection getEmbeds() __attribute__((weak));
    html::HTMLCollection getPlugins() __attribute__((weak));
    html::HTMLCollection getLinks() __attribute__((weak));
    html::HTMLCollection getForms() __attribute__((weak));
    html::HTMLCollection getScripts() __attribute__((weak));
    NodeList getElementsByName(std::u16string elementName) __attribute__((weak));
    DOMElementMap getCssElementMap() __attribute__((weak));
    std::u16string getInnerHTML() __attribute__((weak));
    void setInnerHTML(std::u16string innerHTML) __attribute__((weak));
    html::HTMLDocument open() __attribute__((weak));
    html::HTMLDocument open(std::u16string type) __attribute__((weak));
    html::HTMLDocument open(std::u16string type, std::u16string replace) __attribute__((weak));
    html::Window open(std::u16string url, std::u16string name, std::u16string features) __attribute__((weak));
    html::Window open(std::u16string url, std::u16string name, std::u16string features, bool replace) __attribute__((weak));
    void close() __attribute__((weak));
    void write(Variadic<std::u16string> text = Variadic<std::u16string>()) __attribute__((weak));
    void writeln(Variadic<std::u16string> text = Variadic<std::u16string>()) __attribute__((weak));
    html::Window getDefaultView() __attribute__((weak));
    Element getActiveElement() __attribute__((weak));
    bool hasFocus() __attribute__((weak));
    std::u16string getDesignMode() __attribute__((weak));
    void setDesignMode(std::u16string designMode) __attribute__((weak));
    bool execCommand(std::u16string commandId) __attribute__((weak));
    bool execCommand(std::u16string commandId, bool showUI) __attribute__((weak));
    bool execCommand(std::u16string commandId, bool showUI, std::u16string value) __attribute__((weak));
    bool queryCommandEnabled(std::u16string commandId) __attribute__((weak));
    bool queryCommandIndeterm(std::u16string commandId) __attribute__((weak));
    bool queryCommandState(std::u16string commandId) __attribute__((weak));
    bool queryCommandSupported(std::u16string commandId) __attribute__((weak));
    std::u16string queryCommandValue(std::u16string commandId) __attribute__((weak));
    html::HTMLCollection getCommands() __attribute__((weak));
    html::Function getOnabort() __attribute__((weak));
    void setOnabort(html::Function onabort) __attribute__((weak));
    html::Function getOnblur() __attribute__((weak));
    void setOnblur(html::Function onblur) __attribute__((weak));
    html::Function getOncanplay() __attribute__((weak));
    void setOncanplay(html::Function oncanplay) __attribute__((weak));
    html::Function getOncanplaythrough() __attribute__((weak));
    void setOncanplaythrough(html::Function oncanplaythrough) __attribute__((weak));
    html::Function getOnchange() __attribute__((weak));
    void setOnchange(html::Function onchange) __attribute__((weak));
    html::Function getOnclick() __attribute__((weak));
    void setOnclick(html::Function onclick) __attribute__((weak));
    html::Function getOncontextmenu() __attribute__((weak));
    void setOncontextmenu(html::Function oncontextmenu) __attribute__((weak));
    html::Function getOncuechange() __attribute__((weak));
    void setOncuechange(html::Function oncuechange) __attribute__((weak));
    html::Function getOndblclick() __attribute__((weak));
    void setOndblclick(html::Function ondblclick) __attribute__((weak));
    html::Function getOndrag() __attribute__((weak));
    void setOndrag(html::Function ondrag) __attribute__((weak));
    html::Function getOndragend() __attribute__((weak));
    void setOndragend(html::Function ondragend) __attribute__((weak));
    html::Function getOndragenter() __attribute__((weak));
    void setOndragenter(html::Function ondragenter) __attribute__((weak));
    html::Function getOndragleave() __attribute__((weak));
    void setOndragleave(html::Function ondragleave) __attribute__((weak));
    html::Function getOndragover() __attribute__((weak));
    void setOndragover(html::Function ondragover) __attribute__((weak));
    html::Function getOndragstart() __attribute__((weak));
    void setOndragstart(html::Function ondragstart) __attribute__((weak));
    html::Function getOndrop() __attribute__((weak));
    void setOndrop(html::Function ondrop) __attribute__((weak));
    html::Function getOndurationchange() __attribute__((weak));
    void setOndurationchange(html::Function ondurationchange) __attribute__((weak));
    html::Function getOnemptied() __attribute__((weak));
    void setOnemptied(html::Function onemptied) __attribute__((weak));
    html::Function getOnended() __attribute__((weak));
    void setOnended(html::Function onended) __attribute__((weak));
    html::Function getOnerror() __attribute__((weak));
    void setOnerror(html::Function onerror) __attribute__((weak));
    html::Function getOnfocus() __attribute__((weak));
    void setOnfocus(html::Function onfocus) __attribute__((weak));
    html::Function getOninput() __attribute__((weak));
    void setOninput(html::Function oninput) __attribute__((weak));
    html::Function getOninvalid() __attribute__((weak));
    void setOninvalid(html::Function oninvalid) __attribute__((weak));
    html::Function getOnkeydown() __attribute__((weak));
    void setOnkeydown(html::Function onkeydown) __attribute__((weak));
    html::Function getOnkeypress() __attribute__((weak));
    void setOnkeypress(html::Function onkeypress) __attribute__((weak));
    html::Function getOnkeyup() __attribute__((weak));
    void setOnkeyup(html::Function onkeyup) __attribute__((weak));
    html::Function getOnload() __attribute__((weak));
    void setOnload(html::Function onload) __attribute__((weak));
    html::Function getOnloadeddata() __attribute__((weak));
    void setOnloadeddata(html::Function onloadeddata) __attribute__((weak));
    html::Function getOnloadedmetadata() __attribute__((weak));
    void setOnloadedmetadata(html::Function onloadedmetadata) __attribute__((weak));
    html::Function getOnloadstart() __attribute__((weak));
    void setOnloadstart(html::Function onloadstart) __attribute__((weak));
    html::Function getOnmousedown() __attribute__((weak));
    void setOnmousedown(html::Function onmousedown) __attribute__((weak));
    html::Function getOnmousemove() __attribute__((weak));
    void setOnmousemove(html::Function onmousemove) __attribute__((weak));
    html::Function getOnmouseout() __attribute__((weak));
    void setOnmouseout(html::Function onmouseout) __attribute__((weak));
    html::Function getOnmouseover() __attribute__((weak));
    void setOnmouseover(html::Function onmouseover) __attribute__((weak));
    html::Function getOnmouseup() __attribute__((weak));
    void setOnmouseup(html::Function onmouseup) __attribute__((weak));
    html::Function getOnmousewheel() __attribute__((weak));
    void setOnmousewheel(html::Function onmousewheel) __attribute__((weak));
    html::Function getOnpause() __attribute__((weak));
    void setOnpause(html::Function onpause) __attribute__((weak));
    html::Function getOnplay() __attribute__((weak));
    void setOnplay(html::Function onplay) __attribute__((weak));
    html::Function getOnplaying() __attribute__((weak));
    void setOnplaying(html::Function onplaying) __attribute__((weak));
    html::Function getOnprogress() __attribute__((weak));
    void setOnprogress(html::Function onprogress) __attribute__((weak));
    html::Function getOnratechange() __attribute__((weak));
    void setOnratechange(html::Function onratechange) __attribute__((weak));
    html::Function getOnreadystatechange() __attribute__((weak));
    void setOnreadystatechange(html::Function onreadystatechange) __attribute__((weak));
    html::Function getOnreset() __attribute__((weak));
    void setOnreset(html::Function onreset) __attribute__((weak));
    html::Function getOnscroll() __attribute__((weak));
    void setOnscroll(html::Function onscroll) __attribute__((weak));
    html::Function getOnseeked() __attribute__((weak));
    void setOnseeked(html::Function onseeked) __attribute__((weak));
    html::Function getOnseeking() __attribute__((weak));
    void setOnseeking(html::Function onseeking) __attribute__((weak));
    html::Function getOnselect() __attribute__((weak));
    void setOnselect(html::Function onselect) __attribute__((weak));
    html::Function getOnshow() __attribute__((weak));
    void setOnshow(html::Function onshow) __attribute__((weak));
    html::Function getOnstalled() __attribute__((weak));
    void setOnstalled(html::Function onstalled) __attribute__((weak));
    html::Function getOnsubmit() __attribute__((weak));
    void setOnsubmit(html::Function onsubmit) __attribute__((weak));
    html::Function getOnsuspend() __attribute__((weak));
    void setOnsuspend(html::Function onsuspend) __attribute__((weak));
    html::Function getOntimeupdate() __attribute__((weak));
    void setOntimeupdate(html::Function ontimeupdate) __attribute__((weak));
    html::Function getOnvolumechange() __attribute__((weak));
    void setOnvolumechange(html::Function onvolumechange) __attribute__((weak));
    html::Function getOnwaiting() __attribute__((weak));
    void setOnwaiting(html::Function onwaiting) __attribute__((weak));
    // HTMLDocument-38
    std::u16string getFgColor() __attribute__((weak));
    void setFgColor(std::u16string fgColor) __attribute__((weak));
    std::u16string getBgColor() __attribute__((weak));
    void setBgColor(std::u16string bgColor) __attribute__((weak));
    std::u16string getLinkColor() __attribute__((weak));
    void setLinkColor(std::u16string linkColor) __attribute__((weak));
    std::u16string getVlinkColor() __attribute__((weak));
    void setVlinkColor(std::u16string vlinkColor) __attribute__((weak));
    std::u16string getAlinkColor() __attribute__((weak));
    void setAlinkColor(std::u16string alinkColor) __attribute__((weak));
    html::HTMLCollection getAnchors() __attribute__((weak));
    html::HTMLCollection getApplets() __attribute__((weak));
    void clear() __attribute__((weak));
    html::HTMLAllCollection getAll() __attribute__((weak));
    // DocumentRange
    ranges::Range createRange() __attribute__((weak));
    // DocumentTraversal
    traversal::NodeIterator createNodeIterator(Node root, unsigned int whatToShow, traversal::NodeFilter filter, bool entityReferenceExpansion) throw(DOMException) __attribute__((weak));
    traversal::TreeWalker createTreeWalker(Node root, unsigned int whatToShow, traversal::NodeFilter filter, bool entityReferenceExpansion) throw(DOMException) __attribute__((weak));
    // Object
    virtual Any message_(uint32_t selector, const char* id, int argc, Any* argv)
    {
        return Document::dispatch(this, selector, id, argc, argv);
    }
    static const char* const getMetaData()
    {
        return Document::getMetaData();
    }
};

}}}}  // org::w3c::dom::bootstrap

#endif  // DOCUMENT_IMP_H
