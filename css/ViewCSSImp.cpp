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

#include "ViewCSSImp.h"

#include <org/w3c/dom/Text.h>
#include <org/w3c/dom/Comment.h>
#include <org/w3c/dom/html/HTMLDivElement.h>
#include <org/w3c/dom/html/HTMLInputElement.h>
#include <org/w3c/dom/html/HTMLLinkElement.h>
#include <org/w3c/dom/html/HTMLStyleElement.h>

#include <new>
#include <boost/bind.hpp>

#include "CSSImportRuleImp.h"
#include "CSSMediaRuleImp.h"
#include "CSSStyleRuleImp.h"
#include "CSSStyleDeclarationImp.h"
#include "CSSStyleSheetImp.h"
#include "DocumentImp.h"
#include "MediaListImp.h"
#include "html/HTMLElementImp.h"
#include "html/HTMLTemplateElementImp.h"

#include "Box.h"
#include "Table.h"
#include "StackingContext.h"

#include "Test.util.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

ViewCSSImp::ViewCSSImp(DocumentWindowPtr window, css::CSSStyleSheet defaultStyleSheet, css::CSSStyleSheet userStyleSheet) :
    window(window),
    dpi(96),
    zoom(1.0f),
    hovered(0),
    mutationListener(boost::bind(&ViewCSSImp::handleMutation, this, _1)),
    defaultStyleSheet(defaultStyleSheet),
    userStyleSheet(userStyleSheet),
    stackingContexts(0),
    overflow(CSSOverflowValueImp::Auto),
    quotingDepth(0),
    scrollWidth(0.0f),
    scrollHeight(0.0f)
{
    setMediumFontSize(16);
    getDocument().addEventListener(u"DOMAttrModified", &mutationListener);
    getDocument().addEventListener(u"DOMCharacterDataModified", &mutationListener);
    getDocument().addEventListener(u"DOMNodeInserted", &mutationListener);
    getDocument().addEventListener(u"DOMNodeRemoved", &mutationListener);
}

ViewCSSImp::~ViewCSSImp()
{
    getDocument().removeEventListener(u"DOMAttrModified", &mutationListener);
    getDocument().removeEventListener(u"DOMCharacterDataModified", &mutationListener);
    getDocument().removeEventListener(u"DOMNodeInserted", &mutationListener);
    getDocument().removeEventListener(u"DOMNodeRemoved", &mutationListener);
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

void ViewCSSImp::findDeclarations(CSSRuleListImp::DeclarationSet& set, Element element, css::CSSRuleList list, unsigned importance)
{
    CSSRuleListImp* ruleList = dynamic_cast<CSSRuleListImp*>(list.self());
    if (!ruleList)
        return;
    ruleList->find(set, this, element, importance);
}

void ViewCSSImp::resolveXY(float left, float top)
{
    if (boxTree)
        boxTree->resolveXY(this, left, top, 0);
}

void ViewCSSImp::cascade()
{
    Document document = getDocument();

    map.clear();
    delete stackingContexts;
    stackingContexts = 0;

    styleSheets.clear();
    html::HTMLHeadElement head = document.getHead();
    if (head) {
        for (auto element = head.getFirstElementChild(); element; element = element.getNextElementSibling()) {
            if (html::HTMLStyleElement::hasInstance(element)) {
                html::HTMLStyleElement styleElement = interface_cast<html::HTMLStyleElement>(element);
                stylesheets::StyleSheet styleSheet = styleElement.getSheet();
                if (CSSStyleSheetImp* sheet = dynamic_cast<CSSStyleSheetImp*>(styleSheet.self()))
                    styleSheets.push_back(sheet);
            } else if (html::HTMLLinkElement::hasInstance(element)) {
                html::HTMLLinkElement linkElement = interface_cast<html::HTMLLinkElement>(element);
                stylesheets::StyleSheet styleSheet = linkElement.getSheet();
                if (CSSStyleSheetImp* sheet = dynamic_cast<CSSStyleSheetImp*>(styleSheet.self()))
                    styleSheets.push_back(sheet);
            }
        }
    }

    cascade(document, 0);
    if (DocumentImp* imp = dynamic_cast<DocumentImp*>(document.self())) {
        imp->clearStyleSheets();
        if (CSSStyleSheetImp* sheet = dynamic_cast<CSSStyleSheetImp*>(defaultStyleSheet.self()))
            imp->addStyleSheet(sheet);
        if (CSSStyleSheetImp* sheet = dynamic_cast<CSSStyleSheetImp*>(userStyleSheet.self()))
            imp->addStyleSheet(sheet);
        for (auto i = styleSheets.begin(); i != styleSheets.end(); ++i)
            imp->addStyleSheet(*i);
    }
    styleSheets.clear();
    if (3 <= getLogLevel())
        printComputedValues(document, this);
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

        CSSStyleDeclarationImp* elementDecl = 0;
        if (html::HTMLElement::hasInstance(element)) {
            html::HTMLElement htmlElement = interface_cast<html::HTMLElement>(element);
            elementDecl = dynamic_cast<CSSStyleDeclarationImp*>(htmlElement.getStyle().self());
        }

        CSSRuleListImp::DeclarationSet set;
        if (CSSStyleSheetImp* sheet = dynamic_cast<CSSStyleSheetImp*>(defaultStyleSheet.self()))
            findDeclarations(set, element, sheet->getCssRules(), CSSRuleListImp::UserAgent);
        if (CSSStyleSheetImp* sheet = dynamic_cast<CSSStyleSheetImp*>(userStyleSheet.self()))
            findDeclarations(set, element, sheet->getCssRules(), CSSRuleListImp::User);
        if (elementDecl) {
            CSSStyleDeclarationImp* nonCSS = elementDecl->getPseudoElementStyle(CSSPseudoElementSelector::NonCSS);
            if (nonCSS) {
                CSSRuleListImp::PrioritizedDeclaration decl(CSSRuleListImp::User | 0, nonCSS, CSSPseudoElementSelector::NonPseudo, 0);
                set.insert(decl);
            }
        }
        unsigned importance = CSSRuleListImp::Author;
        for (auto i = styleSheets.begin(); i != styleSheets.end(); ++i) {
            CSSStyleSheetImp* sheet = *i;
            findDeclarations(set, element, sheet->getCssRules(), importance++);
            // TODO: Check overflow of importance
        }
        for (auto i = set.begin(); i != set.end(); ++i) {
            if (CSSStyleDeclarationImp* pseudo = style->createPseudoElementStyle((*i).pseudoElementID))
                pseudo->specify((*i).decl);
        }
        if (elementDecl)
            style->specify(elementDecl);
        for (auto i = set.begin(); i != set.end(); ++i) {
            if (CSSStyleDeclarationImp* pseudo = style->createPseudoElementStyle((*i).pseudoElementID)) {
                if ((*i).isUserStyle())
                    continue;
                pseudo->specifyImportant((*i).decl);
            }
        }
        if (elementDecl)
            style->specifyImportant(elementDecl);
        for (auto i = set.begin(); i != set.end(); ++i) {
            if (CSSStyleDeclarationImp* pseudo = style->createPseudoElementStyle((*i).pseudoElementID)) {
                if (!((*i).isUserStyle()))
                    continue;
                pseudo->specifyImportant((*i).decl);
            }
        }
        set.clear();

        // expand binding
        if (style->binding.getValue() != CSSBindingValueImp::None) {
            if (HTMLElementImp* imp = dynamic_cast<HTMLElementImp*>(element.self())) {
                imp->generateShadowContent(style);
                if (html::HTMLTemplateElement shadowTree = imp->getShadowTree()) {
                    if (auto imp = dynamic_cast<HTMLTemplateElementImp*>(shadowTree.self()))
                        imp->setHost(element);
                    node = shadowTree;
                }
            }
        } // TODO: detach the shadow tree froom element (if any)

        style->compute(this, parentStyle, element);
    }
    for (Node child = node.getFirstChild(); child; child = child.getNextSibling())
        cascade(child, style);

    if (node.getNodeType() == Node::ELEMENT_NODE) {
        Element element = interface_cast<Element>(node);
        if (element.getLocalName() == u"body") {  // TODO: check HTML namespace
            assert(parentStyle);
            assert(style);
            if (parentStyle->overflow.getValue() == CSSOverflowValueImp::Visible) {
                parentStyle->overflow.specify(style->overflow);
                style->overflow.setValue(CSSOverflowValueImp::Visible);
            }
            if (parentStyle->backgroundColor.getARGB() == 0 &&  // transparent?
                parentStyle->backgroundImage.isNone()) {
                parentStyle->background.specify(parentStyle, style);
                style->background.reset(style);
            }
        }
    }

    if (!parentStyle && style)
        overflow = style->overflow.getValue();
}

// In this step, neither inline-level boxes nor line boxes are generated.
// Those will be generated later by layOut().
BlockLevelBox* ViewCSSImp::layOutBlockBoxes(Node node, BlockLevelBox* parentBox, CSSStyleDeclarationImp* style, CSSAutoNumberingValueImp::CounterContext* counterContext)
{
    BlockLevelBox* newBox = 0;
    switch (node.getNodeType()) {
    case Node::TEXT_NODE:
        newBox = layOutBlockBoxes(interface_cast<Text>(node), parentBox, style, counterContext);
        break;
    case Node::ELEMENT_NODE:
        newBox = layOutBlockBoxes(interface_cast<Element>(node), parentBox, style, counterContext);
        break;
    case Node::DOCUMENT_NODE:
        for (Node child = node.getFirstChild(); child; child = child.getNextSibling()) {
            if (BlockLevelBox* box = layOutBlockBoxes(child, parentBox, style, counterContext))
                newBox = box;
        }
        break;
    default:
        break;
    }
    return newBox;
}

BlockLevelBox* ViewCSSImp::layOutBlockBoxes(Text text, BlockLevelBox* parentBox, CSSStyleDeclarationImp* style, CSSAutoNumberingValueImp::CounterContext* counterContext)
{
    bool discardable = true;
    if (style->display.isInline()) {
        Element element = interface_cast<Element>(text.getParentNode());
        assert(element);
        if (element.getFirstChild() == text && (style->marginLeft.getPx() || style->borderLeftWidth.getPx() || style->paddingLeft.getPx()) ||
            element.getLastChild() == text && (style->marginRight.getPx() || style->borderRightWidth.getPx() || style->paddingRight.getPx()))
            discardable = false;
    }

    if (!parentBox || !style)
        return 0;
    if (!parentBox->hasChildBoxes()) {
        if (discardable && !parentBox->hasInline() && style->whiteSpace.isCollapsingSpace()) {
            std::u16string data = text.getData();
            if (data.length() <= style->processLineHeadWhiteSpace(data, 0))
                return 0;
        }
        parentBox->insertInline(text);
        return 0;
    }
    if (TableWrapperBox* table = dynamic_cast<TableWrapperBox*>(parentBox->getLastChild())) {
        if (table && table->isAnonymousTableObject()) {
            if (table->processTableChild(text, style))
                return 0;
        }
    }
    if (discardable && !style->display.isInline() && !parentBox->hasAnonymousBox()) {
        // cf. http://www.w3.org/TR/CSS2/visuren.html#anonymous
        // White space content that would subsequently be collapsed
        // away according to the 'white-space' property does not
        // generate any anonymous inline boxes.
        if (style->whiteSpace.isCollapsingSpace()) {
            std::u16string data = text.getData();
            if (data.length() <= style->processLineHeadWhiteSpace(data, 0))
                return 0;
        }
    }
    if (BlockLevelBox* anonymousBox = parentBox->getAnonymousBox()) {
        anonymousBox->insertInline(text);
        return anonymousBox;
    }
    return 0;
}

BlockLevelBox* ViewCSSImp::createBlockLevelBox(Element element, BlockLevelBox* parentBox, CSSStyleDeclarationImp* style, bool newContext, bool asBlock)
{
    assert(style);
    BlockLevelBox* block;
    if (style->display == CSSDisplayValueImp::Table || style->display == CSSDisplayValueImp::InlineTable) {
        block = new(std::nothrow) TableWrapperBox(this, element, style);
        newContext = true;
    } else if (style->display.isTableParts()) {
        if (asBlock) {
            if (style->display == CSSDisplayValueImp::TableCell)
                block = new(std::nothrow) CellBox(element, style);
            else
                block =  new(std::nothrow) BlockLevelBox(element, style);
        } else {
            if (parentBox) {
                TableWrapperBox* table = dynamic_cast<TableWrapperBox*>(parentBox->getLastChild());
                if (table && table->isAnonymousTableObject()) {
                    table->processTableChild(element, style);
                    return 0;
                }
            }
            block = new(std::nothrow) TableWrapperBox(this, element, style);
        }
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

BlockLevelBox* ViewCSSImp::layOutBlockBoxes(Element element, BlockLevelBox* parentBox, CSSStyleDeclarationImp* parentStyle, CSSAutoNumberingValueImp::CounterContext* counterContext, bool asBlock)
{
#ifndef NDEBUG
    std::u16string tag = interface_cast<html::HTMLElement>(element).getTagName();
#endif

    CSSStyleDeclarationImp* style = map[element].get();
    if (!style || style->display.isNone())
        return 0;
    style->clearBox();
    bool runIn = style->display.isRunIn() && parentBox;
    bool anonInlineTable = style->display.isTableParts() && parentStyle && parentStyle->display.isInlineLevel();

    CSSAutoNumberingValueImp::CounterContext cc(this);
    if (!counterContext)    // TODO: This is only to workaround the fatal errors.
        counterContext = &cc;

    if (style->getPseudoElementSelectorType() == CSSPseudoElementSelector::NonPseudo)
        counterContext->update(style);

    Element shadow = element;
    if (HTMLElementImp* imp = dynamic_cast<HTMLElementImp*>(element.self())) {
        if (imp->getShadowTree())
            shadow = imp->getShadowTree();
    }

    BlockLevelBox* currentBox = parentBox;
    if (style->isFloat() || style->isAbsolutelyPositioned() || !parentBox) {
        currentBox = createBlockLevelBox(element, parentBox, style, true, asBlock);
        if (!currentBox)
            return 0;  // TODO: error
        // Do not insert currentBox into parentBox. currentBox will be
        // inserted in a lineBox of parentBox later
    } else if ((style->isBlockLevel() && !anonInlineTable) || runIn || asBlock) {
        // Create a temporary block-level box for the run-in box, too.
        if (parentBox->hasInline()) {
            BlockLevelBox* anon = parentBox->getAnonymousBox();
            if (!anon)
                return 0;
            assert(!parentBox->hasInline());
        }
        currentBox = createBlockLevelBox(element, parentBox, style, style->isFlowRoot(), asBlock);
        if (!currentBox)
            return 0;
        parentBox->appendChild(currentBox);
    } else if (anonInlineTable || style->isInlineBlock() || isReplacedElement(element)) {
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
        CSSStyleDeclarationImp* markerStyle = 0;
        if (style->display.isListItem()) {
            markerStyle = style->getPseudoElementStyle(CSSPseudoElementSelector::Marker);
            if (!markerStyle) {
                // Generate the default markerStyle.
                markerStyle = style->createPseudoElementStyle(CSSPseudoElementSelector::Marker);
                if (markerStyle) {
                    // Set the default marker style
                    markerStyle->setDisplay(u"inline-block");
                    markerStyle->setLetterSpacing(u"normal");
                    markerStyle->setWordSpacing(u"normal");
                    markerStyle->setFontFamily(u"sans-serif");
                }
            }
        }
        if (markerStyle) {
            markerStyle->compute(this, style, element);
            if (markerStyle->display.isNone() || markerStyle->content.isNone())
                markerStyle = 0;
        }
        CSSStyleDeclarationImp* beforeStyle = style->getPseudoElementStyle(CSSPseudoElementSelector::Before);
        if (beforeStyle) {
            beforeStyle->compute(this, style, element);
            if (beforeStyle->display.isNone() || beforeStyle->content.isNone())
                beforeStyle = 0;
        }
        CSSStyleDeclarationImp* afterStyle = style->getPseudoElementStyle(CSSPseudoElementSelector::After);
        if (afterStyle) {
            afterStyle->compute(this, style, element);
            if (afterStyle->display.isNone() || afterStyle->content.isNone())
                afterStyle = 0;
        }

        style->emptyInline = 0;
        if (style->display.isInline() && !isReplacedElement(element)) {
            // Empty inline elements still have margins, padding, borders and a line height. cf. 10.8
            if (!shadow.hasChildNodes() && !markerStyle && !beforeStyle && !afterStyle)
                style->emptyInline = 4;
            else {
                if (markerStyle || beforeStyle)
                    style->emptyInline = 1;
                else if (style->marginLeft.getPx() || style->borderLeftWidth.getPx() || style->paddingLeft.getPx()) {
                    Node child = shadow.getFirstChild();
                    if (child.getNodeType() != Node::TEXT_NODE)
                        style->emptyInline = 1;
                }
                if (afterStyle)
                    style->emptyInline |= 2;
                else if (style->marginRight.getPx() || style->borderRightWidth.getPx() || style->paddingRight.getPx()) {
                    Node child = shadow.getLastChild();
                    if (child.getNodeType() != Node::TEXT_NODE)
                        style->emptyInline |= 2;
                }
            }
        }

        if ((style->emptyInline & 1) || style->emptyInline == 4) {
            if (!currentBox->hasChildBoxes())
                currentBox->insertInline(element);
            else if (BlockLevelBox* anonymousBox = currentBox->getAnonymousBox())
                anonymousBox->insertInline(element);
        }

        BlockLevelBox* childBox = 0;
        CSSAutoNumberingValueImp::CounterContext ccPseudo(this);
        TableWrapperBox* tableWrapperBox = 0;

        if (markerStyle) {
            // Execute implicit 'counter-increment: list-item;'
            CounterImpPtr counter = getCounter(u"list-item");
            if (counter)
                counter->increment(1);
            ccPseudo.update(markerStyle);
            if (Element marker = markerStyle->content.eval(this, element, &cc)) {
                map[marker] = markerStyle;
                if (BlockLevelBox* box = layOutBlockBoxes(marker, currentBox, style, &cc)) {
                    childBox = box;
                    if (tableWrapperBox = dynamic_cast<TableWrapperBox*>(box)) {
                        if (tableWrapperBox->isAnonymousTableObject()) {
                            tableWrapperBox->layOutBlockBoxes();
                            tableWrapperBox = 0;
                        }
                    }
                }
            }
            // Deal with an empty list item; cf. list-alignment-001, acid2.
            // TODO: Find out where the exact behavior is defined in the specifications.
            if (style->height.isAuto() && !shadow.hasChildNodes() && !beforeStyle && !afterStyle) {
                if (!currentBox->hasChildBoxes())
                    currentBox->insertInline(element);
                else if (BlockLevelBox* anonymousBox = currentBox->getAnonymousBox())
                    anonymousBox->insertInline(element);
            }
        }

        if (beforeStyle) {
            ccPseudo.update(beforeStyle);
            if (Element before = beforeStyle->content.eval(this, element, &cc)) {
                map[before] = beforeStyle;
                if (BlockLevelBox* box = layOutBlockBoxes(before, currentBox, style, &cc)) {
                    childBox = box;
                    if (tableWrapperBox = dynamic_cast<TableWrapperBox*>(box)) {
                        if (tableWrapperBox->isAnonymousTableObject()) {
                            tableWrapperBox->layOutBlockBoxes();
                            tableWrapperBox = 0;
                        }
                    }
                }
            }
        }

        for (Node child = shadow.getFirstChild(); child; child = child.getNextSibling()) {
            if (BlockLevelBox* box = layOutBlockBoxes(child, currentBox, style, ccPseudo.hasCounter() ? &ccPseudo : &cc)) {
                childBox = box;
                if (tableWrapperBox && tableWrapperBox->isAnonymousTableObject())
                    tableWrapperBox->layOutBlockBoxes();
                tableWrapperBox = dynamic_cast<TableWrapperBox*>(box);
            }
        }
        if (tableWrapperBox && tableWrapperBox->isAnonymousTableObject())
            tableWrapperBox->layOutBlockBoxes();

        if (afterStyle) {
            ccPseudo.update(afterStyle);
            if (Element after = afterStyle->content.eval(this, element, &cc)) {
                map[after] = afterStyle;
                if (BlockLevelBox* box = layOutBlockBoxes(after, currentBox, style, &cc)) {
                    childBox = box;
                    if (tableWrapperBox = dynamic_cast<TableWrapperBox*>(box)) {
                        if (tableWrapperBox->isAnonymousTableObject()) {
                            tableWrapperBox->layOutBlockBoxes();
                            tableWrapperBox = 0;
                        }
                    }
                }

            }
        }

        if (style->emptyInline & 2) {
            if (!currentBox->hasChildBoxes())
                currentBox->insertInline(element);
            else if (BlockLevelBox* anonymousBox = currentBox->getAnonymousBox())
                anonymousBox->insertInline(element);
        }
    }

#if 0
    // cf. http://www.w3.org/TR/2010/WD-CSS2-20101207/generate.html#before-after-content
    if (runIn && !currentBox->hasChildBoxes() && siblingBox) {
        assert(siblingBox->getBoxType() == Box::BLOCK_LEVEL_BOX);
        siblingBox->spliceInline(currentBox);
        parentBox->removeChild(currentBox);
        delete currentBox;
        currentBox = 0;
    }
#endif

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
    assert(absoluteList.empty());

    CSSAutoNumberingValueImp::CounterContext cc(this);
    boxTree = 0;
    floatMap.clear();
    boxTree = layOutBlockBoxes(getDocument(), 0, 0, &cc);
    clearCounters();
    return boxTree.get();
}

BlockLevelBox* ViewCSSImp::layOut()
{
    if (stackingContexts)
        stackingContexts->clearBase();

    quotingDepth = 0;
    scrollWidth = 0.0f;

    layOutBlockBoxes();
    if (!boxTree)
        return 0;
    // Expand line boxes and inline-level boxes in each block-level box
    if (!boxTree->isAbsolutelyPositioned()) {
        boxTree->layOut(this, 0);
        boxTree->resolveXY(this, 0.0f, 0.0f, 0);
    }

    // Lay out absolute boxes.
    while (!absoluteList.empty()) {
        BlockLevelBox* box = absoluteList.front();
        absoluteList.pop_front();
        box->layOutAbsolute(this);
        box->resolveXY(this, box->x, box->y, box->clipBox);
    }

    if (stackingContexts) {
        stackingContexts->addBase(boxTree.get());
        if (2 <= getLogLevel()) {
            std::cout << "## stacking contexts\n";
            stackingContexts->dump();
        }
    }
    return boxTree.get();
}

BlockLevelBox* ViewCSSImp::dump()
{
    std::cout << "## render tree\n";
    // When the root element has display:none, no box is created at all.
    if (boxTree) {
        boxTree->dump();
        return boxTree.get();
    }
    return 0;
}

CounterImpPtr ViewCSSImp::getCounter(const std::u16string identifier)
{
    assert(!identifier.empty());
    for (auto i = counterList.begin(); i != counterList.end(); ++i) {
        CounterImpPtr counter = *i;
        if (counter->getIdentifier() == identifier)
            return counter;
    }
    CounterImpPtr counter = new(std::nothrow) CounterImp(identifier);
    if (counter)
        counterList.push_back(counter);
    return counter;
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
