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
#include <org/w3c/dom/html/HTMLDivElement.h>
#include <org/w3c/dom/html/HTMLInputElement.h>

#include <new>
#include <boost/bind.hpp>

#include "CSSStyleRuleImp.h"
#include "CSSStyleDeclarationImp.h"
#include "CSSStyleSheetImp.h"
#include "DocumentImp.h"

#include "Box.h"
#include "Table.h"
#include "StackingContext.h"

#include "Test.util.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

ViewCSSImp::ViewCSSImp(DocumentWindowPtr window, css::CSSStyleSheet defaultStyleSheet) :
    window(window),
    defaultStyleSheet(defaultStyleSheet),
    stackingContexts(0),
    hovered(0),
    dpi(96),
    mutationListener(boost::bind(&ViewCSSImp::handleMutation, this, _1))
{
    setMediumFontSize(16);
    getDocument().addEventListener(u"DOMAttrModified", &mutationListener);
}

ViewCSSImp::~ViewCSSImp()
{
    getDocument().removeEventListener(u"DOMAttrModified", &mutationListener);
}

Box* ViewCSSImp::boxFromPoint(int x, int y)
{
    if (!boxTree)
        return 0;
    if (Box* target = boxTree.get()->boxFromPoint(x, y))
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

void ViewCSSImp::resolveXY(float left, float top)
{
    if (boxTree)
        boxTree->resolveXY(this, left, top);
}

void ViewCSSImp::cascade()
{
    map.clear();
    delete stackingContexts;
    stackingContexts = 0;
    cascade(getDocument(), 0);

    printComputedValues(getDocument(), this);  // for debug
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
        for (auto i = set.begin(); i != set.end(); ++i) {
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
        for (auto i = set.begin(); i != set.end(); ++i) {
            if (CSSStyleDeclarationImp* pseudo = style->createPseudoElementStyle((*i).pseudoElementID))
                pseudo->specify((*i).decl);
        }
        for (auto i = set.begin(); i != set.end(); ++i) {
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
    if (!style->display.isInline() && !parentBox->hasAnonymousBox()) {
        // cf. http://www.w3.org/TR/CSS2/visuren.html#anonymous
        // White space content that would subsequently be collapsed
        // away according to the 'white-space' property does not
        // generate any anonymous inline boxes.
        if (style->whiteSpace.isCollapsingSpace()) {
            std::u16string data = text.getData();  // TODO: make this faster
            bool whiteSpace = true;
            for (auto i = data.begin(); i != data.end(); ++i) {
                if (!isSpace(*i)) {
                    whiteSpace = false;
                    break;
                }
            }
            if (whiteSpace)
                return 0;
        }
    }
    if (BlockLevelBox* anonymousBox = parentBox->getAnonymousBox()) {
        anonymousBox->insertInline(text);
        return anonymousBox;
    }
    return 0;
}

Element ViewCSSImp::expandBinding(Element element, CSSStyleDeclarationImp* style)
{
    switch (style->binding.getValue()) {
    case CSSBindingValueImp::InputTextfield: {
        html::HTMLInputElement input = interface_cast<html::HTMLInputElement>(element);
        html::HTMLDivElement div = interface_cast<html::HTMLDivElement>(getDocument().createElement(u"div"));
        Text text = getDocument().createTextNode(input.getValue());
        if (div && text) {
            div.appendChild(text);
            css::CSSStyleDeclaration divStyle = div.getStyle();
            divStyle.setCssText(u"float: left; border-style: solid; border-width: thin; height: 1.2em; text-align: left");
            CSSStyleDeclarationImp* imp = dynamic_cast<CSSStyleDeclarationImp*>(divStyle.self());
            if (imp) {
                imp->specify(style);
                imp->specifyImportant(style);
            }
            divStyle.setDisplay(u"block");
            cascade(div, style);
            return div;
        }
        break;
    }
    case CSSBindingValueImp::InputButton: {
        html::HTMLInputElement input = interface_cast<html::HTMLInputElement>(element);
        html::HTMLDivElement div = interface_cast<html::HTMLDivElement>(getDocument().createElement(u"div"));
        Text text = getDocument().createTextNode(input.getValue());
        if (div && text) {
            div.appendChild(text);
            css::CSSStyleDeclaration divStyle = div.getStyle();
            divStyle.setCssText(u"float: left; border-style: outset; border-width: thin; height: 1.2em; padding: 0 0.5em; text-align: center");
            CSSStyleDeclarationImp* imp = dynamic_cast<CSSStyleDeclarationImp*>(divStyle.self());
            if (imp) {
                imp->specify(style);
                imp->specifyImportant(style);
            }
            divStyle.setLineHeight(utfconv(std::to_string(style->height.getPx())) + u"px");  // TODO:
            divStyle.setDisplay(u"block");
            cascade(div, style);
            return div;
        }
        break;
    }
    case CSSBindingValueImp::InputRadio: {
        html::HTMLInputElement input = interface_cast<html::HTMLInputElement>(element);
        html::HTMLDivElement div = interface_cast<html::HTMLDivElement>(getDocument().createElement(u"div"));
        Text text = getDocument().createTextNode(input.getChecked() ? u"\u25c9" : u"\u25cb");
        if (div && text) {
            div.appendChild(text);
            css::CSSStyleDeclaration divStyle = div.getStyle();
            divStyle.setCssText(u"float: left; border-style: none;  height: 1.2em; padding: 0.5em");
            CSSStyleDeclarationImp* imp = dynamic_cast<CSSStyleDeclarationImp*>(divStyle.self());
            if (imp) {
                imp->specify(style);
                imp->specifyImportant(style);
            }
            divStyle.setDisplay(u"block");
            cascade(div, style);
            return div;
        }
        break;
    }
    default:
        break;
    }
    return element;
}

BlockLevelBox* ViewCSSImp::createBlockLevelBox(Element element, CSSStyleDeclarationImp* style, bool newContext)
{
    assert(style);
    BlockLevelBox* block;
    if (style->display == CSSDisplayValueImp::Table || style->display == CSSDisplayValueImp::InlineTable) {
        // TODO
        block = new(std::nothrow) TableWrapperBox(this, element, style);
        newContext = true;
    } else if (style->display == CSSDisplayValueImp::TableCell) {
        block = new(std::nothrow) CellBox(element, style);
        newContext = true;
    } else
        block =  new(std::nothrow) BlockLevelBox(element, style);
    if (!block)
        return 0;
    if (newContext)
        block->establishFormattingContext();  // TODO: check error
    style->addBox(block);

    StackingContext* stackingContext = style->getStackingContext();
    assert(stackingContext);
    if (style->isPositioned())
        stackingContext->addBase(block);

    return block;
}

BlockLevelBox* ViewCSSImp::layOutBlockBoxes(Element element, BlockLevelBox* parentBox, BlockLevelBox* siblingBox, CSSStyleDeclarationImp* style, bool asBlock)
{
    style = map[element].get();
    if (!style || style->display.isNone())
        return 0;
    bool runIn = style->display.isRunIn() && parentBox;

    BlockLevelBox* currentBox = parentBox;
    if (style->isFloat() || style->isAbsolutelyPositioned() || !parentBox) {
        element = expandBinding(element, style);
        currentBox = createBlockLevelBox(element, style, true);
        if (!currentBox)
            return 0;  // TODO: error
        // Do not insert currentBox into parentBox. currentBox will be
        // inserted in a lineBox of parentBox later
    } else if (style->isBlockLevel() || runIn || asBlock) {
        // Create a temporary block-level box for the run-in box, too.
        element = expandBinding(element, style);
        if (parentBox->hasInline()) {
            if (!parentBox->getAnonymousBox())
                return 0;
            assert(!parentBox->hasInline());
        }
        currentBox = createBlockLevelBox(element, style, style->isFlowRoot());
        if (!currentBox)
            return 0;
        parentBox->insertBefore(currentBox, parentBox->getFirstChild());
    } else if (style->isInlineBlock() || isReplacedElement(element)) {
        assert(currentBox);
        if (!currentBox->hasChildBoxes())
            currentBox->insertInline(element);
        else if (BlockLevelBox* anonymousBox = currentBox->getAnonymousBox()) {
            anonymousBox->insertInline(element);
            return anonymousBox;
        }
        return currentBox;
    }

    if (!dynamic_cast<TableWrapperBox*>(currentBox)) {
        bool emptyInline = style->display.isInline() && !element.hasChildNodes();
        BlockLevelBox* childBox = 0;
        if (CSSStyleDeclarationImp* afterStyle = style->getPseudoElementStyle(CSSPseudoElementSelector::After)) {
            afterStyle->compute(this, style, element);
            if (Element after = afterStyle->content.eval(getDocument(), element)) {
                emptyInline = false;
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
        if (CSSStyleDeclarationImp* beforeStyle = style->getPseudoElementStyle(CSSPseudoElementSelector::Before)) {
            beforeStyle->compute(this, style, element);
            if (Element before = beforeStyle->content.eval(getDocument(), element)) {
                emptyInline = false;
                map[before] = beforeStyle;
                if (BlockLevelBox* box = layOutBlockBoxes(before, currentBox, childBox, style))
                    childBox = box;
            }
        }
        if (emptyInline) {
            // Empty inline elements still have margins, padding, borders and a line height. cf. 10.8
            layOutBlockBoxes(Text(element.self()), currentBox, 0, style);
        }
    }

    // cf. http://www.w3.org/TR/2010/WD-CSS2-20101207/generate.html#before-after-content
    if (runIn && !currentBox->hasChildBoxes() && siblingBox) {
        assert(siblingBox->getBoxType() == Box::BLOCK_LEVEL_BOX);
        siblingBox->spliceInline(currentBox);
        parentBox->removeChild(currentBox);
        delete currentBox;
        currentBox = 0;
    }

    if (!currentBox || currentBox == parentBox)
        return 0;
    if (currentBox->isFloat() || currentBox->isAbsolutelyPositioned()) {
        floatMap[element] = currentBox;
        if (currentBox->isAbsolutelyPositioned())
            absoluteList.push_front(currentBox);
        if (parentBox) {
            // Set currentBox->parentBox to parentBox for now so that the correct
            // containing block can be retrieved before currentBox will be
            // inserted in a lineBox of parentBox later
            currentBox->parentBox = parentBox;
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
    boxTree = layOutBlockBoxes(getDocument(), 0, 0, 0);
    return boxTree.get();
}

BlockLevelBox* ViewCSSImp::layOut()
{
    if (stackingContexts)
        stackingContexts->clearBase();

    layOutBlockBoxes();
    if (!boxTree)
        return 0;
    // Expand line boxes and inline-level boxes in each block-level box
    if (!boxTree->isAbsolutelyPositioned()) {
        boxTree->layOut(this, 0);
        boxTree->resolveXY(this, 0.0f, 0.0f);
    }

    // Lay out absolute boxes.
    while (!absoluteList.empty()) {
        BlockLevelBox* box = absoluteList.front();
        absoluteList.pop_front();
        box->layOutAbsolute(this);
        box->resolveXY(this, box->x, box->y);
    }

    if (stackingContexts) {
        stackingContexts->addBase(boxTree.get());
        stackingContexts->dump();
    }
    return boxTree.get();
}

BlockLevelBox* ViewCSSImp::dump()
{
    boxTree->dump();
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
