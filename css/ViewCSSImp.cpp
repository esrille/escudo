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

#include "ViewCSSImp.h"

#include <org/w3c/dom/Text.h>
#include <org/w3c/dom/Comment.h>

#include <new>
#include <boost/bind.hpp>

#include "CSSStyleRuleImp.h"
#include "CSSStyleDeclarationImp.h"
#include "CSSStyleSheetImp.h"
#include "DocumentImp.h"

#include "Box.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

ViewCSSImp::ViewCSSImp(Document document, css::CSSStyleSheet defaultStyleSheet) :
    document(document),
    defaultStyleSheet(defaultStyleSheet),
    hovered(0),
    dpi(96),
    mutationListener(boost::bind(&ViewCSSImp::handleMutation, this, _1))
{
    setMediumFontSize(16);
    document.addEventListener(u"DOMAttrModified", &mutationListener);
}

ViewCSSImp::~ViewCSSImp()
{
    document.removeEventListener(u"DOMAttrModified", &mutationListener);
}

Box* ViewCSSImp::lookupTarget(int x, int y)
{
    if (!boxTree)
        return 0;
    if (Box* target = boxTree.get()->lookupTarget(x, y))
        return target;
    return boxTree.get();
}

bool ViewCSSImp::isHovered(Node node)
{
    for (Node i = hovered; i; i = i.getParentNode()) {
        if (node == i)
            return true;
    }
    return false;
}

void ViewCSSImp::handleMutation(events::Event event)
{
    if (boxTree)
        boxTree->setFlags(1);
}

void ViewCSSImp::findDeclarations(DeclarationSet& set, Element element, css::CSSRuleList list)
{
    if (!list)
        return;
    unsigned int size = list.getLength();
    for (unsigned int i = 0; i < size; ++i) {
        css::CSSRule rule = list.getElement(i);
        if (CSSStyleRuleImp* imp = dynamic_cast<CSSStyleRuleImp*>(rule.self())) {
            CSSSelector* selector = imp->match(element, this);
            if (!selector)
                continue;
            unsigned pseudoElementID = 0;
            if (CSSPseudoElementSelector* pseudo = selector->getPseudoElement())
                pseudoElementID = pseudo->getID();
            PrioritizedDeclaration decl(imp->getLastSpecificity(), dynamic_cast<CSSStyleDeclarationImp*>(imp->getStyle().self()), pseudoElementID);
            set.insert(decl);
        }
    }
}

void ViewCSSImp::cascade(Node node, CSSStyleDeclarationImp* parentStyle)
{
    CSSStyleDeclarationImp* style = 0;
    if (node.getNodeType() == Node::ELEMENT_NODE) {
        Element element = interface_cast<Element>(node);
        style = new(std::nothrow) CSSStyleDeclarationImp;
        if (!style) {
            map.erase(element);
            return;  // TODO: error
        }
        map[element] = style;
        DeclarationSet set;
        findDeclarations(set, element, defaultStyleSheet.getCssRules());
        for (auto i = set.rbegin(); i != set.rend(); ++i) {
            if (CSSStyleDeclarationImp* pseudo = style->createPseudoElementStyle((*i).pseudoElementID))
                pseudo->specify((*i).decl);
        }
        set.clear();

        stylesheets::StyleSheetList styleSheets = element.getOwnerDocument().getStyleSheets();
        for (unsigned i = 0; i < styleSheets.getLength(); ++i) {
            stylesheets::StyleSheet sheet = styleSheets.getElement(i);
            if (CSSStyleSheetImp* imp = dynamic_cast<CSSStyleSheetImp*>(sheet.self()))
                findDeclarations(set, element, imp->getCssRules());
        }
        for (auto i = set.rbegin(); i != set.rend(); ++i) {
            if (CSSStyleDeclarationImp* pseudo = style->createPseudoElementStyle((*i).pseudoElementID))
                pseudo->specify((*i).decl);
        }
        for (auto i = set.rbegin(); i != set.rend(); ++i) {
            if (CSSStyleDeclarationImp* pseudo = style->createPseudoElementStyle((*i).pseudoElementID))
                pseudo->specifyImportant((*i).decl);
        }
        set.clear();

        // TODO: process user normal declarations and user important declarations

        if (html::HTMLElement htmlElement = interface_cast<html::HTMLElement>(element)) {  // TODO: type check
            if (css::CSSStyleDeclaration decl = htmlElement.getStyle())
                style->specify(static_cast<CSSStyleDeclarationImp*>(decl.self()));
        }

        style->compute(this, parentStyle, element);
    }
    for (Node child = node.getFirstChild(); child; child = child.getNextSibling())
        cascade(child, style);
}

// In this step, neither inline-level boxes nor line boxes are generated.
// Those will be generated later by layOut().
BlockLevelBox* ViewCSSImp::layOutBlockBoxes(Node node, BlockLevelBox* parentBox, BlockLevelBox* siblingBox, CSSStyleDeclarationImp* style)
{
    BlockLevelBox* newBox = 0;
    switch (node.getNodeType()) {
    case Node::TEXT_NODE:
        newBox = layOutBlockBoxes(interface_cast<Text>(node), parentBox, siblingBox, style);
        break;
    case Node::ELEMENT_NODE:
        newBox = layOutBlockBoxes(interface_cast<Element>(node), parentBox, siblingBox, style);
        break;
    case Node::DOCUMENT_NODE:
        // Iterate over from back to front to process run-in boxes
        for (Node child = node.getLastChild(); child; child = child.getPreviousSibling()) {
            if (BlockLevelBox* box = layOutBlockBoxes(child, parentBox, newBox, style))
                newBox = box;
        }
        break;
    default:
        break;
    }
    return newBox;
}

BlockLevelBox* ViewCSSImp::layOutBlockBoxes(Text text, BlockLevelBox* parentBox, BlockLevelBox* siblingBox, CSSStyleDeclarationImp* style)
{
    if (!parentBox || !style)
        return 0;
    if (!parentBox->hasChildBoxes()) {
        parentBox->insertInline(text);
        return 0;
    }
    if (!parentBox->hasAnonymousBox()) {
        // cf. http://www.w3.org/TR/CSS2/visuren.html#anonymous
        // White space content that would subsequently be collapsed
        // away according to the 'white-space' property does not
        // generate any anonymous inline boxes.
        std::u16string data;
        bool whiteSpace = true;
        switch (style->whiteSpace.getValue()) {
        case CSSWhiteSpaceValueImp::Normal:
        case CSSWhiteSpaceValueImp::Nowrap:
        case CSSWhiteSpaceValueImp::PreLine:
            data = text.getData();  // TODO: make this faster
            for (auto i = data.begin(); i != data.end(); ++i) {
                if (!isSpace(*i)) {
                    whiteSpace = false;
                    break;
                }
            }
            if (whiteSpace)
                return 0;
            break;
        }
    }
    if (BlockLevelBox* anonymousBox = parentBox->getAnonymousBox()) {
        anonymousBox->insertInline(text);
        return anonymousBox;
    }
    return 0;
}

BlockLevelBox* ViewCSSImp::layOutBlockBoxes(Element element, BlockLevelBox* parentBox, BlockLevelBox* siblingBox, CSSStyleDeclarationImp* style)
{
    style = map[element].get();
    if (!style || style->display.isNone())
        return 0;
    bool runIn = style->display.isRunIn();
    BlockLevelBox* currentBox = parentBox;
    BlockLevelBox* childBox = 0;
    if (style->isFloat() || style->isAbsolutelyPositioned()) {
        currentBox = new(std::nothrow) BlockLevelBox(element, style);
        if (!currentBox)
            return 0;  // TODO: error
        if (!currentBox->establishFormattingContext())
            return 0;  // TODO: error
        style->addBox(currentBox);
        // Do not insert currentBox into parentBox
    } else if (runIn || style->display.isBlockLevel()) {
        // Create a temporary block-level box for the run-in box, too.
        if (parentBox && parentBox->hasInline()) {
            if (!parentBox->getAnonymousBox())
                return 0;
            assert(!parentBox->hasInline());
        }
        currentBox = new(std::nothrow) BlockLevelBox(element, style);
        if (!currentBox)
            return 0;
        style->addBox(currentBox);
        if (parentBox) {
            if (Box* first = parentBox->getFirstChild())
                parentBox->insertBefore(currentBox, first);
            else
                parentBox->appendChild(currentBox);
        } else
            runIn = false;

        // Establish a new formatting context?
        if (!parentBox || style->isFlowRoot()) {
            if (!currentBox->establishFormattingContext())
                return 0;  // TODO error
        }
    } else {
        std::u16string tag = element.getLocalName();  // TODO: Check HTML namespace
        if (tag == u"img" || tag == u"iframe" || tag == u"video" || tag == u"input") {  // TODO: more tags to come...
            // Replaced element
            assert(currentBox);
            if (!currentBox->hasChildBoxes())
                currentBox->insertInline(element);
            else if (BlockLevelBox* anonymousBox = currentBox->getAnonymousBox()) {
                anonymousBox->insertInline(element);
                return anonymousBox;
            }
            return currentBox;
        }
    }

    if (CSSStyleDeclarationImp* afterStyle = style->getPseudoElementStyle(CSSPseudoElementSelector::After)) {
        afterStyle->compute(this, style, element);
        if (Element after = afterStyle->content.eval(document, element)) {
            map[after] = afterStyle;
            if (BlockLevelBox* box = layOutBlockBoxes(after, currentBox, childBox, style))
                childBox = box;
        }
    }

    // Iterate over from back to front to process run-in boxes
    for (Node child = element.getLastChild(); child; child = child.getPreviousSibling()) {
        if (BlockLevelBox* box = layOutBlockBoxes(child, currentBox, childBox, style))
            childBox = box;
    }

    if (runIn && !currentBox->hasChildBoxes() && siblingBox) {
        assert(siblingBox->getBoxType() == Box::BLOCK_LEVEL_BOX);
        siblingBox->spliceInline(currentBox);
        parentBox->removeChild(currentBox);
        delete currentBox;
        currentBox = 0;
    }

    if (CSSStyleDeclarationImp* beforeStyle = style->getPseudoElementStyle(CSSPseudoElementSelector::Before)) {
        beforeStyle->compute(this, style, element);
        if (Element before = beforeStyle->content.eval(document, element)) {
            map[before] = beforeStyle;
            if (BlockLevelBox* box = layOutBlockBoxes(before, currentBox, childBox, style))
                childBox = box;
        }
    }

    if (!currentBox)
        currentBox = childBox;
    else if (currentBox == parentBox)
        currentBox = 0;
    else if (currentBox->isFloat() || currentBox->isAbsolutelyPositioned()) {
        floatMap[element] = currentBox;
        if (parentBox) {
            if (!parentBox->hasChildBoxes())
                parentBox->insertInline(element);
            else if (BlockLevelBox* anonymousBox = parentBox->getAnonymousBox()) {
                anonymousBox->insertInline(element);
                return anonymousBox;
            }
        }
    }
    return currentBox;
}

// Lay out a tree box block-level boxes
BlockLevelBox* ViewCSSImp::layOutBlockBoxes()
{
    floatMap.clear();
    boxTree = layOutBlockBoxes(document, 0, 0, 0);
    return boxTree.get();
}

BlockLevelBox* ViewCSSImp::layOut()
{
    layOutBlockBoxes();
    if (!boxTree)
        return 0;
    // Expand line boxes and inline-level boxes in each block-level box
    boxTree->layOut(this, 0);
    // Lay out absolute boxes
    for (auto i = floatMap.begin(); i != floatMap.end(); ++i) {
        if (i->second->isAbsolutelyPositioned())
            i->second->layOutAbsolute(this, i->first);
    }
    return boxTree.get();
}

BlockLevelBox* ViewCSSImp::dump()
{
    boxTree->dump(this);
    return boxTree.get();
}

CSSStyleDeclarationImp* ViewCSSImp::getStyle(Element elt, Nullable<std::u16string> pseudoElt)
{
    auto i = map.find(elt);
    if (i == map.end())
        return 0;
    CSSStyleDeclarationImp* style = i->second.get();
    if (!pseudoElt.hasValue() || pseudoElt.value().length() == 0)
        return style;
    return style->getPseudoElementStyle(pseudoElt.value());
}

// ViewCSS
css::CSSStyleDeclaration ViewCSSImp::getComputedStyle(Element elt, Nullable<std::u16string> pseudoElt) {
    return getStyle(elt, pseudoElt);
}

}}}}  // org::w3c::dom::bootstrap
