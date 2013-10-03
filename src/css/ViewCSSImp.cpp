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
#include "DOMImplementationImp.h"
#include "MediaListImp.h"
#include "html/HTMLElementImp.h"
#include "html/HTMLTemplateElementImp.h"
#include "html/MediaQueryListImp.h"

#include "Box.h"
#include "Table.h"
#include "StackingContext.h"

#include "Test.util.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

namespace {

Block* getCurrentBox(const CSSStyleDeclarationPtr& style, bool asTablePart)
{
    Box* box = style->getBox();
    if (!box)
        return 0;
    if (!style->display.isTableParts() || asTablePart)
        return dynamic_cast<Block*>(box);

    // return the surrounding anonymous table wrapper box
    while (box && !dynamic_cast<TableWrapperBox*>(box))
        box = box->getParentBox();
    assert(box);
    return dynamic_cast<TableWrapperBox*>(box);
}

}

ViewCSSImp::ViewCSSImp(WindowPtr window) :
    window(window),
    dpi(96),
    zoom(1.0f),
    mutationListener(boost::bind(&ViewCSSImp::handleMutation, this, _1, _2)),
    mediaCheck(false),
    overflow(CSSOverflowValueImp::Auto),
    stackingContexts(0),
    quotingDepth(0),
    scrollWidth(0.0f),
    scrollHeight(0.0f),
    hoveredBox(0),
    last(0),
    delay(0)
{
    initialContainingBlock.retain_();
    setMediumFontSize(16);
    if (DocumentPtr document = getDocument()) {
        document->addEventListener(u"DOMAttrModified", mutationListener, false, EventTargetImp::UseDefault);
        document->addEventListener(u"DOMCharacterDataModified", mutationListener, false, EventTargetImp::UseDefault);
        document->addEventListener(u"DOMNodeInserted", mutationListener, false, EventTargetImp::UseDefault);
        document->addEventListener(u"DOMNodeRemoved", mutationListener, false, EventTargetImp::UseDefault);
    }
}

ViewCSSImp::~ViewCSSImp()
{
    if (DocumentPtr document = getDocument()) {
        document->removeEventListener(u"DOMAttrModified", mutationListener, false, EventTargetImp::UseDefault);
        document->removeEventListener(u"DOMCharacterDataModified", mutationListener, false, EventTargetImp::UseDefault);
        document->removeEventListener(u"DOMNodeInserted", mutationListener, false, EventTargetImp::UseDefault);
        document->removeEventListener(u"DOMNodeRemoved", mutationListener, false, EventTargetImp::UseDefault);
    }
}

Box* ViewCSSImp::boxFromPoint(int x, int y)
{
    if (stackingContexts) {
        x += window->getScrollX();
        y += window->getScrollY();
        if (Box* target = stackingContexts->boxFromPoint(x, y))
            return target;
    }
    return boxTree.get();
}

bool ViewCSSImp::isHovered(Element node)
{
    // TODO: Check if we need to process forefront node only or not.
    for (Node i = hovered; i; i = i.getParentNode()) {
        if (node == i)
            return true;
    }
    return false;
}

void ViewCSSImp::removeComputedStyle(Element element)
{
    if (CSSStyleDeclarationPtr style = getStyle(element)) {
        style->revert(element);
        map.erase(element);
    }
}

void ViewCSSImp::handleMutation(EventListenerImp* listener, events::Event event)
{
    if (!boxTree)
        return;

    events::MutationEvent mutation(interface_cast<events::MutationEvent>(event));
    if (mutation.getType() == u"DOMCharacterDataModified") {
        Node parentNode = interface_cast<Node>(mutation.getRelatedNode());
        if (Element::hasInstance(parentNode)) {
            Element element(interface_cast<Element>(parentNode));
            if (CSSStyleDeclarationPtr style = getStyle(element))
                style->updateInlines(element);
        }
        return;
    } else if (mutation.getType() == u"DOMNodeInserted") {
        Node parentNode = interface_cast<Node>(mutation.getRelatedNode());
        if (!Element::hasInstance(parentNode))
            return;
        Node target = interface_cast<Node>(event.getTarget());
        if (Element::hasInstance(target))
            setFlags(Box::NEED_SELECTOR_MATCHING);
        else if (Element::hasInstance(parentNode)) {
            Element element(interface_cast<Element>(parentNode));
            if (CSSStyleDeclarationPtr style = getStyle(element))
                style->updateInlines(element);
        }
        return;
    } else if (mutation.getType() == u"DOMNodeRemoved") {
        Node parentNode = interface_cast<Node>(mutation.getRelatedNode());
        if (!Element::hasInstance(parentNode))
            return;
        Node target = interface_cast<Node>(event.getTarget());
        if (Element::hasInstance(target)) {
            removeComputedStyle(interface_cast<Element>(target));
            setFlags(Box::NEED_SELECTOR_MATCHING);
        } else if (Element::hasInstance(parentNode)) {
            Element element(interface_cast<Element>(parentNode));
            if (CSSStyleDeclarationPtr style = getStyle(element))
                style->updateInlines(element);
        }
        return;
    } else if (mutation.getType() == u"DOMAttrModified") {
        Node target = interface_cast<Node>(event.getTarget());
        if (Element::hasInstance(target)) {
            if (CSSStyleDeclarationPtr style = getStyle(interface_cast<Element>(target))) {
                style->requestReconstruct(Box::NEED_STYLE_RECALCULATION);
                style->clearFlags(CSSStyleDeclarationImp::Computed);
                if (mutation.getAttrName() != u"style") {
                    // Request a selector re-matching for the element
                    style->setFlags(CSSStyleDeclarationImp::NeedSelectorMatching);
                    setFlags(Box::NEED_SELECTOR_MATCHING);
                }
            }
        }
        return;
    }

    setFlags(Box::NEED_SELECTOR_REMATCHING);
}

void ViewCSSImp::collectRules(CSSRuleListImp::RuleSet& set, Element element, css::CSSRuleList list, unsigned importance, MediaListPtr mediaList)
{
    auto ruleList = std::dynamic_pointer_cast<CSSRuleListImp>(list.self());
    if (!ruleList)
        return;
    ruleList->collectRules(set, this, element, importance, mediaList);
}

void ViewCSSImp::resolveXY(float left, float top)
{
    if (boxTree)
        boxTree->resolveXY(this, left, top, 0);
}

MediaQueryListPtr ViewCSSImp::matchMedia(const MediaListPtr& mediaList)
{
    if (!mediaList)
        return nullptr;
    auto found = mediaListMap.find(mediaList.get());
    if (found != mediaListMap.end())
        return found->second;
    if (auto mql = std::make_shared<MediaQueryListImp>(getWindow())) {
        mql->setMediaList(mediaList);
        mql->evaluate();
        mediaListMap.insert({ mediaList.get(), mql });
        return mql;
    }
    return nullptr;
}

void ViewCSSImp::addStyle(const Element& element, const CSSStyleDeclarationPtr& style)
{
    assert(element);
    assert(style);
    map[element] = style;
}

void ViewCSSImp::constructComputedStyles()
{
    constructComputedStyle(getDocument(), nullptr);
    clearFlags(Box::NEED_SELECTOR_MATCHING | Box::NEED_SELECTOR_REMATCHING);  // TODO: Refine
}

void ViewCSSImp::updateStyleRules(Element element, const CSSStyleDeclarationPtr& style, CSSStyleDeclarationPtr parentStyle)
{
#ifndef NDEBUG
    std::u16string tag(interface_cast<html::HTMLElement>(element).getTagName());
    std::u16string id(interface_cast<html::HTMLElement>(element).getId());
#endif

    CSSStyleDeclarationPtr elementDecl;
    html::HTMLElement htmlElement;
    if (html::HTMLElement::hasInstance(element)) {
        htmlElement = interface_cast<html::HTMLElement>(element);
        elementDecl = std::dynamic_pointer_cast<CSSStyleDeclarationImp>(htmlElement.getStyle().self());
    }

    if (auto sheet = getDOMImplementation()->getDefaultStyleSheet())
        collectRules(style->ruleSet, element, sheet->getCssRules(), CSSRuleListImp::UserAgent);
    if (auto sheet = getDOMImplementation()->getUserStyleSheet())
        collectRules(style->ruleSet, element, sheet->getCssRules(), CSSRuleListImp::User);
    if (elementDecl) {
        if (CSSStyleDeclarationPtr nonCSS = elementDecl->getPseudoElementStyle(CSSPseudoElementSelector::NonCSS)) {
            // TODO: emplace() seems to be not ready yet with libstdc++.
            CSSRuleListImp::PrioritizedRule rule(CSSRuleListImp::Presentational, nonCSS.get());
            style->ruleSet.insert(rule);
        }
    }
    if (auto sheet = getDOMImplementation()->getPresentationalHints())
        collectRules(style->ruleSet, element, sheet->getCssRules(), CSSRuleListImp::Presentational);

    unsigned importance = CSSRuleListImp::Author;
    stylesheets::StyleSheetList styleSheetList(getDocument()->getStyleSheets());
    for (unsigned i = 0; i < styleSheetList.getLength(); ++i) {
        CSSStyleSheetPtr sheet = std::dynamic_pointer_cast<CSSStyleSheetImp>(styleSheetList.getElement(i).self());
        MediaListPtr mediaList = std::dynamic_pointer_cast<MediaListImp>(sheet->getMedia().self());
        collectRules(style->ruleSet, element, sheet->getCssRules(), importance++, mediaList);
    }

    style->compute(this, parentStyle, element);
    if (parentStyle && htmlElement && htmlElement.getLocalName() == u"body") {
        parentStyle->bodyStyle = style;
        parentStyle->clearFlags(CSSStyleDeclarationImp::Computed);
    }

    // Set style->affectedBits
    if (!hoverList.empty()) {
        style->affectedBits |= 1u << CSSPseudoClassSelector::Hover;
        for (auto i = hoverList.begin(); i != hoverList.end(); ++i) {
            if (*i != element) {
                Element e(*i);
                if (CSSStyleDeclarationPtr s = getStyle(e))
                    s->affectedBits |= 1u << CSSPseudoClassSelector::Hover;
            }
        }
        hoverList.clear();
    }

    expandBinding(element, style);
    style->updateInlines(element); // TODO ???
    style->clearFlags(CSSStyleDeclarationImp::Computed);    // TODO: Only styles of children need to be recomputed
}

// Return true if its shadow tree is changed
bool ViewCSSImp::expandBinding(Element element, const CSSStyleDeclarationPtr& style)
{
    if (style->binding.getValue() == CSSBindingValueImp::None) {
        // TODO: detach the shadow tree from element (if any)
        return false;
    }

    html::HTMLTemplateElement shadowTree;
    auto imp = std::dynamic_pointer_cast<HTMLElementImp>(element.self());
    if (!imp)
        return false;
    if (!imp->generateShadowContent(style))
        return false;
    if (shadowTree = imp->getShadowTree()) {
        if (auto imp = std::dynamic_pointer_cast<HTMLTemplateElementImp>(shadowTree.self()))
            imp->setHost(element);
        return true;
    }
    return false;
}

unsigned ViewCSSImp::constructComputedStyle(Node node, CSSStyleDeclarationPtr parentStyle, unsigned propagetFlags)
{
    CSSStyleDeclarationPtr style;
    Element element((node.getNodeType() == Node::ELEMENT_NODE) ? interface_cast<Element>(node) : nullptr);
    if (element) {
        auto found = map.find(element);
        if (found != map.end()) {
            style = found->second;
            assert(style);
            if ((style->getFlags() | propagetFlags) & CSSStyleDeclarationImp::NeedSelectorMatching) {
                propagetFlags = CSSStyleDeclarationImp::NeedSelectorMatching;
                style->clearFlags(CSSStyleDeclarationImp::NeedSelectorMatching);
                CSSStyleDeclarationBoard board(style);
                style->resetComputedStyle();
                updateStyleRules(element, style, parentStyle);
                style->restoreComputedValues(board);
            } else if (expandBinding(element, style))
                style->updateInlines(element); // TODO ??
            if (!style->getStackingContext())
                style->computeStackingContext(this, parentStyle, false);
        } else {
            style = window->getComputedStyle(element);
            if (!style)
                return propagetFlags;  // TODO: error
            addStyle(element, style);
            updateStyleRules(element, style, parentStyle);
        }
        if (auto imp = std::dynamic_pointer_cast<HTMLElementImp>(element.self())) {
            if (html::HTMLTemplateElement shadow = imp->getShadowTree())
                node = shadow;
        }
    }
    unsigned siblingFlags = propagetFlags;
    for (Node child = node.getFirstChild(); child; child = child.getNextSibling())
        siblingFlags = constructComputedStyle(child, style, siblingFlags);
    return propagetFlags;
}

void ViewCSSImp::calculateComputedStyles()
{
    if (getWindow()->getMediaCheck()) {
        getWindow()->setMediaCheck(false);
        setMediaCheck(true);
    }
    CSSAutoNumberingValueImp::CounterContext counterContext(this);
    if (DocumentPtr document = getDocument()) {
        for (Node child = document->getFirstChild(); child; child = child.getNextSibling()) {
            if (child.getNodeType() == Node::ELEMENT_NODE)
                calculateComputedStyle(interface_cast<Element>(child), 0, &counterContext, 0);
        }
    }
    clearFlags(Box::NEED_STYLE_RECALCULATION);  // TODO: Refine
    setMediaCheck(false);
}

void ViewCSSImp::calculateComputedStyle(Element element, const CSSStyleDeclarationPtr& parentStyle, CSSAutoNumberingValueImp::CounterContext* counterContext, unsigned flags)
{
    assert(counterContext);

#ifndef NDEBUG
    std::u16string tag(interface_cast<html::HTMLElement>(element).getTagName());
    std::u16string id(interface_cast<html::HTMLElement>(element).getId());
#endif

    CSSStyleDeclarationPtr style = getStyle(element);
    if (!style)
        return;
    if (flags && (!parentStyle || parentStyle->bodyStyle.lock() != style))
        style->clearFlags(flags);

    // If the fundamental values such as 'display' are changed, the box(es) associated with the
    // style need to be reverted.
    if (!style->isComputed() || (getMediaCheck() && (style->flags & CSSStyleDeclarationImp::MediaDependent))) {
        CSSStyleDeclarationBoard board(style);
        style->compute(this, parentStyle, element);
        unsigned comp = board.compare(style);
        if (comp & Box::NEED_TABLE_REFLOW) {
            for (CSSStyleDeclarationPtr s = style; s; s = s->getParentStyle()) {
                Box* box = getCurrentBox(s, true);
                if (dynamic_cast<TableWrapperBox*>(box)) {
                    box->setFlags(Box::NEED_EXPANSION);
                    break;
                }
            }
        } else if (comp & Box::NEED_EXPANSION) {
            Block* block = style->revert(element);
            if (block) {
                if (style->display.isInlineLevel() && !(block->getFlags() & Box::NEED_EXPANSION))
                    block->clearInlines();
                else if (!style->display.isNone())
                    block->setFlags(Box::NEED_EXPANSION);
            }
        } else if (comp & Box::NEED_REFLOW) {
            if (Block* block = getCurrentBox(style, true)) {
                block->defaultBaseline = block->defaultLineHeight = 0.0f;
                block->setFlags(Box::NEED_REFLOW);
            } else
                style->updateInlines(element);
        } else if (comp & Box::NEED_REPOSITION) {
            if (Block* block = getCurrentBox(style, true))
                block->setFlags(Box::NEED_REPOSITION);
            // else 'position' is relative
        } else if (Block* block = getCurrentBox(style, true))
            block->resolveBackground(this);
        if (!parentStyle)
            overflow = style->overflow.getValue();
        flags |= CSSStyleDeclarationImp::Computed;  // The child styles have to be recomputed.
    }
    if (!style->getStackingContext())
        style->computeStackingContext(this, parentStyle, false);

    Element shadow = element;
    if (auto imp = std::dynamic_pointer_cast<HTMLElementImp>(element.self())) {
        if (imp->getShadowTree())
            shadow = imp->getShadowTree();
    }

    if (!style->display.isNone())
        style->updateCounters(this, counterContext);

    CSSAutoNumberingValueImp::CounterContext cc(this);
    ElementPtr imp(std::dynamic_pointer_cast<ElementImp>(shadow.self()));
    assert(imp);

    style->marker = updatePseudoElement(style, CSSPseudoElementSelector::Marker, shadow, style->marker, &cc);
    style->before = updatePseudoElement(style, CSSPseudoElementSelector::Before, shadow, style->before, &cc);
    for (Node child = shadow.getFirstChild(); child; child = child.getNextSibling()) {
        if (child.getNodeType() == Node::ELEMENT_NODE)
            calculateComputedStyle(interface_cast<Element>(child), style, &cc, flags);
    }
    style->after = updatePseudoElement(style, CSSPseudoElementSelector::After, shadow, style->after, &cc);

    style->emptyInline = 0;
    if (style->display.isInline() && !isReplacedElement(shadow)) {
        // Empty inline elements still have margins, padding, borders and a line height. cf. 10.8
        if (!shadow.hasChildNodes() && !style->marker && !style->before && !style->after)
            style->emptyInline = 4;
        else {
            if (style->marker || style->before)
                style->emptyInline = 1;
            else if (style->marginLeft.getPx() || style->borderLeftWidth.getPx() || style->paddingLeft.getPx()) {
                Node child = shadow.getFirstChild();
                if (child.getNodeType() != Node::TEXT_NODE)
                    style->emptyInline = 1;
            }
            if (style->after)
                style->emptyInline |= 2;
            else if (style->marginRight.getPx() || style->borderRightWidth.getPx() || style->paddingRight.getPx()) {
                Node child = shadow.getLastChild();
                if (child.getNodeType() != Node::TEXT_NODE)
                    style->emptyInline |= 2;
            }
        }
    }
}

Element ViewCSSImp::updatePseudoElement(const CSSStyleDeclarationPtr& style, int id, Element element, Element pseudoElement, CSSAutoNumberingValueImp::CounterContext* counterContext)
{
    if (style->display.isNone())
        return nullptr;
    CSSStyleDeclarationPtr pseudoStyle = style->getPseudoElementStyle(id);
    if (!pseudoStyle)
        return nullptr;
    pseudoStyle->compute(this, style, element);
    if (pseudoStyle->display.isNone() || pseudoStyle->content.isNone())
        return nullptr;

    pseudoStyle->updateCounters(this, counterContext);

    if (!pseudoElement)
        pseudoElement = pseudoStyle->content.eval(this, element, counterContext);
    else {
        // TODO: what to do if content has been changed
        std::u16string text = pseudoStyle->content.evalText(this, element, counterContext);
        if (!text.empty() && text != static_cast<std::u16string>(pseudoElement.getTextContent()))
            pseudoElement.setTextContent(text);
    }

    if (id != CSSPseudoElementSelector::Marker) {
        CSSAutoNumberingValueImp::CounterContext ccMarker(this);
        pseudoStyle->marker = updatePseudoElement(pseudoStyle, CSSPseudoElementSelector::Marker, pseudoElement, pseudoStyle->marker, &ccMarker);
    }

    return pseudoElement;
}

// In this step, neither inline-level boxes nor line boxes are generated.
// Those will be generated later by layOut().
Block* ViewCSSImp::constructBlock(Node node, Block* parentBox, const CSSStyleDeclarationPtr& style, Block* prevBox, bool asTablePart)
{
    Block* newBox = 0;
    switch (node.getNodeType()) {
    case Node::TEXT_NODE:
        newBox = constructBlock(interface_cast<Text>(node), parentBox, style, prevBox);
        break;
    case Node::ELEMENT_NODE:
        newBox = constructBlock(interface_cast<Element>(node), parentBox, style, 0, prevBox, asTablePart);
        break;
    case Node::DOCUMENT_NODE:
        for (Node child = node.getFirstChild(); child; child = child.getNextSibling()) {
            if (Block* box = constructBlock(child, parentBox, style, newBox))
                newBox = box;
        }
        break;
    default:
        break;
    }
    return newBox;
}

Block* ViewCSSImp::constructBlock(Text text, Block* parentBox, const CSSStyleDeclarationPtr& style, Block* prevBox)
{
    if (!parentBox || !style)
        return 0;

    bool discardable = true;
    if (style->display.isInline()) {
        Element element = interface_cast<Element>(text.getParentNode());
        assert(element);
        if (element.getFirstChild() == text && (style->marginLeft.getPx() || style->borderLeftWidth.getPx() || style->paddingLeft.getPx()) ||
            element.getLastChild() == text && (style->marginRight.getPx() || style->borderRightWidth.getPx() || style->paddingRight.getPx()))
            discardable = false;
    }

    if (!parentBox->hasChildBoxes()) {
        if (discardable && !parentBox->hasInline() && style->whiteSpace.isCollapsingSpace()) {
            std::u16string data = text.getData();
            if (data.length() <= style->processLineHeadWhiteSpace(data, 0))
                return 0;
        }
        parentBox->insertInline(text);
        return 0;
    }
    if (TableWrapperBox* table = dynamic_cast<TableWrapperBox*>(prevBox)) {
        if (table && table->isAnonymousTableObject()) {
            if (table->processTableChild(text, style))
                return 0;
            else {
                assert(parentBox->anonymousTable == table);
                table->constructBlocks();
                parentBox->anonymousTable = 0;
            }
        }
    }
    if (discardable && !style->display.isInline() && !parentBox->hasAnonymousBox(prevBox)) {
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
    if (Block* anonymousBox = parentBox->getAnonymousBox(prevBox)) {
        anonymousBox->insertInline(text);
        return anonymousBox;
    }
    return 0;
}

Block* ViewCSSImp::createBlock(Element element, Block* parentBox, const CSSStyleDeclarationPtr& style, bool newContext, bool asTablePart)
{
    assert(style);
    Block* block;
    if (style->display == CSSDisplayValueImp::Table || style->display == CSSDisplayValueImp::InlineTable) {
        block = new(std::nothrow) TableWrapperBox(this, element, style);
        newContext = true;
    } else if (style->display.isTableParts()) {
        if (asTablePart) {
            if (style->display == CSSDisplayValueImp::TableCell)
                block = new(std::nothrow) CellBox(element, style);
            else
                block = new(std::nothrow) Block(element, style);
        } else {
            assert(parentBox);  // cf. http://www.w3.org/TR/CSS21/visuren.html#dis-pos-flo
            if (parentBox->anonymousTable) {
                assert(parentBox->anonymousTable->isAnonymousTableObject());
                parentBox->anonymousTable->processTableChild(element, style);
                return 0;
            }
            parentBox->anonymousTable = new(std::nothrow) TableWrapperBox(this, element, style);
            block = parentBox->anonymousTable;
        }
        newContext = true;
    } else
        block = new(std::nothrow) Block(element, style);
    if (!block)
        return 0;
    if (newContext)
        block->establishFormattingContext();  // TODO: check error

    StackingContextPtr stackingContext = style->getStackingContext();
    assert(stackingContext);
    if (parentBox) {
        stackingContext->addBox(block, parentBox);
        parentBox->setFlags(Box::NEED_CHILD_REFLOW);
    }
    if (!parentBox || parentBox->anonymousTable != block)
        style->addBox(block);
    return block;
}

Block* ViewCSSImp::constructBlock(Element element, Block* parentBox, const CSSStyleDeclarationPtr& parentStyle, CSSStyleDeclarationPtr style, Block* prevBox, bool asTablePart)
{
#ifndef NDEBUG
    std::u16string tag(interface_cast<html::HTMLElement>(element).getTagName());
    std::u16string id(interface_cast<html::HTMLElement>(element).getId());
#endif

    if (!style)
        style = getStyle(element);
    if (!style)
        return 0;
    if (style->display.isNone())
        return 0;

    Element shadow = element;
    if (auto imp = std::dynamic_pointer_cast<HTMLElementImp>(element.self())) {
        if (imp->getShadowTree())
            shadow = imp->getShadowTree();
    }

    Block* currentBox = parentBox;
    bool anonInlineTable = !asTablePart && style->display.isTableParts() && parentStyle && parentStyle->display.isInlineLevel();
    bool inlineReplace = isReplacedElement(element) && !style->isBlockLevel();
    bool isFlowRoot = (!parentBox || anonInlineTable || inlineReplace) ? true : style->isFlowRoot();
    bool inlineBlock = anonInlineTable || style->isFloat() || style->isAbsolutelyPositioned() || style->isInlineBlock() || inlineReplace;
    if (asTablePart) {
        currentBox = getCurrentBox(style, true);
        if (!currentBox) {
            currentBox = createBlock(element, 0, style, isFlowRoot, true);
            if (!currentBox)
                return 0;
        }
    } else if (!parentBox || inlineBlock) {
        currentBox = getCurrentBox(style, false);
        if (!currentBox)
            currentBox = createBlock(element, parentBox, style, isFlowRoot, false);
        if (!currentBox)
            return 0;
        // Do not insert currentBox into parentBox. currentBox will be
        // inserted in a lineBox of parentBox later
        if (parentBox) {
            if (!currentBox->parentBox) {
                parentBox->addBlock(element, currentBox);
                // Set currentBox->parentBox to parentBox for now so that the correct
                // containing block can be retrieved before currentBox will be
                // inserted in a lineBox of parentBox later
                currentBox->parentBox = parentBox;
                if (!parentBox->hasChildBoxes())
                    parentBox->insertInline(element);
                else if (prevBox = parentBox->getAnonymousBox(prevBox))
                    prevBox->insertInline(element);
            } else {
                prevBox = dynamic_cast<Block*>(currentBox->parentBox->parentBox);
                if (prevBox == parentBox)
                    prevBox = 0;
                else
                    assert(!prevBox || prevBox->parentBox == parentBox ||
                           currentBox->parentBox == parentBox); // TODO: cf. html4/table-anonymous-objects-207.htm
            }
        }
    } else if (style->isBlockLevel()) {
        currentBox = getCurrentBox(style, false);
        if (!currentBox) {
            currentBox = createBlock(element, parentBox, style, isFlowRoot);
            if (!currentBox)
                return 0;
            if (parentBox) {
                if (parentBox->hasInline()) {
                    prevBox = parentBox->getAnonymousBox(prevBox);
                    if (!prevBox)
                        return 0;
                    assert(!parentBox->hasInline());
                }
                if (!prevBox)
                    parentBox->insertBefore(currentBox, parentBox->getFirstChild());
                else
                    parentBox->insertBefore(currentBox, prevBox->getNextSibling());
            }
        }
    }

    if (TableWrapperBox* table = dynamic_cast<TableWrapperBox*>(currentBox)) {
        switch (table->getFlags() & (Box::NEED_EXPANSION | Box::NEED_CHILD_EXPANSION)) {
        case Box::NEED_EXPANSION | Box::NEED_CHILD_EXPANSION:
        case Box::NEED_EXPANSION:
            table->clearGrid();
            if (table->isAnonymousTableObject()) {
                table->processTableChild(element, style);
            } else {
                // TODO: Process pseudo elements
                for (Node node = element.getFirstChild(); node; node = node.getNextSibling())
                    table->processTableChild(node, style);
                table->constructBlocks();
            }
            break;
        case Box::NEED_CHILD_EXPANSION:
            table->reconstructBlocks();
            // FALL THROUGH
        default:
            return (!parentBox || !inlineBlock) ? currentBox : prevBox;
        }
    } else {
        if (currentBox && currentBox != parentBox) {
            Block* prev = 0;
            switch (currentBox->flags & (Box::NEED_EXPANSION | Box::NEED_CHILD_EXPANSION)) {
            case Box::NEED_EXPANSION | Box::NEED_CHILD_EXPANSION:
            case Box::NEED_EXPANSION:
                // TODO: check pseudo elements
                if (!currentBox->inlines.empty()) {
                    currentBox->inlines.clear();
                    currentBox->removeChildren();
                } else {
                    Box* next;
                    for (Box* child = currentBox->getFirstChild(); child; child = next) {
                        next = child->getNextSibling();
                        if (child->isAnonymous() ||
                            child->getNode().getParentNode() != element)  // cf. html4/block-in-inline-remove-000.htm
                        {
                            currentBox->removeChild(child);
                            child->release_();
                        }
                    }
                }
                break;
            case Box::NEED_CHILD_EXPANSION:
                for (auto box = dynamic_cast<Block*>(currentBox->getFirstChild());
                     box;
                     prev = box, box = dynamic_cast<Block*>(box->getNextSibling()))
                {
                    if (box->flags & (Box::NEED_EXPANSION | Box::NEED_CHILD_EXPANSION)) {
                        if (box->getNode())
                            constructBlock(box->getNode(), currentBox, style, prev);
                    }
                }
                for (auto it = currentBox->blockMap.begin(); it != currentBox->blockMap.end(); ++it) {
                    if (it->second.get()->flags & (Box::NEED_EXPANSION | Box::NEED_CHILD_EXPANSION))
                        constructBlock(it->first, currentBox, style, 0);
                }
                currentBox->flags &= ~(Box::NEED_EXPANSION | Box::NEED_CHILD_EXPANSION);
                // FALL THROUGH
            default:
                return (!parentBox || !inlineBlock) ? currentBox : prevBox;
            }
        }

        Block* prev = (currentBox != parentBox) ? 0 : prevBox;

        if ((style->emptyInline & 1) || style->emptyInline == 4) {
            if (!currentBox->hasChildBoxes())
                currentBox->insertInline(element);
            else if (Block* anonymousBox = currentBox->getAnonymousBox(prev))
                anonymousBox->insertInline(element);
        }

        if (style->marker) {
            if (Block* box = constructBlock(style->marker, currentBox, style, style->getPseudoElementStyle(CSSPseudoElementSelector::Marker), prev))
                prev = box;
            // Deal with an empty list item; cf. list-alignment-001, acid2.
            // TODO: Find out where the exact behavior is defined in the specifications.
            if (style->height.isAuto() && !shadow.hasChildNodes() && !style->before && !style->after) {
                if (!currentBox->hasChildBoxes())
                    currentBox->insertInline(element);
                else if (Block* anonymousBox = currentBox->getAnonymousBox(prev))
                    anonymousBox->insertInline(element);
            }
        }
        if (style->before) {
            if (Block* box = constructBlock(style->before, currentBox, style, style->getPseudoElementStyle(CSSPseudoElementSelector::Before), prev))
                prev = box;
        }
        for (Node child = shadow.getFirstChild(); child; child = child.getNextSibling()) {
            if (Block* box = constructBlock(child, currentBox, style, prev))
                prev = box;
        }
        if (style->after) {
            if (Block* box = constructBlock(style->after, currentBox, style, style->getPseudoElementStyle(CSSPseudoElementSelector::After), prev))
                prev = box;
        }

        if (currentBox->anonymousTable && currentBox->anonymousTable->isAnonymousTableObject()) {
            currentBox->anonymousTable->constructBlocks();
            currentBox->anonymousTable = 0;
        }

        if (style->emptyInline & 2) {
            if (!currentBox->hasChildBoxes())
                currentBox->insertInline(element);
            else if (Block* anonymousBox = currentBox->getAnonymousBox(prev))
                anonymousBox->insertInline(element);
        }

        if (currentBox == parentBox)
            return prev;
    }

    if (!currentBox || currentBox == parentBox)
        return 0;

    currentBox->flags &= ~(Box::NEED_EXPANSION | Box::NEED_CHILD_EXPANSION);
    if (parentBox && parentBox->anonymousTable && currentBox != parentBox->anonymousTable) {
        if (parentBox->anonymousTable->isAnonymousTableObject())
            parentBox->anonymousTable->constructBlocks();
        parentBox->anonymousTable = 0;
    }
    return (!parentBox || !inlineBlock) ? currentBox : prevBox;
}

// Construct the render tree
Block* ViewCSSImp::constructBlocks()
{
    boxTree = constructBlock(getDocument(), 0, 0, 0, false);
    clearCounters();
    return boxTree.get();
}

Block* ViewCSSImp::layOut()
{
    quotingDepth = 0;
    scrollWidth = 0.0f;
    scrollHeight = 0.0f;

    if (!constructBlocks())
        return 0;

    // Expand line boxes and inline-level boxes in each block-level box
    if (!boxTree->isAbsolutelyPositioned()) {
        boxTree->layOut(this, 0);
        boxTree->resolveXY(this, 0.0f, 0.0f, 0);
    }

    if (stackingContexts) {
        if (!stackingContexts->getBase())
            stackingContexts->addBase(boxTree.get());
        stackingContexts->layOutAbsolute(this);
    }

    if (3 <= getLogLevel()) {
        printComputedValues(getDocument(), this);
        if (stackingContexts) {
            std::cout << "## stacking contexts\n";
            stackingContexts->dump();
        }
    }

    return boxTree.get();
}

Block* ViewCSSImp::dump()
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
    CounterImpPtr counter = std::make_shared<CounterImp>(identifier);
    if (counter)
        counterList.push_back(counter);
    return counter;
}

CSSStyleDeclarationPtr ViewCSSImp::getStyle(Element elt, Nullable<std::u16string> pseudoElt)
{
    if (!elt)
        return nullptr;
    auto i = map.find(elt);
    if (i == map.end()) {
        if (auto parent = std::dynamic_pointer_cast<ElementImp>(elt.getParentElement().self())) {
            auto i = map.find(parent);
            if (i != map.end()) {
                CSSStyleDeclarationPtr style = i->second;
                assert(style);
                if (style->marker == elt)
                    return style->getPseudoElementStyle(CSSPseudoElementSelector::Marker);
                if (style->before == elt)
                    return style->getPseudoElementStyle(CSSPseudoElementSelector::Before);
                if (style->after == elt)
                    return style->getPseudoElementStyle(CSSPseudoElementSelector::After);
                return nullptr;
            } else if (auto grandParent = std::dynamic_pointer_cast<ElementImp>(parent->getParentElement().self())) {
                auto i = map.find(grandParent);
                if (i != map.end()) {
                    CSSStyleDeclarationPtr style = i->second;
                    assert(style);
                    if (style->before.self() == parent)
                        return style->getPseudoElementStyle(CSSPseudoElementSelector::Before)->getPseudoElementStyle(CSSPseudoElementSelector::Marker);
                    if (style->after.self() == parent)
                        return style->getPseudoElementStyle(CSSPseudoElementSelector::After)->getPseudoElementStyle(CSSPseudoElementSelector::Marker);
                    return nullptr;
                }
            }
        }
        return nullptr;
    }
    if (!pseudoElt.hasValue() || pseudoElt.value().empty())
        return i->second;
    return i->second->getPseudoElementStyle(pseudoElt.value());
}

// ViewCSS
css::CSSStyleDeclaration ViewCSSImp::getComputedStyle(Element elt, Nullable<std::u16string> pseudoElt)
{
    return getStyle(elt, pseudoElt);
}

unsigned ViewCSSImp::getBackgroundColor()
{
    if (boxTree)
        return boxTree->backgroundColor;
    // cf. http://test.csswg.org/suites/css2.1/20110323/html4/root-box-003.htm
    if (DocumentPtr document = getDocument()) {
        if (CSSStyleDeclarationPtr style = getStyle(document->getDocumentElement()))
            return style->backgroundColor.getARGB();
    }
    return 0;
}

}}}}  // org::w3c::dom::bootstrap
