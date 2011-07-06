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

#ifndef HTMLELEMENT_IMP_H
#define HTMLELEMENT_IMP_H

#include <Object.h>
#include <org/w3c/dom/html/HTMLElement.h>

#include <org/w3c/dom/css/CSSStyleDeclaration.h>
#include <org/w3c/dom/Element.h>
#include <org/w3c/dom/NodeList.h>
#include <org/w3c/dom/DOMTokenList.h>
#include <org/w3c/dom/DOMSettableTokenList.h>
#include <org/w3c/dom/DOMStringMap.h>
#include <org/w3c/dom/html/HTMLMenuElement.h>
#include <org/w3c/dom/html/Function.h>
#include <org/w3c/dom/html/HTMLPropertiesCollection.h>

#include "ElementImp.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

class HTMLElementImp : public ObjectMixin<HTMLElementImp, ElementImp>
{
    css::CSSStyleDeclaration style;

public:
    HTMLElementImp(DocumentImp* ownerDocument, const std::u16string& localName);
    HTMLElementImp(HTMLElementImp* org, bool deep);
    ~HTMLElementImp();

    virtual void eval();

    // Node
    virtual Node cloneNode(bool deep);

    // HTMLElement
    virtual NodeList getElementsByClassName(std::u16string classNames) __attribute__((weak));
    virtual std::u16string getInnerHTML() __attribute__((weak));
    virtual void setInnerHTML(std::u16string innerHTML) __attribute__((weak));
    virtual std::u16string getOuterHTML() __attribute__((weak));
    virtual void setOuterHTML(std::u16string outerHTML) __attribute__((weak));
    virtual void insertAdjacentHTML(std::u16string position, std::u16string text) __attribute__((weak));
    virtual std::u16string getId() __attribute__((weak));
    virtual void setId(std::u16string id) __attribute__((weak));
    virtual std::u16string getTitle() __attribute__((weak));
    virtual void setTitle(std::u16string title) __attribute__((weak));
    virtual std::u16string getLang() __attribute__((weak));
    virtual void setLang(std::u16string lang) __attribute__((weak));
    virtual std::u16string getDir() __attribute__((weak));
    virtual void setDir(std::u16string dir) __attribute__((weak));
    virtual std::u16string getClassName() __attribute__((weak));
    virtual void setClassName(std::u16string className) __attribute__((weak));
    virtual DOMTokenList getClassList() __attribute__((weak));
    virtual DOMStringMap getDataset() __attribute__((weak));
    virtual bool getItemScope() __attribute__((weak));
    virtual void setItemScope(bool itemScope) __attribute__((weak));
    virtual std::u16string getItemType() __attribute__((weak));
    virtual void setItemType(std::u16string itemType) __attribute__((weak));
    virtual std::u16string getItemId() __attribute__((weak));
    virtual void setItemId(std::u16string itemId) __attribute__((weak));
    virtual DOMSettableTokenList getItemRef() __attribute__((weak));
    virtual void setItemRef(std::u16string itemRef) __attribute__((weak));
    virtual DOMSettableTokenList getItemProp() __attribute__((weak));
    virtual void setItemProp(std::u16string itemProp) __attribute__((weak));
    virtual html::HTMLPropertiesCollection getProperties() __attribute__((weak));
    virtual Any getItemValue() __attribute__((weak));
    virtual void setItemValue(Any itemValue) __attribute__((weak));
    virtual bool getHidden() __attribute__((weak));
    virtual void setHidden(bool hidden) __attribute__((weak));
    virtual void click() __attribute__((weak));
    virtual int getTabIndex() __attribute__((weak));
    virtual void setTabIndex(int tabIndex) __attribute__((weak));
    virtual void focus() __attribute__((weak));
    virtual void blur() __attribute__((weak));
    virtual std::u16string getAccessKey() __attribute__((weak));
    virtual void setAccessKey(std::u16string accessKey) __attribute__((weak));
    virtual std::u16string getAccessKeyLabel() __attribute__((weak));
    virtual bool getDraggable() __attribute__((weak));
    virtual void setDraggable(bool draggable) __attribute__((weak));
    virtual DOMSettableTokenList getDropzone() __attribute__((weak));
    virtual void setDropzone(std::u16string dropzone) __attribute__((weak));
    virtual std::u16string getContentEditable() __attribute__((weak));
    virtual void setContentEditable(std::u16string contentEditable) __attribute__((weak));
    virtual bool getIsContentEditable() __attribute__((weak));
    virtual html::HTMLMenuElement getContextMenu() __attribute__((weak));
    virtual void setContextMenu(html::HTMLMenuElement contextMenu) __attribute__((weak));
    virtual bool getSpellcheck() __attribute__((weak));
    virtual void setSpellcheck(bool spellcheck) __attribute__((weak));
    virtual Nullable<std::u16string> getCommandType() __attribute__((weak));
    virtual Nullable<std::u16string> getLabel() __attribute__((weak));
    virtual Nullable<std::u16string> getIcon() __attribute__((weak));
    virtual bool getDisabled() __attribute__((weak));
    virtual bool getChecked() __attribute__((weak));
    virtual css::CSSStyleDeclaration getStyle() __attribute__((weak));
    virtual html::Function getOnabort() __attribute__((weak));
    virtual void setOnabort(html::Function onabort) __attribute__((weak));
    virtual html::Function getOnblur() __attribute__((weak));
    virtual void setOnblur(html::Function onblur) __attribute__((weak));
    virtual html::Function getOncanplay() __attribute__((weak));
    virtual void setOncanplay(html::Function oncanplay) __attribute__((weak));
    virtual html::Function getOncanplaythrough() __attribute__((weak));
    virtual void setOncanplaythrough(html::Function oncanplaythrough) __attribute__((weak));
    virtual html::Function getOnchange() __attribute__((weak));
    virtual void setOnchange(html::Function onchange) __attribute__((weak));
    virtual html::Function getOnclick() __attribute__((weak));
    virtual void setOnclick(html::Function onclick) __attribute__((weak));
    virtual html::Function getOncontextmenu() __attribute__((weak));
    virtual void setOncontextmenu(html::Function oncontextmenu) __attribute__((weak));
    virtual html::Function getOncuechange() __attribute__((weak));
    virtual void setOncuechange(html::Function oncuechange) __attribute__((weak));
    virtual html::Function getOndblclick() __attribute__((weak));
    virtual void setOndblclick(html::Function ondblclick) __attribute__((weak));
    virtual html::Function getOndrag() __attribute__((weak));
    virtual void setOndrag(html::Function ondrag) __attribute__((weak));
    virtual html::Function getOndragend() __attribute__((weak));
    virtual void setOndragend(html::Function ondragend) __attribute__((weak));
    virtual html::Function getOndragenter() __attribute__((weak));
    virtual void setOndragenter(html::Function ondragenter) __attribute__((weak));
    virtual html::Function getOndragleave() __attribute__((weak));
    virtual void setOndragleave(html::Function ondragleave) __attribute__((weak));
    virtual html::Function getOndragover() __attribute__((weak));
    virtual void setOndragover(html::Function ondragover) __attribute__((weak));
    virtual html::Function getOndragstart() __attribute__((weak));
    virtual void setOndragstart(html::Function ondragstart) __attribute__((weak));
    virtual html::Function getOndrop() __attribute__((weak));
    virtual void setOndrop(html::Function ondrop) __attribute__((weak));
    virtual html::Function getOndurationchange() __attribute__((weak));
    virtual void setOndurationchange(html::Function ondurationchange) __attribute__((weak));
    virtual html::Function getOnemptied() __attribute__((weak));
    virtual void setOnemptied(html::Function onemptied) __attribute__((weak));
    virtual html::Function getOnended() __attribute__((weak));
    virtual void setOnended(html::Function onended) __attribute__((weak));
    virtual html::Function getOnerror() __attribute__((weak));
    virtual void setOnerror(html::Function onerror) __attribute__((weak));
    virtual html::Function getOnfocus() __attribute__((weak));
    virtual void setOnfocus(html::Function onfocus) __attribute__((weak));
    virtual html::Function getOninput() __attribute__((weak));
    virtual void setOninput(html::Function oninput) __attribute__((weak));
    virtual html::Function getOninvalid() __attribute__((weak));
    virtual void setOninvalid(html::Function oninvalid) __attribute__((weak));
    virtual html::Function getOnkeydown() __attribute__((weak));
    virtual void setOnkeydown(html::Function onkeydown) __attribute__((weak));
    virtual html::Function getOnkeypress() __attribute__((weak));
    virtual void setOnkeypress(html::Function onkeypress) __attribute__((weak));
    virtual html::Function getOnkeyup() __attribute__((weak));
    virtual void setOnkeyup(html::Function onkeyup) __attribute__((weak));
    virtual html::Function getOnload() __attribute__((weak));
    virtual void setOnload(html::Function onload) __attribute__((weak));
    virtual html::Function getOnloadeddata() __attribute__((weak));
    virtual void setOnloadeddata(html::Function onloadeddata) __attribute__((weak));
    virtual html::Function getOnloadedmetadata() __attribute__((weak));
    virtual void setOnloadedmetadata(html::Function onloadedmetadata) __attribute__((weak));
    virtual html::Function getOnloadstart() __attribute__((weak));
    virtual void setOnloadstart(html::Function onloadstart) __attribute__((weak));
    virtual html::Function getOnmousedown() __attribute__((weak));
    virtual void setOnmousedown(html::Function onmousedown) __attribute__((weak));
    virtual html::Function getOnmousemove() __attribute__((weak));
    virtual void setOnmousemove(html::Function onmousemove) __attribute__((weak));
    virtual html::Function getOnmouseout() __attribute__((weak));
    virtual void setOnmouseout(html::Function onmouseout) __attribute__((weak));
    virtual html::Function getOnmouseover() __attribute__((weak));
    virtual void setOnmouseover(html::Function onmouseover) __attribute__((weak));
    virtual html::Function getOnmouseup() __attribute__((weak));
    virtual void setOnmouseup(html::Function onmouseup) __attribute__((weak));
    virtual html::Function getOnmousewheel() __attribute__((weak));
    virtual void setOnmousewheel(html::Function onmousewheel) __attribute__((weak));
    virtual html::Function getOnpause() __attribute__((weak));
    virtual void setOnpause(html::Function onpause) __attribute__((weak));
    virtual html::Function getOnplay() __attribute__((weak));
    virtual void setOnplay(html::Function onplay) __attribute__((weak));
    virtual html::Function getOnplaying() __attribute__((weak));
    virtual void setOnplaying(html::Function onplaying) __attribute__((weak));
    virtual html::Function getOnprogress() __attribute__((weak));
    virtual void setOnprogress(html::Function onprogress) __attribute__((weak));
    virtual html::Function getOnratechange() __attribute__((weak));
    virtual void setOnratechange(html::Function onratechange) __attribute__((weak));
    virtual html::Function getOnreadystatechange() __attribute__((weak));
    virtual void setOnreadystatechange(html::Function onreadystatechange) __attribute__((weak));
    virtual html::Function getOnreset() __attribute__((weak));
    virtual void setOnreset(html::Function onreset) __attribute__((weak));
    virtual html::Function getOnscroll() __attribute__((weak));
    virtual void setOnscroll(html::Function onscroll) __attribute__((weak));
    virtual html::Function getOnseeked() __attribute__((weak));
    virtual void setOnseeked(html::Function onseeked) __attribute__((weak));
    virtual html::Function getOnseeking() __attribute__((weak));
    virtual void setOnseeking(html::Function onseeking) __attribute__((weak));
    virtual html::Function getOnselect() __attribute__((weak));
    virtual void setOnselect(html::Function onselect) __attribute__((weak));
    virtual html::Function getOnshow() __attribute__((weak));
    virtual void setOnshow(html::Function onshow) __attribute__((weak));
    virtual html::Function getOnstalled() __attribute__((weak));
    virtual void setOnstalled(html::Function onstalled) __attribute__((weak));
    virtual html::Function getOnsubmit() __attribute__((weak));
    virtual void setOnsubmit(html::Function onsubmit) __attribute__((weak));
    virtual html::Function getOnsuspend() __attribute__((weak));
    virtual void setOnsuspend(html::Function onsuspend) __attribute__((weak));
    virtual html::Function getOntimeupdate() __attribute__((weak));
    virtual void setOntimeupdate(html::Function ontimeupdate) __attribute__((weak));
    virtual html::Function getOnvolumechange() __attribute__((weak));
    virtual void setOnvolumechange(html::Function onvolumechange) __attribute__((weak));
    virtual html::Function getOnwaiting() __attribute__((weak));
    virtual void setOnwaiting(html::Function onwaiting) __attribute__((weak));
    // HTMLElement-6
    virtual Element getOffsetParent() __attribute__((weak));
    virtual int getOffsetTop() __attribute__((weak));
    virtual int getOffsetLeft() __attribute__((weak));
    virtual int getOffsetWidth() __attribute__((weak));
    virtual int getOffsetHeight() __attribute__((weak));
    // Object
    virtual Any message_(uint32_t selector, const char* id, int argc, Any* argv)
    {
        return html::HTMLElement::dispatch(this, selector, id, argc, argv);
    }
    static const char* const getMetaData()
    {
        return html::HTMLElement::getMetaData();
    }
};

}}}}  // org::w3c::dom::bootstrap

#endif  // HTMLELEMENT_IMP_H
