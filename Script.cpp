/*
 * Copyright 2011 Esrille Inc.
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

// #define DEBUG 1 // for JS_DumpHeap

#include <esjsapi.h>

#include "Test.util.h"

#include "ApplicationCacheImp.h"
#include "AttrImp.h"
#include "BarPropImp.h"
#include "BeforeUnloadEventImp.h"
#include "BlobImp.h"
#include "BooleanCallbackImp.h"
#include "CaretPositionImp.h"
#include "CharacterDataImp.h"
#include "ClientRectImp.h"
#include "ClientRectListImp.h"
#include "CommentImp.h"
#include "CompositionEventImp.h"
#include "CustomEventImp.h"
// #include "DOMConfigurationImp.h"
#include "DOMImplementationImp.h"
#include "DOMSettableTokenListImp.h"
#include "DOMStringListImp.h"
#include "DOMStringMapImp.h"
#include "DOMTokenListImp.h"
#include "DataTransferImp.h"
#include "DocumentFragmentImp.h"
#include "DocumentImp.h"
#include "DocumentRangeImp.h"
#include "DocumentTraversalImp.h"
#include "DocumentTypeImp.h"
#include "DragEventImp.h"
#include "ElementImp.h"
#include "EventImp.h"
#include "EventListenerImp.h"
#include "EventSourceImp.h"
#include "EventTargetImp.h"
#include "FocusEventImp.h"
#include "FunctionImp.h"
#include "HashChangeEventImp.h"
#include "HistoryImp.h"
#include "KeyboardEventImp.h"
#include "LinkStyleImp.h"
#include "LocationImp.h"
#include "MediaErrorImp.h"
#include "MediaListImp.h"
#include "MessageChannelImp.h"
#include "MessageEventImp.h"
#include "MessagePortImp.h"
#include "MouseEventImp.h"
#include "MutationEventImp.h"
#include "MutationNameEventImp.h"
// #include "NameListImp.h"
#include "NavigatorImp.h"
#include "NodeFilterImp.h"
#include "NodeImp.h"
#include "NodeIteratorImp.h"
#include "NodeListImp.h"
#include "ObjectArrayImp.h"
#include "PageTransitionEventImp.h"
#include "PopStateEventImp.h"
#include "ProcessingInstructionImp.h"
#include "ProgressEventImp.h"
#include "PropertyNodeListImp.h"
#include "RangeImp.h"
#include "ScreenImp.h"
#include "SelectionImp.h"
#include "StyleMediaImp.h"
#include "StyleSheetImp.h"
#include "TextEventImp.h"
#include "TextImp.h"
#include "TreeWalkerImp.h"
#include "UIEventImp.h"
#include "UndoManagerEventImp.h"
#include "UndoManagerImp.h"
#include "WheelEventImp.h"
#include "WindowImp.h"

#include "css/CSS2PropertiesImp.h"
#include "css/CSSCharsetRuleImp.h"
#include "css/CSSColorComponentValueImp.h"
#include "css/CSSComponentValueImp.h"
#include "css/CSSFontFaceRuleImp.h"
#include "css/CSSIdentifierComponentValueImp.h"
#include "css/CSSImportRuleImp.h"
#include "css/CSSKeywordComponentValueImp.h"
#include "css/CSSLengthComponentValueImp.h"
#include "css/CSSMapValueImp.h"
#include "css/CSSMediaRuleImp.h"
#include "css/CSSNamespaceRuleImp.h"
#include "css/CSSPageRuleImp.h"
#include "css/CSSPercentageComponentValueImp.h"
#include "css/CSSPrimitiveValueImp.h"
#include "css/CSSPropertyValueImp.h"
#include "css/CSSPropertyValueListImp.h"
#include "css/CSSRuleImp.h"
#include "css/CSSStringComponentValueImp.h"
#include "css/CSSStyleDeclarationImp.h"
#include "css/CSSStyleDeclarationValueImp.h"
#include "css/CSSStyleRuleImp.h"
#include "css/CSSStyleSheetImp.h"
#include "css/CSSURLComponentValueImp.h"
#include "css/CSSUnknownRuleImp.h"
#include "css/CSSValueImp.h"
#include "css/CSSValueListImp.h"
#include "css/CounterImp.h"
#include "css/DOMImplementationCSSImp.h"
#include "css/DocumentCSSImp.h"
#include "css/ElementCSSInlineStyleImp.h"
#include "css/RGBColorImp.h"
#include "css/RectImp.h"
#include "css/ViewCSSImp.h"

#include "html/CanvasGradientImp.h"
#include "html/CanvasPatternImp.h"
#include "html/CanvasPixelArrayImp.h"
#include "html/CanvasRenderingContext2DImp.h"
#include "html/HTMLAllCollectionImp.h"
#include "html/HTMLAnchorElementImp.h"
#include "html/HTMLAppletElementImp.h"
#include "html/HTMLAreaElementImp.h"
#include "html/HTMLAudioElementImp.h"
#include "html/HTMLBRElementImp.h"
#include "html/HTMLBaseElementImp.h"
#include "html/HTMLBaseFontElementImp.h"
#include "html/HTMLBodyElementImp.h"
#include "html/HTMLButtonElementImp.h"
#include "html/HTMLCanvasElementImp.h"
#include "html/HTMLCollectionImp.h"
#include "html/HTMLCommandElementImp.h"
#include "html/HTMLDListElementImp.h"
#include "html/HTMLDataListElementImp.h"
#include "html/HTMLDetailsElementImp.h"
#include "html/HTMLDirectoryElementImp.h"
#include "html/HTMLDivElementImp.h"
#include "html/HTMLDocumentImp.h"
#include "html/HTMLElementImp.h"
#include "html/HTMLEmbedElementImp.h"
#include "html/HTMLFieldSetElementImp.h"
#include "html/HTMLFontElementImp.h"
#include "html/HTMLFormControlsCollectionImp.h"
#include "html/HTMLFormElementImp.h"
#include "html/HTMLFrameElementImp.h"
#include "html/HTMLFrameSetElementImp.h"
#include "html/HTMLHRElementImp.h"
#include "html/HTMLHeadElementImp.h"
#include "html/HTMLHeadingElementImp.h"
#include "html/HTMLHtmlElementImp.h"
#include "html/HTMLIFrameElementImp.h"
#include "html/HTMLImageElementImp.h"
#include "html/HTMLInputElementImp.h"
#include "html/HTMLKeygenElementImp.h"
#include "html/HTMLLIElementImp.h"
#include "html/HTMLLabelElementImp.h"
#include "html/HTMLLegendElementImp.h"
#include "html/HTMLLinkElementImp.h"
#include "html/HTMLMapElementImp.h"
#include "html/HTMLMarqueeElementImp.h"
#include "html/HTMLMediaElementImp.h"
#include "html/HTMLMenuElementImp.h"
#include "html/HTMLMetaElementImp.h"
#include "html/HTMLMeterElementImp.h"
#include "html/HTMLModElementImp.h"
#include "html/HTMLOListElementImp.h"
#include "html/HTMLObjectElementImp.h"
#include "html/HTMLOptGroupElementImp.h"
#include "html/HTMLOptionElementImp.h"
#include "html/HTMLOptionsCollectionImp.h"
#include "html/HTMLOutputElementImp.h"
#include "html/HTMLParagraphElementImp.h"
#include "html/HTMLParamElementImp.h"
#include "html/HTMLPreElementImp.h"
#include "html/HTMLProgressElementImp.h"
#include "html/HTMLPropertiesCollectionImp.h"
#include "html/HTMLQuoteElementImp.h"
#include "html/HTMLScriptElementImp.h"
#include "html/HTMLSelectElementImp.h"
#include "html/HTMLSourceElementImp.h"
#include "html/HTMLSpanElementImp.h"
#include "html/HTMLStyleElementImp.h"
#include "html/HTMLTableCaptionElementImp.h"
#include "html/HTMLTableCellElementImp.h"
#include "html/HTMLTableColElementImp.h"
#include "html/HTMLTableDataCellElementImp.h"
#include "html/HTMLTableElementImp.h"
#include "html/HTMLTableHeaderCellElementImp.h"
#include "html/HTMLTableRowElementImp.h"
#include "html/HTMLTableSectionElementImp.h"
#include "html/HTMLTextAreaElementImp.h"
#include "html/HTMLTimeElementImp.h"
#include "html/HTMLTitleElementImp.h"
#include "html/HTMLUListElementImp.h"
#include "html/HTMLUnknownElementImp.h"
#include "html/HTMLVideoElementImp.h"
#include "html/ImageDataImp.h"
#include "html/RadioNodeListImp.h"
#include "html/TextMetricsImp.h"
#include "html/TimeRangesImp.h"
#include "html/ValidityStateImp.h"

#include "xhr/AnonXMLHttpRequestImp.h"
#include "xhr/FormDataImp.h"
#include "xhr/XMLHttpRequestEventTargetImp.h"
#include "xhr/XMLHttpRequestImp.h"
#include "xhr/XMLHttpRequestUploadImp.h"

#include "file/FileErrorImp.h"
#include "file/FileImp.h"
#include "file/FileListImp.h"
#include "file/FileReaderImp.h"
#include "file/FileReaderSyncImp.h"

using namespace org::w3c::dom::bootstrap;
using namespace org::w3c::dom;

// JSAPI bridge

namespace {

JSClass globalClass = {
    "global", JSCLASS_GLOBAL_FLAGS | JSCLASS_HAS_PRIVATE,
    JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_StrictPropertyStub,
    JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, JS_FinalizeStub,
    JSCLASS_NO_OPTIONAL_MEMBERS
};

void reportError(JSContext* cx, const char* message, JSErrorReport* report)
{
    std::cout << (report->filename ? report->filename : "<no filename>") << report->lineno << message;
}

void registerClasses(JSContext* cx)
{
    // prototype roots
    AttrImp::setStaticPrivate(new NativeClass(cx, Attr::getMetaData()));
    CSSRuleImp::setStaticPrivate(new NativeClass(cx, css::CSSRule::getMetaData()));
    CSSStyleDeclarationImp::setStaticPrivate(new NativeClass(cx, css::CSSStyleDeclaration::getMetaData()));
    DOMImplementationImp::setStaticPrivate(new NativeClass(cx, DOMImplementation::getMetaData()));
    EventImp::setStaticPrivate(new NativeClass(cx, events::Event::getMetaData()));
    EventTargetImp::setStaticPrivate(new NativeClass(cx, events::EventTarget::getMetaData()));
    LocationImp::setStaticPrivate(new NativeClass(cx, html::Location::getMetaData()));
    MediaListImp::setStaticPrivate(new NativeClass(cx, stylesheets::MediaList::getMetaData()));
    NodeListImp::setStaticPrivate(new NativeClass(cx, NodeList::getMetaData()));
    StyleSheetImp::setStaticPrivate(new NativeClass(cx, stylesheets::StyleSheet::getMetaData()));

    // dom
    ApplicationCacheImp::setStaticPrivate(new NativeClass(cx, ApplicationCacheImp::getMetaData()));
    BarPropImp::setStaticPrivate(new NativeClass(cx, BarPropImp::getMetaData()));
    BlobImp::setStaticPrivate(new NativeClass(cx, BlobImp::getMetaData()));
    BooleanCallbackImp::setStaticPrivate(new NativeClass(cx, BooleanCallbackImp::getMetaData()));
    CanvasGradientImp::setStaticPrivate(new NativeClass(cx, CanvasGradientImp::getMetaData()));
    CanvasPatternImp::setStaticPrivate(new NativeClass(cx, CanvasPatternImp::getMetaData()));
    CanvasPixelArrayImp::setStaticPrivate(new NativeClass(cx, CanvasPixelArrayImp::getMetaData()));
    CanvasRenderingContext2DImp::setStaticPrivate(new NativeClass(cx, CanvasRenderingContext2DImp::getMetaData()));
    CaretPositionImp::setStaticPrivate(new NativeClass(cx, CaretPositionImp::getMetaData()));
    ClientRectImp::setStaticPrivate(new NativeClass(cx, ClientRectImp::getMetaData()));
    ClientRectListImp::setStaticPrivate(new NativeClass(cx, ClientRectListImp::getMetaData()));
    CounterImp::setStaticPrivate(new NativeClass(cx, CounterImp::getMetaData()));
    // DOMConfigurationImp::setStaticPrivate(new NativeClass(cx, DOMConfigurationImp::getMetaData()));
    DOMStringListImp::setStaticPrivate(new NativeClass(cx, DOMStringListImp::getMetaData()));
    DOMStringMapImp::setStaticPrivate(new NativeClass(cx, DOMStringMapImp::getMetaData()));
    DataTransferImp::setStaticPrivate(new NativeClass(cx, DataTransferImp::getMetaData()));
    DocumentRangeImp::setStaticPrivate(new NativeClass(cx, DocumentRangeImp::getMetaData()));
    DocumentTraversalImp::setStaticPrivate(new NativeClass(cx, DocumentTraversalImp::getMetaData()));
    ElementCSSInlineStyleImp::setStaticPrivate(new NativeClass(cx, ElementCSSInlineStyleImp::getMetaData()));
    EventListenerImp::setStaticPrivate(new NativeClass(cx, EventListenerImp::getMetaData()));
    EventSourceImp::setStaticPrivate(new NativeClass(cx, EventSourceImp::getMetaData()));
    FormDataImp::setStaticPrivate(new NativeClass(cx, FormDataImp::getMetaData()));
    HistoryImp::setStaticPrivate(new NativeClass(cx, HistoryImp::getMetaData()));
    ImageDataImp::setStaticPrivate(new NativeClass(cx, ImageDataImp::getMetaData()));
    LinkStyleImp::setStaticPrivate(new NativeClass(cx, LinkStyleImp::getMetaData()));
    MediaErrorImp::setStaticPrivate(new NativeClass(cx, MediaErrorImp::getMetaData()));
    MessageChannelImp::setStaticPrivate(new NativeClass(cx, MessageChannelImp::getMetaData()));
    MessagePortImp::setStaticPrivate(new NativeClass(cx, MessagePortImp::getMetaData()));
    // NameListImp::setStaticPrivate(new NativeClass(cx, NameListImp::getMetaData()));
    NavigatorImp::setStaticPrivate(new NativeClass(cx, NavigatorImp::getMetaData()));
    NodeFilterImp::setStaticPrivate(new NativeClass(cx, NodeFilterImp::getMetaData()));
    NodeIteratorImp::setStaticPrivate(new NativeClass(cx, NodeIteratorImp::getMetaData()));
    PropertyNodeListImp::setStaticPrivate(new NativeClass(cx, PropertyNodeListImp::getMetaData()));
    RGBColorImp::setStaticPrivate(new NativeClass(cx, RGBColorImp::getMetaData()));
    RadioNodeListImp::setStaticPrivate(new NativeClass(cx, RadioNodeListImp::getMetaData()));
    RangeImp::setStaticPrivate(new NativeClass(cx, RangeImp::getMetaData()));
    RectImp::setStaticPrivate(new NativeClass(cx, RectImp::getMetaData()));
    ScreenImp::setStaticPrivate(new NativeClass(cx, ScreenImp::getMetaData()));
    SelectionImp::setStaticPrivate(new NativeClass(cx, SelectionImp::getMetaData()));
    StyleMediaImp::setStaticPrivate(new NativeClass(cx, StyleMediaImp::getMetaData()));
    TextMetricsImp::setStaticPrivate(new NativeClass(cx, TextMetricsImp::getMetaData()));
    TimeRangesImp::setStaticPrivate(new NativeClass(cx, TimeRangesImp::getMetaData()));
    TreeWalkerImp::setStaticPrivate(new NativeClass(cx, TreeWalkerImp::getMetaData()));
    // URLImp::setStaticPrivate(new NativeClass(cx, URLImp::getMetaData()));
    UndoManagerImp::setStaticPrivate(new NativeClass(cx, UndoManagerImp::getMetaData()));
    ValidityStateImp::setStaticPrivate(new NativeClass(cx, ValidityStateImp::getMetaData()));

    DOMTokenListImp::setStaticPrivate(new NativeClass(cx, DOMTokenListImp::getMetaData()));
    DOMSettableTokenListImp::setStaticPrivate(new NativeClass(cx, DOMSettableTokenListImp::getMetaData()));

    // window
    WindowImp::setStaticPrivate(new NativeClass(cx, html::Window::getMetaData()));

    // node
    NodeImp::setStaticPrivate(new NativeClass(cx, Node::getMetaData()));
    CharacterDataImp::setStaticPrivate(new NativeClass(cx, CharacterData::getMetaData()));
    DocumentImp::setStaticPrivate(new NativeClass(cx, Document::getMetaData()));
    DocumentFragmentImp::setStaticPrivate(new NativeClass(cx, DocumentFragmentImp::getMetaData()));
    DocumentTypeImp::setStaticPrivate(new NativeClass(cx, DocumentType::getMetaData()));
    ElementImp::setStaticPrivate(new NativeClass(cx, Element::getMetaData()));
    ProcessingInstructionImp::setStaticPrivate(new NativeClass(cx, ProcessingInstructionImp::getMetaData()));

    // event
    UIEventImp::setStaticPrivate(new NativeClass(cx, events::UIEvent::getMetaData()));
    MouseEventImp::setStaticPrivate(new NativeClass(cx, MouseEventImp::getMetaData()));
    DragEventImp::setStaticPrivate(new NativeClass(cx, DragEventImp::getMetaData()));
    MutationEventImp::setStaticPrivate(new NativeClass(cx, MutationEventImp::getMetaData()));
    PageTransitionEventImp::setStaticPrivate(new NativeClass(cx, PageTransitionEventImp::getMetaData()));
    PopStateEventImp::setStaticPrivate(new NativeClass(cx, PopStateEventImp::getMetaData()));
    ProgressEventImp::setStaticPrivate(new NativeClass(cx, ProgressEventImp::getMetaData()));
    TextEventImp::setStaticPrivate(new NativeClass(cx, TextEventImp::getMetaData()));
    UndoManagerEventImp::setStaticPrivate(new NativeClass(cx, UndoManagerEventImp::getMetaData()));
    WheelEventImp::setStaticPrivate(new NativeClass(cx, WheelEventImp::getMetaData()));
    BeforeUnloadEventImp::setStaticPrivate(new NativeClass(cx, BeforeUnloadEventImp::getMetaData()));
    CompositionEventImp::setStaticPrivate(new NativeClass(cx, CompositionEventImp::getMetaData()));
    CustomEventImp::setStaticPrivate(new NativeClass(cx, CustomEventImp::getMetaData()));
    FocusEventImp::setStaticPrivate(new NativeClass(cx, FocusEventImp::getMetaData()));
    HashChangeEventImp::setStaticPrivate(new NativeClass(cx, HashChangeEventImp::getMetaData()));
    KeyboardEventImp::setStaticPrivate(new NativeClass(cx, KeyboardEventImp::getMetaData()));
    MessageEventImp::setStaticPrivate(new NativeClass(cx, MessageEventImp::getMetaData()));
    MutationNameEventImp::setStaticPrivate(new NativeClass(cx, MutationNameEventImp::getMetaData()));

    // character data
    CommentImp::setStaticPrivate(new NativeClass(cx, Comment::getMetaData()));
    TextImp::setStaticPrivate(new NativeClass(cx, Text::getMetaData()));

    // html element
    HTMLElementImp::setStaticPrivate(new NativeClass(cx, HTMLElementImp::getMetaData()));
    HTMLAnchorElementImp::setStaticPrivate(new NativeClass(cx, HTMLAnchorElementImp::getMetaData()));
    HTMLAppletElementImp::setStaticPrivate(new NativeClass(cx, HTMLAppletElementImp::getMetaData()));
    HTMLAreaElementImp::setStaticPrivate(new NativeClass(cx, HTMLAreaElementImp::getMetaData()));
    HTMLBRElementImp::setStaticPrivate(new NativeClass(cx, HTMLBRElementImp::getMetaData()));
    HTMLBaseElementImp::setStaticPrivate(new NativeClass(cx, HTMLBaseElementImp::getMetaData()));
    HTMLBaseFontElementImp::setStaticPrivate(new NativeClass(cx, HTMLBaseFontElementImp::getMetaData()));
    HTMLBodyElementImp::setStaticPrivate(new NativeClass(cx, HTMLBodyElementImp::getMetaData()));
    HTMLButtonElementImp::setStaticPrivate(new NativeClass(cx, HTMLButtonElementImp::getMetaData()));
    HTMLCanvasElementImp::setStaticPrivate(new NativeClass(cx, HTMLCanvasElementImp::getMetaData()));
    HTMLCommandElementImp::setStaticPrivate(new NativeClass(cx, HTMLCommandElementImp::getMetaData()));
    HTMLDListElementImp::setStaticPrivate(new NativeClass(cx, HTMLDListElementImp::getMetaData()));
    HTMLDataListElementImp::setStaticPrivate(new NativeClass(cx, HTMLDataListElementImp::getMetaData()));
    HTMLDetailsElementImp::setStaticPrivate(new NativeClass(cx, HTMLDetailsElementImp::getMetaData()));
    HTMLDirectoryElementImp::setStaticPrivate(new NativeClass(cx, HTMLDirectoryElementImp::getMetaData()));
    HTMLDivElementImp::setStaticPrivate(new NativeClass(cx, HTMLDivElementImp::getMetaData()));
    HTMLDocumentImp::setStaticPrivate(new NativeClass(cx, HTMLDocumentImp::getMetaData()));
    HTMLEmbedElementImp::setStaticPrivate(new NativeClass(cx, HTMLEmbedElementImp::getMetaData()));
    HTMLFieldSetElementImp::setStaticPrivate(new NativeClass(cx, HTMLFieldSetElementImp::getMetaData()));
    HTMLFontElementImp::setStaticPrivate(new NativeClass(cx, HTMLFontElementImp::getMetaData()));
    HTMLFormElementImp::setStaticPrivate(new NativeClass(cx, HTMLFormElementImp::getMetaData()));
    HTMLFrameElementImp::setStaticPrivate(new NativeClass(cx, HTMLFrameElementImp::getMetaData()));
    HTMLFrameSetElementImp::setStaticPrivate(new NativeClass(cx, HTMLFrameSetElementImp::getMetaData()));
    HTMLHRElementImp::setStaticPrivate(new NativeClass(cx, HTMLHRElementImp::getMetaData()));
    HTMLHeadElementImp::setStaticPrivate(new NativeClass(cx, HTMLHeadElementImp::getMetaData()));
    HTMLHeadingElementImp::setStaticPrivate(new NativeClass(cx, HTMLHeadingElementImp::getMetaData()));
    HTMLHtmlElementImp::setStaticPrivate(new NativeClass(cx, HTMLHtmlElementImp::getMetaData()));
    HTMLIFrameElementImp::setStaticPrivate(new NativeClass(cx, HTMLIFrameElementImp::getMetaData()));
    HTMLImageElementImp::setStaticPrivate(new NativeClass(cx, HTMLImageElementImp::getMetaData()));
    HTMLInputElementImp::setStaticPrivate(new NativeClass(cx, HTMLInputElementImp::getMetaData()));
    HTMLKeygenElementImp::setStaticPrivate(new NativeClass(cx, HTMLKeygenElementImp::getMetaData()));
    HTMLLIElementImp::setStaticPrivate(new NativeClass(cx, HTMLLIElementImp::getMetaData()));
    HTMLLabelElementImp::setStaticPrivate(new NativeClass(cx, HTMLLabelElementImp::getMetaData()));
    HTMLLegendElementImp::setStaticPrivate(new NativeClass(cx, HTMLLegendElementImp::getMetaData()));
    HTMLLinkElementImp::setStaticPrivate(new NativeClass(cx, HTMLLinkElementImp::getMetaData()));
    HTMLMapElementImp::setStaticPrivate(new NativeClass(cx, HTMLMapElementImp::getMetaData()));
    HTMLMarqueeElementImp::setStaticPrivate(new NativeClass(cx, HTMLMarqueeElementImp::getMetaData()));
    HTMLMenuElementImp::setStaticPrivate(new NativeClass(cx, HTMLMenuElementImp::getMetaData()));
    HTMLMetaElementImp::setStaticPrivate(new NativeClass(cx, HTMLMetaElementImp::getMetaData()));
    HTMLMeterElementImp::setStaticPrivate(new NativeClass(cx, HTMLMeterElementImp::getMetaData()));
    HTMLModElementImp::setStaticPrivate(new NativeClass(cx, HTMLModElementImp::getMetaData()));
    HTMLOListElementImp::setStaticPrivate(new NativeClass(cx, HTMLOListElementImp::getMetaData()));
    HTMLObjectElementImp::setStaticPrivate(new NativeClass(cx, HTMLObjectElementImp::getMetaData()));
    HTMLOptGroupElementImp::setStaticPrivate(new NativeClass(cx, HTMLOptGroupElementImp::getMetaData()));
    HTMLOptionElementImp::setStaticPrivate(new NativeClass(cx, HTMLOptionElementImp::getMetaData()));
    HTMLOutputElementImp::setStaticPrivate(new NativeClass(cx, HTMLOutputElementImp::getMetaData()));
    HTMLParagraphElementImp::setStaticPrivate(new NativeClass(cx, HTMLParagraphElementImp::getMetaData()));
    HTMLParamElementImp::setStaticPrivate(new NativeClass(cx, HTMLParamElementImp::getMetaData()));
    HTMLPreElementImp::setStaticPrivate(new NativeClass(cx, HTMLPreElementImp::getMetaData()));
    HTMLProgressElementImp::setStaticPrivate(new NativeClass(cx, HTMLProgressElementImp::getMetaData()));
    HTMLQuoteElementImp::setStaticPrivate(new NativeClass(cx, HTMLQuoteElementImp::getMetaData()));
    HTMLScriptElementImp::setStaticPrivate(new NativeClass(cx, HTMLScriptElementImp::getMetaData()));
    HTMLSelectElementImp::setStaticPrivate(new NativeClass(cx, HTMLSelectElementImp::getMetaData()));
    HTMLSourceElementImp::setStaticPrivate(new NativeClass(cx, HTMLSourceElementImp::getMetaData()));
    HTMLSpanElementImp::setStaticPrivate(new NativeClass(cx, HTMLSpanElementImp::getMetaData()));
    HTMLStyleElementImp::setStaticPrivate(new NativeClass(cx, HTMLStyleElementImp::getMetaData()));
    HTMLTableCaptionElementImp::setStaticPrivate(new NativeClass(cx, HTMLTableCaptionElementImp::getMetaData()));
    HTMLTableCellElementImp::setStaticPrivate(new NativeClass(cx, HTMLTableCellElementImp::getMetaData()));
    HTMLTableColElementImp::setStaticPrivate(new NativeClass(cx, HTMLTableColElementImp::getMetaData()));
    HTMLTableDataCellElementImp::setStaticPrivate(new NativeClass(cx, HTMLTableDataCellElementImp::getMetaData()));
    HTMLTableElementImp::setStaticPrivate(new NativeClass(cx, HTMLTableElementImp::getMetaData()));
    HTMLTableHeaderCellElementImp::setStaticPrivate(new NativeClass(cx, HTMLTableHeaderCellElementImp::getMetaData()));
    HTMLTableRowElementImp::setStaticPrivate(new NativeClass(cx, HTMLTableRowElementImp::getMetaData()));
    HTMLTableSectionElementImp::setStaticPrivate(new NativeClass(cx, HTMLTableSectionElementImp::getMetaData()));
    HTMLTextAreaElementImp::setStaticPrivate(new NativeClass(cx, HTMLTextAreaElementImp::getMetaData()));
    HTMLTimeElementImp::setStaticPrivate(new NativeClass(cx, HTMLTimeElementImp::getMetaData()));
    HTMLTitleElementImp::setStaticPrivate(new NativeClass(cx, HTMLTitleElementImp::getMetaData()));
    HTMLUListElementImp::setStaticPrivate(new NativeClass(cx, HTMLUListElementImp::getMetaData()));
    HTMLUnknownElementImp::setStaticPrivate(new NativeClass(cx, HTMLUnknownElementImp::getMetaData()));

    // media element
    HTMLMediaElementImp::setStaticPrivate(new NativeClass(cx, HTMLMediaElementImp::getMetaData()));
    HTMLAudioElementImp::setStaticPrivate(new NativeClass(cx, HTMLAudioElementImp::getMetaData()));
    HTMLVideoElementImp::setStaticPrivate(new NativeClass(cx, HTMLVideoElementImp::getMetaData()));

    // html collection
    HTMLCollectionImp::setStaticPrivate(new NativeClass(cx, HTMLCollectionImp::getMetaData()));
    HTMLAllCollectionImp::setStaticPrivate(new NativeClass(cx, HTMLAllCollectionImp::getMetaData()));
    HTMLOptionsCollectionImp::setStaticPrivate(new NativeClass(cx, HTMLOptionsCollectionImp::getMetaData()));
    HTMLFormControlsCollectionImp::setStaticPrivate(new NativeClass(cx, HTMLFormControlsCollectionImp::getMetaData()));
    HTMLPropertiesCollectionImp::setStaticPrivate(new NativeClass(cx, HTMLPropertiesCollectionImp::getMetaData()));

    // style sheet
    CSSStyleSheetImp::setStaticPrivate(new NativeClass(cx, css::CSSStyleSheet::getMetaData()));

    // css rule
    CSSMediaRuleImp::setStaticPrivate(new NativeClass(cx, css::CSSMediaRule::getMetaData()));
    CSSStyleRuleImp::setStaticPrivate(new NativeClass(cx, css::CSSStyleRule::getMetaData()));

    // css value
    CSSValueImp::setStaticPrivate(new NativeClass(cx, CSSValueImp::getMetaData()));

    // css
    DOMImplementationCSSImp::setStaticPrivate(new NativeClass(cx, DOMImplementationCSSImp::getMetaData()));
    DocumentCSSImp::setStaticPrivate(new NativeClass(cx, DocumentCSSImp::getMetaData()));
    CSS2PropertiesImp::setStaticPrivate(new NativeClass(cx, CSS2PropertiesImp::getMetaData()));
    CSSCharsetRuleImp::setStaticPrivate(new NativeClass(cx, CSSCharsetRuleImp::getMetaData()));
    CSSColorComponentValueImp::setStaticPrivate(new NativeClass(cx, CSSColorComponentValueImp::getMetaData()));
    CSSComponentValueImp::setStaticPrivate(new NativeClass(cx, CSSComponentValueImp::getMetaData()));
    CSSFontFaceRuleImp::setStaticPrivate(new NativeClass(cx, CSSFontFaceRuleImp::getMetaData()));
    CSSIdentifierComponentValueImp::setStaticPrivate(new NativeClass(cx, CSSIdentifierComponentValueImp::getMetaData()));
    CSSImportRuleImp::setStaticPrivate(new NativeClass(cx, CSSImportRuleImp::getMetaData()));
    CSSKeywordComponentValueImp::setStaticPrivate(new NativeClass(cx, CSSKeywordComponentValueImp::getMetaData()));
    CSSLengthComponentValueImp::setStaticPrivate(new NativeClass(cx, CSSLengthComponentValueImp::getMetaData()));
    CSSMapValueImp::setStaticPrivate(new NativeClass(cx, CSSMapValueImp::getMetaData()));
    CSSNamespaceRuleImp::setStaticPrivate(new NativeClass(cx, CSSNamespaceRuleImp::getMetaData()));
    CSSPageRuleImp::setStaticPrivate(new NativeClass(cx, CSSPageRuleImp::getMetaData()));
    CSSPercentageComponentValueImp::setStaticPrivate(new NativeClass(cx, CSSPercentageComponentValueImp::getMetaData()));
    CSSPrimitiveValueImp::setStaticPrivate(new NativeClass(cx, CSSPrimitiveValueImp::getMetaData()));
    // CSSPropertyValueImp::setStaticPrivate(new NativeClass(cx, CSSPropertyValueImp::getMetaData()));
    CSSPropertyValueListImp::setStaticPrivate(new NativeClass(cx, CSSPropertyValueListImp::getMetaData()));
    CSSStringComponentValueImp::setStaticPrivate(new NativeClass(cx, CSSStringComponentValueImp::getMetaData()));
    CSSStyleDeclarationValueImp::setStaticPrivate(new NativeClass(cx, CSSStyleDeclarationValueImp::getMetaData()));
    CSSURLComponentValueImp::setStaticPrivate(new NativeClass(cx, CSSURLComponentValueImp::getMetaData()));
    CSSUnknownRuleImp::setStaticPrivate(new NativeClass(cx, CSSUnknownRuleImp::getMetaData()));
    CSSValueListImp::setStaticPrivate(new NativeClass(cx, CSSValueListImp::getMetaData()));

    // XMLHttpRequest
    XMLHttpRequestEventTargetImp::setStaticPrivate(new NativeClass(cx, XMLHttpRequestEventTargetImp::getMetaData()));
    XMLHttpRequestImp::setStaticPrivate(new NativeClass(cx, XMLHttpRequestImp::getMetaData()));
    AnonXMLHttpRequestImp::setStaticPrivate(new NativeClass(cx, AnonXMLHttpRequestImp::getMetaData()));
    XMLHttpRequestUploadImp::setStaticPrivate(new NativeClass(cx, XMLHttpRequestUploadImp::getMetaData()));

    // file
    FileErrorImp::setStaticPrivate(new NativeClass(cx, FileErrorImp::getMetaData()));
    FileImp::setStaticPrivate(new NativeClass(cx, FileImp::getMetaData()));
    FileListImp::setStaticPrivate(new NativeClass(cx, FileListImp::getMetaData()));
    FileReaderImp::setStaticPrivate(new NativeClass(cx, FileReaderImp::getMetaData()));
    FileReaderSyncImp::setStaticPrivate(new NativeClass(cx, FileReaderSyncImp::getMetaData()));
}

JSRuntime* jsruntime = 0;

}  // namespace

JSRuntime* getRuntime()
{
    return jsruntime;
}

JSObject* getGlobal()
{
    if (!jsruntime) {
        jsruntime = JS_NewRuntime(8L * 1024L * 1024L);
        if (!jsruntime)
            return 0;
        jscontext = JS_NewContext(jsruntime, 8192);
        if (!jscontext) {
            JS_DestroyRuntime(jsruntime);
            jsruntime = 0;
            return 0;
        }
        JS_SetOptions(jscontext, JSOPTION_VAROBJFIX | JSOPTION_JIT | JSOPTION_METHODJIT);
        JS_SetVersion(jscontext, JSVERSION_LATEST);
        JS_SetErrorReporter(jscontext, reportError);
    }

    JSObject* global = JS_NewCompartmentAndGlobalObject(jscontext, &globalClass, NULL);
    if (!global)
        return 0;
    if (!JS_InitStandardClasses(jscontext, global))
        return 0;

    registerClasses(jscontext);

    Reflect::Interface globalMeta(html::Window::getMetaData());
    std::string name = Reflect::getIdentifier(globalMeta.getName());
    if (0 < name.length()) {
        jsval val;
        if (JS_GetProperty(jscontext, global, name.c_str(), &val) && JSVAL_IS_OBJECT(val)) {
            JSObject* parent = JSVAL_TO_OBJECT(val);
            if (JS_GetProperty(jscontext, parent, "prototype", &val) && JSVAL_IS_OBJECT(val)) {
                JSObject* proto = JSVAL_TO_OBJECT(val);
                JS_SetPrototype(jscontext, global, proto);
            }
        }
    }
    return global;
}

void putGlobal(JSObject* global)
{
    // TODO: how?
}
