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

#include "WindowProxy.h"

#include <new>
#include <iostream>
#include <boost/version.hpp>
#include <boost/bind.hpp>

#include "utf.h"

#include "BeforeUnloadEventImp.h"
#include "DOMImplementationImp.h"
#include "DocumentImp.h"
#include "HashChangeEventImp.h"
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

WindowProxy::Parser::Parser(DocumentImp* document, int fd, const std::string& optionalEncoding) :
    stream(fd, boost::iostreams::close_handle),
    htmlInputStream(stream, optionalEncoding),
    tokenizer(&htmlInputStream),
    parser(document, &tokenizer)
{
    document->setCharacterSet(utfconv(htmlInputStream.getEncoding()));
}

WindowProxy::WindowProxy(WindowProxy* parent, ElementImp* frameElement, unsigned short flags) :
    request(parent ? parent->getLocation().getHref() : u""),
    history(this),
    backgroundTask(this),
    thread(std::ref(backgroundTask)),
    window(0),
    view(0),
    viewFlags(0),
    flags(flags),
    parent(parent),
    frameElement(frameElement),
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

WindowProxy::~WindowProxy()
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

void WindowProxy::setSize(unsigned w, unsigned h)
{
    if (width != w || height != h) {
        width = w;
        height = h;
        setViewFlags(Box::NEED_REFLOW);
        if (window && window->evaluateMedia())
            setViewFlags(Box::NEED_STYLE_RECALCULATION);
    }
}

void WindowProxy::setViewFlags(unsigned short flags)
{
    if (view) {
        view->setFlags(flags | viewFlags);
        viewFlags = 0;
    } else
        viewFlags |= flags;
}

void WindowProxy::enter()
{
    assert(window);
    window->enter(this);
}

void WindowProxy::exit()
{
    assert(window);
    window->exit(this);
}

void WindowProxy::enableZoom(bool value)
{
    zoomable = value;
}

float WindowProxy::getZoom() const
{
    return zoom;
}

void WindowProxy::setZoom(float value)
{
    if (zoomable) {
        zoom = value;
        if (view) {
            view->setZoom(zoom);
            redisplay = true;
        }
    }
}

void WindowProxy::updateView(ViewCSSImp* next)
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
        setViewFlags(flags | viewFlags);
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

    window->flushMediaQueryLists(view);
}

void WindowProxy::setWindowPtr(const WindowPtr& window)
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

bool WindowProxy::isBindingDocumentWindow() const
{
    if (!parent)
        return false;
    DocumentImp* document = dynamic_cast<DocumentImp*>(parent->getDocument().self());
    return document->isBindingDocumentWindow(this);
}

bool WindowProxy::poll()
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
            if (flags & Loading) {
                flags &= ~Loading;
                document->decrementLoadEventDelayCount();
            }
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
        WindowProxy* child = *i;
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

    switch (request.getReadyState()) {
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
            Document newDocument = getDOMImplementation()->createDocument(u"", u"", 0); // TODO: Create HTML document
            if ((document = dynamic_cast<DocumentImp*>(newDocument.self()))) {
                // TODO: Fire a simple unload event.
                document->setDefaultView(this);
                document->setURL(request.getRequestMessage().getURL());
                document->setLastModified(request.getLastModified());
                window->setDocument(newDocument);
                if (!request.getError())
                    history.update(window);
                else
                    document->setError(request.getError());
                document->enter();
                parser.reset(new(std::nothrow) Parser(document, request.getContentDescriptor(), request.getResponseMessage().getContentCharset()));
                document->exit();
                if (!parser)
                    break;  // TODO: error handling
            } else
                break;  // TODO: error handling
        }
        if (document->getReadyState() == u"loading") {
            // TODO: Note white it would be nice to parse the HTML docucment in
            // the background task, firstly we need to check if we can run JS
            // in the background.

            document->enter();

            if (!parser->processPendingParsingBlockingScript()) {
                document->exit();
                break;
            }
            // TODO: run this in the background
            Token token;
            do {
                token = parser->getToken();
                parser->processToken(token);
            } while (token.getType() != Token::Type::EndOfFile && !document->getPendingParsingBlockingScript());

            if (document->getPendingParsingBlockingScript()) {
                document->exit();
                break;
            }

            // TODO: Check if the parser has been aborted.
            document->resetStyleSheets();
            setViewFlags(Box::NEED_SELECTOR_REMATCHING);
            if (!(flags & Loading) && !isBindingDocumentWindow()) { // Note a binding document does not create its view.
                flags |= Loading;
                document->incrementLoadEventDelayCount();
            }

            parser.reset();
            document->exit();

            recordTime("%*shtml parsed", windowDepth * 2, "");
            if (4 <= getLogLevel())
                dumpTree(std::cerr, document);
        }

        if (!backgroundTask.isRestarting()) {
            switch (backgroundTask.getState()) {
            case BackgroundTask::Cascaded:
                HTMLElementImp::xblEnteredDocument(document);
                backgroundTask.wakeUp(BackgroundTask::Layout);
                break;
            case BackgroundTask::Init:
            case BackgroundTask::Done: {

                eventLoop();

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
                        assert(!(flags & (Box::NEED_EXPANSION | Box::NEED_CHILD_EXPANSION)) || view->getTree()->getFlags());
                        assert(!(flags & (Box::NEED_REFLOW | Box::NEED_CHILD_REFLOW)) || view->getTree()->getFlags());
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
                        } else if (flags & Box::NEED_REPAINT) {
                            redisplay = true;
                            if (flags & Loading) {
                                flags &= ~Loading;
                                document->decrementLoadEventDelayCount();
                            }
                        }
                    }
                }
                break;
            }
            default:
                break;
            }
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

void WindowProxy::render(ViewCSSImp* parentView)
{
    if (view) {
        recordTime("%*srepaint begin: %s (%s)", windowDepth * 2, "", utfconv(window->getDocument().getReadyState()).c_str(), view ? "render" : "canvas");
        if (view->gatherFlags() & Box::NEED_REPAINT) {
            view->clearFlags(Box::NEED_REPAINT);
            // TODO: if the size of the canvas has not been changed, reuse the same canvas.
            canvas.shutdown();
            canvas.setup(width, height);

            unsigned backgroundColor = view->getBackgroundColor();
            if (backgroundColor == 0 && !parent)
                backgroundColor = 0xffffffff;
            canvas.beginRender(backgroundColor);

            view->render(parentView ? parentView->getClipCount() : 0);
            scrollWidth = view->getScrollWidth();
            scrollHeight = view->getScrollHeight();
            canvas.endRender();
        }
        if (2 <= getLogLevel() && backgroundTask.isIdle() && !view->gatherFlags()) {
            unsigned depth = 1;
            for (WindowProxy* w = this; w->parent; w = w->parent)
                ++depth;
            std::cout << "\n## " << window->getDocument().getReadyState();
            if (1 < depth)
                std::cout << " (" << depth << ')';
            std::cout << '\n';
            view->dump();
            std::cout << "##\n";
            std::cout.flush();
        }
        recordTime("%*srepaint end", windowDepth * 2, "");
    }
    canvas.render(width, height);
}

void WindowProxy::mouse(int button, int up, int x, int y, int modifiers)
{
    eventQueue.emplace_back(up ? EventTask::MouseUp : EventTask::MouseDown, modifiers, x, y, button);
}

void WindowProxy::mouseMove(int x, int y, int modifiers)
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

void WindowProxy::keydown(unsigned charCode, unsigned keyCode, int modifiers)
{
    eventQueue.emplace_back(EventTask::KeyDown, modifiers, charCode, keyCode);
}

void WindowProxy::keyup(unsigned charCode, unsigned keyCode, int modifiers)
{
    eventQueue.emplace_back(EventTask::KeyUp, modifiers, charCode, keyCode);
}

void WindowProxy::mouse(const EventTask& task)
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
    if (WindowProxy* childWindow = box->getChildWindow()) {
        childWindow->mouse(button, up,
                           x - box->getX() - box->getBlankLeft(), y - box->getY() - box->getBlankTop(),
                           modifiers);
    }

    Element target = Box::getContainingElement(box->getTargetNode());

    // mousedown, mousemove
    if ((imp = new(std::nothrow) MouseEventImp)) {
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
    if ((imp = new(std::nothrow) MouseEventImp)) {
        event = imp;
        imp->initMouseEvent(u"click",
                            true, true, this, detail, x, y, x, y,
                            modifiers & 2, modifiers & 4, modifiers & 1, false, button, 0);
        imp->setButtons(buttons);
        target.dispatchEvent(event);
    }
}

void WindowProxy::mouseMove(const EventTask& task)
{
    int x = task.x;
    int y = task.y;
    int modifiers = task.modifiers;

    if (!view)
        return;
    Box* box = view->boxFromPoint(x, y);
    if (!box)
        return;

    Element target = Box::getContainingElement(box->getTargetNode());
    Element prev = view->setHovered(target);

    if (prev != target) {
        if (html::Window c = interface_cast<html::HTMLIFrameElement>(prev).getContentWindow()) {
            if (auto w = dynamic_cast<WindowProxy*>(c.self()))
                w->mouseMove(-1, -1, modifiers);
        }
    }
    if (WindowProxy* childWindow = box->getChildWindow()) {
        childWindow->mouseMove(x - box->getX() - box->getBlankLeft(),
                               y - box->getY() - box->getBlankTop(),
                               modifiers);
    }
    if (prev != target) {
        // mouseout
        if (MouseEventImp* imp = new(std::nothrow) MouseEventImp) {
            imp->initMouseEvent(u"mouseout",
                                true, true, this, 0, x, y, x, y,
                                modifiers & 2, modifiers & 4, modifiers & 1, false, 0, 0);
            imp->setButtons(buttons);
            prev.dispatchEvent(imp);
        }

        // mouseover
        if (MouseEventImp* imp = new(std::nothrow) MouseEventImp) {
            imp->initMouseEvent(u"mouseover",
                                true, true, this, 0, x, y, x, y,
                                modifiers & 2, modifiers & 4, modifiers & 1, false, 0, 0);
            imp->setButtons(buttons);
            target.dispatchEvent(imp);
        }
    }

    // mousemove
    if (MouseEventImp* imp = new(std::nothrow) MouseEventImp) {
        if (target != clickTarget)
            detail = 0;
        imp->initMouseEvent(u"mousemove",
                            true, true, this, detail, x, y, x, y,
                            modifiers & 2, modifiers & 4, modifiers & 1, false, 0, 0);
        imp->setButtons(buttons);
        target.dispatchEvent(imp);
    }
}

void WindowProxy::keydown(const EventTask& task)
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
        if (auto child = dynamic_cast<WindowProxy*>(iframe->getContentWindow().self()))
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

void WindowProxy::keyup(const EventTask& task)
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
        if (auto child = dynamic_cast<WindowProxy*>(iframe->getContentWindow().self()))
            child->keyup(charCode, keyCode, modifiers);
    }

    KeyboardEventImp* imp = new(std::nothrow) KeyboardEventImp(modifiers, charCode, keyCode, 0);
    events::KeyboardEvent event(imp);
    imp->initKeyboardEvent(u"keyup", true, true, this,
                           u"", u"", 0, u"", false, u"");
    e.dispatchEvent(event);
}

void WindowProxy::setFavicon(IcoImage* ico, std::FILE* file)
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

void WindowProxy::setFavicon(BoxImage* image)
{
    if (parent) {
        if (parent->getFaviconOverridable())
            parent->setFavicon(image);
        return;
    }
    uint32_t* pixels = reinterpret_cast<uint32_t*>(image->getPixels());
    setIcon(0, image->getNaturalWidth(), image->getNaturalHeight(), pixels);
}

void WindowProxy::setFavicon()
{
    for (WindowProxy* w = this; w; w = w->parent) {
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
            if (auto link = dynamic_cast<HTMLLinkElementImp*>(i.self())) {
                if (link->setFavicon(document))
                    return;
            }
        }
    }
}

//
// CSSOM View support operations
//

Element WindowProxy::elementFromPoint(float x, float y)
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

html::Window WindowProxy::getWindow()
{
    return this;
}

Any WindowProxy::getSelf()
{
    return this;
}

void WindowProxy::setSelf(Any self)
{
    // TODO: implement me!
}

Document WindowProxy::getDocument()
{
    return !window ? 0 : window->getDocument();
}

std::u16string WindowProxy::getName()
{
    return name;
}

void WindowProxy::setName(const std::u16string& name)
{
    if (!name.empty() && name[0] == u'_')
        return;
    this->name = name;
}

html::Location WindowProxy::getLocation()
{
    return getDocument().getLocation();
}

void WindowProxy::setLocation(const std::u16string& location)
{
    getLocation().setHref(location);
}

html::History WindowProxy::getHistory()
{
    return &history;
}

Any WindowProxy::getLocationbar()
{
    // TODO: implement me!
    return 0;
}

void WindowProxy::setLocationbar(Any locationbar)
{
    // TODO: implement me!
}

Any WindowProxy::getMenubar()
{
    // TODO: implement me!
    return 0;
}

void WindowProxy::setMenubar(Any menubar)
{
    // TODO: implement me!
}

Any WindowProxy::getPersonalbar()
{
    // TODO: implement me!
    return 0;
}

void WindowProxy::setPersonalbar(Any personalbar)
{
    // TODO: implement me!
}

Any WindowProxy::getScrollbars()
{
    // TODO: implement me!
    return 0;
}

void WindowProxy::setScrollbars(Any scrollbars)
{
    // TODO: implement me!
}

Any WindowProxy::getStatusbar()
{
    // TODO: implement me!
    return 0;
}

void WindowProxy::setStatusbar(Any statusbar)
{
    // TODO: implement me!
}

Any WindowProxy::getToolbar()
{
    // TODO: implement me!
    return 0;
}

void WindowProxy::setToolbar(Any toolbar)
{
    // TODO: implement me!
}

std::u16string WindowProxy::getStatus()
{
    // TODO: implement me!
    return u"";
}

void WindowProxy::setStatus(const std::u16string& status)
{
    // TODO: implement me!
}

void WindowProxy::close()
{
    // TODO: implement me!
}

void WindowProxy::stop()
{
    // TODO: implement me!
}

void WindowProxy::focus()
{
    // TODO: implement me!
}

void WindowProxy::blur()
{
    // TODO: implement me!
}

Any WindowProxy::getFrames()
{
    return this;
}

void WindowProxy::setFrames(Any frames)
{
    // TODO: implement me!
}

Any WindowProxy::getLength()
{
    return childWindows.size();
}

void WindowProxy::setLength(Any length)
{
    // TODO: implement me!
}

html::Window WindowProxy::getTop()
{
    WindowProxy* top = this;
    while (!top->isTopLevel())
        top = top->parent;
    return top;
}

html::Window WindowProxy::getOpener()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowProxy::setOpener(html::Window opener)
{
    // TODO: implement me!
}

html::Window WindowProxy::getParent()
{
    if (isTopLevel())
        return this;
    return parent;
}

Element WindowProxy::getFrameElement()
{
    return frameElement;
}

void WindowProxy::navigateToFragmentIdentifier(URL target)
{
    DocumentImp* document = dynamic_cast<DocumentImp*>(window->getDocument().self());
    if (!document)
        return;

    std::u16string oldURL = document->getURL();

    // cf. http://www.w3.org/TR/html5/browsers.html#scroll-to-fragid
    history.update(target, window);

    // TODO: Remove any tasks queued by the history traversal task source that
    //       are associated with any Document objects in the top-level
    //       browsing context's document family.

    std::u16string hash = target.getHash();
    if (hash[0] == '#')
        hash.erase(0, 1);
    if (Element element = document->getElementById(hash))
        element.scrollIntoView(true);

    html::HashChangeEvent event(new(std::nothrow) HashChangeEventImp(u"hashchange", oldURL, static_cast<std::u16string>(target)));    // TODO: set oldURL
    window->dispatchEvent(event);
}

// cf. http://www.whatwg.org/specs/web-apps/current-work/multipage/browsers.html#the-rules-for-choosing-a-browsing-context-given-a-browsing-context-name
WindowProxy* WindowProxy::selectBrowsingContext(std::u16string target, bool& replace)
{
    if (target.empty())
        return this;

    WindowProxy* top = this;
    while (!top->isTopLevel())
        top = top->parent;

    if (target[0] == u'_') {
        toLower(target);
        if (target == u"_self")
            return this;
        if (target == u"_parent") {
            if (parent)
                return parent;
            return this;
        }
        if (target == u"_top")
            return top;
        if (target != u"_blank")
            return 0;
    } else {
        // TODO: implement me!
        return 0;
    }

    if (!top->parent)
        return this;
    DocumentImp* ownerDocument = dynamic_cast<DocumentImp*>(top->parent->getDocument().self());
    if (!ownerDocument)
        return this;

    HTMLIFrameElementImp* iframe = new(std::nothrow) HTMLIFrameElementImp(ownerDocument, TopLevel);
    if (!iframe)
        return 0;
    WindowProxy* context = dynamic_cast<WindowProxy*>(iframe->getContentWindow().self());
    if (!context) {
        // TODO: release iframe
        return 0;
    }
    replace = true;
    if (target != u"_blank")
        context->name = target;

    top->parent->enter();
    if (Node next = frameElement->getNextSibling())
        frameElement->getParentNode().insertBefore(iframe, next);
    else
        frameElement->getParentNode().appendChild(iframe);
    top->parent->exit();

    return context;
}

void WindowProxy::navigate(std::u16string url, bool replace, WindowProxy* srcWindow)
{
    backgroundTask.restart();

    // TODO: add more details
    if (srcWindow->window) {
        if (DocumentImp* document = dynamic_cast<DocumentImp*>(srcWindow->window->getDocument().self())) {
            URL base(document->getDocumentURI());
            URL resolved(base, url);
            if (this == srcWindow) {
                if (base.isSameExceptFragments(resolved)) {
                    Task task(this, boost::bind(&WindowProxy::navigateToFragmentIdentifier, this, resolved));
                    putTask(task);
                    return;
                }
                // Prompt to unload the Document object.
                if (html::BeforeUnloadEvent event = new(std::nothrow) BeforeUnloadEventImp) {
                    window->dispatchEvent(event);
                    if (!event.getReturnValue().empty() || event.getDefaultPrevented())
                        return;
                    if (parent && parent->getFaviconOverridable())
                        parent->setFavicon();
                }
            }
            url = resolved;
        }
    }

    window = new(std::nothrow) WindowImp;
    request.abort();
    history.setReplace(replace);
    request.open(u"get", url.empty() ? u"about:blank" : url);
    request.send();
}

html::Window WindowProxy::open(const std::u16string& url, const std::u16string& target, const std::u16string& features, bool replace)
{
    WindowProxy* targetWindow = selectBrowsingContext(target, replace);
    if (!targetWindow)
        return 0;   // TODO: throw an InvalidAccessError exception
    targetWindow->navigate(url, replace, this);
    return targetWindow;
}

html::Window WindowProxy::getElement(unsigned int index)
{
    if (index < childWindows.size())
        return childWindows[index];
    return 0;
}

Object WindowProxy::getElement(const std::u16string& name)
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

html::Navigator WindowProxy::getNavigator()
{
    return &navigator;
}

html::External WindowProxy::getExternal()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

html::ApplicationCache WindowProxy::getApplicationCache()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowProxy::alert(const std::u16string& message)
{
    std::cerr << message << '\n';
}

bool WindowProxy::confirm(const std::u16string& message)
{
    // TODO: implement me!
    return 0;
}

Nullable<std::u16string> WindowProxy::prompt(const std::u16string& message)
{
    // TODO: implement me!
    return u"";
}

Nullable<std::u16string> WindowProxy::prompt(const std::u16string& message, const std::u16string& _default)
{
    // TODO: implement me!
    return u"";
}

void WindowProxy::print()
{
    // TODO: implement me!
}

Any WindowProxy::showModalDialog(const std::u16string& url)
{
    // TODO: implement me!
    return 0;
}

Any WindowProxy::showModalDialog(const std::u16string& url, Any argument)
{
    // TODO: implement me!
    return 0;
}

events::EventHandlerNonNull WindowProxy::getOnabort()
{
    return interface_cast<events::EventHandlerNonNull>(window->getEventHandler(u"abort"));
}

void WindowProxy::setOnabort(events::EventHandlerNonNull onabort)
{
    window->setEventHandler(u"abort", onabort);
}

events::EventHandlerNonNull WindowProxy::getOnafterprint()
{
    return interface_cast<events::EventHandlerNonNull>(window->getEventHandler(u"afterprint"));
}

void WindowProxy::setOnafterprint(events::EventHandlerNonNull onafterprint)
{
    window->setEventHandler(u"afterprint", onafterprint);
}

events::EventHandlerNonNull WindowProxy::getOnbeforeprint()
{
    return interface_cast<events::EventHandlerNonNull>(window->getEventHandler(u"beforeprint"));
}

void WindowProxy::setOnbeforeprint(events::EventHandlerNonNull onbeforeprint)
{
    window->setEventHandler(u"beforeprint", onbeforeprint);
}

events::EventHandlerNonNull WindowProxy::getOnbeforeunload()
{
    return interface_cast<events::EventHandlerNonNull>(window->getEventHandler(u"beforeunload"));
}

void WindowProxy::setOnbeforeunload(events::EventHandlerNonNull onbeforeunload)
{
    window->setEventHandler(u"beforeunload", onbeforeunload);
}

events::EventHandlerNonNull WindowProxy::getOnblur()
{
    return interface_cast<events::EventHandlerNonNull>(window->getEventHandler(u"blur"));
}

void WindowProxy::setOnblur(events::EventHandlerNonNull onblur)
{
    window->setEventHandler(u"blur", onblur);
}

events::EventHandlerNonNull WindowProxy::getOncancel()
{
    return interface_cast<events::EventHandlerNonNull>(window->getEventHandler(u"cancel"));
}

void WindowProxy::setOncancel(events::EventHandlerNonNull oncancel)
{
    window->setEventHandler(u"cancel", oncancel);
}

events::EventHandlerNonNull WindowProxy::getOncanplay()
{
    return interface_cast<events::EventHandlerNonNull>(window->getEventHandler(u"canplay"));
}

void WindowProxy::setOncanplay(events::EventHandlerNonNull oncanplay)
{
    window->setEventHandler(u"canplay", oncanplay);
}

events::EventHandlerNonNull WindowProxy::getOncanplaythrough()
{
    return interface_cast<events::EventHandlerNonNull>(window->getEventHandler(u"canplaythrough"));
}

void WindowProxy::setOncanplaythrough(events::EventHandlerNonNull oncanplaythrough)
{
    window->setEventHandler(u"canplaythrough", oncanplaythrough);
}

events::EventHandlerNonNull WindowProxy::getOnchange()
{
    return interface_cast<events::EventHandlerNonNull>(window->getEventHandler(u"change"));
}

void WindowProxy::setOnchange(events::EventHandlerNonNull onchange)
{
    window->setEventHandler(u"change", onchange);
}

events::EventHandlerNonNull WindowProxy::getOnclick()
{
    return interface_cast<events::EventHandlerNonNull>(window->getEventHandler(u"click"));
}

void WindowProxy::setOnclick(events::EventHandlerNonNull onclick)
{
    window->setEventHandler(u"click", onclick);
}

events::EventHandlerNonNull WindowProxy::getOnclose()
{
    return interface_cast<events::EventHandlerNonNull>(window->getEventHandler(u"close"));
}

void WindowProxy::setOnclose(events::EventHandlerNonNull onclose)
{
    window->setEventHandler(u"close", onclose);
}

events::EventHandlerNonNull WindowProxy::getOncontextmenu()
{
    return interface_cast<events::EventHandlerNonNull>(window->getEventHandler(u"contextmenu"));
}

void WindowProxy::setOncontextmenu(events::EventHandlerNonNull oncontextmenu)
{
    window->setEventHandler(u"contextmenu", oncontextmenu);
}

events::EventHandlerNonNull WindowProxy::getOncuechange()
{
    return interface_cast<events::EventHandlerNonNull>(window->getEventHandler(u"cuechange"));
}

void WindowProxy::setOncuechange(events::EventHandlerNonNull oncuechange)
{
    window->setEventHandler(u"cuechange", oncuechange);
}

events::EventHandlerNonNull WindowProxy::getOndblclick()
{
    return interface_cast<events::EventHandlerNonNull>(window->getEventHandler(u"dblclick"));
}

void WindowProxy::setOndblclick(events::EventHandlerNonNull ondblclick)
{
    window->setEventHandler(u"dblclick", ondblclick);
}

events::EventHandlerNonNull WindowProxy::getOndrag()
{
    return interface_cast<events::EventHandlerNonNull>(window->getEventHandler(u"drag"));
}

void WindowProxy::setOndrag(events::EventHandlerNonNull ondrag)
{
    window->setEventHandler(u"drag", ondrag);
}

events::EventHandlerNonNull WindowProxy::getOndragend()
{
    return interface_cast<events::EventHandlerNonNull>(window->getEventHandler(u"dragend"));
}

void WindowProxy::setOndragend(events::EventHandlerNonNull ondragend)
{
    window->setEventHandler(u"dragend", ondragend);
}

events::EventHandlerNonNull WindowProxy::getOndragenter()
{
    return interface_cast<events::EventHandlerNonNull>(window->getEventHandler(u"dragenter"));
}

void WindowProxy::setOndragenter(events::EventHandlerNonNull ondragenter)
{
    window->setEventHandler(u"dragenter", ondragenter);
}

events::EventHandlerNonNull WindowProxy::getOndragleave()
{
    return interface_cast<events::EventHandlerNonNull>(window->getEventHandler(u"dragleave"));
}

void WindowProxy::setOndragleave(events::EventHandlerNonNull ondragleave)
{
    window->setEventHandler(u"dragleave", ondragleave);
}

events::EventHandlerNonNull WindowProxy::getOndragover()
{
    return interface_cast<events::EventHandlerNonNull>(window->getEventHandler(u"dragover"));
}

void WindowProxy::setOndragover(events::EventHandlerNonNull ondragover)
{
    window->setEventHandler(u"dragover", ondragover);
}

events::EventHandlerNonNull WindowProxy::getOndragstart()
{
    return interface_cast<events::EventHandlerNonNull>(window->getEventHandler(u"dragstart"));
}

void WindowProxy::setOndragstart(events::EventHandlerNonNull ondragstart)
{
    window->setEventHandler(u"dragstart", ondragstart);
}

events::EventHandlerNonNull WindowProxy::getOndrop()
{
    return interface_cast<events::EventHandlerNonNull>(window->getEventHandler(u"drop"));
}

void WindowProxy::setOndrop(events::EventHandlerNonNull ondrop)
{
    window->setEventHandler(u"drop", ondrop);
}

events::EventHandlerNonNull WindowProxy::getOndurationchange()
{
    return interface_cast<events::EventHandlerNonNull>(window->getEventHandler(u"durationchange"));
}

void WindowProxy::setOndurationchange(events::EventHandlerNonNull ondurationchange)
{
    window->setEventHandler(u"durationchange", ondurationchange);
}

events::EventHandlerNonNull WindowProxy::getOnemptied()
{
    return interface_cast<events::EventHandlerNonNull>(window->getEventHandler(u"emptied"));
}

void WindowProxy::setOnemptied(events::EventHandlerNonNull onemptied)
{
    window->setEventHandler(u"emptied", onemptied);
}

events::EventHandlerNonNull WindowProxy::getOnended()
{
    return interface_cast<events::EventHandlerNonNull>(window->getEventHandler(u"ended"));
}

void WindowProxy::setOnended(events::EventHandlerNonNull onended)
{
    window->setEventHandler(u"ended", onended);
}

events::OnErrorEventHandlerNonNull WindowProxy::getOnerror()
{
    return interface_cast<events::OnErrorEventHandlerNonNull>(window->getEventHandler(u"error"));
}

void WindowProxy::setOnerror(events::OnErrorEventHandlerNonNull onerror)
{
    window->setEventHandler(u"error", onerror);
}

events::EventHandlerNonNull WindowProxy::getOnfocus()
{
    return interface_cast<events::EventHandlerNonNull>(window->getEventHandler(u"focus"));
}

void WindowProxy::setOnfocus(events::EventHandlerNonNull onfocus)
{
    window->setEventHandler(u"focus", onfocus);
}

events::EventHandlerNonNull WindowProxy::getOnhashchange()
{
    return interface_cast<events::EventHandlerNonNull>(window->getEventHandler(u"hashchange"));
}

void WindowProxy::setOnhashchange(events::EventHandlerNonNull onhashchange)
{
    window->setEventHandler(u"hashchange", onhashchange);
}

events::EventHandlerNonNull WindowProxy::getOninput()
{
    return interface_cast<events::EventHandlerNonNull>(window->getEventHandler(u"input"));
}

void WindowProxy::setOninput(events::EventHandlerNonNull oninput)
{
    window->setEventHandler(u"input", oninput);
}

events::EventHandlerNonNull WindowProxy::getOninvalid()
{
    return interface_cast<events::EventHandlerNonNull>(window->getEventHandler(u"invalid"));
}

void WindowProxy::setOninvalid(events::EventHandlerNonNull oninvalid)
{
    window->setEventHandler(u"invalid", oninvalid);
}

events::EventHandlerNonNull WindowProxy::getOnkeydown()
{
    return interface_cast<events::EventHandlerNonNull>(window->getEventHandler(u"keydown"));
}

void WindowProxy::setOnkeydown(events::EventHandlerNonNull onkeydown)
{
    window->setEventHandler(u"keydown", onkeydown);
}

events::EventHandlerNonNull WindowProxy::getOnkeypress()
{
    return interface_cast<events::EventHandlerNonNull>(window->getEventHandler(u"keypress"));
}

void WindowProxy::setOnkeypress(events::EventHandlerNonNull onkeypress)
{
    window->setEventHandler(u"keypress", onkeypress);
}

events::EventHandlerNonNull WindowProxy::getOnkeyup()
{
    return interface_cast<events::EventHandlerNonNull>(window->getEventHandler(u"keyup"));
}

void WindowProxy::setOnkeyup(events::EventHandlerNonNull onkeyup)
{
    window->setEventHandler(u"keyup", onkeyup);
}

events::EventHandlerNonNull WindowProxy::getOnload()
{
    return interface_cast<events::EventHandlerNonNull>(window->getEventHandler(u"load"));
}

void WindowProxy::setOnload(events::EventHandlerNonNull onload)
{
    window->setEventHandler(u"load", onload);
}

events::EventHandlerNonNull WindowProxy::getOnloadeddata()
{
    return interface_cast<events::EventHandlerNonNull>(window->getEventHandler(u"loadeddata"));
}

void WindowProxy::setOnloadeddata(events::EventHandlerNonNull onloadeddata)
{
    window->setEventHandler(u"loadeddata", onloadeddata);
}

events::EventHandlerNonNull WindowProxy::getOnloadedmetadata()
{
    return interface_cast<events::EventHandlerNonNull>(window->getEventHandler(u"loadedmetadata"));
}

void WindowProxy::setOnloadedmetadata(events::EventHandlerNonNull onloadedmetadata)
{
    window->setEventHandler(u"loadedmetadata", onloadedmetadata);
}

events::EventHandlerNonNull WindowProxy::getOnloadstart()
{
    return interface_cast<events::EventHandlerNonNull>(window->getEventHandler(u"loadstart"));
}

void WindowProxy::setOnloadstart(events::EventHandlerNonNull onloadstart)
{
    window->setEventHandler(u"loadstart", onloadstart);
}

events::EventHandlerNonNull WindowProxy::getOnmessage()
{
    return interface_cast<events::EventHandlerNonNull>(window->getEventHandler(u"message"));
}

void WindowProxy::setOnmessage(events::EventHandlerNonNull onmessage)
{
    window->setEventHandler(u"message", onmessage);
}

events::EventHandlerNonNull WindowProxy::getOnmousedown()
{
    return interface_cast<events::EventHandlerNonNull>(window->getEventHandler(u"mousedown"));
}

void WindowProxy::setOnmousedown(events::EventHandlerNonNull onmousedown)
{
    window->setEventHandler(u"mousedown", onmousedown);
}

events::EventHandlerNonNull WindowProxy::getOnmousemove()
{
    return interface_cast<events::EventHandlerNonNull>(window->getEventHandler(u"mousemove"));
}

void WindowProxy::setOnmousemove(events::EventHandlerNonNull onmousemove)
{
    window->setEventHandler(u"mousemove", onmousemove);
}

events::EventHandlerNonNull WindowProxy::getOnmouseout()
{
    return interface_cast<events::EventHandlerNonNull>(window->getEventHandler(u"mouseout"));
}

void WindowProxy::setOnmouseout(events::EventHandlerNonNull onmouseout)
{
    window->setEventHandler(u"mouseout", onmouseout);
}

events::EventHandlerNonNull WindowProxy::getOnmouseover()
{
    return interface_cast<events::EventHandlerNonNull>(window->getEventHandler(u"mouseover"));
}

void WindowProxy::setOnmouseover(events::EventHandlerNonNull onmouseover)
{
    window->setEventHandler(u"mouseover", onmouseover);
}

events::EventHandlerNonNull WindowProxy::getOnmouseup()
{
    return interface_cast<events::EventHandlerNonNull>(window->getEventHandler(u"mouseup"));
}

void WindowProxy::setOnmouseup(events::EventHandlerNonNull onmouseup)
{
    window->setEventHandler(u"mouseup", onmouseup);
}

events::EventHandlerNonNull WindowProxy::getOnmousewheel()
{
    return interface_cast<events::EventHandlerNonNull>(window->getEventHandler(u"mousewheel"));
}

void WindowProxy::setOnmousewheel(events::EventHandlerNonNull onmousewheel)
{
    window->setEventHandler(u"mousewheel", onmousewheel);
}

events::EventHandlerNonNull WindowProxy::getOnoffline()
{
    return interface_cast<events::EventHandlerNonNull>(window->getEventHandler(u"offline"));
}

void WindowProxy::setOnoffline(events::EventHandlerNonNull onoffline)
{
    window->setEventHandler(u"offline", onoffline);
}

events::EventHandlerNonNull WindowProxy::getOnonline()
{
    return interface_cast<events::EventHandlerNonNull>(window->getEventHandler(u"online"));
}

void WindowProxy::setOnonline(events::EventHandlerNonNull ononline)
{
    window->setEventHandler(u"online", ononline);
}

events::EventHandlerNonNull WindowProxy::getOnpause()
{
    return interface_cast<events::EventHandlerNonNull>(window->getEventHandler(u"pause"));
}

void WindowProxy::setOnpause(events::EventHandlerNonNull onpause)
{
    window->setEventHandler(u"pause", onpause);
}

events::EventHandlerNonNull WindowProxy::getOnplay()
{
    return interface_cast<events::EventHandlerNonNull>(window->getEventHandler(u"play"));
}

void WindowProxy::setOnplay(events::EventHandlerNonNull onplay)
{
    window->setEventHandler(u"play", onplay);
}

events::EventHandlerNonNull WindowProxy::getOnplaying()
{
    return interface_cast<events::EventHandlerNonNull>(window->getEventHandler(u"playing"));
}

void WindowProxy::setOnplaying(events::EventHandlerNonNull onplaying)
{
    window->setEventHandler(u"playing", onplaying);
}

events::EventHandlerNonNull WindowProxy::getOnpagehide()
{
    return interface_cast<events::EventHandlerNonNull>(window->getEventHandler(u"pagehide"));
}

void WindowProxy::setOnpagehide(events::EventHandlerNonNull onpagehide)
{
    window->setEventHandler(u"pagehide", onpagehide);
}

events::EventHandlerNonNull WindowProxy::getOnpageshow()
{
    return interface_cast<events::EventHandlerNonNull>(window->getEventHandler(u"pageshow"));
}

void WindowProxy::setOnpageshow(events::EventHandlerNonNull onpageshow)
{
    window->setEventHandler(u"pageshow", onpageshow);
}

events::EventHandlerNonNull WindowProxy::getOnpopstate()
{
    return interface_cast<events::EventHandlerNonNull>(window->getEventHandler(u"popstate"));
}

void WindowProxy::setOnpopstate(events::EventHandlerNonNull onpopstate)
{
    window->setEventHandler(u"popstate", onpopstate);
}

events::EventHandlerNonNull WindowProxy::getOnprogress()
{
    return interface_cast<events::EventHandlerNonNull>(window->getEventHandler(u"progress"));
}

void WindowProxy::setOnprogress(events::EventHandlerNonNull onprogress)
{
    window->setEventHandler(u"progress", onprogress);
}

events::EventHandlerNonNull WindowProxy::getOnratechange()
{
    return interface_cast<events::EventHandlerNonNull>(window->getEventHandler(u"ratechange"));
}

void WindowProxy::setOnratechange(events::EventHandlerNonNull onratechange)
{
    window->setEventHandler(u"ratechange", onratechange);
}

events::EventHandlerNonNull WindowProxy::getOnreset()
{
    return interface_cast<events::EventHandlerNonNull>(window->getEventHandler(u"reset"));
}

void WindowProxy::setOnreset(events::EventHandlerNonNull onreset)
{
    window->setEventHandler(u"reset", onreset);
}

events::EventHandlerNonNull WindowProxy::getOnresize()
{
    return interface_cast<events::EventHandlerNonNull>(window->getEventHandler(u"resize"));
}

void WindowProxy::setOnresize(events::EventHandlerNonNull onresize)
{
    window->setEventHandler(u"resize", onresize);
}

events::EventHandlerNonNull WindowProxy::getOnscroll()
{
    return interface_cast<events::EventHandlerNonNull>(window->getEventHandler(u"scroll"));
}

void WindowProxy::setOnscroll(events::EventHandlerNonNull onscroll)
{
    window->setEventHandler(u"scroll", onscroll);
}

events::EventHandlerNonNull WindowProxy::getOnseeked()
{
    return interface_cast<events::EventHandlerNonNull>(window->getEventHandler(u"seeked"));
}

void WindowProxy::setOnseeked(events::EventHandlerNonNull onseeked)
{
    window->setEventHandler(u"seeked", onseeked);
}

events::EventHandlerNonNull WindowProxy::getOnseeking()
{
    return interface_cast<events::EventHandlerNonNull>(window->getEventHandler(u"seeking"));
}

void WindowProxy::setOnseeking(events::EventHandlerNonNull onseeking)
{
    window->setEventHandler(u"seeking", onseeking);
}

events::EventHandlerNonNull WindowProxy::getOnselect()
{
    return interface_cast<events::EventHandlerNonNull>(window->getEventHandler(u"select"));
}

void WindowProxy::setOnselect(events::EventHandlerNonNull onselect)
{
    window->setEventHandler(u"select", onselect);
}

events::EventHandlerNonNull WindowProxy::getOnshow()
{
    return interface_cast<events::EventHandlerNonNull>(window->getEventHandler(u"show"));
}

void WindowProxy::setOnshow(events::EventHandlerNonNull onshow)
{
    window->setEventHandler(u"show", onshow);
}

events::EventHandlerNonNull WindowProxy::getOnstalled()
{
    return interface_cast<events::EventHandlerNonNull>(window->getEventHandler(u"stalled"));
}

void WindowProxy::setOnstalled(events::EventHandlerNonNull onstalled)
{
    window->setEventHandler(u"stalled", onstalled);
}

events::EventHandlerNonNull WindowProxy::getOnstorage()
{
    return interface_cast<events::EventHandlerNonNull>(window->getEventHandler(u"storage"));
}

void WindowProxy::setOnstorage(events::EventHandlerNonNull onstorage)
{
    window->setEventHandler(u"storage", onstorage);
}

events::EventHandlerNonNull WindowProxy::getOnsubmit()
{
    return interface_cast<events::EventHandlerNonNull>(window->getEventHandler(u"submit"));
}

void WindowProxy::setOnsubmit(events::EventHandlerNonNull onsubmit)
{
    window->setEventHandler(u"submit", onsubmit);
}

events::EventHandlerNonNull WindowProxy::getOnsuspend()
{
    return interface_cast<events::EventHandlerNonNull>(window->getEventHandler(u"suspend"));
}

void WindowProxy::setOnsuspend(events::EventHandlerNonNull onsuspend)
{
    window->setEventHandler(u"suspend", onsuspend);
}

events::EventHandlerNonNull WindowProxy::getOntimeupdate()
{
    return interface_cast<events::EventHandlerNonNull>(window->getEventHandler(u"timeupdate"));
}

void WindowProxy::setOntimeupdate(events::EventHandlerNonNull ontimeupdate)
{
    window->setEventHandler(u"timeupdate", ontimeupdate);
}

events::EventHandlerNonNull WindowProxy::getOnunload()
{
    return interface_cast<events::EventHandlerNonNull>(window->getEventHandler(u"unload"));
}

void WindowProxy::setOnunload(events::EventHandlerNonNull onunload)
{
    window->setEventHandler(u"unload", onunload);
}

events::EventHandlerNonNull WindowProxy::getOnvolumechange()
{
    return interface_cast<events::EventHandlerNonNull>(window->getEventHandler(u"volumechange"));
}

void WindowProxy::setOnvolumechange(events::EventHandlerNonNull onvolumechange)
{
    window->setEventHandler(u"volumechange", onvolumechange);
}

events::EventHandlerNonNull WindowProxy::getOnwaiting()
{
    return interface_cast<events::EventHandlerNonNull>(window->getEventHandler(u"waiting"));
}

void WindowProxy::setOnwaiting(events::EventHandlerNonNull onwaiting)
{
    window->setEventHandler(u"waiting", onwaiting);
}

void WindowProxy::updateView()
{
    if (parent)
        parent->updateView();
    while (view) {
        unsigned flags = view->gatherFlags();
        if (!flags || flags == Box::NEED_REPAINT)
            return;
        if (flags & Box::NEED_SELECTOR_REMATCHING) {
            backgroundTask.restart(BackgroundTask::Cascade);
            view = 0;
        } else if (flags & Box::NEED_SELECTOR_MATCHING) {
            backgroundTask.wakeUp(BackgroundTask::Cascade);
            view = 0;
        } else if (flags & (Box::NEED_STYLE_RECALCULATION | Box::NEED_EXPANSION | Box::NEED_CHILD_REFLOW | Box::NEED_REFLOW)) {
            backgroundTask.wakeUp(BackgroundTask::Layout);
            view = 0;
        }
        while (backgroundTask.isRestarting() ||
               backgroundTask.getState() != BackgroundTask::Done && backgroundTask.getState() != BackgroundTask::Init)
        {
            backgroundTask.wait();
            if (backgroundTask.getState() == BackgroundTask::Cascaded) {
                if (DocumentImp* document = dynamic_cast<DocumentImp*>(window->getDocument().self()))
                    HTMLElementImp::xblEnteredDocument(document);
                backgroundTask.wakeUp(BackgroundTask::Layout);
            }
        }
        ViewCSSImp* next = backgroundTask.getView();
        updateView(next);
    }
}

css::CSSStyleDeclaration WindowProxy::getComputedStyle(Element elt)
{
    updateView();
    if (!view)
        return 0;
    return view->getStyle(elt);
}

css::CSSStyleDeclaration WindowProxy::getComputedStyle(Element elt, const std::u16string& pseudoElt)
{
    updateView();
    if (!view)
        return 0;
    return view->getStyle(elt, pseudoElt);
}

html::MediaQueryList WindowProxy::matchMedia(const std::u16string& media_query_list)
{
    return window->matchMedia(media_query_list);
}

html::Screen WindowProxy::getScreen()
{
    return &screen;
}

int WindowProxy::getInnerWidth()
{
    return width;
}

int WindowProxy::getInnerHeight()
{
    return height;
}

int WindowProxy::getScrollX()
{
    return window->getScrollX();
}

int WindowProxy::getPageXOffset()
{
    return getScrollX();
}

int WindowProxy::getScrollY()
{
    return window->getScrollY();
}

int WindowProxy::getPageYOffset()
{
    return getScrollY();
}

void WindowProxy::scroll(int x, int y)
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

void WindowProxy::scrollTo(int x, int y)
{
    window->scroll(x, y);
}

void WindowProxy::scrollBy(int x, int y)
{
    scroll(getScrollX() + x, getScrollY() + y);
}

int WindowProxy::getScreenX()
{
    // TODO: implement me!
    return 0;
}

int WindowProxy::getScreenY()
{
    // TODO: implement me!
    return 0;
}

int WindowProxy::getOuterWidth()
{
    // TODO: implement me!
    return 0;
}

int WindowProxy::getOuterHeight()
{
    // TODO: implement me!
    return 0;
}

void WindowProxy::addEventListener(const std::u16string& type, events::EventListener listener, bool capture)
{
    if (window)
        window->addEventListener(type, listener, capture);
}

void WindowProxy::removeEventListener(const std::u16string& type, events::EventListener listener, bool capture)
{
    if (window)
        window->removeEventListener(type, listener, capture);
}

bool WindowProxy::dispatchEvent(events::Event event)
{
    if (window)
        return window->dispatchEvent(event);
    return false;
}

std::u16string WindowProxy::btoa(const std::u16string& btoa)
{
    // TODO: implement me!
    return u"";
}

std::u16string WindowProxy::atob(const std::u16string& atob)
{
    // TODO: implement me!
    return u"";
}

int WindowProxy::setTimeout(events::EventHandlerNonNull handler)
{
    // TODO: implement me!
    return 0;
}

int WindowProxy::setTimeout(events::EventHandlerNonNull handler, int timeout, Variadic<Any> arguments)
{
    // TODO: implement me!
    return 0;
}

int WindowProxy::setTimeout(const std::u16string& handler)
{
    // TODO: implement me!
    return 0;
}

int WindowProxy::setTimeout(const std::u16string& handler, int timeout, Variadic<Any> arguments)
{
    // TODO: implement me!
    return 0;
}

void WindowProxy::clearTimeout(int handle)
{
    // TODO: implement me!
}

int WindowProxy::setInterval(events::EventHandlerNonNull handler)
{
    // TODO: implement me!
    return 0;
}

int WindowProxy::setInterval(events::EventHandlerNonNull handler, int timeout, Variadic<Any> arguments)
{
    // TODO: implement me!
    return 0;
}

int WindowProxy::setInterval(const std::u16string& handler)
{
    // TODO: implement me!
    return 0;
}

int WindowProxy::setInterval(const std::u16string& handler, int timeout, Variadic<Any> arguments)
{
    // TODO: implement me!
    return 0;
}

void WindowProxy::clearInterval(int handle)
{
    // TODO: implement me!
}

void WindowProxy::postMessage(Any message, const std::u16string& targetOrigin)
{
    // TODO: implement me!
}

void WindowProxy::postMessage(Any message, const std::u16string& targetOrigin, Sequence<html::Transferable> transfer)
{
    // TODO: implement me!
}

std::u16string WindowProxy::toNativeLineEndings(const std::u16string& string)
{
    // TODO: implement me!
    return u"";
}

}}}}  // org::w3c::dom::bootstrap
