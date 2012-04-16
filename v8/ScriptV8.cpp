/*
 * Copyright 2011, 2012 Esrille Inc.
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

#include "ScriptV8.h"

#include <iostream>

#include "ApplicationCacheImp.h"
#include "AttrImp.h"
#include "BarPropImp.h"
#include "BeforeUnloadEventImp.h"
#include "BlobImp.h"
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
#include "MediaQueryListImp.h"
#include "MediaQueryListListenerImp.h"
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
#include "html/HTMLBindingElementImp.h"
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
#include "html/HTMLTemplateElementImp.h"
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

void registerClasses(v8::Handle<v8::ObjectTemplate> global)
{
    // prototype roots
    AttrImp::setStaticPrivate(new NativeClass(global, AttrImp::getMetaData()));
    CSSRuleImp::setStaticPrivate(new NativeClass(global, CSSRuleImp::getMetaData()));
    CSSStyleDeclarationImp::setStaticPrivate(new NativeClass(global, CSSStyleDeclarationImp::getMetaData()));
    DOMImplementationImp::setStaticPrivate(new NativeClass(global, DOMImplementationImp::getMetaData()));
    EventImp::setStaticPrivate(new NativeClass(global, EventImp::getMetaData()));
    EventTargetImp::setStaticPrivate(new NativeClass(global, EventTargetImp::getMetaData()));
    LocationImp::setStaticPrivate(new NativeClass(global, LocationImp::getMetaData()));
    MediaListImp::setStaticPrivate(new NativeClass(global, MediaListImp::getMetaData()));
    NodeListImp::setStaticPrivate(new NativeClass(global, NodeListImp::getMetaData()));
    StyleSheetImp::setStaticPrivate(new NativeClass(global, StyleSheetImp::getMetaData()));

    // dom
    ApplicationCacheImp::setStaticPrivate(new NativeClass(global, ApplicationCacheImp::getMetaData()));
    BarPropImp::setStaticPrivate(new NativeClass(global, BarPropImp::getMetaData()));
    BlobImp::setStaticPrivate(new NativeClass(global, BlobImp::getMetaData()));
    CanvasGradientImp::setStaticPrivate(new NativeClass(global, CanvasGradientImp::getMetaData()));
    CanvasPatternImp::setStaticPrivate(new NativeClass(global, CanvasPatternImp::getMetaData()));
    CanvasPixelArrayImp::setStaticPrivate(new NativeClass(global, CanvasPixelArrayImp::getMetaData()));
    CanvasRenderingContext2DImp::setStaticPrivate(new NativeClass(global, CanvasRenderingContext2DImp::getMetaData()));
    CaretPositionImp::setStaticPrivate(new NativeClass(global, CaretPositionImp::getMetaData()));
    ClientRectImp::setStaticPrivate(new NativeClass(global, ClientRectImp::getMetaData()));
    ClientRectListImp::setStaticPrivate(new NativeClass(global, ClientRectListImp::getMetaData()));
    CounterImp::setStaticPrivate(new NativeClass(global, CounterImp::getMetaData()));
    // DOMConfigurationImp::setStaticPrivate(new NativeClass(global, DOMConfigurationImp::getMetaData()));
    DOMStringListImp::setStaticPrivate(new NativeClass(global, DOMStringListImp::getMetaData()));
    DOMStringMapImp::setStaticPrivate(new NativeClass(global, DOMStringMapImp::getMetaData()));
    DataTransferImp::setStaticPrivate(new NativeClass(global, DataTransferImp::getMetaData()));
    DocumentRangeImp::setStaticPrivate(new NativeClass(global, DocumentRangeImp::getMetaData()));
    DocumentTraversalImp::setStaticPrivate(new NativeClass(global, DocumentTraversalImp::getMetaData()));
    ElementCSSInlineStyleImp::setStaticPrivate(new NativeClass(global, ElementCSSInlineStyleImp::getMetaData()));
    EventListenerImp::setStaticPrivate(new NativeClass(global, EventListenerImp::getMetaData()));
    EventSourceImp::setStaticPrivate(new NativeClass(global, EventSourceImp::getMetaData()));
    FormDataImp::setStaticPrivate(new NativeClass(global, FormDataImp::getMetaData()));
    HistoryImp::setStaticPrivate(new NativeClass(global, HistoryImp::getMetaData()));
    ImageDataImp::setStaticPrivate(new NativeClass(global, ImageDataImp::getMetaData()));
    LinkStyleImp::setStaticPrivate(new NativeClass(global, LinkStyleImp::getMetaData()));
    MediaErrorImp::setStaticPrivate(new NativeClass(global, MediaErrorImp::getMetaData()));
    MediaQueryListImp::setStaticPrivate(new NativeClass(global, MediaQueryListImp::getMetaData()));
    MediaQueryListListenerImp::setStaticPrivate(new NativeClass(global, MediaQueryListListenerImp::getMetaData()));
    MessageChannelImp::setStaticPrivate(new NativeClass(global, MessageChannelImp::getMetaData()));
    MessagePortImp::setStaticPrivate(new NativeClass(global, MessagePortImp::getMetaData()));
    // NameListImp::setStaticPrivate(new NativeClass(global, NameListImp::getMetaData()));
    NavigatorImp::setStaticPrivate(new NativeClass(global, NavigatorImp::getMetaData()));
    NodeFilterImp::setStaticPrivate(new NativeClass(global, NodeFilterImp::getMetaData()));
    NodeIteratorImp::setStaticPrivate(new NativeClass(global, NodeIteratorImp::getMetaData()));
    PropertyNodeListImp::setStaticPrivate(new NativeClass(global, PropertyNodeListImp::getMetaData()));
    RGBColorImp::setStaticPrivate(new NativeClass(global, RGBColorImp::getMetaData()));
    RadioNodeListImp::setStaticPrivate(new NativeClass(global, RadioNodeListImp::getMetaData()));
    RangeImp::setStaticPrivate(new NativeClass(global, RangeImp::getMetaData()));
    RectImp::setStaticPrivate(new NativeClass(global, RectImp::getMetaData()));
    ScreenImp::setStaticPrivate(new NativeClass(global, ScreenImp::getMetaData()));
    TextMetricsImp::setStaticPrivate(new NativeClass(global, TextMetricsImp::getMetaData()));
    TimeRangesImp::setStaticPrivate(new NativeClass(global, TimeRangesImp::getMetaData()));
    TreeWalkerImp::setStaticPrivate(new NativeClass(global, TreeWalkerImp::getMetaData()));
    // URLImp::setStaticPrivate(new NativeClass(global, URLImp::getMetaData()));
    UndoManagerImp::setStaticPrivate(new NativeClass(global, UndoManagerImp::getMetaData()));
    ValidityStateImp::setStaticPrivate(new NativeClass(global, ValidityStateImp::getMetaData()));

    DOMTokenListImp::setStaticPrivate(new NativeClass(global, DOMTokenListImp::getMetaData()));
    DOMSettableTokenListImp::setStaticPrivate(new NativeClass(global, DOMSettableTokenListImp::getMetaData()));

    // window
    WindowImp::setStaticPrivate(new NativeClass(global, html::Window::getMetaData()));

    // node
    NodeImp::setStaticPrivate(new NativeClass(global, Node::getMetaData()));
    CharacterDataImp::setStaticPrivate(new NativeClass(global, CharacterData::getMetaData()));
    DocumentImp::setStaticPrivate(new NativeClass(global, Document::getMetaData()));
    DocumentFragmentImp::setStaticPrivate(new NativeClass(global, DocumentFragmentImp::getMetaData()));
    DocumentTypeImp::setStaticPrivate(new NativeClass(global, DocumentType::getMetaData()));
    ElementImp::setStaticPrivate(new NativeClass(global, Element::getMetaData()));
    ProcessingInstructionImp::setStaticPrivate(new NativeClass(global, ProcessingInstructionImp::getMetaData()));

    // event
    UIEventImp::setStaticPrivate(new NativeClass(global, events::UIEvent::getMetaData()));
    MouseEventImp::setStaticPrivate(new NativeClass(global, MouseEventImp::getMetaData()));
    DragEventImp::setStaticPrivate(new NativeClass(global, DragEventImp::getMetaData()));
    MutationEventImp::setStaticPrivate(new NativeClass(global, MutationEventImp::getMetaData()));
    PageTransitionEventImp::setStaticPrivate(new NativeClass(global, PageTransitionEventImp::getMetaData()));
    PopStateEventImp::setStaticPrivate(new NativeClass(global, PopStateEventImp::getMetaData()));
    ProgressEventImp::setStaticPrivate(new NativeClass(global, ProgressEventImp::getMetaData()));
    TextEventImp::setStaticPrivate(new NativeClass(global, TextEventImp::getMetaData()));
    UndoManagerEventImp::setStaticPrivate(new NativeClass(global, UndoManagerEventImp::getMetaData()));
    WheelEventImp::setStaticPrivate(new NativeClass(global, WheelEventImp::getMetaData()));
    BeforeUnloadEventImp::setStaticPrivate(new NativeClass(global, BeforeUnloadEventImp::getMetaData()));
    CompositionEventImp::setStaticPrivate(new NativeClass(global, CompositionEventImp::getMetaData()));
    CustomEventImp::setStaticPrivate(new NativeClass(global, CustomEventImp::getMetaData()));
    FocusEventImp::setStaticPrivate(new NativeClass(global, FocusEventImp::getMetaData()));
    HashChangeEventImp::setStaticPrivate(new NativeClass(global, HashChangeEventImp::getMetaData()));
    KeyboardEventImp::setStaticPrivate(new NativeClass(global, KeyboardEventImp::getMetaData()));
    MessageEventImp::setStaticPrivate(new NativeClass(global, MessageEventImp::getMetaData()));
    MutationNameEventImp::setStaticPrivate(new NativeClass(global, MutationNameEventImp::getMetaData()));

    // character data
    CommentImp::setStaticPrivate(new NativeClass(global, Comment::getMetaData()));
    TextImp::setStaticPrivate(new NativeClass(global, Text::getMetaData()));

    // html element
    HTMLElementImp::setStaticPrivate(new NativeClass(global, HTMLElementImp::getMetaData()));
    HTMLAnchorElementImp::setStaticPrivate(new NativeClass(global, HTMLAnchorElementImp::getMetaData()));
    HTMLAppletElementImp::setStaticPrivate(new NativeClass(global, HTMLAppletElementImp::getMetaData()));
    HTMLAreaElementImp::setStaticPrivate(new NativeClass(global, HTMLAreaElementImp::getMetaData()));
    HTMLBRElementImp::setStaticPrivate(new NativeClass(global, HTMLBRElementImp::getMetaData()));
    HTMLBaseElementImp::setStaticPrivate(new NativeClass(global, HTMLBaseElementImp::getMetaData()));
    HTMLBaseFontElementImp::setStaticPrivate(new NativeClass(global, HTMLBaseFontElementImp::getMetaData()));
    HTMLBindingElementImp::setStaticPrivate(new NativeClass(global, HTMLBindingElementImp::getMetaData()));
    HTMLBodyElementImp::setStaticPrivate(new NativeClass(global, HTMLBodyElementImp::getMetaData()));
    HTMLButtonElementImp::setStaticPrivate(new NativeClass(global, HTMLButtonElementImp::getMetaData()));
    HTMLCanvasElementImp::setStaticPrivate(new NativeClass(global, HTMLCanvasElementImp::getMetaData()));
    HTMLCommandElementImp::setStaticPrivate(new NativeClass(global, HTMLCommandElementImp::getMetaData()));
    HTMLDListElementImp::setStaticPrivate(new NativeClass(global, HTMLDListElementImp::getMetaData()));
    HTMLDataListElementImp::setStaticPrivate(new NativeClass(global, HTMLDataListElementImp::getMetaData()));
    HTMLDetailsElementImp::setStaticPrivate(new NativeClass(global, HTMLDetailsElementImp::getMetaData()));
    HTMLDirectoryElementImp::setStaticPrivate(new NativeClass(global, HTMLDirectoryElementImp::getMetaData()));
    HTMLDivElementImp::setStaticPrivate(new NativeClass(global, HTMLDivElementImp::getMetaData()));
    HTMLDocumentImp::setStaticPrivate(new NativeClass(global, HTMLDocumentImp::getMetaData()));
    HTMLEmbedElementImp::setStaticPrivate(new NativeClass(global, HTMLEmbedElementImp::getMetaData()));
    HTMLFieldSetElementImp::setStaticPrivate(new NativeClass(global, HTMLFieldSetElementImp::getMetaData()));
    HTMLFontElementImp::setStaticPrivate(new NativeClass(global, HTMLFontElementImp::getMetaData()));
    HTMLFormElementImp::setStaticPrivate(new NativeClass(global, HTMLFormElementImp::getMetaData()));
    HTMLFrameElementImp::setStaticPrivate(new NativeClass(global, HTMLFrameElementImp::getMetaData()));
    HTMLFrameSetElementImp::setStaticPrivate(new NativeClass(global, HTMLFrameSetElementImp::getMetaData()));
    HTMLHRElementImp::setStaticPrivate(new NativeClass(global, HTMLHRElementImp::getMetaData()));
    HTMLHeadElementImp::setStaticPrivate(new NativeClass(global, HTMLHeadElementImp::getMetaData()));
    HTMLHeadingElementImp::setStaticPrivate(new NativeClass(global, HTMLHeadingElementImp::getMetaData()));
    HTMLHtmlElementImp::setStaticPrivate(new NativeClass(global, HTMLHtmlElementImp::getMetaData()));
    HTMLIFrameElementImp::setStaticPrivate(new NativeClass(global, HTMLIFrameElementImp::getMetaData()));
    HTMLImageElementImp::setStaticPrivate(new NativeClass(global, HTMLImageElementImp::getMetaData()));
    HTMLInputElementImp::setStaticPrivate(new NativeClass(global, HTMLInputElementImp::getMetaData()));
    HTMLKeygenElementImp::setStaticPrivate(new NativeClass(global, HTMLKeygenElementImp::getMetaData()));
    HTMLLIElementImp::setStaticPrivate(new NativeClass(global, HTMLLIElementImp::getMetaData()));
    HTMLLabelElementImp::setStaticPrivate(new NativeClass(global, HTMLLabelElementImp::getMetaData()));
    HTMLLegendElementImp::setStaticPrivate(new NativeClass(global, HTMLLegendElementImp::getMetaData()));
    HTMLLinkElementImp::setStaticPrivate(new NativeClass(global, HTMLLinkElementImp::getMetaData()));
    HTMLMapElementImp::setStaticPrivate(new NativeClass(global, HTMLMapElementImp::getMetaData()));
    HTMLMarqueeElementImp::setStaticPrivate(new NativeClass(global, HTMLMarqueeElementImp::getMetaData()));
    HTMLMenuElementImp::setStaticPrivate(new NativeClass(global, HTMLMenuElementImp::getMetaData()));
    HTMLMetaElementImp::setStaticPrivate(new NativeClass(global, HTMLMetaElementImp::getMetaData()));
    HTMLMeterElementImp::setStaticPrivate(new NativeClass(global, HTMLMeterElementImp::getMetaData()));
    HTMLModElementImp::setStaticPrivate(new NativeClass(global, HTMLModElementImp::getMetaData()));
    HTMLOListElementImp::setStaticPrivate(new NativeClass(global, HTMLOListElementImp::getMetaData()));
    HTMLObjectElementImp::setStaticPrivate(new NativeClass(global, HTMLObjectElementImp::getMetaData()));
    HTMLOptGroupElementImp::setStaticPrivate(new NativeClass(global, HTMLOptGroupElementImp::getMetaData()));
    HTMLOptionElementImp::setStaticPrivate(new NativeClass(global, HTMLOptionElementImp::getMetaData()));
    HTMLOutputElementImp::setStaticPrivate(new NativeClass(global, HTMLOutputElementImp::getMetaData()));
    HTMLParagraphElementImp::setStaticPrivate(new NativeClass(global, HTMLParagraphElementImp::getMetaData()));
    HTMLParamElementImp::setStaticPrivate(new NativeClass(global, HTMLParamElementImp::getMetaData()));
    HTMLPreElementImp::setStaticPrivate(new NativeClass(global, HTMLPreElementImp::getMetaData()));
    HTMLProgressElementImp::setStaticPrivate(new NativeClass(global, HTMLProgressElementImp::getMetaData()));
    HTMLQuoteElementImp::setStaticPrivate(new NativeClass(global, HTMLQuoteElementImp::getMetaData()));
    HTMLScriptElementImp::setStaticPrivate(new NativeClass(global, HTMLScriptElementImp::getMetaData()));
    HTMLSelectElementImp::setStaticPrivate(new NativeClass(global, HTMLSelectElementImp::getMetaData()));
    HTMLSourceElementImp::setStaticPrivate(new NativeClass(global, HTMLSourceElementImp::getMetaData()));
    HTMLSpanElementImp::setStaticPrivate(new NativeClass(global, HTMLSpanElementImp::getMetaData()));
    HTMLStyleElementImp::setStaticPrivate(new NativeClass(global, HTMLStyleElementImp::getMetaData()));
    HTMLTableCaptionElementImp::setStaticPrivate(new NativeClass(global, HTMLTableCaptionElementImp::getMetaData()));
    HTMLTableCellElementImp::setStaticPrivate(new NativeClass(global, HTMLTableCellElementImp::getMetaData()));
    HTMLTableColElementImp::setStaticPrivate(new NativeClass(global, HTMLTableColElementImp::getMetaData()));
    HTMLTableDataCellElementImp::setStaticPrivate(new NativeClass(global, HTMLTableDataCellElementImp::getMetaData()));
    HTMLTableElementImp::setStaticPrivate(new NativeClass(global, HTMLTableElementImp::getMetaData()));
    HTMLTableHeaderCellElementImp::setStaticPrivate(new NativeClass(global, HTMLTableHeaderCellElementImp::getMetaData()));
    HTMLTableRowElementImp::setStaticPrivate(new NativeClass(global, HTMLTableRowElementImp::getMetaData()));
    HTMLTableSectionElementImp::setStaticPrivate(new NativeClass(global, HTMLTableSectionElementImp::getMetaData()));
    HTMLTemplateElementImp::setStaticPrivate(new NativeClass(global, HTMLTemplateElementImp::getMetaData()));
    HTMLTextAreaElementImp::setStaticPrivate(new NativeClass(global, HTMLTextAreaElementImp::getMetaData()));
    HTMLTimeElementImp::setStaticPrivate(new NativeClass(global, HTMLTimeElementImp::getMetaData()));
    HTMLTitleElementImp::setStaticPrivate(new NativeClass(global, HTMLTitleElementImp::getMetaData()));
    HTMLUListElementImp::setStaticPrivate(new NativeClass(global, HTMLUListElementImp::getMetaData()));
    HTMLUnknownElementImp::setStaticPrivate(new NativeClass(global, HTMLUnknownElementImp::getMetaData()));

    // media element
    HTMLMediaElementImp::setStaticPrivate(new NativeClass(global, HTMLMediaElementImp::getMetaData()));
    HTMLAudioElementImp::setStaticPrivate(new NativeClass(global, HTMLAudioElementImp::getMetaData()));
    HTMLVideoElementImp::setStaticPrivate(new NativeClass(global, HTMLVideoElementImp::getMetaData()));

    // html collection
    HTMLCollectionImp::setStaticPrivate(new NativeClass(global, HTMLCollectionImp::getMetaData()));
    HTMLAllCollectionImp::setStaticPrivate(new NativeClass(global, HTMLAllCollectionImp::getMetaData()));
    HTMLOptionsCollectionImp::setStaticPrivate(new NativeClass(global, HTMLOptionsCollectionImp::getMetaData()));
    HTMLFormControlsCollectionImp::setStaticPrivate(new NativeClass(global, HTMLFormControlsCollectionImp::getMetaData()));
    HTMLPropertiesCollectionImp::setStaticPrivate(new NativeClass(global, HTMLPropertiesCollectionImp::getMetaData()));

    // style sheet
    CSSStyleSheetImp::setStaticPrivate(new NativeClass(global, css::CSSStyleSheet::getMetaData()));

    // css rule
    CSSMediaRuleImp::setStaticPrivate(new NativeClass(global, css::CSSMediaRule::getMetaData()));
    CSSStyleRuleImp::setStaticPrivate(new NativeClass(global, css::CSSStyleRule::getMetaData()));

    // css value
    CSSValueImp::setStaticPrivate(new NativeClass(global, CSSValueImp::getMetaData()));

    // css
    DOMImplementationCSSImp::setStaticPrivate(new NativeClass(global, DOMImplementationCSSImp::getMetaData()));
    DocumentCSSImp::setStaticPrivate(new NativeClass(global, DocumentCSSImp::getMetaData()));
    CSS2PropertiesImp::setStaticPrivate(new NativeClass(global, CSS2PropertiesImp::getMetaData()));
    CSSCharsetRuleImp::setStaticPrivate(new NativeClass(global, CSSCharsetRuleImp::getMetaData()));
    CSSColorComponentValueImp::setStaticPrivate(new NativeClass(global, CSSColorComponentValueImp::getMetaData()));
    CSSComponentValueImp::setStaticPrivate(new NativeClass(global, CSSComponentValueImp::getMetaData()));
    CSSFontFaceRuleImp::setStaticPrivate(new NativeClass(global, CSSFontFaceRuleImp::getMetaData()));
    CSSIdentifierComponentValueImp::setStaticPrivate(new NativeClass(global, CSSIdentifierComponentValueImp::getMetaData()));
    CSSImportRuleImp::setStaticPrivate(new NativeClass(global, CSSImportRuleImp::getMetaData()));
    CSSKeywordComponentValueImp::setStaticPrivate(new NativeClass(global, CSSKeywordComponentValueImp::getMetaData()));
    CSSLengthComponentValueImp::setStaticPrivate(new NativeClass(global, CSSLengthComponentValueImp::getMetaData()));
    CSSMapValueImp::setStaticPrivate(new NativeClass(global, CSSMapValueImp::getMetaData()));
    CSSNamespaceRuleImp::setStaticPrivate(new NativeClass(global, CSSNamespaceRuleImp::getMetaData()));
    CSSPageRuleImp::setStaticPrivate(new NativeClass(global, CSSPageRuleImp::getMetaData()));
    CSSPercentageComponentValueImp::setStaticPrivate(new NativeClass(global, CSSPercentageComponentValueImp::getMetaData()));
    CSSPrimitiveValueImp::setStaticPrivate(new NativeClass(global, CSSPrimitiveValueImp::getMetaData()));
    // CSSPropertyValueImp::setStaticPrivate(new NativeClass(global, CSSPropertyValueImp::getMetaData()));
    CSSPropertyValueListImp::setStaticPrivate(new NativeClass(global, CSSPropertyValueListImp::getMetaData()));
    CSSStringComponentValueImp::setStaticPrivate(new NativeClass(global, CSSStringComponentValueImp::getMetaData()));
    CSSStyleDeclarationValueImp::setStaticPrivate(new NativeClass(global, CSSStyleDeclarationValueImp::getMetaData()));
    CSSURLComponentValueImp::setStaticPrivate(new NativeClass(global, CSSURLComponentValueImp::getMetaData()));
    CSSUnknownRuleImp::setStaticPrivate(new NativeClass(global, CSSUnknownRuleImp::getMetaData()));
    CSSValueListImp::setStaticPrivate(new NativeClass(global, CSSValueListImp::getMetaData()));

    // XMLHttpRequest
    XMLHttpRequestEventTargetImp::setStaticPrivate(new NativeClass(global, XMLHttpRequestEventTargetImp::getMetaData()));
    XMLHttpRequestImp::setStaticPrivate(new NativeClass(global, XMLHttpRequestImp::getMetaData()));
    AnonXMLHttpRequestImp::setStaticPrivate(new NativeClass(global, AnonXMLHttpRequestImp::getMetaData()));
    XMLHttpRequestUploadImp::setStaticPrivate(new NativeClass(global, XMLHttpRequestUploadImp::getMetaData()));

    // file
    FileErrorImp::setStaticPrivate(new NativeClass(global, FileErrorImp::getMetaData()));
    FileImp::setStaticPrivate(new NativeClass(global, FileImp::getMetaData()));
    FileListImp::setStaticPrivate(new NativeClass(global, FileListImp::getMetaData()));
    FileReaderImp::setStaticPrivate(new NativeClass(global, FileReaderImp::getMetaData()));
    FileReaderSyncImp::setStaticPrivate(new NativeClass(global, FileReaderSyncImp::getMetaData()));

    // new stuff
    EventInitImp::setStaticPrivate(new NativeClass(global, EventInitImp::getMetaData()));
    CustomEventInitImp::setStaticPrivate(new NativeClass(global, CustomEventInitImp::getMetaData()));
    DataTransferItemImp::setStaticPrivate(new NativeClass(global, DataTransferItemImp::getMetaData()));
    DataTransferItemListImp::setStaticPrivate(new NativeClass(global, DataTransferItemListImp::getMetaData()));
}

}  // namespace

v8::Persistent<v8::ObjectTemplate> ECMAScriptContext::Impl::getGlobalTemplate() {
    static bool initilized = false;
    static v8::Persistent<v8::ObjectTemplate> globalTemplate;
    if (initilized)
        return globalTemplate;
    globalTemplate = v8::Persistent<v8::ObjectTemplate>::New(v8::ObjectTemplate::New());
    registerClasses(globalTemplate);
    globalTemplate->SetInternalFieldCount(1);
    initilized = true;
    return globalTemplate;
}

ECMAScriptContext::Impl::Impl()
{
    const char* extensionNames[] = {
        "v8/gc",
    };
    v8::ExtensionConfiguration extensions(1, extensionNames);
    context = v8::Context::New(&extensions, getGlobalTemplate());
    context->Enter();
    v8::Handle<v8::Object> globalProxy = context->Global();
    v8::Handle<v8::Object> global = globalProxy->GetPrototype().As<v8::Object>();
    assert(global->InternalFieldCount() == 1);
    Reflect::Interface globalMeta(html::Window::getMetaData());
    std::string name = Reflect::getIdentifier(globalMeta.getName());
    if (0 < name.length()) {
        auto window = v8::Local<v8::Function>::Cast(globalProxy->Get(v8::String::New(name.c_str(), name.length())));
        v8::Local<v8::Value> prototype = window->Get(v8::String::New("prototype"));
        global->SetPrototype(prototype);
    }
}

ECMAScriptContext::Impl::~Impl()
{
    v8::Local<v8::Context> current = v8::Context::GetCurrent();
    if (current == context)
        current->Exit();
    context.Dispose();
    context.Clear();

}

void ECMAScriptContext::Impl::activate(ObjectImp* window)
{
    v8::Local<v8::Context> current = v8::Context::GetCurrent();
    if (current != context)
        current->Exit();
    context->Enter();

    v8::Handle<v8::Object> globalProxy = context->Global();
    v8::Handle<v8::Object> global = globalProxy->GetPrototype().As<v8::Object>();
    global->SetInternalField(0, v8::External::New(window));
    window->setPrivate(*context->Global());
}

void ECMAScriptContext::Impl::shutDown()
{
    getGlobalTemplate().Dispose();
    getGlobalTemplate().Clear();
}

ECMAScriptContext::ECMAScriptContext()
  : pimpl(new Impl())
{
}

ECMAScriptContext::~ECMAScriptContext()
{
}

void ECMAScriptContext::activate(ObjectImp* window)
{
    pimpl->activate(window);
}

Object* ECMAScriptContext::compileFunction(const std::u16string& body)
{
    return pimpl->compileFunction(body);
}

Object* ECMAScriptContext::xblCreateImplementation(Object object, Object prototype, Object boundElement, Object shadowTree)
{
    return pimpl->xblCreateImplementation(object, prototype, boundElement, shadowTree);
}

void ECMAScriptContext::shutDown()
{
    Impl::shutDown();
}
