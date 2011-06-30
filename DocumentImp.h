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

#include <Object.h>
#include <org/w3c/dom/Document.h>

#include <org/w3c/dom/stylesheets/StyleSheet.h>
#include <org/w3c/dom/css/CSSStyleDeclaration.h>
#include <org/w3c/dom/html/HTMLElement.h>
#include <org/w3c/dom/CaretPosition.h>
#include <org/w3c/dom/ranges/Range.h>
#include <org/w3c/dom/traversal/NodeIterator.h>
#include <org/w3c/dom/traversal/NodeFilter.h>
#include <org/w3c/dom/traversal/TreeWalker.h>
#include <org/w3c/dom/events/Event.h>
#include <org/w3c/dom/DOMException.h>
#include <org/w3c/dom/Node.h>
#include <org/w3c/dom/DocumentFragment.h>
#include <org/w3c/dom/DOMImplementation.h>
#include <org/w3c/dom/Element.h>
#include <org/w3c/dom/DocumentType.h>
#include <org/w3c/dom/ProcessingInstruction.h>
#include <org/w3c/dom/Text.h>
#include <org/w3c/dom/Comment.h>
#include <org/w3c/dom/NodeList.h>
#include <org/w3c/dom/DOMStringList.h>
#include <org/w3c/dom/html/HTMLCollection.h>
#include <org/w3c/dom/html/HTMLAllCollection.h>
#include <org/w3c/dom/html/HTMLDocument.h>
#include <org/w3c/dom/html/HTMLHeadElement.h>
#include <org/w3c/dom/html/Location.h>
#include <org/w3c/dom/html/Function.h>
#include <org/w3c/dom/html/Selection.h>
#include <org/w3c/dom/html/Window.h>

#include <deque>

#include "NodeImp.h"
#include "EventListenerImp.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

class WindowImp;
class ElementImp;

// implements Document_Mixin
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

    // Document
    virtual DOMImplementation getImplementation();
    virtual std::u16string getDocumentURI();
    virtual void setDocumentURI(std::u16string documentURI);
    virtual std::u16string getCompatMode();
    virtual DocumentType getDoctype();
    virtual Element getDocumentElement();
    virtual NodeList getElementsByTagName(std::u16string qualifiedName);
    virtual NodeList getElementsByTagNameNS(std::u16string _namespace, std::u16string localName);
    virtual NodeList getElementsByClassName(std::u16string classNames);
    virtual Element getElementById(std::u16string elementId);
    virtual Element createElement(std::u16string localName);
    virtual Element createElementNS(std::u16string _namespace, std::u16string qualifiedName);
    virtual DocumentFragment createDocumentFragment();
    virtual Text createTextNode(std::u16string data);
    virtual Comment createComment(std::u16string data);
    virtual ProcessingInstruction createProcessingInstruction(std::u16string target, std::u16string data);
    virtual Node importNode(Node node, bool deep);
    virtual Node adoptNode(Node node);
    // Document-38
    virtual stylesheets::StyleSheetList getStyleSheets();
    virtual Nullable<std::u16string> getSelectedStyleSheetSet();
    virtual void setSelectedStyleSheetSet(Nullable<std::u16string> selectedStyleSheetSet);
    virtual Nullable<std::u16string> getLastStyleSheetSet();
    virtual Nullable<std::u16string> getPreferredStyleSheetSet();
    virtual DOMStringList getStyleSheetSets();
    virtual void enableStyleSheetsForSet(Nullable<std::u16string> name);
    // Document-39
    virtual Element elementFromPoint(float x, float y);
    virtual CaretPosition caretPositionFromPoint(float x, float y);
    // NodeSelector
    virtual Element querySelector(std::u16string selectors);
    virtual NodeList querySelectorAll(std::u16string selectors);

#ifdef ENABLE_SVG
    // SVGDocument
    // TODO: The following three methods are also defined in HTMLDocument.
    // virtual std::u16string getTitle();
    // virtual std::u16string getReferrer();
    // virtual std::u16string getDomain();
    // virtual std::u16string getURL();
    virtual svg::SVGSVGElement getRootElement();
#endif // ENABLE_SVG

    // DocumentTraversal
    virtual traversal::NodeIterator createNodeIterator(Node root, unsigned int whatToShow, traversal::NodeFilter filter, bool entityReferenceExpansion) throw(DOMException);
    virtual traversal::TreeWalker createTreeWalker(Node root, unsigned int whatToShow, traversal::NodeFilter filter, bool entityReferenceExpansion) throw(DOMException);

    // DocumentEvent
    virtual events::Event createEvent(std::u16string eventInterface);

    // DocumentRange
    virtual ranges::Range createRange();

    // DocumentCSS
    virtual css::CSSStyleDeclaration getOverrideStyle(Element elt, Nullable<std::u16string> pseudoElt);

    // HTMLDocument
    virtual html::Location getLocation();
    virtual void setLocation(std::u16string location);
    virtual std::u16string getURL();
    virtual std::u16string getDomain();
    virtual void setDomain(std::u16string domain);
    virtual std::u16string getReferrer();
    virtual std::u16string getCookie();
    virtual void setCookie(std::u16string cookie);
    virtual std::u16string getLastModified();
    virtual std::u16string getCharset();
    virtual void setCharset(std::u16string charset);
    virtual std::u16string getCharacterSet();
    virtual std::u16string getDefaultCharset();
    virtual std::u16string getReadyState();
    virtual Any getElement(std::u16string name);
    virtual std::u16string getTitle();
    virtual void setTitle(std::u16string title);
    virtual std::u16string getDir();
    virtual void setDir(std::u16string dir);
    virtual html::HTMLElement getBody();
    virtual void setBody(html::HTMLElement body);
    virtual html::HTMLHeadElement getHead();
    virtual html::HTMLCollection getImages();
    virtual html::HTMLCollection getEmbeds();
    virtual html::HTMLCollection getPlugins();
    virtual html::HTMLCollection getLinks();
    virtual html::HTMLCollection getForms();
    virtual html::HTMLCollection getScripts();
    virtual NodeList getElementsByName(std::u16string elementName);
    virtual std::u16string getInnerHTML();
    virtual void setInnerHTML(std::u16string innerHTML);
    virtual html::HTMLDocument open(std::u16string type = u"text/html", std::u16string replace = u"");
    virtual html::Window open(std::u16string url, std::u16string name, std::u16string features, bool replace = false);
    virtual void close();
    virtual void write(Variadic<std::u16string> text = Variadic<std::u16string>());
    virtual void writeln(Variadic<std::u16string> text = Variadic<std::u16string>());
    virtual html::Window getDefaultView();
    virtual html::Selection getSelection();
    virtual Element getActiveElement();
    virtual bool hasFocus();
    virtual std::u16string getDesignMode();
    virtual void setDesignMode(std::u16string designMode);
    virtual bool execCommand(std::u16string commandId);
    virtual bool execCommand(std::u16string commandId, bool showUI);
    virtual bool execCommand(std::u16string commandId, bool showUI, std::u16string value);
    virtual bool queryCommandEnabled(std::u16string commandId);
    virtual bool queryCommandIndeterm(std::u16string commandId);
    virtual bool queryCommandState(std::u16string commandId);
    virtual bool queryCommandSupported(std::u16string commandId);
    virtual std::u16string queryCommandValue(std::u16string commandId);
    virtual html::HTMLCollection getCommands();
    virtual html::Function getOnabort();
    virtual void setOnabort(html::Function onabort);
    virtual html::Function getOnblur();
    virtual void setOnblur(html::Function onblur);
    virtual html::Function getOncanplay();
    virtual void setOncanplay(html::Function oncanplay);
    virtual html::Function getOncanplaythrough();
    virtual void setOncanplaythrough(html::Function oncanplaythrough);
    virtual html::Function getOnchange();
    virtual void setOnchange(html::Function onchange);
    virtual html::Function getOnclick();
    virtual void setOnclick(html::Function onclick);
    virtual html::Function getOncontextmenu();
    virtual void setOncontextmenu(html::Function oncontextmenu);
    virtual html::Function getOndblclick();
    virtual void setOndblclick(html::Function ondblclick);
    virtual html::Function getOndrag();
    virtual void setOndrag(html::Function ondrag);
    virtual html::Function getOndragend();
    virtual void setOndragend(html::Function ondragend);
    virtual html::Function getOndragenter();
    virtual void setOndragenter(html::Function ondragenter);
    virtual html::Function getOndragleave();
    virtual void setOndragleave(html::Function ondragleave);
    virtual html::Function getOndragover();
    virtual void setOndragover(html::Function ondragover);
    virtual html::Function getOndragstart();
    virtual void setOndragstart(html::Function ondragstart);
    virtual html::Function getOndrop();
    virtual void setOndrop(html::Function ondrop);
    virtual html::Function getOndurationchange();
    virtual void setOndurationchange(html::Function ondurationchange);
    virtual html::Function getOnemptied();
    virtual void setOnemptied(html::Function onemptied);
    virtual html::Function getOnended();
    virtual void setOnended(html::Function onended);
    virtual html::Function getOnerror();
    virtual void setOnerror(html::Function onerror);
    virtual html::Function getOnfocus();
    virtual void setOnfocus(html::Function onfocus);
    virtual html::Function getOnformchange();
    virtual void setOnformchange(html::Function onformchange);
    virtual html::Function getOnforminput();
    virtual void setOnforminput(html::Function onforminput);
    virtual html::Function getOninput();
    virtual void setOninput(html::Function oninput);
    virtual html::Function getOninvalid();
    virtual void setOninvalid(html::Function oninvalid);
    virtual html::Function getOnkeydown();
    virtual void setOnkeydown(html::Function onkeydown);
    virtual html::Function getOnkeypress();
    virtual void setOnkeypress(html::Function onkeypress);
    virtual html::Function getOnkeyup();
    virtual void setOnkeyup(html::Function onkeyup);
    virtual html::Function getOnload();
    virtual void setOnload(html::Function onload);
    virtual html::Function getOnloadeddata();
    virtual void setOnloadeddata(html::Function onloadeddata);
    virtual html::Function getOnloadedmetadata();
    virtual void setOnloadedmetadata(html::Function onloadedmetadata);
    virtual html::Function getOnloadstart();
    virtual void setOnloadstart(html::Function onloadstart);
    virtual html::Function getOnmousedown();
    virtual void setOnmousedown(html::Function onmousedown);
    virtual html::Function getOnmousemove();
    virtual void setOnmousemove(html::Function onmousemove);
    virtual html::Function getOnmouseout();
    virtual void setOnmouseout(html::Function onmouseout);
    virtual html::Function getOnmouseover();
    virtual void setOnmouseover(html::Function onmouseover);
    virtual html::Function getOnmouseup();
    virtual void setOnmouseup(html::Function onmouseup);
    virtual html::Function getOnmousewheel();
    virtual void setOnmousewheel(html::Function onmousewheel);
    virtual html::Function getOnpause();
    virtual void setOnpause(html::Function onpause);
    virtual html::Function getOnplay();
    virtual void setOnplay(html::Function onplay);
    virtual html::Function getOnplaying();
    virtual void setOnplaying(html::Function onplaying);
    virtual html::Function getOnprogress();
    virtual void setOnprogress(html::Function onprogress);
    virtual html::Function getOnratechange();
    virtual void setOnratechange(html::Function onratechange);
    virtual html::Function getOnreadystatechange();
    virtual void setOnreadystatechange(html::Function onreadystatechange);
    virtual html::Function getOnscroll();
    virtual void setOnscroll(html::Function onscroll);
    virtual html::Function getOnseeked();
    virtual void setOnseeked(html::Function onseeked);
    virtual html::Function getOnseeking();
    virtual void setOnseeking(html::Function onseeking);
    virtual html::Function getOnselect();
    virtual void setOnselect(html::Function onselect);
    virtual html::Function getOnshow();
    virtual void setOnshow(html::Function onshow);
    virtual html::Function getOnstalled();
    virtual void setOnstalled(html::Function onstalled);
    virtual html::Function getOnsubmit();
    virtual void setOnsubmit(html::Function onsubmit);
    virtual html::Function getOnsuspend();
    virtual void setOnsuspend(html::Function onsuspend);
    virtual html::Function getOntimeupdate();
    virtual void setOntimeupdate(html::Function ontimeupdate);
    virtual html::Function getOnvolumechange();
    virtual void setOnvolumechange(html::Function onvolumechange);
    virtual html::Function getOnwaiting();
    virtual void setOnwaiting(html::Function onwaiting);
    // HTMLDocument-35
    virtual std::u16string getFgColor();
    virtual void setFgColor(std::u16string fgColor);
    virtual std::u16string getBgColor();
    virtual void setBgColor(std::u16string bgColor);
    virtual std::u16string getLinkColor();
    virtual void setLinkColor(std::u16string linkColor);
    virtual std::u16string getVlinkColor();
    virtual void setVlinkColor(std::u16string vlinkColor);
    virtual std::u16string getAlinkColor();
    virtual void setAlinkColor(std::u16string alinkColor);
    virtual html::HTMLCollection getAnchors();
    virtual html::HTMLCollection getApplets();
    virtual void clear();
    virtual html::HTMLAllCollection getAll();
    // HTMLDocument-36
    virtual NodeList getItems();
    virtual NodeList getItems(std::u16string typeNames);

    // Node - override
    virtual unsigned short getNodeType();
    virtual Node appendChild(Node newChild) throw(DOMException);

    // Object
    virtual Any message_(uint32_t selector, const char* id, int argc, Any* argv) {
        return Document::dispatch(this, selector, id, argc, argv);
    }

    DocumentImp(const std::u16string& url = u"about:blank");
    ~DocumentImp();
};

}}}}  // org::w3c::dom::bootstrap

#endif  // DOCUMENT_IMP_H
