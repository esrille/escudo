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

#include "Box.h"

#include <org/w3c/dom/html/HTMLIFrameElement.h>
#include <org/w3c/dom/html/HTMLImageElement.h>

#include "ViewCSSImp.h"
#include "WindowImp.h"

#include "html/HTMLReplacedElementImp.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

void Box::resolveReplacedWidth(float intrinsicWidth, float intrinsicHeight)
{
    if (style->width.isAuto() && style->height.isAuto()) {
        if (0.0f < intrinsicWidth && 0.0f < intrinsicHeight)
            applyReplacedMinMax(intrinsicWidth, intrinsicHeight);
        else {
            if (0.0f <= intrinsicWidth)
                width = intrinsicWidth;
            else
                width = 300.0f;
            if (0.0f <= intrinsicHeight)
                height = intrinsicHeight;
            else
                height = 150.0f;
        }
    } else if (style->width.isAuto()) {
        height = style->height.getPx();
        if (0.0f < intrinsicHeight && 0.0f <= intrinsicWidth)
            width = height * intrinsicWidth / intrinsicHeight;
        else
            width = 300.0f;
    } else if (style->height.isAuto()) {
        width = style->width.getPx();
        if (0.0f <= intrinsicHeight && 0.0f < intrinsicWidth)
            height = width * intrinsicHeight / intrinsicWidth;
        else
            height = 150.0f;
    } else {
        width = style->width.getPx();
        height = style->height.getPx();
    }
    intrinsic = true;
}

void Box::applyReplacedMinMax(float w, float h)
{
    float minWidth = style->minWidth.getPx();
    float minHeight = style->minHeight.getPx();
    float maxWidth = style->maxWidth.isNone() ? w : style->maxWidth.getPx();
    maxWidth = std::max(minWidth, maxWidth);
    float maxHeight = style->maxHeight.isNone() ? h : style->maxHeight.getPx();
    maxHeight = std::max(minHeight, maxHeight);

    if (maxWidth < w && maxHeight < h) {
        if (maxWidth / w <= maxHeight / h) {
            width = maxWidth;
            height = std::max(minHeight, maxWidth * h / w);
        } else {
            width = std::max(minWidth, maxHeight * w / h);
            height = maxHeight;
        }
    } else if (w < minWidth && h < minHeight) {
        if (minWidth / w <= minHeight / h) {
            width = std::min(maxWidth, minHeight * w / h);
            height = minHeight;
        } else {
            width = minWidth;
            height = std::min(maxHeight, minWidth * h / w);
        }
    } else if (w < minWidth && maxHeight < h) {
        width = minWidth;
        height = maxHeight;
    } else if (maxWidth < w && h < minHeight) {
        width = maxWidth;
        height = minHeight;
    } else if (maxWidth < w) {
        width = maxWidth;
        height = std::max(maxWidth * h / w, minHeight);
    } else if (w < minWidth) {
        width = minWidth;
        height = std::min(minWidth * h / w, maxHeight);
    } else if (maxHeight < h) {
        width = std::max(maxHeight * w / h, minWidth);
        height = maxHeight;
    } else if (h < minHeight) {
        width = std::min(minHeight * w / h, maxWidth);
        height = minHeight;
    } else {
        width = w;
        height = h;
    }
}

bool BlockLevelBox::layOutReplacedElement(ViewCSSImp* view, Box* replaced, Element element, CSSStyleDeclarationImp* style)
{
    float intrinsicWidth = -1.0f;
    float intrinsicHeight = -1.0f;
    std::u16string tag = element.getLocalName();
    if (tag == u"img" || tag == u"object") {
        HTMLReplacedElementImp* replaced = dynamic_cast<HTMLReplacedElementImp*>(element.self());
        if (!replaced)
            return false;
        if (!replaced->getIntrinsicSize(intrinsicWidth, intrinsicHeight))
            return false;
    } else if (tag != u"iframe")
        return false;

    replaced->resolveReplacedWidth(intrinsicWidth, intrinsicHeight);

    if (tag == u"iframe") {
        html::HTMLIFrameElement iframe = interface_cast<html::HTMLIFrameElement>(element);
        html::Window contentWindow = iframe.getContentWindow();
        if (WindowImp* imp = dynamic_cast<WindowImp*>(contentWindow.self())) {
            imp->setSize(replaced->width, replaced->height);
            replaced->shadow = imp->getView();
            std::u16string src = iframe.getSrc();
            if (!src.empty() && !imp->getDocument())
                iframe.setSrc(src);
        }
    }

    return true;
}

}}}}  // org::w3c::dom::bootstrap
