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
#include <org/w3c/dom/events/MutationEvent.h>
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
    mutationListener(boost::bind(&ViewCSSImp::handleMutation, this, _1)),
    defaultStyleSheet(defaultStyleSheet),
    userStyleSheet(userStyleSheet),
    stackingContexts(0),
    overflow(CSSOverflowValueImp::Auto),
    hovered(0),
    quotingDepth(0),
    scrollWidth(0.0f),
    scrollHeight(0.0f),
    hoveredBox(0),
    last(0),
    delay(0)
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

    delete stackingContexts;
}

Box* ViewCSSImp::boxFromPoint(int x, int y)
{
    if (!boxTree)
        return 0;
    x += window->getScrollX();
    y += window->getScrollY();
    if (Box* target = boxTree.get()->boxFromPoint(x, y))
        return target;
    return boxTree.get();
}

bool ViewCSSImp::isHovered(Node node)
{
    // TODO: Check if we need to process forefront node only or not.
    for (Node i = hovered; i; i = i.getParentNode()) {
        if (node == i)
            return true;
    }
    return false;
}

void ViewCSSImp::handleMutation(events::Event event)
{
    if (!boxTree)
        return;

    events::MutationEvent mutation(interface_cast<events::MutationEvent>(event));
    if (mutation.getType() == u"DOMCharacterDataModified") {
        Node parentNode = interface_cast<Node>(mutation.getRelatedNode());
        if (!Element::hasInstance(parentNode))
            return;
        Element element = interface_cast<Element>(parentNode);
        while (element) {
            CSSStyleDeclarationImp* style = getStyle(element);
            if (!style)
                return;
            switch (style->display.getValue()) {
            case CSSDisplayValueImp::Block:
            case CSSDisplayValueImp::ListItem:
            case CSSDisplayValueImp::Table:
                if (Box* box = style->getBox()) {
                    box->setFlags(Box::NEED_REFLOW);
                    return;
                }
                element = element.getParentElement();
                break;
            // TODO: Support inline block and table later
            // case CSSDisplayValueImp::InlineBlock:
            // case CSSDisplayValueImp::InlineTable:
            default:
                element = element.getParentElement();
                break;
            }
        }
    }

    boxTree->setFlags(Box::NEED_SELECTOR_REMATCHING);
}

void ViewCSSImp::findDeclarations(CSSRuleListImp::RuleSet& set, Element element, css::CSSRuleList list, unsigned importance)
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
    DocumentImp* document = dynamic_cast<DocumentImp*>(getDocument().self());
    if (!document)
        return;

    map.clear();
    delete stackingContexts;
    stackingContexts = 0;

    document->clearStyleSheets();
    html::HTMLHeadElement head = document->getHead();
    if (head) {
        for (auto element = head.getFirstElementChild(); element; element = element.getNextElementSibling()) {
            if (html::HTMLStyleElement::hasInstance(element)) {
                html::HTMLStyleElement styleElement = interface_cast<html::HTMLStyleElement>(element);
                stylesheets::StyleSheet styleSheet = styleElement.getSheet();
                document->addStyleSheet(styleSheet);
            } else if (html::HTMLLinkElement::hasInstance(element)) {
                html::HTMLLinkElement linkElement = interface_cast<html::HTMLLinkElement>(element);
                stylesheets::StyleSheet styleSheet = linkElement.getSheet();
                document->addStyleSheet(styleSheet);
            }
        }
    }

    CSSAutoNumberingValueImp::CounterContext cc(this);
    cascade(getDocument(), 0, &cc);

    if (3 <= getLogLevel())
        printComputedValues(document, this);
}

void ViewCSSImp::cascade(Node node, CSSStyleDeclarationImp* parentStyle, CSSAutoNumberingValueImp::CounterContext* counterContext)
{
    CSSStyleDeclarationImp* style = 0;
    if (node.getNodeType() == Node::ELEMENT_NODE) {
        Element element = interface_cast<Element>(node);

        style = new(std::nothrow) CSSStyleDeclarationImp;
        if (!style)
            return;  // TODO: error
        map[element] = style;

        CSSStyleDeclarationImp* elementDecl = 0;
        if (html::HTMLElement::hasInstance(element)) {
            html::HTMLElement htmlElement = interface_cast<html::HTMLElement>(element);
            elementDecl = dynamic_cast<CSSStyleDeclarationImp*>(htmlElement.getStyle().self());
        }

        if (CSSStyleSheetImp* sheet = dynamic_cast<CSSStyleSheetImp*>(defaultStyleSheet.self()))
            findDeclarations(style->ruleSet, element, sheet->getCssRules(), CSSRuleListImp::UserAgent);
        if (CSSStyleSheetImp* sheet = dynamic_cast<CSSStyleSheetImp*>(userStyleSheet.self()))
            findDeclarations(style->ruleSet, element, sheet->getCssRules(), CSSRuleListImp::User);
        if (elementDecl) {
            CSSStyleDeclarationImp* nonCSS = elementDecl->getPseudoElementStyle(CSSPseudoElementSelector::NonCSS);
            if (nonCSS) {
                // TODO: emplace() seems to be not ready yet with libstdc++.
                CSSRuleListImp::PrioritizedRule rule(CSSRuleListImp::User, nonCSS);
                style->ruleSet.insert(rule);
            }
        }
        unsigned importance = CSSRuleListImp::Author;
        stylesheets::StyleSheetList styleSheetList(getDocument().getStyleSheets());
        for (unsigned i = 0; i < styleSheetList.getLength(); ++i) {
            CSSStyleSheetImp* sheet = dynamic_cast<CSSStyleSheetImp*>(styleSheetList.getElement(i).self());
            findDeclarations(style->ruleSet, element, sheet->getCssRules(), importance++);
            // TODO: Check overflow of importance
        }

        style->compute(this, parentStyle, element);
        html::HTMLElement htmlElement(0);
        if (html::HTMLElement::hasInstance(element))
            htmlElement = interface_cast<html::HTMLElement>(element);
        if (parentStyle && htmlElement && htmlElement.getLocalName() == u"body")
            parentStyle->bodyStyle = style;

        // Set style->affectedBits
        if (!hoverList.empty()) {
            style->affectedBits |= 1u << CSSPseudoClassSelector::Hover;
            for (auto i = hoverList.begin(); i != hoverList.end(); ++i) {
                if (*i != element.self()) {
                    Element e(*i);
                    if (CSSStyleDeclarationImp* s = getStyle(e))
                        s->affectedBits |= 1u << CSSPseudoClassSelector::Hover;
                }
            }
            hoverList.clear();
        }

        // Expand binding
        if (style->binding.getValue() != CSSBindingValueImp::None) {
            if (HTMLElementImp* imp = dynamic_cast<HTMLElementImp*>(element.self())) {
                imp->generateShadowContent(style);
                if (html::HTMLTemplateElement shadowTree = imp->getShadowTree()) {
                    if (auto imp = dynamic_cast<HTMLTemplateElementImp*>(shadowTree.self()))
                        imp->setHost(element);
                    node = shadowTree;
                }
            }
        } // TODO: detach the shadow tree from element (if any)

        // Process pseudo elements:
        assert(counterContext);
        counterContext->update(style);
        CSSAutoNumberingValueImp::CounterContext cc(this);
        CSSStyleDeclarationImp* markerStyle = checkMarker(style, element, &cc);
        ElementImp* imp = dynamic_cast<ElementImp*>(element.self());
        assert(imp);
        CSSStyleDeclarationImp* beforeStyle = style->getPseudoElementStyle(CSSPseudoElementSelector::Before);
        if (beforeStyle) {
            beforeStyle->compute(this, style, element);
            if (style->display.isNone() || beforeStyle->display.isNone() || beforeStyle->content.isNone())
                beforeStyle = 0;
        }
        if (!beforeStyle)
            imp->before = 0;
        else {
            cc.update(beforeStyle);
            imp->before = beforeStyle->content.eval(this, element, &cc);
            CSSAutoNumberingValueImp::CounterContext ccBefore(this);
            checkMarker(beforeStyle, imp->before, &ccBefore);
        }

        for (Node child = node.getFirstChild(); child; child = child.getNextSibling())
            cascade(child, style, &cc);

        CSSStyleDeclarationImp* afterStyle = style->getPseudoElementStyle(CSSPseudoElementSelector::After);
        if (afterStyle) {
            afterStyle->compute(this, style, element);
            if (style->display.isNone() || afterStyle->display.isNone() || afterStyle->content.isNone())
                afterStyle = 0;
        }
        if (!afterStyle)
            imp->after = 0;
        else {
            cc.update(afterStyle);
            imp->after = afterStyle->content.eval(this, element, &cc);
            CSSAutoNumberingValueImp::CounterContext ccAfter(this);
            checkMarker(afterStyle, imp->after, &ccAfter);
        }

        style->emptyInline = 0;
        if (style->display.isInline() && !isReplacedElement(element)) {
            // Empty inline elements still have margins, padding, borders and a line height. cf. 10.8
            if (!node.hasChildNodes() && !markerStyle && !beforeStyle && !afterStyle)
                style->emptyInline = 4;
            else {
                if (markerStyle || beforeStyle)
                    style->emptyInline = 1;
                else if (style->marginLeft.getPx() || style->borderLeftWidth.getPx() || style->paddingLeft.getPx()) {
                    Node child = node.getFirstChild();
                    if (child.getNodeType() != Node::TEXT_NODE)
                        style->emptyInline = 1;
                }
                if (afterStyle)
                    style->emptyInline |= 2;
                else if (style->marginRight.getPx() || style->borderRightWidth.getPx() || style->paddingRight.getPx()) {
                    Node child = node.getLastChild();
                    if (child.getNodeType() != Node::TEXT_NODE)
                        style->emptyInline |= 2;
                }
            }
        }
    } else {
        for (Node child = node.getFirstChild(); child; child = child.getNextSibling())
            cascade(child, style, counterContext);
    }

    if (!parentStyle && style)
        overflow = style->overflow.getValue();
}

CSSStyleDeclarationImp* ViewCSSImp::checkMarker(CSSStyleDeclarationImp* style, Element& element, CSSAutoNumberingValueImp::CounterContext* counterContext)
{
    if (!style->display.isListItem() || style->display.isNone())
        return 0;
    ElementImp* imp = dynamic_cast<ElementImp*>(element.self());
    assert(imp);
    imp->marker = 0;
    CSSStyleDeclarationImp* markerStyle = style->getPseudoElementStyle(CSSPseudoElementSelector::Marker);
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
    if (markerStyle) {
        markerStyle->compute(this, style, element);
        if (markerStyle->display.isNone() || markerStyle->content.isNone())
            markerStyle = 0;
    }
    if (markerStyle) {
        // Execute implicit 'counter-increment: list-item;'
        CounterImpPtr counter = getCounter(u"list-item");
        if (counter)
            counter->increment(1);
        counterContext->update(markerStyle);
        imp->marker = markerStyle->content.eval(this, element, counterContext);
        assert(style->getPseudoElementStyle(CSSPseudoElementSelector::Marker));
    }
    return markerStyle;
}

// In this step, neither inline-level boxes nor line boxes are generated.
// Those will be generated later by layOut().
BlockLevelBox* ViewCSSImp::layOutBlockBoxes(Node node, BlockLevelBox* parentBox, CSSStyleDeclarationImp* style)
{
    BlockLevelBox* newBox = 0;
    switch (node.getNodeType()) {
    case Node::TEXT_NODE:
        newBox = layOutBlockBoxes(interface_cast<Text>(node), parentBox, style);
        break;
    case Node::ELEMENT_NODE:
        newBox = layOutBlockBoxes(interface_cast<Element>(node), parentBox, style, 0);
        break;
    case Node::DOCUMENT_NODE:
        for (Node child = node.getFirstChild(); child; child = child.getNextSibling()) {
            if (BlockLevelBox* box = layOutBlockBoxes(child, parentBox, style))
                newBox = box;
        }
        break;
    default:
        break;
    }
    return newBox;
}

BlockLevelBox* ViewCSSImp::layOutBlockBoxes(Text text, BlockLevelBox* parentBox, CSSStyleDeclarationImp* style)
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
                block = new(std::nothrow) BlockLevelBox(element, style);
        } else {
            if (parentBox && parentBox->anonymousTable) {
                assert(parentBox->anonymousTable->isAnonymousTableObject());
                parentBox->anonymousTable->processTableChild(element, style);
                return 0;
            }
            parentBox->anonymousTable = new(std::nothrow) TableWrapperBox(this, element, style);
            block = parentBox->anonymousTable;
        }
        newContext = true;
    } else
        block = new(std::nothrow) BlockLevelBox(element, style);
    if (!block)
        return 0;
    if (newContext)
        block->establishFormattingContext();  // TODO: check error

    StackingContext* stackingContext = style->getStackingContext();
    assert(stackingContext);
    if (parentBox)
        stackingContext->addBox(block, parentBox);

    return block;
}

BlockLevelBox* ViewCSSImp::layOutBlockBoxes(Element element, BlockLevelBox* parentBox, CSSStyleDeclarationImp* parentStyle, CSSStyleDeclarationImp* style, bool asBlock)
{
#ifndef NDEBUG
    std::u16string tag(interface_cast<html::HTMLElement>(element).getTagName());
#endif

    if (!style)
        style = map[element].get();
    if (!style)
        return 0;
    style->compute(this, parentStyle, element);
    if (style->display.isNone())
        return 0;
    style->clearBox();

    Element shadow = element;
    if (HTMLElementImp* imp = dynamic_cast<HTMLElementImp*>(element.self())) {
        if (imp->getShadowTree())
            shadow = imp->getShadowTree();
    }

    BlockLevelBox* currentBox = parentBox;
    bool anonInlineTable = style->display.isTableParts() && parentStyle && parentStyle->display.isInlineLevel();
    bool inlineReplace = isReplacedElement(element) && !style->isBlockLevel();
    bool isFlowRoot = (!parentBox || anonInlineTable || inlineReplace) ? true : style->isFlowRoot();
    if (!parentBox || anonInlineTable ||
        style->isFloat() || style->isAbsolutelyPositioned() || style->isInlineBlock() ||
        inlineReplace)
    {
        currentBox = createBlockLevelBox(element, parentBox, style, isFlowRoot, asBlock);
        if (!currentBox)
            return 0;  // TODO: error
        // Do not insert currentBox into parentBox. currentBox will be
        // inserted in a lineBox of parentBox later
    } else if (style->isBlockLevel() || asBlock) {
        if (parentBox->hasInline()) {
            BlockLevelBox* anon = parentBox->getAnonymousBox();
            if (!anon)
                return 0;
            assert(!parentBox->hasInline());
        }
        currentBox = createBlockLevelBox(element, parentBox, style, isFlowRoot, asBlock);
        if (!currentBox)
            return 0;
        parentBox->appendChild(currentBox);
    }

    if (!dynamic_cast<TableWrapperBox*>(currentBox)) {
        if ((style->emptyInline & 1) || style->emptyInline == 4) {
            if (!currentBox->hasChildBoxes())
                currentBox->insertInline(element);
            else if (BlockLevelBox* anonymousBox = currentBox->getAnonymousBox())
                anonymousBox->insertInline(element);
        }

        ElementImp* imp = dynamic_cast<ElementImp*>(element.self());
        if (imp->marker) {
            layOutBlockBoxes(imp->marker, currentBox, style, style->getPseudoElementStyle(CSSPseudoElementSelector::Marker));
            // Deal with an empty list item; cf. list-alignment-001, acid2.
            // TODO: Find out where the exact behavior is defined in the specifications.
            if (style->height.isAuto() && !shadow.hasChildNodes() && !imp->before && !imp->after) {
                if (!currentBox->hasChildBoxes())
                    currentBox->insertInline(element);
                else if (BlockLevelBox* anonymousBox = currentBox->getAnonymousBox())
                    anonymousBox->insertInline(element);
            }
        }
        if (imp->before)
            layOutBlockBoxes(imp->before, currentBox, style, style->getPseudoElementStyle(CSSPseudoElementSelector::Before));
        for (Node child = shadow.getFirstChild(); child; child = child.getNextSibling())
            layOutBlockBoxes(child, currentBox, style);
        if (imp->after)
            layOutBlockBoxes(imp->after, currentBox, style, style->getPseudoElementStyle(CSSPseudoElementSelector::After));

        if (currentBox->anonymousTable && currentBox->anonymousTable->isAnonymousTableObject()) {
            currentBox->anonymousTable->layOutBlockBoxes();
            currentBox->anonymousTable = 0;
        }

        if (style->emptyInline & 2) {
            if (!currentBox->hasChildBoxes())
                currentBox->insertInline(element);
            else if (BlockLevelBox* anonymousBox = currentBox->getAnonymousBox())
                anonymousBox->insertInline(element);
        }
    }

    if (!currentBox || currentBox == parentBox)
        return 0;
    if (parentBox) {
        if (parentBox->anonymousTable && currentBox != parentBox->anonymousTable) {
            if (parentBox->anonymousTable->isAnonymousTableObject())
                parentBox->anonymousTable->layOutBlockBoxes();
            parentBox->anonymousTable = 0;
        }
        if (!currentBox->parentBox) {
            parentBox->addBlock(element, currentBox);
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
    boxTree = layOutBlockBoxes(getDocument(), 0, 0);
    clearCounters();
    return boxTree.get();
}

BlockLevelBox* ViewCSSImp::layOut()
{
    if (boxTree && (boxTree->flags & Box::NEED_REFLOW))
        boxTree = 0;

    if (!boxTree && stackingContexts)
        stackingContexts->clearBase();

    quotingDepth = 0;
    scrollWidth = 0.0f;
    scrollHeight = 0.0f;

    if (!boxTree && !layOutBlockBoxes())
        return 0;

    // Expand line boxes and inline-level boxes in each block-level box
    if (!boxTree->isAbsolutelyPositioned()) {
        boxTree->layOut(this, 0);
        boxTree->resolveXY(this, 0.0f, 0.0f, 0);
    }

    if (stackingContexts) {
        stackingContexts->addBase(boxTree.get());
        stackingContexts->layOutAbsolute(this);
        if (3 <= getLogLevel()) {
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
    if (i == map.end()) {
        if (auto parent = dynamic_cast<ElementImp*>(elt.getParentElement().self())) {
            auto i = map.find(parent);
            if (i != map.end()) {
                CSSStyleDeclarationImp* style = i->second.get();
                assert(style);
                if (parent->marker == elt)
                    return style->getPseudoElementStyle(CSSPseudoElementSelector::Marker);
                if (parent->before == elt)
                    return style->getPseudoElementStyle(CSSPseudoElementSelector::Before);
                if (parent->after == elt)
                    return style->getPseudoElementStyle(CSSPseudoElementSelector::After);
                return 0;
            } else if (auto grandParent = dynamic_cast<ElementImp*>(parent->getParentElement().self())) {
                auto i = map.find(grandParent);
                if (i != map.end()) {
                    CSSStyleDeclarationImp* style = i->second.get();
                    assert(style);
                    if (grandParent->before.self() == parent)
                        return style->getPseudoElementStyle(CSSPseudoElementSelector::Before)->getPseudoElementStyle(CSSPseudoElementSelector::Marker);
                    if (grandParent->after.self() == parent)
                        return style->getPseudoElementStyle(CSSPseudoElementSelector::After)->getPseudoElementStyle(CSSPseudoElementSelector::Marker);
                    return 0;
                }
            }
        }
        return 0;
    }
    CSSStyleDeclarationImp* style = i->second.get();
    if (!pseudoElt.hasValue() || pseudoElt.value().length() == 0)
        return style;
    return style->getPseudoElementStyle(pseudoElt.value());
}

// ViewCSS
css::CSSStyleDeclaration ViewCSSImp::getComputedStyle(Element elt, Nullable<std::u16string> pseudoElt)
{
    return getStyle(elt, pseudoElt);
}

}}}}  // org::w3c::dom::bootstrap
