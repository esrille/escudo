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

#ifndef ORG_W3C_DOM_BOOTSTRAP_DOCUMENTIMP_H_INCLUDED
#define ORG_W3C_DOM_BOOTSTRAP_DOCUMENTIMP_H_INCLUDED

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <org/w3c/dom/Document.h>
#include "NodeImp.h"

#include <org/w3c/dom/stylesheets/StyleSheet.h>
#include <org/w3c/dom/css/CSSStyleDeclaration.h>
#include <org/w3c/dom/CaretPosition.h>
#include <org/w3c/dom/events/Event.h>
#include <org/w3c/dom/Node.h>
#include <org/w3c/dom/DOMImplementation.h>
#include <org/w3c/dom/DocumentFragment.h>
#include <org/w3c/dom/DocumentType.h>
#include <org/w3c/dom/Element.h>
#include <org/w3c/dom/Text.h>
#include <org/w3c/dom/ProcessingInstruction.h>
#include <org/w3c/dom/Comment.h>
#include <org/w3c/dom/ranges/Range.h>
#include <org/w3c/dom/traversal/NodeIterator.h>
#include <org/w3c/dom/traversal/TreeWalker.h>
#include <org/w3c/dom/traversal/NodeFilter.h>
#include <org/w3c/dom/NodeList.h>
#include <org/w3c/dom/html/HTMLCollection.h>
#include <org/w3c/dom/DOMStringList.h>
#include <org/w3c/dom/events/EventHandlerNonNull.h>
#include <org/w3c/dom/events/OnErrorEventHandlerNonNull.h>
#include <org/w3c/dom/html/HTMLAllCollection.h>
#include <org/w3c/dom/html/HTMLElement.h>
#include <org/w3c/dom/html/HTMLHeadElement.h>
#include <org/w3c/dom/html/Window.h>
#include <org/w3c/dom/html/Location.h>

#include <deque>
#include <list>

#include "NodeImp.h"
#include "DocumentWindow.h"
#include "EventListenerImp.h"
#include "WindowImp.h"
#include "html/HTMLScriptElementImp.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

class DocumentImp : public ObjectMixin<DocumentImp, NodeImp>
{
    std::u16string url;
    std::u16string contentType;
    DocumentType doctype;
    int mode;
    std::u16string charset;
    std::u16string readyState;
    std::u16string compatMode;
    std::deque<stylesheets::StyleSheet> styleSheets;
    unsigned loadEventDelayCount;
    bool contentLoaded;

    HTMLScriptElementImp* pendingParsingBlockingScript;
    std::list<html::HTMLScriptElement> deferScripts;
    std::list<html::HTMLScriptElement> asyncScripts;
    std::list<html::HTMLScriptElement> orderedScripts;

    WindowImp* defaultView;
    ElementImp* activeElement;
    int error;

    virtual void setEventHandler(const std::u16string& type, Object handler);

    // XBL 2.0
    std::map<const std::u16string, html::Window> bindingDocuments;

    bool processScripts(std::list<html::HTMLScriptElement>& scripts);

public:
    DocumentImp(const std::u16string& url = u"about:blank");
    ~DocumentImp();

    enum
    {
         NoQuirksMode,  // default
         QuirksMode,
         LimitedQuirksMode
    };

    WindowImp* getDefaultWindow() const {
        return defaultView;
    }
    void setDefaultView(WindowImp* view);

    int getMode() const {
        return mode;
    }
    void setMode(int value) {
        mode = value;
    }

    void addDeferScript(HTMLScriptElementImp* script) {
        deferScripts.push_back(script);
    }
    void addAsyncScript(HTMLScriptElementImp* script) {
        asyncScripts.push_back(script);
    }
    void addOrderedScript(HTMLScriptElementImp* script) {
        orderedScripts.push_back(script);
    }
    void removeDeferScript(HTMLScriptElementImp* script) {
        for (auto i = deferScripts.begin(); i != deferScripts.end(); ++i) {
            if (i->self() == script) {
                deferScripts.erase(i);
                break;
            }
        }
    }
    void removeAsyncScript(HTMLScriptElementImp* script) {
        for (auto i = asyncScripts.begin(); i != asyncScripts.end(); ++i) {
            if (i->self() == script) {
                asyncScripts.erase(i);
                break;
            }
        }
    }
    void removeOrderedScript(HTMLScriptElementImp* script) {
        for (auto i = orderedScripts.begin(); i != orderedScripts.end(); ++i) {
            if (i->self() == script) {
                orderedScripts.erase(i);
                break;
            }
        }
    }
    bool processDeferScripts() {
        return processScripts(deferScripts);
    }
    bool hasAsyncScripts() {
        return !asyncScripts.empty();
    }
    bool processOrderedScripts() {
        return processScripts(orderedScripts);
    }

    bool hasContentLoaded() const {
        return contentLoaded;
    }
    void setContentLoaded() {
        contentLoaded = true;
    }

    void setCharacterSet(const std::u16string& charset);

    int getError() const {
        return error;
    }
    void setError(int value) {
        error = value;
    }

    HTMLScriptElementImp* getPendingParsingBlockingScript() const {
        return pendingParsingBlockingScript;
    }
    void setPendingParsingBlockingScript(HTMLScriptElementImp* element) {
        pendingParsingBlockingScript = element;
    }

    void setReadyState(const std::u16string& readyState);

    void clearStyleSheets() {
        styleSheets.clear();
    }
    void addStyleSheet(stylesheets::StyleSheet sheet) {
        if (sheet)
            styleSheets.push_back(sheet);
    }
    void resetStyleSheets();

    void setURL(const std::u16string& url);
    void setContentType(const std::u16string& type);
    void setDoctype(DocumentType type);

    ElementImp* getFocus() const {
        return activeElement;
    }
    void setFocus(ElementImp* element);

    DocumentWindowPtr activate();

    unsigned incrementLoadEventDelayCount() {
        return ++loadEventDelayCount;
    }
    unsigned decrementLoadEventDelayCount();

    bool isBindingDocumentWindow(const WindowImp* window) const;

    // Node - override
    virtual unsigned short getNodeType();
    virtual Node appendChild(Node newChild) throw(DOMException);

    // Document
    DOMImplementation getImplementation();
    std::u16string getURL();
    std::u16string getDocumentURI();
    void setDocumentURI(const std::u16string& documentURI);
    std::u16string getCompatMode();
    std::u16string getCharacterSet();
    std::u16string getContentType();
    DocumentType getDoctype();
    Element getDocumentElement();
    html::HTMLCollection getElementsByTagName(const std::u16string& localName);
    html::HTMLCollection getElementsByTagNameNS(const Nullable<std::u16string>& _namespace, const std::u16string& localName);
    html::HTMLCollection getElementsByClassName(const std::u16string& classNames);
    Element getElementById(const std::u16string& elementId);
    Element createElement(const std::u16string& localName);
    Element createElementNS(const Nullable<std::u16string>& _namespace, const std::u16string& qualifiedName);
    DocumentFragment createDocumentFragment();
    Text createTextNode(const std::u16string& data);
    Comment createComment(const std::u16string& data);
    ProcessingInstruction createProcessingInstruction(const std::u16string& target, const std::u16string& data);
    Node importNode(Node node, bool deep = true);
    Node adoptNode(Node node);
    events::Event createEvent(const std::u16string& eventInterfaceName);
    ranges::Range createRange();
    traversal::NodeIterator createNodeIterator(Node root, unsigned int whatToShow = 0xFFFFFFFF, traversal::NodeFilter filter = 0);
    traversal::TreeWalker createTreeWalker(Node root, unsigned int whatToShow = 0xFFFFFFFF, traversal::NodeFilter filter = 0);
    // DocumentCSS
    css::CSSStyleDeclaration getOverrideStyle(Element elt, const Nullable<std::u16string>& pseudoElt);
    // Document-47
    stylesheets::StyleSheetList getStyleSheets();
    Nullable<std::u16string> getSelectedStyleSheetSet();
    void setSelectedStyleSheetSet(const Nullable<std::u16string>& selectedStyleSheetSet);
    Nullable<std::u16string> getLastStyleSheetSet();
    Nullable<std::u16string> getPreferredStyleSheetSet();
    DOMStringList getStyleSheetSets();
    void enableStyleSheetsForSet(const Nullable<std::u16string>& name);
    // Document-48
    Element elementFromPoint(float x, float y);
    CaretPosition caretPositionFromPoint(float x, float y);
    // Document-50
    html::Location getLocation();
    void setLocation(const std::u16string& location);
    std::u16string getDomain();
    void setDomain(const std::u16string& domain);
    std::u16string getReferrer();
    std::u16string getCookie();
    void setCookie(const std::u16string& cookie);
    std::u16string getLastModified();
    std::u16string getReadyState();
    Any getElement(const std::u16string& name);
    std::u16string getTitle();
    void setTitle(const std::u16string& title);
    std::u16string getDir();
    void setDir(const std::u16string& dir);
    html::HTMLElement getBody();
    void setBody(html::HTMLElement body);
    html::HTMLHeadElement getHead();
    html::HTMLCollection getImages();
    html::HTMLCollection getEmbeds();
    html::HTMLCollection getPlugins();
    html::HTMLCollection getLinks();
    html::HTMLCollection getForms();
    html::HTMLCollection getScripts();
    NodeList getElementsByName(const std::u16string& elementName);
    Document open(const std::u16string& type = u"text/html", const std::u16string& replace = u"");
    html::Window open(const std::u16string& url, const std::u16string& name, const std::u16string& features, bool replace = false);
    void close();
    void write(Variadic<std::u16string> text = Variadic<std::u16string>());
    void writeln(Variadic<std::u16string> text = Variadic<std::u16string>());
    html::Window getDefaultView();
    Element getActiveElement();
    bool hasFocus();
    std::u16string getDesignMode();
    void setDesignMode(const std::u16string& designMode);
    bool execCommand(const std::u16string& commandId);
    bool execCommand(const std::u16string& commandId, bool showUI);
    bool execCommand(const std::u16string& commandId, bool showUI, const std::u16string& value);
    bool queryCommandEnabled(const std::u16string& commandId);
    bool queryCommandIndeterm(const std::u16string& commandId);
    bool queryCommandState(const std::u16string& commandId);
    bool queryCommandSupported(const std::u16string& commandId);
    std::u16string queryCommandValue(const std::u16string& commandId);
    html::HTMLCollection getCommands();
    events::EventHandlerNonNull getOnabort();
    void setOnabort(events::EventHandlerNonNull onabort);
    events::EventHandlerNonNull getOnblur();
    void setOnblur(events::EventHandlerNonNull onblur);
    events::EventHandlerNonNull getOncancel();
    void setOncancel(events::EventHandlerNonNull oncancel);
    events::EventHandlerNonNull getOncanplay();
    void setOncanplay(events::EventHandlerNonNull oncanplay);
    events::EventHandlerNonNull getOncanplaythrough();
    void setOncanplaythrough(events::EventHandlerNonNull oncanplaythrough);
    events::EventHandlerNonNull getOnchange();
    void setOnchange(events::EventHandlerNonNull onchange);
    events::EventHandlerNonNull getOnclick();
    void setOnclick(events::EventHandlerNonNull onclick);
    events::EventHandlerNonNull getOnclose();
    void setOnclose(events::EventHandlerNonNull onclose);
    events::EventHandlerNonNull getOncontextmenu();
    void setOncontextmenu(events::EventHandlerNonNull oncontextmenu);
    events::EventHandlerNonNull getOncuechange();
    void setOncuechange(events::EventHandlerNonNull oncuechange);
    events::EventHandlerNonNull getOndblclick();
    void setOndblclick(events::EventHandlerNonNull ondblclick);
    events::EventHandlerNonNull getOndrag();
    void setOndrag(events::EventHandlerNonNull ondrag);
    events::EventHandlerNonNull getOndragend();
    void setOndragend(events::EventHandlerNonNull ondragend);
    events::EventHandlerNonNull getOndragenter();
    void setOndragenter(events::EventHandlerNonNull ondragenter);
    events::EventHandlerNonNull getOndragleave();
    void setOndragleave(events::EventHandlerNonNull ondragleave);
    events::EventHandlerNonNull getOndragover();
    void setOndragover(events::EventHandlerNonNull ondragover);
    events::EventHandlerNonNull getOndragstart();
    void setOndragstart(events::EventHandlerNonNull ondragstart);
    events::EventHandlerNonNull getOndrop();
    void setOndrop(events::EventHandlerNonNull ondrop);
    events::EventHandlerNonNull getOndurationchange();
    void setOndurationchange(events::EventHandlerNonNull ondurationchange);
    events::EventHandlerNonNull getOnemptied();
    void setOnemptied(events::EventHandlerNonNull onemptied);
    events::EventHandlerNonNull getOnended();
    void setOnended(events::EventHandlerNonNull onended);
    events::OnErrorEventHandlerNonNull getOnerror();
    void setOnerror(events::OnErrorEventHandlerNonNull onerror);
    events::EventHandlerNonNull getOnfocus();
    void setOnfocus(events::EventHandlerNonNull onfocus);
    events::EventHandlerNonNull getOninput();
    void setOninput(events::EventHandlerNonNull oninput);
    events::EventHandlerNonNull getOninvalid();
    void setOninvalid(events::EventHandlerNonNull oninvalid);
    events::EventHandlerNonNull getOnkeydown();
    void setOnkeydown(events::EventHandlerNonNull onkeydown);
    events::EventHandlerNonNull getOnkeypress();
    void setOnkeypress(events::EventHandlerNonNull onkeypress);
    events::EventHandlerNonNull getOnkeyup();
    void setOnkeyup(events::EventHandlerNonNull onkeyup);
    events::EventHandlerNonNull getOnload();
    void setOnload(events::EventHandlerNonNull onload);
    events::EventHandlerNonNull getOnloadeddata();
    void setOnloadeddata(events::EventHandlerNonNull onloadeddata);
    events::EventHandlerNonNull getOnloadedmetadata();
    void setOnloadedmetadata(events::EventHandlerNonNull onloadedmetadata);
    events::EventHandlerNonNull getOnloadstart();
    void setOnloadstart(events::EventHandlerNonNull onloadstart);
    events::EventHandlerNonNull getOnmousedown();
    void setOnmousedown(events::EventHandlerNonNull onmousedown);
    events::EventHandlerNonNull getOnmousemove();
    void setOnmousemove(events::EventHandlerNonNull onmousemove);
    events::EventHandlerNonNull getOnmouseout();
    void setOnmouseout(events::EventHandlerNonNull onmouseout);
    events::EventHandlerNonNull getOnmouseover();
    void setOnmouseover(events::EventHandlerNonNull onmouseover);
    events::EventHandlerNonNull getOnmouseup();
    void setOnmouseup(events::EventHandlerNonNull onmouseup);
    events::EventHandlerNonNull getOnmousewheel();
    void setOnmousewheel(events::EventHandlerNonNull onmousewheel);
    events::EventHandlerNonNull getOnpause();
    void setOnpause(events::EventHandlerNonNull onpause);
    events::EventHandlerNonNull getOnplay();
    void setOnplay(events::EventHandlerNonNull onplay);
    events::EventHandlerNonNull getOnplaying();
    void setOnplaying(events::EventHandlerNonNull onplaying);
    events::EventHandlerNonNull getOnprogress();
    void setOnprogress(events::EventHandlerNonNull onprogress);
    events::EventHandlerNonNull getOnratechange();
    void setOnratechange(events::EventHandlerNonNull onratechange);
    events::EventHandlerNonNull getOnreset();
    void setOnreset(events::EventHandlerNonNull onreset);
    events::EventHandlerNonNull getOnscroll();
    void setOnscroll(events::EventHandlerNonNull onscroll);
    events::EventHandlerNonNull getOnseeked();
    void setOnseeked(events::EventHandlerNonNull onseeked);
    events::EventHandlerNonNull getOnseeking();
    void setOnseeking(events::EventHandlerNonNull onseeking);
    events::EventHandlerNonNull getOnselect();
    void setOnselect(events::EventHandlerNonNull onselect);
    events::EventHandlerNonNull getOnshow();
    void setOnshow(events::EventHandlerNonNull onshow);
    events::EventHandlerNonNull getOnstalled();
    void setOnstalled(events::EventHandlerNonNull onstalled);
    events::EventHandlerNonNull getOnsubmit();
    void setOnsubmit(events::EventHandlerNonNull onsubmit);
    events::EventHandlerNonNull getOnsuspend();
    void setOnsuspend(events::EventHandlerNonNull onsuspend);
    events::EventHandlerNonNull getOntimeupdate();
    void setOntimeupdate(events::EventHandlerNonNull ontimeupdate);
    events::EventHandlerNonNull getOnvolumechange();
    void setOnvolumechange(events::EventHandlerNonNull onvolumechange);
    events::EventHandlerNonNull getOnwaiting();
    void setOnwaiting(events::EventHandlerNonNull onwaiting);
    events::EventHandlerNonNull getOnreadystatechange();
    void setOnreadystatechange(events::EventHandlerNonNull onreadystatechange);
    // Document-52
    std::u16string getFgColor();
    void setFgColor(const std::u16string& fgColor);
    std::u16string getLinkColor();
    void setLinkColor(const std::u16string& linkColor);
    std::u16string getVlinkColor();
    void setVlinkColor(const std::u16string& vlinkColor);
    std::u16string getAlinkColor();
    void setAlinkColor(const std::u16string& alinkColor);
    std::u16string getBgColor();
    void setBgColor(const std::u16string& bgColor);
    html::HTMLCollection getAnchors();
    html::HTMLCollection getApplets();
    void clear();
    html::HTMLAllCollection getAll();
    // NodeSelector
    Element querySelector(const std::u16string& selectors);
    NodeList querySelectorAll(const std::u16string& selectors);
    // DocumentXBL
    html::HTMLCollection getBindingDocuments();
    Document loadBindingDocument(const std::u16string& documentURI);
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

}
}
}
}

#endif  // ORG_W3C_DOM_BOOTSTRAP_DOCUMENTIMP_H_INCLUDED
