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

#include "Script.h"

#include <iostream>

#include "esjsapi.h"

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
#include "CustomEventInitImp.h"
#include "DOMImplementationImp.h"
#include "DOMSettableTokenListImp.h"
#include "DOMStringListImp.h"
#include "DOMStringMapImp.h"
#include "DOMTokenListImp.h"
#include "DocumentFragmentImp.h"
#include "DocumentImp.h"
#include "DocumentRangeImp.h"
#include "DocumentTraversalImp.h"
#include "DocumentTypeImp.h"
#include "DragEventImp.h"
#include "ElementImp.h"
#include "EventImp.h"
#include "EventInitImp.h"
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
#include "file/FileErrorImp.h"
#include "file/FileImp.h"
#include "file/FileListImp.h"
#include "file/FileReaderImp.h"
#include "file/FileReaderSyncImp.h"
#include "html/CanvasGradientImp.h"
#include "html/CanvasPatternImp.h"
#include "html/CanvasPixelArrayImp.h"
#include "html/CanvasRenderingContext2DImp.h"
#include "html/DataTransferImp.h"
#include "html/DataTransferItemImp.h"
#include "html/DataTransferItemListImp.h"
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
#if 0
#include "typedarray/ArrayBufferImp.h"
#include "typedarray/ArrayBufferViewImp.h"
#include "typedarray/DataViewImp.h"
#include "typedarray/Float32ArrayImp.h"
#include "typedarray/Float64ArrayImp.h"
#include "typedarray/Int16ArrayImp.h"
#include "typedarray/Int32ArrayImp.h"
#include "typedarray/Int8ArrayImp.h"
#include "typedarray/Uint16ArrayImp.h"
#include "typedarray/Uint32ArrayImp.h"
#include "typedarray/Uint8ArrayImp.h"
#endif
#include "xhr/AnonXMLHttpRequestImp.h"
#include "xhr/FormDataImp.h"
#include "xhr/XMLHttpRequestEventTargetImp.h"
#include "xhr/XMLHttpRequestImp.h"
#include "xhr/XMLHttpRequestUploadImp.h"

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

void registerClasses(JSContext* cx, JSObject* global)
{
    // prototype roots
    AttrImp::setStaticPrivate(new NativeClass(cx, global, AttrImp::getMetaData()));
    CSSRuleImp::setStaticPrivate(new NativeClass(cx, global, CSSRuleImp::getMetaData()));
    CSSStyleDeclarationImp::setStaticPrivate(new NativeClass(cx, global, CSSStyleDeclarationImp::getMetaData()));
    DOMImplementationImp::setStaticPrivate(new NativeClass(cx, global, DOMImplementationImp::getMetaData()));
    EventImp::setStaticPrivate(new NativeClass(cx, global, EventImp::getMetaData()));
    EventTargetImp::setStaticPrivate(new NativeClass(cx, global, EventTargetImp::getMetaData()));
    LocationImp::setStaticPrivate(new NativeClass(cx, global, LocationImp::getMetaData()));
    MediaListImp::setStaticPrivate(new NativeClass(cx, global, MediaListImp::getMetaData()));
    NodeListImp::setStaticPrivate(new NativeClass(cx, global, NodeListImp::getMetaData()));
    StyleSheetImp::setStaticPrivate(new NativeClass(cx, global, StyleSheetImp::getMetaData()));

    // dom
    ApplicationCacheImp::setStaticPrivate(new NativeClass(cx, global, ApplicationCacheImp::getMetaData()));
    BarPropImp::setStaticPrivate(new NativeClass(cx, global, BarPropImp::getMetaData()));
    BlobImp::setStaticPrivate(new NativeClass(cx, global, BlobImp::getMetaData()));
    BooleanCallbackImp::setStaticPrivate(new NativeClass(cx, global, BooleanCallbackImp::getMetaData()));
    CanvasGradientImp::setStaticPrivate(new NativeClass(cx, global, CanvasGradientImp::getMetaData()));
    CanvasPatternImp::setStaticPrivate(new NativeClass(cx, global, CanvasPatternImp::getMetaData()));
    CanvasPixelArrayImp::setStaticPrivate(new NativeClass(cx, global, CanvasPixelArrayImp::getMetaData()));
    CanvasRenderingContext2DImp::setStaticPrivate(new NativeClass(cx, global, CanvasRenderingContext2DImp::getMetaData()));
    CaretPositionImp::setStaticPrivate(new NativeClass(cx, global, CaretPositionImp::getMetaData()));
    ClientRectImp::setStaticPrivate(new NativeClass(cx, global, ClientRectImp::getMetaData()));
    ClientRectListImp::setStaticPrivate(new NativeClass(cx, global, ClientRectListImp::getMetaData()));
    CounterImp::setStaticPrivate(new NativeClass(cx, global, CounterImp::getMetaData()));
    // DOMConfigurationImp::setStaticPrivate(new NativeClass(cx, global, DOMConfigurationImp::getMetaData()));
    DOMStringListImp::setStaticPrivate(new NativeClass(cx, global, DOMStringListImp::getMetaData()));
    DOMStringMapImp::setStaticPrivate(new NativeClass(cx, global, DOMStringMapImp::getMetaData()));
    DataTransferImp::setStaticPrivate(new NativeClass(cx, global, DataTransferImp::getMetaData()));
    DocumentRangeImp::setStaticPrivate(new NativeClass(cx, global, DocumentRangeImp::getMetaData()));
    DocumentTraversalImp::setStaticPrivate(new NativeClass(cx, global, DocumentTraversalImp::getMetaData()));
    ElementCSSInlineStyleImp::setStaticPrivate(new NativeClass(cx, global, ElementCSSInlineStyleImp::getMetaData()));
    EventListenerImp::setStaticPrivate(new NativeClass(cx, global, EventListenerImp::getMetaData()));
    EventSourceImp::setStaticPrivate(new NativeClass(cx, global, EventSourceImp::getMetaData()));
    FormDataImp::setStaticPrivate(new NativeClass(cx, global, FormDataImp::getMetaData()));
    HistoryImp::setStaticPrivate(new NativeClass(cx, global, HistoryImp::getMetaData()));
    ImageDataImp::setStaticPrivate(new NativeClass(cx, global, ImageDataImp::getMetaData()));
    LinkStyleImp::setStaticPrivate(new NativeClass(cx, global, LinkStyleImp::getMetaData()));
    MediaErrorImp::setStaticPrivate(new NativeClass(cx, global, MediaErrorImp::getMetaData()));
    MessageChannelImp::setStaticPrivate(new NativeClass(cx, global, MessageChannelImp::getMetaData()));
    MessagePortImp::setStaticPrivate(new NativeClass(cx, global, MessagePortImp::getMetaData()));
    // NameListImp::setStaticPrivate(new NativeClass(cx, global, NameListImp::getMetaData()));
    NavigatorImp::setStaticPrivate(new NativeClass(cx, global, NavigatorImp::getMetaData()));
    NodeFilterImp::setStaticPrivate(new NativeClass(cx, global, NodeFilterImp::getMetaData()));
    NodeIteratorImp::setStaticPrivate(new NativeClass(cx, global, NodeIteratorImp::getMetaData()));
    PropertyNodeListImp::setStaticPrivate(new NativeClass(cx, global, PropertyNodeListImp::getMetaData()));
    RGBColorImp::setStaticPrivate(new NativeClass(cx, global, RGBColorImp::getMetaData()));
    RadioNodeListImp::setStaticPrivate(new NativeClass(cx, global, RadioNodeListImp::getMetaData()));
    RangeImp::setStaticPrivate(new NativeClass(cx, global, RangeImp::getMetaData()));
    RectImp::setStaticPrivate(new NativeClass(cx, global, RectImp::getMetaData()));
    ScreenImp::setStaticPrivate(new NativeClass(cx, global, ScreenImp::getMetaData()));
    StyleMediaImp::setStaticPrivate(new NativeClass(cx, global, StyleMediaImp::getMetaData()));
    TextMetricsImp::setStaticPrivate(new NativeClass(cx, global, TextMetricsImp::getMetaData()));
    TimeRangesImp::setStaticPrivate(new NativeClass(cx, global, TimeRangesImp::getMetaData()));
    TreeWalkerImp::setStaticPrivate(new NativeClass(cx, global, TreeWalkerImp::getMetaData()));
    // URLImp::setStaticPrivate(new NativeClass(cx, global, URLImp::getMetaData()));
    UndoManagerImp::setStaticPrivate(new NativeClass(cx, global, UndoManagerImp::getMetaData()));
    ValidityStateImp::setStaticPrivate(new NativeClass(cx, global, ValidityStateImp::getMetaData()));

    DOMTokenListImp::setStaticPrivate(new NativeClass(cx, global, DOMTokenListImp::getMetaData()));
    DOMSettableTokenListImp::setStaticPrivate(new NativeClass(cx, global, DOMSettableTokenListImp::getMetaData()));

    // window
    WindowImp::setStaticPrivate(new NativeClass(cx, global, html::Window::getMetaData()));

    // node
    NodeImp::setStaticPrivate(new NativeClass(cx, global, Node::getMetaData()));
    CharacterDataImp::setStaticPrivate(new NativeClass(cx, global, CharacterData::getMetaData()));
    DocumentImp::setStaticPrivate(new NativeClass(cx, global, Document::getMetaData()));
    DocumentFragmentImp::setStaticPrivate(new NativeClass(cx, global, DocumentFragmentImp::getMetaData()));
    DocumentTypeImp::setStaticPrivate(new NativeClass(cx, global, DocumentType::getMetaData()));
    ElementImp::setStaticPrivate(new NativeClass(cx, global, Element::getMetaData()));
    ProcessingInstructionImp::setStaticPrivate(new NativeClass(cx, global, ProcessingInstructionImp::getMetaData()));

    // event
    UIEventImp::setStaticPrivate(new NativeClass(cx, global, events::UIEvent::getMetaData()));
    MouseEventImp::setStaticPrivate(new NativeClass(cx, global, MouseEventImp::getMetaData()));
    DragEventImp::setStaticPrivate(new NativeClass(cx, global, DragEventImp::getMetaData()));
    MutationEventImp::setStaticPrivate(new NativeClass(cx, global, MutationEventImp::getMetaData()));
    PageTransitionEventImp::setStaticPrivate(new NativeClass(cx, global, PageTransitionEventImp::getMetaData()));
    PopStateEventImp::setStaticPrivate(new NativeClass(cx, global, PopStateEventImp::getMetaData()));
    ProgressEventImp::setStaticPrivate(new NativeClass(cx, global, ProgressEventImp::getMetaData()));
    TextEventImp::setStaticPrivate(new NativeClass(cx, global, TextEventImp::getMetaData()));
    UndoManagerEventImp::setStaticPrivate(new NativeClass(cx, global, UndoManagerEventImp::getMetaData()));
    WheelEventImp::setStaticPrivate(new NativeClass(cx, global, WheelEventImp::getMetaData()));
    BeforeUnloadEventImp::setStaticPrivate(new NativeClass(cx, global, BeforeUnloadEventImp::getMetaData()));
    CompositionEventImp::setStaticPrivate(new NativeClass(cx, global, CompositionEventImp::getMetaData()));
    CustomEventImp::setStaticPrivate(new NativeClass(cx, global, CustomEventImp::getMetaData()));
    FocusEventImp::setStaticPrivate(new NativeClass(cx, global, FocusEventImp::getMetaData()));
    HashChangeEventImp::setStaticPrivate(new NativeClass(cx, global, HashChangeEventImp::getMetaData()));
    KeyboardEventImp::setStaticPrivate(new NativeClass(cx, global, KeyboardEventImp::getMetaData()));
    MessageEventImp::setStaticPrivate(new NativeClass(cx, global, MessageEventImp::getMetaData()));
    MutationNameEventImp::setStaticPrivate(new NativeClass(cx, global, MutationNameEventImp::getMetaData()));

    // character data
    CommentImp::setStaticPrivate(new NativeClass(cx, global, Comment::getMetaData()));
    TextImp::setStaticPrivate(new NativeClass(cx, global, Text::getMetaData()));

    // html element
    HTMLElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLElementImp::getMetaData()));
    HTMLAnchorElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLAnchorElementImp::getMetaData()));
    HTMLAppletElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLAppletElementImp::getMetaData()));
    HTMLAreaElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLAreaElementImp::getMetaData()));
    HTMLBRElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLBRElementImp::getMetaData()));
    HTMLBaseElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLBaseElementImp::getMetaData()));
    HTMLBaseFontElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLBaseFontElementImp::getMetaData()));
    HTMLBodyElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLBodyElementImp::getMetaData()));
    HTMLButtonElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLButtonElementImp::getMetaData()));
    HTMLCanvasElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLCanvasElementImp::getMetaData()));
    HTMLCommandElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLCommandElementImp::getMetaData()));
    HTMLDListElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLDListElementImp::getMetaData()));
    HTMLDataListElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLDataListElementImp::getMetaData()));
    HTMLDetailsElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLDetailsElementImp::getMetaData()));
    HTMLDirectoryElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLDirectoryElementImp::getMetaData()));
    HTMLDivElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLDivElementImp::getMetaData()));
    HTMLDocumentImp::setStaticPrivate(new NativeClass(cx, global, HTMLDocumentImp::getMetaData()));
    HTMLEmbedElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLEmbedElementImp::getMetaData()));
    HTMLFieldSetElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLFieldSetElementImp::getMetaData()));
    HTMLFontElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLFontElementImp::getMetaData()));
    HTMLFormElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLFormElementImp::getMetaData()));
    HTMLFrameElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLFrameElementImp::getMetaData()));
    HTMLFrameSetElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLFrameSetElementImp::getMetaData()));
    HTMLHRElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLHRElementImp::getMetaData()));
    HTMLHeadElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLHeadElementImp::getMetaData()));
    HTMLHeadingElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLHeadingElementImp::getMetaData()));
    HTMLHtmlElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLHtmlElementImp::getMetaData()));
    HTMLIFrameElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLIFrameElementImp::getMetaData()));
    HTMLImageElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLImageElementImp::getMetaData()));
    HTMLInputElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLInputElementImp::getMetaData()));
    HTMLKeygenElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLKeygenElementImp::getMetaData()));
    HTMLLIElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLLIElementImp::getMetaData()));
    HTMLLabelElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLLabelElementImp::getMetaData()));
    HTMLLegendElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLLegendElementImp::getMetaData()));
    HTMLLinkElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLLinkElementImp::getMetaData()));
    HTMLMapElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLMapElementImp::getMetaData()));
    HTMLMarqueeElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLMarqueeElementImp::getMetaData()));
    HTMLMenuElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLMenuElementImp::getMetaData()));
    HTMLMetaElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLMetaElementImp::getMetaData()));
    HTMLMeterElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLMeterElementImp::getMetaData()));
    HTMLModElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLModElementImp::getMetaData()));
    HTMLOListElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLOListElementImp::getMetaData()));
    HTMLObjectElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLObjectElementImp::getMetaData()));
    HTMLOptGroupElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLOptGroupElementImp::getMetaData()));
    HTMLOptionElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLOptionElementImp::getMetaData()));
    HTMLOutputElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLOutputElementImp::getMetaData()));
    HTMLParagraphElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLParagraphElementImp::getMetaData()));
    HTMLParamElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLParamElementImp::getMetaData()));
    HTMLPreElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLPreElementImp::getMetaData()));
    HTMLProgressElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLProgressElementImp::getMetaData()));
    HTMLQuoteElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLQuoteElementImp::getMetaData()));
    HTMLScriptElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLScriptElementImp::getMetaData()));
    HTMLSelectElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLSelectElementImp::getMetaData()));
    HTMLSourceElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLSourceElementImp::getMetaData()));
    HTMLSpanElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLSpanElementImp::getMetaData()));
    HTMLStyleElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLStyleElementImp::getMetaData()));
    HTMLTableCaptionElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLTableCaptionElementImp::getMetaData()));
    HTMLTableCellElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLTableCellElementImp::getMetaData()));
    HTMLTableColElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLTableColElementImp::getMetaData()));
    HTMLTableDataCellElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLTableDataCellElementImp::getMetaData()));
    HTMLTableElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLTableElementImp::getMetaData()));
    HTMLTableHeaderCellElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLTableHeaderCellElementImp::getMetaData()));
    HTMLTableRowElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLTableRowElementImp::getMetaData()));
    HTMLTableSectionElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLTableSectionElementImp::getMetaData()));
    HTMLTextAreaElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLTextAreaElementImp::getMetaData()));
    HTMLTimeElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLTimeElementImp::getMetaData()));
    HTMLTitleElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLTitleElementImp::getMetaData()));
    HTMLUListElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLUListElementImp::getMetaData()));
    HTMLUnknownElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLUnknownElementImp::getMetaData()));

    // media element
    HTMLMediaElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLMediaElementImp::getMetaData()));
    HTMLAudioElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLAudioElementImp::getMetaData()));
    HTMLVideoElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLVideoElementImp::getMetaData()));

    // html collection
    HTMLCollectionImp::setStaticPrivate(new NativeClass(cx, global, HTMLCollectionImp::getMetaData()));
    HTMLAllCollectionImp::setStaticPrivate(new NativeClass(cx, global, HTMLAllCollectionImp::getMetaData()));
    HTMLOptionsCollectionImp::setStaticPrivate(new NativeClass(cx, global, HTMLOptionsCollectionImp::getMetaData()));
    HTMLFormControlsCollectionImp::setStaticPrivate(new NativeClass(cx, global, HTMLFormControlsCollectionImp::getMetaData()));
    HTMLPropertiesCollectionImp::setStaticPrivate(new NativeClass(cx, global, HTMLPropertiesCollectionImp::getMetaData()));

    // style sheet
    CSSStyleSheetImp::setStaticPrivate(new NativeClass(cx, global, css::CSSStyleSheet::getMetaData()));

    // css rule
    CSSMediaRuleImp::setStaticPrivate(new NativeClass(cx, global, css::CSSMediaRule::getMetaData()));
    CSSStyleRuleImp::setStaticPrivate(new NativeClass(cx, global, css::CSSStyleRule::getMetaData()));

    // css value
    CSSValueImp::setStaticPrivate(new NativeClass(cx, global, CSSValueImp::getMetaData()));

    // css
    DOMImplementationCSSImp::setStaticPrivate(new NativeClass(cx, global, DOMImplementationCSSImp::getMetaData()));
    DocumentCSSImp::setStaticPrivate(new NativeClass(cx, global, DocumentCSSImp::getMetaData()));
    CSS2PropertiesImp::setStaticPrivate(new NativeClass(cx, global, CSS2PropertiesImp::getMetaData()));
    CSSCharsetRuleImp::setStaticPrivate(new NativeClass(cx, global, CSSCharsetRuleImp::getMetaData()));
    CSSColorComponentValueImp::setStaticPrivate(new NativeClass(cx, global, CSSColorComponentValueImp::getMetaData()));
    CSSComponentValueImp::setStaticPrivate(new NativeClass(cx, global, CSSComponentValueImp::getMetaData()));
    CSSFontFaceRuleImp::setStaticPrivate(new NativeClass(cx, global, CSSFontFaceRuleImp::getMetaData()));
    CSSIdentifierComponentValueImp::setStaticPrivate(new NativeClass(cx, global, CSSIdentifierComponentValueImp::getMetaData()));
    CSSImportRuleImp::setStaticPrivate(new NativeClass(cx, global, CSSImportRuleImp::getMetaData()));
    CSSKeywordComponentValueImp::setStaticPrivate(new NativeClass(cx, global, CSSKeywordComponentValueImp::getMetaData()));
    CSSLengthComponentValueImp::setStaticPrivate(new NativeClass(cx, global, CSSLengthComponentValueImp::getMetaData()));
    CSSMapValueImp::setStaticPrivate(new NativeClass(cx, global, CSSMapValueImp::getMetaData()));
    CSSNamespaceRuleImp::setStaticPrivate(new NativeClass(cx, global, CSSNamespaceRuleImp::getMetaData()));
    CSSPageRuleImp::setStaticPrivate(new NativeClass(cx, global, CSSPageRuleImp::getMetaData()));
    CSSPercentageComponentValueImp::setStaticPrivate(new NativeClass(cx, global, CSSPercentageComponentValueImp::getMetaData()));
    CSSPrimitiveValueImp::setStaticPrivate(new NativeClass(cx, global, CSSPrimitiveValueImp::getMetaData()));
    // CSSPropertyValueImp::setStaticPrivate(new NativeClass(cx, global, CSSPropertyValueImp::getMetaData()));
    CSSPropertyValueListImp::setStaticPrivate(new NativeClass(cx, global, CSSPropertyValueListImp::getMetaData()));
    CSSStringComponentValueImp::setStaticPrivate(new NativeClass(cx, global, CSSStringComponentValueImp::getMetaData()));
    CSSStyleDeclarationValueImp::setStaticPrivate(new NativeClass(cx, global, CSSStyleDeclarationValueImp::getMetaData()));
    CSSURLComponentValueImp::setStaticPrivate(new NativeClass(cx, global, CSSURLComponentValueImp::getMetaData()));
    CSSUnknownRuleImp::setStaticPrivate(new NativeClass(cx, global, CSSUnknownRuleImp::getMetaData()));
    CSSValueListImp::setStaticPrivate(new NativeClass(cx, global, CSSValueListImp::getMetaData()));

    // XMLHttpRequest
    XMLHttpRequestEventTargetImp::setStaticPrivate(new NativeClass(cx, global, XMLHttpRequestEventTargetImp::getMetaData()));
    XMLHttpRequestImp::setStaticPrivate(new NativeClass(cx, global, XMLHttpRequestImp::getMetaData()));
    AnonXMLHttpRequestImp::setStaticPrivate(new NativeClass(cx, global, AnonXMLHttpRequestImp::getMetaData()));
    XMLHttpRequestUploadImp::setStaticPrivate(new NativeClass(cx, global, XMLHttpRequestUploadImp::getMetaData()));

    // file
    FileErrorImp::setStaticPrivate(new NativeClass(cx, global, FileErrorImp::getMetaData()));
    FileImp::setStaticPrivate(new NativeClass(cx, global, FileImp::getMetaData()));
    FileListImp::setStaticPrivate(new NativeClass(cx, global, FileListImp::getMetaData()));
    FileReaderImp::setStaticPrivate(new NativeClass(cx, global, FileReaderImp::getMetaData()));
    FileReaderSyncImp::setStaticPrivate(new NativeClass(cx, global, FileReaderSyncImp::getMetaData()));

    // new stuff
    EventInitImp::setStaticPrivate(new NativeClass(cx, global, EventInitImp::getMetaData()));
    CustomEventInitImp::setStaticPrivate(new NativeClass(cx, global, CustomEventInitImp::getMetaData()));
    DataTransferItemImp::setStaticPrivate(new NativeClass(cx, global, DataTransferItemImp::getMetaData()));
    DataTransferItemListImp::setStaticPrivate(new NativeClass(cx, global, DataTransferItemListImp::getMetaData()));
}

JSRuntime* jsruntime = 0;

}  // namespace

JSRuntime* getRuntime()
{
    return jsruntime;
}

JSObject* newGlobal()
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

    registerClasses(jscontext, global);

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

    JS_AddObjectRoot(jscontext, &global);

    return global;
}

void putGlobal(JSObject* global)
{
    JS_RemoveObjectRoot(jscontext, &global);
}

void callFunction(html::Function function, events::Event event)
{
    Any arg(event);
    Any result = callFunction(event.getTarget(), function, 1, &arg);
    if (event.getType() == u"mouseover") {
        if (result.toBoolean())
            event.preventDefault();
    } else if (html::BeforeUnloadEvent::hasInstance(event)) {
        html::BeforeUnloadEvent unloadEvent = interface_cast<html::BeforeUnloadEvent>(event);
        if (unloadEvent.getReturnValue().empty() && result.isString())
            unloadEvent.setReturnValue(result.toString());
    } else if (!result.toBoolean())
        event.preventDefault();
}
