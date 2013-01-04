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

#include "WindowImp.h"

#include <new>
#include <iostream>
#include <boost/version.hpp>
#include <boost/bind.hpp>

#include "BeforeUnloadEventImp.h"
#include "DOMImplementationImp.h"
#include "DocumentImp.h"
#include "KeyboardEventImp.h"
#include "MouseEventImp.h"
#include "NodeImp.h"
#include "css/BoxImage.h"
#include "css/Ico.h"
#include "css/ViewCSSImp.h"
#include "html/HTMLIFrameElementImp.h"
#include "html/HTMLLinkElementImp.h"
#include "html/HTMLScriptElementImp.h"
#include "http/HTTPConnection.h"

#include "Test.util.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

WindowImp::Parser::Parser(DocumentImp* document, int fd, const std::string& optionalEncoding) :
    stream(fd, boost::iostreams::never_close_handle),   // TODO: never_close_handle?
    htmlInputStream(stream, optionalEncoding),
    tokenizer(&htmlInputStream),
    parser(document, &tokenizer)
{
    stream.seekg(0, std::ios::beg);
}

WindowImp::WindowImp(WindowImp* parent, ElementImp* frameElement) :
    request(parent ? parent->getLocation().getHref() : u""),
    history(this),
    backgroundTask(this),
    thread(std::ref(backgroundTask)),
    window(0),
    view(0),
    viewFlags(0),
    parent(parent),
    frameElement(frameElement),
    parser(0),
    clickTarget(0),
    detail(0),
    buttons(0),
    scrollWidth(0),
    scrollHeight(0),
    width(816),     // US letter size, 96 DPI
    height(1056),
    redisplay(false),
    zoomable(true),
    zoom(1.0f),
    faviconOverridable(false),
    windowDepth(0)
{
    if (parent) {
        parent->childWindows.push_back(this);
        windowDepth = parent->windowDepth + 1;
    }
}

WindowImp::~WindowImp()
{
    if (parent) {
        for (auto i = parent->childWindows.begin(); i != parent->childWindows.end(); ++i) {
            if (*i == this) {
                parent->childWindows.erase(i);
                break;
            }
        }
    }
    backgroundTask.abort();
    thread.join();
}

void WindowImp::setSize(unsigned w, unsigned h)
{
    if (width != w || height != h) {
        width = w;
        height = h;
        setFlags(Box::NEED_REFLOW);
    }
}

void WindowImp::setFlags(unsigned f)
{
    if (view) {
        view->setFlags(f | viewFlags);
        viewFlags = 0;
    } else
        viewFlags |= f;
}

DocumentWindowPtr WindowImp::activate()
{
    if (window) {
        window->activate(this);
        return window;
    }
    return 0;
}

void WindowImp::enableZoom(bool value)
{
    zoomable = value;
}

float WindowImp::getZoom() const
{
    return zoom;
}

void WindowImp::setZoom(float value)
{
    if (zoomable) {
        zoom = value;
        if (view) {
            view->setZoom(zoom);
            redisplay = true;
        }
    }
}

void WindowImp::updateView(ViewCSSImp* next)
{
    if (!next || view == next || !window || !window->getDocument())
        return;

    unsigned flags = 0;
    if (view) {
        flags |= view->gatherFlags();
        delete view;
    }
    view = next;
    if (viewFlags)
        setFlags(flags | viewFlags);
    view->setZoom(zoom);
    detail = 0;
    redisplay = true;

    if (!view->getTree()) {
        // cf. http://test.csswg.org/suites/css2.1/20110323/html4/root-box-003.htm
        canvas.shutdown();
        canvas.setup(width, height);
        canvas.beginRender(view->getBackgroundColor());
        canvas.endRender();
    }

    if (!parent) {
        std::u16string title = view->getDocument().getTitle();
        setWindowTitle(utfconv(title).c_str());
    }
}

void WindowImp::setDocumentWindow(const DocumentWindowPtr& window)
{
    this->window = window;
    delete view;
    view = 0;
    viewFlags = 0;
    if (window)
        backgroundTask.restart(BackgroundTask::Cascade);
    detail = 0;
    redisplay = true;
    setFavicon();
}

bool WindowImp::isBindingDocumentWindow() const
{
    if (!parent)
        return false;
    DocumentImp* document = dynamic_cast<DocumentImp*>(parent->getDocument().self());
    return document->isBindingDocumentWindow(this);
}

bool WindowImp::poll()
{
    if (!window)
        return false;
    DocumentImp* document = dynamic_cast<DocumentImp*>(window->getDocument().self());

    // Update the canvas before processing events.
    if (request.getReadyState() == HttpRequest::DONE && document && backgroundTask.getState() == BackgroundTask::Done) {
        ViewCSSImp* next = backgroundTask.getView();
        updateView(next);
        if (view && (view->gatherFlags() & Box::NEED_REPAINT)) {
            redisplay = true;
            render(0);
        }
    }

    while (backgroundTask.getState() == BackgroundTask::Done && !eventQueue.empty()) {
        const EventTask task = eventQueue.front();
        switch (task.type) {
        case EventTask::MouseDown:
        case EventTask::MouseUp:
            mouse(task);
            break;
        case EventTask::MouseMove:
            mouseMove(task);
            break;
        case EventTask::KeyDown:
            keydown(task);
            break;
        case EventTask::KeyUp:
            keyup(task);
            break;
        default:
            break;
        }
        eventQueue.pop_front();
    }

    for (auto i = childWindows.begin(); i != childWindows.end(); ++i) {
        WindowImp* child = *i;
        if (child->poll()) {
            redisplay |= true;
            if (view) {
                if (document && document->isBindingDocumentWindow(child))
                    view->setFlags(Box::NEED_SELECTOR_REMATCHING);
                else
                    view->setFlags(Box::NEED_REPAINT);
            }
        }
    }

    switch (request.getReadyState())
    {
    case HttpRequest::UNSENT:
        break;
    case HttpRequest::OPENED:
    case HttpRequest::HEADERS_RECEIVED:
    case HttpRequest::LOADING:
        break;
    case HttpRequest::DONE:
        if (!document) {
            recordTime("%*shttp request done", windowDepth * 2, "");
            // TODO: Check header
            Document newDocument = getDOMImplementation()->createDocument(u"", u"", 0);
            if (document = dynamic_cast<DocumentImp*>(newDocument.self())) {
                // TODO: Fire a simple unload event.
                window->setDocument(newDocument);
                document->setDefaultView(this);
                document->setURL(request.getRequestMessage().getURL());
                if (!request.getError())
                    history.update(window);
                else
                    document->setError(request.getError());
                parser = new(std::nothrow) Parser(document, request.getContentDescriptor(), request.getResponseMessage().getContentCharset());
                if (!parser)
                    break;  // TODO: error handling
            } else
                break;  // TODO: error handling
        }
        if (document->getReadyState() == u"loading") {
            // TODO: Note white it would be nice to parse the HTML docucment in
            // the background task, firstly we need to check if we can run JS
            // in the background.
            activate();

            HTMLScriptElementImp* script = document->getPendingParsingBlockingScript();
            if (script) {
                if (!script->isReadyToBeParserExecuted())
                    break;
                script->execute();
                document->setPendingParsingBlockingScript(0);
            }

            // TODO: run this in the background
            Token token;
            do {
                token = parser->getToken();
                parser->processToken(token);
            } while (token.getType() != Token::Type::EndOfFile && !document->getPendingParsingBlockingScript());
            if (document->getPendingParsingBlockingScript())
                break;
            document->setCharacterSet(utfconv(parser->getEncoding()));

            // TODO: Check if the parser has been aborted.
            NodeImp::evalTree(document);
            document->resetStyleSheets();
            recordTime("%*shtml parsed", windowDepth * 2, "");
            if (4 <= getLogLevel())
                dumpTree(std::cerr, document);
        }
        switch (backgroundTask.getState()) {
        case BackgroundTask::Cascaded:
            HTMLElementImp::xblEnteredDocument(document);
            backgroundTask.wakeUp(BackgroundTask::Layout);
            break;
        case BackgroundTask::Init:
        case BackgroundTask::Done: {
            ViewCSSImp* next = backgroundTask.getView();
            updateView(next);
            if (document->getReadyState() == u"interactive") {
                // TODO: Check if the parser has been aborted.

                // Run from the step 3. of 'stops parsing'.
                // cf. http://www.whatwg.org/specs/web-apps/current-work/multipage/the-end.html#the-end

                // Process scripts that will execute when the document has finished parsing (i.e., defer).
                // TODO: Check there's no style sheet that is blocking scripts.
                if (document->processDeferScripts()) {
                    if (!document->hasContentLoaded()) {
                        if (events::Event event = new(std::nothrow) EventImp) {
                            event.initEvent(u"DOMContentLoaded", true, false);
                            document->dispatchEvent(event);
                        }
                        document->decrementLoadEventDelayCount();
                        document->setContentLoaded();

                        backgroundTask.restart(BackgroundTask::Cascade);
                    }
                }
            }
            if (document->getReadyState() == u"complete") {
            }
            if (view) {
                if (unsigned flags = view->gatherFlags()) {
                    if (flags & Box::NEED_SELECTOR_REMATCHING) {
                        recordTime("%*strigger selector rematching", windowDepth * 2, "");
                        backgroundTask.restart(BackgroundTask::Cascade);
                        view = 0;
                    } else if (flags & Box::NEED_SELECTOR_MATCHING) {
                        recordTime("%*strigger restyling", windowDepth * 2, "");
                        backgroundTask.wakeUp(BackgroundTask::Cascade);
                        view = 0;
                    } else if (flags & (Box::NEED_STYLE_RECALCULATION | Box::NEED_EXPANSION | Box::NEED_CHILD_REFLOW | Box::NEED_REFLOW)) {
                        recordTime("%*strigger reflow", windowDepth * 2, "");
                        backgroundTask.wakeUp(BackgroundTask::Layout);
                        view = 0;
                    } else if (flags & Box::NEED_REPAINT)
                        redisplay = true;
                }
            }
            break;
        }
        default:
            break;
        }
        break;
    default:
        break;
    }
    bool result = redisplay;
    redisplay = false;
    if (!result && view && view->hasExpired(getTick())) {
        view->setFlags(Box::NEED_REPAINT);
        result = true;
    }
    if (result)
        recordTime("%*strigger repaint", windowDepth * 2, "");
    return result;
}

void WindowImp::render(ViewCSSImp* parentView)
{
    recordTime("%*srepaint begin: %s (%s)", windowDepth * 2, "", utfconv(window->getDocument().getReadyState()).c_str(), view ? "render" : "canvas");
    if (view) {
        if (view->gatherFlags() & Box::NEED_REPAINT) {
            view->clearFlags(Box::NEED_REPAINT);
            canvas.shutdown();
            canvas.setup(width, height);
            canvas.beginRender(view->getBackgroundColor());
            view->render(parentView ? parentView->getClipCount() : 0);
            scrollWidth = view->getScrollWidth();
            scrollHeight = view->getScrollHeight();
            canvas.endRender();
        }
        if (2 <= getLogLevel() && backgroundTask.isIdle() && !view->gatherFlags()) {
            unsigned depth = 1;
            for (WindowImp* w = this; w->parent; w = w->parent)
                ++depth;
            std::cout << "\n## " << window->getDocument().getReadyState();
            if (1 < depth)
                std::cout << " (" << depth << ')';
            std::cout << '\n';
            view->dump();
            std::cout << "##\n";
            std::cout.flush();
        }
    }
    canvas.render(width, height);
    recordTime("%*srepaint end", windowDepth * 2, "");
}

void WindowImp::mouse(int button, int up, int x, int y, int modifiers)
{
    eventQueue.emplace_back(up ? EventTask::MouseUp : EventTask::MouseDown, modifiers, x, y, button);
}

void WindowImp::mouseMove(int x, int y, int modifiers)
{
    if (!eventQueue.empty()) {
        EventTask& back = eventQueue.back();
        if (back.type == EventTask::MouseMove) {
            back.x = x;
            back.y = y;
            back.modifiers = modifiers;
            return;
        }
    }
    eventQueue.emplace_back(EventTask::MouseMove, modifiers, x, y);
}

void WindowImp::keydown(unsigned charCode, unsigned keyCode, int modifiers)
{
    eventQueue.emplace_back(EventTask::KeyDown, modifiers, charCode, keyCode);
}

void WindowImp::keyup(unsigned charCode, unsigned keyCode, int modifiers)
{
    eventQueue.emplace_back(EventTask::KeyUp, modifiers, charCode, keyCode);
}

void WindowImp::mouse(const EventTask& task)
{
    int button = task.button;
    int up = (task.type == task.MouseUp) ? true : false;
    int x = task.x;
    int y = task.y;
    int modifiers = task.modifiers;

    if (!view)
        return;

    recordTime("%*smouse (%d, %d)", windowDepth * 2, "", x, y);

    int shift = button;
    if (shift == 1)
        shift = 2;
    else if (shift == 2)
        shift = 1;
    if (up)
        buttons &= ~(1u << shift);
    else
        buttons |= (1u << shift);

    events::MouseEvent event(0);
    MouseEventImp* imp = 0;

    Box* box = view->boxFromPoint(x, y);
    if (!box)
        return;
    if (WindowImp* childWindow = box->getChildWindow()) {
        childWindow->mouse(button, up,
                           x - box->getX() - box->getBlankLeft(), y - box->getY() - box->getBlankTop(),
                           modifiers);
    }

    Element target = Box::getContainingElement(box->getTargetNode());

    // mousedown, mousemove
    if (imp = new(std::nothrow) MouseEventImp) {
        event = imp;
        if (!up) {
            clickTarget = target;
            ++detail;
        }
        imp->initMouseEvent(up ? u"mouseup" : u"mousedown",
                            true, true, this, detail, x, y, x, y,
                            modifiers & 2, modifiers & 4, modifiers & 1, false, button, 0);
        imp->setButtons(buttons);
        target.dispatchEvent(event);
    }

    if (!up || detail == 0)
        return;
    Box* clickBox = view->boxFromPoint(x, y);
    if (box != clickBox)
        return;

    // click
    if (imp = new(std::nothrow) MouseEventImp) {
        event = imp;
        imp->initMouseEvent(u"click",
                            true, true, this, detail, x, y, x, y,
                            modifiers & 2, modifiers & 4, modifiers & 1, false, button, 0);
        imp->setButtons(buttons);
        target.dispatchEvent(event);
    }
}

void WindowImp::mouseMove(const EventTask& task)
{
    int x = task.x;
    int y = task.y;
    int modifiers = task.modifiers;

    if (!view)
        return;
    Box* box = view->boxFromPoint(x, y);
    if (!box)
        return;
    if (WindowImp* childWindow = box->getChildWindow()) {
        childWindow->mouseMove(x - box->getX() - box->getBlankLeft(),
                               y - box->getY() - box->getBlankTop(),
                               modifiers);
    }

    Element target = Box::getContainingElement(box->getTargetNode());
    view->setHovered(target);

    // mousemove
    events::MouseEvent event(0);
    MouseEventImp* imp = 0;
    if (imp = new(std::nothrow) MouseEventImp) {
        event = imp;
        if (target != clickTarget)
            detail = 0;
        imp->initMouseEvent(u"mousemove",
                            true, true, this, detail, x, y, x, y,
                            modifiers & 2, modifiers & 4, modifiers & 1, false, 0, 0);
        imp->setButtons(buttons);
        target.dispatchEvent(event);
    }
}

void WindowImp::keydown(const EventTask& task)
{
    unsigned charCode = task.charCode;
    unsigned keyCode = task.keyCode;
    int modifiers = task.modifiers;

    Document document = window->getDocument();
    if (!document)
        return;
    Element e = document.getActiveElement();
    if (!e)
        return;

    if (auto iframe = dynamic_cast<HTMLIFrameElementImp*>(e.self())) {
        if (auto child = dynamic_cast<WindowImp*>(iframe->getContentWindow().self()))
            child->keydown(charCode, keyCode, modifiers);
    }

    KeyboardEventImp* imp = new(std::nothrow) KeyboardEventImp(modifiers, charCode, keyCode, 0);
    events::KeyboardEvent event = imp;
    imp->initKeyboardEvent(u"keydown", true, true, this,
                           u"", u"", 0, u"", false, u"");
    e.dispatchEvent(event);

    if (!charCode)
        return;
    imp = new(std::nothrow) KeyboardEventImp(modifiers, charCode, keyCode, 0);
    event = imp;
    imp->initKeyboardEvent(u"keypress", true, true, this,
                           u"", u"", 0, u"", false, u"");
    e.dispatchEvent(event);
}

void WindowImp::keyup(const EventTask& task)
{
    unsigned charCode = task.charCode;
    unsigned keyCode = task.keyCode;
    int modifiers = task.modifiers;

    Document document = window->getDocument();
    if (!document)
        return;
    Element e = document.getActiveElement();
    if (!e)
        return;

    if (auto iframe = dynamic_cast<HTMLIFrameElementImp*>(e.self())) {
        if (auto child = dynamic_cast<WindowImp*>(iframe->getContentWindow().self()))
            child->keyup(charCode, keyCode, modifiers);
    }

    KeyboardEventImp* imp = new(std::nothrow) KeyboardEventImp(modifiers, charCode, keyCode, 0);
    events::KeyboardEvent event(imp);
    imp->initKeyboardEvent(u"keyup", true, true, this,
                           u"", u"", 0, u"", false, u"");
    e.dispatchEvent(event);
}

void WindowImp::setFavicon(IcoImage* ico, std::FILE* file)
{
    if (parent) {
        if (parent->getFaviconOverridable())
            parent->setFavicon(ico, file);
        return;
    }
    for (size_t i = 0; i < ico->getPlaneCount(); ++i) {
        const IconDirectoryEntry& ent(ico->getEntry(i));
        if (BoxImage* image = ico->open(file, i)) {
            uint32_t* pixels = reinterpret_cast<uint32_t*>(image->getPixels());
            setIcon(i, ent.getWidth(), ent.getHeight(), pixels);
            delete image;
        }
    }
}

void WindowImp::setFavicon(BoxImage* image)
{
    if (parent) {
        if (parent->getFaviconOverridable())
            parent->setFavicon(image);
        return;
    }
    uint32_t* pixels = reinterpret_cast<uint32_t*>(image->getPixels());
    setIcon(0, image->getNaturalWidth(), image->getNaturalHeight(), pixels);
}

void WindowImp::setFavicon()
{
    for (WindowImp* w = this; w; w = w->parent) {
        if (w->parent && !w->parent->getFaviconOverridable())
            return;
        if (!w->window)
            continue;
        DocumentImp* document = dynamic_cast<DocumentImp*>(w->window->getDocument().self());
        if (!document)
            continue;
        html::HTMLHeadElement head = document->getHead();
        if (!head)
            continue;
        for (auto i = head.getFirstElementChild(); i; i = i.getNextElementSibling()) {
            if (i.getTagName() == u"link") {
                if (auto link = dynamic_cast<HTMLLinkElementImp*>(i.self())) {
                    if (link->setFavicon(document))
                        return;
                }
            }
        }
    }
}

//
// CSSOM View support operations
//

Element WindowImp::elementFromPoint(float x, float y)
{
    if (!view)
        return 0;
    if (x < 0.0f || width < x || y < 0.0f || height < y)
        return 0;
    Box* box = view->boxFromPoint(x, y);
    if (!box)
        return 0;
    for (Node node = box->getTargetNode(); node; node = node.getParentNode()) {
        if (node.getNodeType() == Node::ELEMENT_NODE)
            return interface_cast<Element>(node);
    }
    return 0;
}

//
// html::Window
//

html::Window WindowImp::getWindow()
{
    return this;
}

Any WindowImp::getSelf()
{
    return this;
}

void WindowImp::setSelf(Any self)
{
    // TODO: implement me!
}

Document WindowImp::getDocument()
{
    return !window ? 0 : window->getDocument();
}

std::u16string WindowImp::getName()
{
    // TODO: implement me!
    return u"";
}

void WindowImp::setName(const std::u16string& name)
{
    // TODO: implement me!
}

html::Location WindowImp::getLocation()
{
    return getDocument().getLocation();
}

void WindowImp::setLocation(const std::u16string& location)
{
    getLocation().setHref(location);
}

html::History WindowImp::getHistory()
{
    return &history;
}

Any WindowImp::getLocationbar()
{
    // TODO: implement me!
    return 0;
}

void WindowImp::setLocationbar(Any locationbar)
{
    // TODO: implement me!
}

Any WindowImp::getMenubar()
{
    // TODO: implement me!
    return 0;
}

void WindowImp::setMenubar(Any menubar)
{
    // TODO: implement me!
}

Any WindowImp::getPersonalbar()
{
    // TODO: implement me!
    return 0;
}

void WindowImp::setPersonalbar(Any personalbar)
{
    // TODO: implement me!
}

Any WindowImp::getScrollbars()
{
    // TODO: implement me!
    return 0;
}

void WindowImp::setScrollbars(Any scrollbars)
{
    // TODO: implement me!
}

Any WindowImp::getStatusbar()
{
    // TODO: implement me!
    return 0;
}

void WindowImp::setStatusbar(Any statusbar)
{
    // TODO: implement me!
}

Any WindowImp::getToolbar()
{
    // TODO: implement me!
    return 0;
}

void WindowImp::setToolbar(Any toolbar)
{
    // TODO: implement me!
}

std::u16string WindowImp::getStatus()
{
    // TODO: implement me!
    return u"";
}

void WindowImp::setStatus(const std::u16string& status)
{
    // TODO: implement me!
}

void WindowImp::close()
{
    // TODO: implement me!
}

void WindowImp::stop()
{
    // TODO: implement me!
}

void WindowImp::focus()
{
    // TODO: implement me!
}

void WindowImp::blur()
{
    // TODO: implement me!
}

Any WindowImp::getFrames()
{
    return this;
}

void WindowImp::setFrames(Any frames)
{
    // TODO: implement me!
}

Any WindowImp::getLength()
{
    return childWindows.size();
}

void WindowImp::setLength(Any length)
{
    // TODO: implement me!
}

html::Window WindowImp::getTop()
{
    WindowImp* top = this;
    while (top->parent)
        top = top->parent;
    return top;
}

html::Window WindowImp::getOpener()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOpener(html::Window opener)
{
    // TODO: implement me!
}

html::Window WindowImp::getParent()
{
    return parent;
}

Element WindowImp::getFrameElement()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

html::Window WindowImp::open(const std::u16string& url, const std::u16string& target, const std::u16string& features, bool replace)
{
    if (window) {
        if (DocumentImp* document = dynamic_cast<DocumentImp*>(window->getDocument().self())) {
            URL base(document->getDocumentURI());
            URL link(base, url);
            if (base.isSameExceptFragments(link)) {
                // cf. http://www.whatwg.org/specs/web-apps/current-work/multipage/history.html#scroll-to-fragid
                // TODO: update history, etc.
                std::u16string hash = link.getHash();
                if (hash[0] == '#')
                    hash.erase(0, 1);
                if (Element element = document->getElementById(hash))
                    element.scrollIntoView(true);
                return this;
            }

            // Prompt to unload the Document object.
            if (html::BeforeUnloadEvent event = new(std::nothrow) BeforeUnloadEventImp) {
                window->dispatchEvent(event);
                if (!event.getReturnValue().empty() || event.getDefaultPrevented())
                    return this;
                if (parent && parent->getFaviconOverridable())
                    parent->setFavicon();
            }
        }
    }

    backgroundTask.restart();

    // TODO: add more details
    window = new(std::nothrow) DocumentWindow;

    request.abort();
    history.setReplace(replace);
    request.open(u"get", url.empty() ? u"about:blank" : url);
    request.send();
    return this;
}

html::Window WindowImp::getElement(unsigned int index)
{
    if (index < childWindows.size())
        return childWindows[index];
    return 0;
}

Object WindowImp::getElement(const std::u16string& name)
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

html::Navigator WindowImp::getNavigator()
{
    return &navigator;
}

html::External WindowImp::getExternal()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

html::ApplicationCache WindowImp::getApplicationCache()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::alert(const std::u16string& message)
{
    std::cerr << message << '\n';
}

bool WindowImp::confirm(const std::u16string& message)
{
    // TODO: implement me!
    return 0;
}

Nullable<std::u16string> WindowImp::prompt(const std::u16string& message)
{
    // TODO: implement me!
    return u"";
}

Nullable<std::u16string> WindowImp::prompt(const std::u16string& message, const std::u16string& _default)
{
    // TODO: implement me!
    return u"";
}

void WindowImp::print()
{
    // TODO: implement me!
}

Any WindowImp::showModalDialog(const std::u16string& url)
{
    // TODO: implement me!
    return 0;
}

Any WindowImp::showModalDialog(const std::u16string& url, Any argument)
{
    // TODO: implement me!
    return 0;
}

events::EventHandlerNonNull WindowImp::getOnabort()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOnabort(events::EventHandlerNonNull onabort)
{
    // TODO: implement me!
}

events::EventHandlerNonNull WindowImp::getOnafterprint()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOnafterprint(events::EventHandlerNonNull onafterprint)
{
    // TODO: implement me!
}

events::EventHandlerNonNull WindowImp::getOnbeforeprint()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOnbeforeprint(events::EventHandlerNonNull onbeforeprint)
{
    // TODO: implement me!
}

events::EventHandlerNonNull WindowImp::getOnbeforeunload()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOnbeforeunload(events::EventHandlerNonNull onbeforeunload)
{
    // TODO: implement me!
}

events::EventHandlerNonNull WindowImp::getOnblur()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOnblur(events::EventHandlerNonNull onblur)
{
    // TODO: implement me!
}

events::EventHandlerNonNull WindowImp::getOncancel()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOncancel(events::EventHandlerNonNull oncancel)
{
    // TODO: implement me!
}

events::EventHandlerNonNull WindowImp::getOncanplay()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOncanplay(events::EventHandlerNonNull oncanplay)
{
    // TODO: implement me!
}

events::EventHandlerNonNull WindowImp::getOncanplaythrough()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOncanplaythrough(events::EventHandlerNonNull oncanplaythrough)
{
    // TODO: implement me!
}

events::EventHandlerNonNull WindowImp::getOnchange()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOnchange(events::EventHandlerNonNull onchange)
{
    // TODO: implement me!
}

events::EventHandlerNonNull WindowImp::getOnclick()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOnclick(events::EventHandlerNonNull onclick)
{
    // TODO: implement me!
}

events::EventHandlerNonNull WindowImp::getOnclose()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOnclose(events::EventHandlerNonNull onclose)
{
    // TODO: implement me!
}

events::EventHandlerNonNull WindowImp::getOncontextmenu()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOncontextmenu(events::EventHandlerNonNull oncontextmenu)
{
    // TODO: implement me!
}

events::EventHandlerNonNull WindowImp::getOncuechange()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOncuechange(events::EventHandlerNonNull oncuechange)
{
    // TODO: implement me!
}

events::EventHandlerNonNull WindowImp::getOndblclick()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOndblclick(events::EventHandlerNonNull ondblclick)
{
    // TODO: implement me!
}

events::EventHandlerNonNull WindowImp::getOndrag()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOndrag(events::EventHandlerNonNull ondrag)
{
    // TODO: implement me!
}

events::EventHandlerNonNull WindowImp::getOndragend()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOndragend(events::EventHandlerNonNull ondragend)
{
    // TODO: implement me!
}

events::EventHandlerNonNull WindowImp::getOndragenter()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOndragenter(events::EventHandlerNonNull ondragenter)
{
    // TODO: implement me!
}

events::EventHandlerNonNull WindowImp::getOndragleave()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOndragleave(events::EventHandlerNonNull ondragleave)
{
    // TODO: implement me!
}

events::EventHandlerNonNull WindowImp::getOndragover()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOndragover(events::EventHandlerNonNull ondragover)
{
    // TODO: implement me!
}

events::EventHandlerNonNull WindowImp::getOndragstart()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOndragstart(events::EventHandlerNonNull ondragstart)
{
    // TODO: implement me!
}

events::EventHandlerNonNull WindowImp::getOndrop()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOndrop(events::EventHandlerNonNull ondrop)
{
    // TODO: implement me!
}

events::EventHandlerNonNull WindowImp::getOndurationchange()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOndurationchange(events::EventHandlerNonNull ondurationchange)
{
    // TODO: implement me!
}

events::EventHandlerNonNull WindowImp::getOnemptied()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOnemptied(events::EventHandlerNonNull onemptied)
{
    // TODO: implement me!
}

events::EventHandlerNonNull WindowImp::getOnended()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOnended(events::EventHandlerNonNull onended)
{
    // TODO: implement me!
}

events::OnErrorEventHandlerNonNull WindowImp::getOnerror()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOnerror(events::OnErrorEventHandlerNonNull onerror)
{
    // TODO: implement me!
}

events::EventHandlerNonNull WindowImp::getOnfocus()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOnfocus(events::EventHandlerNonNull onfocus)
{
    // TODO: implement me!
}

events::EventHandlerNonNull WindowImp::getOnhashchange()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOnhashchange(events::EventHandlerNonNull onhashchange)
{
    // TODO: implement me!
}

events::EventHandlerNonNull WindowImp::getOninput()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOninput(events::EventHandlerNonNull oninput)
{
    // TODO: implement me!
}

events::EventHandlerNonNull WindowImp::getOninvalid()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOninvalid(events::EventHandlerNonNull oninvalid)
{
    // TODO: implement me!
}

events::EventHandlerNonNull WindowImp::getOnkeydown()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOnkeydown(events::EventHandlerNonNull onkeydown)
{
    // TODO: implement me!
}

events::EventHandlerNonNull WindowImp::getOnkeypress()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOnkeypress(events::EventHandlerNonNull onkeypress)
{
    // TODO: implement me!
}

events::EventHandlerNonNull WindowImp::getOnkeyup()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOnkeyup(events::EventHandlerNonNull onkeyup)
{
    // TODO: implement me!
}

events::EventHandlerNonNull WindowImp::getOnload()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOnload(events::EventHandlerNonNull onload)
{
    if (!window)
        return;
    window->addEventListener(u"load",
                             new(std::nothrow) EventListenerImp(boost::bind(&ECMAScriptContext::dispatchEvent, window->getContext(), onload, _1)));
}

events::EventHandlerNonNull WindowImp::getOnloadeddata()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOnloadeddata(events::EventHandlerNonNull onloadeddata)
{
    // TODO: implement me!
}

events::EventHandlerNonNull WindowImp::getOnloadedmetadata()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOnloadedmetadata(events::EventHandlerNonNull onloadedmetadata)
{
    // TODO: implement me!
}

events::EventHandlerNonNull WindowImp::getOnloadstart()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOnloadstart(events::EventHandlerNonNull onloadstart)
{
    // TODO: implement me!
}

events::EventHandlerNonNull WindowImp::getOnmessage()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOnmessage(events::EventHandlerNonNull onmessage)
{
    // TODO: implement me!
}

events::EventHandlerNonNull WindowImp::getOnmousedown()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOnmousedown(events::EventHandlerNonNull onmousedown)
{
    // TODO: implement me!
}

events::EventHandlerNonNull WindowImp::getOnmousemove()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOnmousemove(events::EventHandlerNonNull onmousemove)
{
    // TODO: implement me!
}

events::EventHandlerNonNull WindowImp::getOnmouseout()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOnmouseout(events::EventHandlerNonNull onmouseout)
{
    // TODO: implement me!
}

events::EventHandlerNonNull WindowImp::getOnmouseover()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOnmouseover(events::EventHandlerNonNull onmouseover)
{
    // TODO: implement me!
}

events::EventHandlerNonNull WindowImp::getOnmouseup()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOnmouseup(events::EventHandlerNonNull onmouseup)
{
    // TODO: implement me!
}

events::EventHandlerNonNull WindowImp::getOnmousewheel()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOnmousewheel(events::EventHandlerNonNull onmousewheel)
{
    // TODO: implement me!
}

events::EventHandlerNonNull WindowImp::getOnoffline()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOnoffline(events::EventHandlerNonNull onoffline)
{
    // TODO: implement me!
}

events::EventHandlerNonNull WindowImp::getOnonline()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOnonline(events::EventHandlerNonNull ononline)
{
    // TODO: implement me!
}

events::EventHandlerNonNull WindowImp::getOnpause()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOnpause(events::EventHandlerNonNull onpause)
{
    // TODO: implement me!
}

events::EventHandlerNonNull WindowImp::getOnplay()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOnplay(events::EventHandlerNonNull onplay)
{
    // TODO: implement me!
}

events::EventHandlerNonNull WindowImp::getOnplaying()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOnplaying(events::EventHandlerNonNull onplaying)
{
    // TODO: implement me!
}

events::EventHandlerNonNull WindowImp::getOnpagehide()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOnpagehide(events::EventHandlerNonNull onpagehide)
{
    // TODO: implement me!
}

events::EventHandlerNonNull WindowImp::getOnpageshow()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOnpageshow(events::EventHandlerNonNull onpageshow)
{
    // TODO: implement me!
}

events::EventHandlerNonNull WindowImp::getOnpopstate()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOnpopstate(events::EventHandlerNonNull onpopstate)
{
    // TODO: implement me!
}

events::EventHandlerNonNull WindowImp::getOnprogress()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOnprogress(events::EventHandlerNonNull onprogress)
{
    // TODO: implement me!
}

events::EventHandlerNonNull WindowImp::getOnratechange()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOnratechange(events::EventHandlerNonNull onratechange)
{
    // TODO: implement me!
}

events::EventHandlerNonNull WindowImp::getOnreset()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOnreset(events::EventHandlerNonNull onreset)
{
    // TODO: implement me!
}

events::EventHandlerNonNull WindowImp::getOnresize()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOnresize(events::EventHandlerNonNull onresize)
{
    // TODO: implement me!
}

events::EventHandlerNonNull WindowImp::getOnscroll()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOnscroll(events::EventHandlerNonNull onscroll)
{
    // TODO: implement me!
}

events::EventHandlerNonNull WindowImp::getOnseeked()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOnseeked(events::EventHandlerNonNull onseeked)
{
    // TODO: implement me!
}

events::EventHandlerNonNull WindowImp::getOnseeking()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOnseeking(events::EventHandlerNonNull onseeking)
{
    // TODO: implement me!
}

events::EventHandlerNonNull WindowImp::getOnselect()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOnselect(events::EventHandlerNonNull onselect)
{
    // TODO: implement me!
}

events::EventHandlerNonNull WindowImp::getOnshow()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOnshow(events::EventHandlerNonNull onshow)
{
    // TODO: implement me!
}

events::EventHandlerNonNull WindowImp::getOnstalled()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOnstalled(events::EventHandlerNonNull onstalled)
{
    // TODO: implement me!
}

events::EventHandlerNonNull WindowImp::getOnstorage()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOnstorage(events::EventHandlerNonNull onstorage)
{
    // TODO: implement me!
}

events::EventHandlerNonNull WindowImp::getOnsubmit()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOnsubmit(events::EventHandlerNonNull onsubmit)
{
    // TODO: implement me!
}

events::EventHandlerNonNull WindowImp::getOnsuspend()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOnsuspend(events::EventHandlerNonNull onsuspend)
{
    // TODO: implement me!
}

events::EventHandlerNonNull WindowImp::getOntimeupdate()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOntimeupdate(events::EventHandlerNonNull ontimeupdate)
{
    // TODO: implement me!
}

events::EventHandlerNonNull WindowImp::getOnunload()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOnunload(events::EventHandlerNonNull onunload)
{
    // TODO: implement me!
}

events::EventHandlerNonNull WindowImp::getOnvolumechange()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOnvolumechange(events::EventHandlerNonNull onvolumechange)
{
    // TODO: implement me!
}

events::EventHandlerNonNull WindowImp::getOnwaiting()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOnwaiting(events::EventHandlerNonNull onwaiting)
{
    // TODO: implement me!
}

css::CSSStyleDeclaration WindowImp::getComputedStyle(Element elt)
{
    if (!view)
        return 0;
    return view->getStyle(elt);
}

css::CSSStyleDeclaration WindowImp::getComputedStyle(Element elt, const std::u16string& pseudoElt)
{
    if (!view)
        return 0;
    return view->getStyle(elt, pseudoElt);
}

html::MediaQueryList WindowImp::matchMedia(const std::u16string& media_query_list)
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

html::Screen WindowImp::getScreen()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

int WindowImp::getInnerWidth()
{
    return width;
}

int WindowImp::getInnerHeight()
{
    return height;
}

int WindowImp::getScrollX()
{
    return window->getScrollX();
}

int WindowImp::getPageXOffset()
{
    return getScrollX();
}

int WindowImp::getScrollY()
{
    return window->getScrollY();
}

int WindowImp::getPageYOffset()
{
    return getScrollY();
}

void WindowImp::scroll(int x, int y)
{
    if (!view)
        return;

    float overflow = getScrollWidth() - width;
    x = std::max(0, std::min(x, static_cast<int>(overflow)));

    overflow = getScrollHeight() - height;
    y = std::max(0, std::min(y, static_cast<int>(overflow)));

    window->scroll(x, y);
    view->setFlags(Box::NEED_REPAINT);
}

void WindowImp::scrollTo(int x, int y)
{
    window->scroll(x, y);
}

void WindowImp::scrollBy(int x, int y)
{
    scroll(getScrollX() + x, getScrollY() + y);
}

int WindowImp::getScreenX()
{
    // TODO: implement me!
    return 0;
}

int WindowImp::getScreenY()
{
    // TODO: implement me!
    return 0;
}

int WindowImp::getOuterWidth()
{
    // TODO: implement me!
    return 0;
}

int WindowImp::getOuterHeight()
{
    // TODO: implement me!
    return 0;
}

void WindowImp::addEventListener(const std::u16string& type, events::EventListener listener, bool capture)
{
    if (window)
        window->addEventListener(type, listener, capture);
}

void WindowImp::removeEventListener(const std::u16string& type, events::EventListener listener, bool capture)
{
    if (window)
        window->removeEventListener(type, listener, capture);
}

bool WindowImp::dispatchEvent(events::Event event)
{
    if (window)
        return window->dispatchEvent(event);
    return false;
}

std::u16string WindowImp::btoa(const std::u16string& btoa)
{
    // TODO: implement me!
    return u"";
}

std::u16string WindowImp::atob(const std::u16string& atob)
{
    // TODO: implement me!
    return u"";
}

int WindowImp::setTimeout(events::EventHandlerNonNull handler)
{
    // TODO: implement me!
    return 0;
}

int WindowImp::setTimeout(events::EventHandlerNonNull handler, int timeout, Variadic<Any> arguments)
{
    // TODO: implement me!
    return 0;
}

int WindowImp::setTimeout(const std::u16string& handler)
{
    // TODO: implement me!
    return 0;
}

int WindowImp::setTimeout(const std::u16string& handler, int timeout, Variadic<Any> arguments)
{
    // TODO: implement me!
    return 0;
}

void WindowImp::clearTimeout(int handle)
{
    // TODO: implement me!
}

int WindowImp::setInterval(events::EventHandlerNonNull handler)
{
    // TODO: implement me!
    return 0;
}

int WindowImp::setInterval(events::EventHandlerNonNull handler, int timeout, Variadic<Any> arguments)
{
    // TODO: implement me!
    return 0;
}

int WindowImp::setInterval(const std::u16string& handler)
{
    // TODO: implement me!
    return 0;
}

int WindowImp::setInterval(const std::u16string& handler, int timeout, Variadic<Any> arguments)
{
    // TODO: implement me!
    return 0;
}

void WindowImp::clearInterval(int handle)
{
    // TODO: implement me!
}

std::u16string WindowImp::toNativeLineEndings(const std::u16string& string)
{
    // TODO: implement me!
    return u"";
}

}}}}  // org::w3c::dom::bootstrap
