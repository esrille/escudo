/*
 * Copyright 2011-2013 Esrille Inc.
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

#include "AttrImp.h"
#include "CaretPositionImp.h"
#include "CharacterDataImp.h"
#include "CommentImp.h"
#include "DocumentFragmentImp.h"
#include "DocumentImp.h"
#include "DocumentTypeImp.h"
#include "DOMErrorImp.h"
// #include "DOMExceptionImp.h"
#include "DOMImplementationImp.h"
#include "DOMSettableTokenListImp.h"
#include "DOMStringListImp.h"
#include "DOMStringMapImp.h"
#include "DOMTokenListImp.h"
#include "ElementImp.h"
#include "NodeImp.h"
#include "NodeListImp.h"
#include "ProcessingInstructionImp.h"
#include "TextImp.h"
#include "XMLDocumentImp.h"

#include "css/CounterImp.h"
#include "css/CSS2PropertiesImp.h"
#include "css/CSSCharsetRuleImp.h"
#include "css/CSSFontFaceRuleImp.h"
#include "css/CSSImportRuleImp.h"
#include "css/CSSMediaRuleImp.h"
#include "css/CSSNamespaceRuleImp.h"
#include "css/CSSPageRuleImp.h"
#include "css/CSSPrimitiveValueImp.h"
#include "css/CSSRuleImp.h"
#include "css/CSSRuleListImp.h"
#include "css/CSSStyleDeclarationImp.h"
#include "css/CSSStyleRuleImp.h"
#include "css/CSSStyleSheetImp.h"
#include "css/CSSUnknownRuleImp.h"
#include "css/CSSValueImp.h"
#include "css/CSSValueListImp.h"
// #include "css/DocumentCSSImp.h"
// #include "css/DOMImplementationCSSImp.h"
// #include "css/ElementCSSInlineStyleImp.h"
#include "css/RectImp.h"
#include "css/RGBColorImp.h"

// events
#include "CompositionEventImp.h"
#include "CompositionEventInitImp.h"
#include "CustomEventImp.h"
#include "CustomEventInitImp.h"
#include "EventImp.h"
#include "EventHandlerNonNullImp.h"
#include "EventInitImp.h"
#include "EventListenerImp.h"
#include "EventTargetImp.h"
// #include "EventXBLImp.h"
#include "FocusEventImp.h"
#include "FocusEventInitImp.h"
#include "KeyboardEventImp.h"
#include "KeyboardEventInitImp.h"
#include "MouseEventImp.h"
#include "MouseEventInitImp.h"
#include "MutationCallbackImp.h"
#include "MutationEventImp.h"
#include "MutationObserverImp.h"
#include "MutationObserverInitImp.h"
#include "MutationRecordImp.h"
#include "OnErrorEventHandlerNonNullImp.h"
#include "ProgressEventImp.h"
#include "UIEventImp.h"
#include "UIEventInitImp.h"
#include "WheelEventImp.h"
#include "WheelEventInitImp.h"

// eventsource
#include "EventSourceImp.h"

// file
#include "file/BlobImp.h"
#include "file/BlobPropertyBagImp.h"
#include "file/FileCallbackImp.h"
#include "file/FileImp.h"
#include "file/FileListImp.h"
#include "file/FileReaderImp.h"
#include "file/FileReaderSyncImp.h"
#include "file/LineEndingsImp.h"

// html
#include "html/ApplicationCacheImp.h"
#include "html/AudioTrackImp.h"
#include "html/AudioTrackListImp.h"
#include "html/BarPropImp.h"
#include "BeforeUnloadEventImp.h"
#include "html/CanvasDrawingStylesImp.h"
#include "html/CanvasGradientImp.h"
#include "html/CanvasPathMethodsImp.h"
#include "html/CanvasPatternImp.h"
#include "html/CanvasRenderingContext2DImp.h"
#include "html/DataTransferImp.h"
#include "html/DataTransferItemImp.h"
#include "html/DataTransferItemListImp.h"
#include "DragEventImp.h"
#include "DragEventInitImp.h"
#include "html/DrawingStyleImp.h"
#include "ExternalImp.h"
#include "FunctionStringCallbackImp.h"
#include "HashChangeEventImp.h"
#include "HashChangeEventInitImp.h"
#include "HistoryImp.h"
#include "html/HitRegionOptionsImp.h"
#include "html/HTMLAllCollectionImp.h"
#include "html/HTMLAnchorElementImp.h"
#include "html/HTMLAppletElementImp.h"
#include "html/HTMLAreaElementImp.h"
#include "html/HTMLAudioElementImp.h"
#include "html/HTMLBaseElementImp.h"
#include "html/HTMLBaseFontElementImp.h"
#include "html/HTMLBindingElementImp.h"
#include "html/HTMLBodyElementImp.h"
#include "html/HTMLBRElementImp.h"
#include "html/HTMLButtonElementImp.h"
#include "html/HTMLCanvasElementImp.h"
#include "html/HTMLCollectionImp.h"
#include "html/HTMLCommandElementImp.h"
#include "html/HTMLDataListElementImp.h"
#include "html/HTMLDetailsElementImp.h"
#include "html/HTMLDialogElementImp.h"
#include "html/HTMLDirectoryElementImp.h"
#include "html/HTMLDivElementImp.h"
#include "html/HTMLDListElementImp.h"
#include "html/HTMLElementImp.h"
#include "html/HTMLEmbedElementImp.h"
#include "html/HTMLFieldSetElementImp.h"
#include "html/HTMLFontElementImp.h"
#include "html/HTMLFormControlsCollectionImp.h"
#include "html/HTMLFormElementImp.h"
#include "html/HTMLFrameElementImp.h"
#include "html/HTMLFrameSetElementImp.h"
#include "html/HTMLHeadElementImp.h"
#include "html/HTMLHeadingElementImp.h"
#include "html/HTMLHRElementImp.h"
#include "html/HTMLHtmlElementImp.h"
#include "html/HTMLIFrameElementImp.h"
#include "html/HTMLImageElementImp.h"
#include "html/HTMLInputElementImp.h"
#include "html/HTMLKeygenElementImp.h"
#include "html/HTMLLabelElementImp.h"
#include "html/HTMLLegendElementImp.h"
#include "html/HTMLLIElementImp.h"
#include "html/HTMLLinkElementImp.h"
#include "html/HTMLMapElementImp.h"
#include "html/HTMLMarqueeElementImp.h"
#include "html/HTMLMediaElementImp.h"
#include "html/HTMLMenuElementImp.h"
#include "html/HTMLMetaElementImp.h"
#include "html/HTMLMeterElementImp.h"
#include "html/HTMLModElementImp.h"
#include "html/HTMLObjectElementImp.h"
#include "html/HTMLOListElementImp.h"
#include "html/HTMLOptGroupElementImp.h"
#include "html/HTMLOptionElementImp.h"
#include "html/HTMLOptionsCollectionImp.h"
#include "html/HTMLOutputElementImp.h"
#include "html/HTMLParagraphElementImp.h"
#include "html/HTMLParamElementImp.h"
#include "html/HTMLPreElementImp.h"
#include "html/HTMLProgressElementImp.h"
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
#include "html/HTMLTrackElementImp.h"
#include "html/HTMLUListElementImp.h"
#include "html/HTMLUnknownElementImp.h"
#include "html/HTMLVideoElementImp.h"
#include "html/ImageDataImp.h"
#include "LocationImp.h"
#include "html/MediaControllerImp.h"
#include "html/MediaErrorImp.h"
#include "html/MediaQueryListImp.h"
#include "html/MediaQueryListListenerImp.h"
#include "html/MessageChannelImp.h"
#include "MessageEventImp.h"
#include "MessageEventInitImp.h"
#include "html/MessagePortImp.h"
#include "NavigatorContentUtilsImp.h"
#include "NavigatorImp.h"
#include "NavigatorIDImp.h"
#include "NavigatorOnLineImp.h"
#include "NavigatorStorageUtilsImp.h"
#include "PageTransitionEventImp.h"
#include "PageTransitionEventInitImp.h"
#include "html/PathImp.h"
#include "PopStateEventImp.h"
#include "PopStateEventInitImp.h"
#include "html/RadioNodeListImp.h"
#include "html/ScreenImp.h"
#include "html/TextMetricsImp.h"
#include "html/TextTrackCueImp.h"
#include "html/TextTrackCueListImp.h"
#include "html/TextTrackImp.h"
#include "html/TextTrackListImp.h"
#include "html/TimeRangesImp.h"
#include "TrackEventImp.h"
#include "TrackEventInitImp.h"
#include "html/TransferableImp.h"
#include "html/ValidityStateImp.h"
#include "html/VideoTrackImp.h"
#include "html/VideoTrackListImp.h"
// #include "WindowBase64Imp.h"
#include "WindowProxy.h"
// #include "WindowModalImp.h"
// #include "WindowTimersImp.h"

// ranges
// #include "RangeExceptionImp.h"
#include "RangeImp.h"

// stylesheets
#include "MediaListImp.h"
#include "StyleSheetImp.h"

// svg
#include "svg/SVGMatrixImp.h"

// traversal
#include "NodeFilterImp.h"
#include "NodeIteratorImp.h"
#include "TreeWalkerImp.h"

// typedarray
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
#include "typedarray/Uint8ClampedArrayImp.h"

// views
#include "ClientRectImp.h"
#include "ClientRectListImp.h"

// xbl2
// #include "xbl/DocumentXBLImp.h"
// #include "xbl/ElementXBLImp.h"
#include "xbl/XBLContentElementImp.h"
#include "xbl/XBLImplementationImp.h"
#include "xbl/XBLImplementationListImp.h"

// xhr
#include "xhr/FormDataImp.h"
#include "xhr/XMLHttpRequestEventTargetImp.h"
#include "xhr/XMLHttpRequestImp.h"
#include "xhr/XMLHttpRequestOptionsImp.h"
#include "xhr/XMLHttpRequestUploadImp.h"

#include "utf.h"

using namespace org::w3c::dom::bootstrap;
using namespace org::w3c::dom;

// JSAPI bridge

namespace {

void reportError(v8::Handle<v8::Message> message, v8::Handle<v8::Value> data)
{
    v8::String::Value s(message->Get());
    std::u16string m(reinterpret_cast<const char16_t*>(*s));
    std::cerr << message->GetLineNumber() << ": " << m << '\n';
}

void registerClasses(v8::Handle<v8::ObjectTemplate> global)
{
    // prototype roots
    AttrImp::setStaticPrivate(new NativeClass(global, AttrImp::getMetaData()));
    CSSRuleImp::setStaticPrivate(new NativeClass(global, CSSRuleImp::getMetaData()));
    CSSStyleDeclarationImp::setStaticPrivate(new NativeClass(global, CSSStyleDeclarationImp::getMetaData()));
    CSSValueImp::setStaticPrivate(new NativeClass(global, CSSValueImp::getMetaData()));
    DOMImplementationImp::setStaticPrivate(new NativeClass(global, DOMImplementationImp::getMetaData()));
    DOMTokenListImp::setStaticPrivate(new NativeClass(global, DOMTokenListImp::getMetaData()));
    EventImp::setStaticPrivate(new NativeClass(global, EventImp::getMetaData()));
    EventInitImp::setStaticPrivate(new NativeClass(global, EventInitImp::getMetaData()));
    EventTargetImp::setStaticPrivate(new NativeClass(global, EventTargetImp::getMetaData()));
    HTMLCollectionImp::setStaticPrivate(new NativeClass(global, HTMLCollectionImp::getMetaData()));
    LocationImp::setStaticPrivate(new NativeClass(global, LocationImp::getMetaData()));
    MediaListImp::setStaticPrivate(new NativeClass(global, MediaListImp::getMetaData()));
    NodeImp::setStaticPrivate(new NativeClass(global, NodeImp::getMetaData()));
    NodeListImp::setStaticPrivate(new NativeClass(global, NodeListImp::getMetaData()));
    StyleSheetImp::setStaticPrivate(new NativeClass(global, StyleSheetImp::getMetaData()));
    UIEventImp::setStaticPrivate(new NativeClass(global, UIEventImp::getMetaData()));

    ElementImp::setStaticPrivate(new NativeClass(global, ElementImp::getMetaData()));
    HTMLElementImp::setStaticPrivate(new NativeClass(global, HTMLElementImp::getMetaData()));
    HTMLMediaElementImp::setStaticPrivate(new NativeClass(global, HTMLMediaElementImp::getMetaData()));

//    AttrImp::setStaticPrivate(new NativeClass(global, AttrImp::getMetaData()));
    CaretPositionImp::setStaticPrivate(new NativeClass(global, CaretPositionImp::getMetaData()));
    CharacterDataImp::setStaticPrivate(new NativeClass(global, CharacterDataImp::getMetaData()));
    CommentImp::setStaticPrivate(new NativeClass(global, CommentImp::getMetaData()));
    DocumentFragmentImp::setStaticPrivate(new NativeClass(global, DocumentFragmentImp::getMetaData()));
    DocumentImp::setStaticPrivate(new NativeClass(global, DocumentImp::getMetaData()));
    DocumentTypeImp::setStaticPrivate(new NativeClass(global, DocumentTypeImp::getMetaData()));
    DOMErrorImp::setStaticPrivate(new NativeClass(global, DOMErrorImp::getMetaData()));
//    DOMImplementationImp::setStaticPrivate(new NativeClass(global, DOMImplementationImp::getMetaData()));
    DOMSettableTokenListImp::setStaticPrivate(new NativeClass(global, DOMSettableTokenListImp::getMetaData()));
    DOMStringListImp::setStaticPrivate(new NativeClass(global, DOMStringListImp::getMetaData()));
    DOMStringMapImp::setStaticPrivate(new NativeClass(global, DOMStringMapImp::getMetaData()));
//    DOMTokenListImp::setStaticPrivate(new NativeClass(global, DOMTokenListImp::getMetaData()));
//    ElementImp::setStaticPrivate(new NativeClass(global, ElementImp::getMetaData()));
//    NodeImp::setStaticPrivate(new NativeClass(global, NodeImp::getMetaData()));
//    NodeListImp::setStaticPrivate(new NativeClass(global, NodeListImp::getMetaData()));
    ProcessingInstructionImp::setStaticPrivate(new NativeClass(global, ProcessingInstructionImp::getMetaData()));
    TextImp::setStaticPrivate(new NativeClass(global, TextImp::getMetaData()));
    XMLDocumentImp::setStaticPrivate(new NativeClass(global, XMLDocumentImp::getMetaData()));
    CounterImp::setStaticPrivate(new NativeClass(global, CounterImp::getMetaData()));
    CSS2PropertiesImp::setStaticPrivate(new NativeClass(global, CSS2PropertiesImp::getMetaData()));
    CSSCharsetRuleImp::setStaticPrivate(new NativeClass(global, CSSCharsetRuleImp::getMetaData()));
    CSSFontFaceRuleImp::setStaticPrivate(new NativeClass(global, CSSFontFaceRuleImp::getMetaData()));
    CSSImportRuleImp::setStaticPrivate(new NativeClass(global, CSSImportRuleImp::getMetaData()));
    CSSMediaRuleImp::setStaticPrivate(new NativeClass(global, CSSMediaRuleImp::getMetaData()));
    CSSNamespaceRuleImp::setStaticPrivate(new NativeClass(global, CSSNamespaceRuleImp::getMetaData()));
    CSSPageRuleImp::setStaticPrivate(new NativeClass(global, CSSPageRuleImp::getMetaData()));
    CSSPrimitiveValueImp::setStaticPrivate(new NativeClass(global, CSSPrimitiveValueImp::getMetaData()));
//    CSSRuleImp::setStaticPrivate(new NativeClass(global, CSSRuleImp::getMetaData()));
    CSSRuleListImp::setStaticPrivate(new NativeClass(global, CSSRuleListImp::getMetaData()));
//    CSSStyleDeclarationImp::setStaticPrivate(new NativeClass(global, CSSStyleDeclarationImp::getMetaData()));
    CSSStyleRuleImp::setStaticPrivate(new NativeClass(global, CSSStyleRuleImp::getMetaData()));
    CSSStyleSheetImp::setStaticPrivate(new NativeClass(global, CSSStyleSheetImp::getMetaData()));
    CSSUnknownRuleImp::setStaticPrivate(new NativeClass(global, CSSUnknownRuleImp::getMetaData()));
//    CSSValueImp::setStaticPrivate(new NativeClass(global, CSSValueImp::getMetaData()));
    CSSValueListImp::setStaticPrivate(new NativeClass(global, CSSValueListImp::getMetaData()));
    RectImp::setStaticPrivate(new NativeClass(global, RectImp::getMetaData()));
    RGBColorImp::setStaticPrivate(new NativeClass(global, RGBColorImp::getMetaData()));
    CompositionEventImp::setStaticPrivate(new NativeClass(global, CompositionEventImp::getMetaData()));
    CompositionEventInitImp::setStaticPrivate(new NativeClass(global, CompositionEventInitImp::getMetaData()));
    CustomEventImp::setStaticPrivate(new NativeClass(global, CustomEventImp::getMetaData()));
    CustomEventInitImp::setStaticPrivate(new NativeClass(global, CustomEventInitImp::getMetaData()));
//    EventImp::setStaticPrivate(new NativeClass(global, EventImp::getMetaData()));
    EventHandlerNonNullImp::setStaticPrivate(new NativeClass(global, EventHandlerNonNullImp::getMetaData()));
//    EventInitImp::setStaticPrivate(new NativeClass(global, EventInitImp::getMetaData()));
    EventListenerImp::setStaticPrivate(new NativeClass(global, EventListenerImp::getMetaData()));
//    EventTargetImp::setStaticPrivate(new NativeClass(global, EventTargetImp::getMetaData()));
    FocusEventImp::setStaticPrivate(new NativeClass(global, FocusEventImp::getMetaData()));
    FocusEventInitImp::setStaticPrivate(new NativeClass(global, FocusEventInitImp::getMetaData()));
    KeyboardEventImp::setStaticPrivate(new NativeClass(global, KeyboardEventImp::getMetaData()));
    KeyboardEventInitImp::setStaticPrivate(new NativeClass(global, KeyboardEventInitImp::getMetaData()));
    MouseEventImp::setStaticPrivate(new NativeClass(global, MouseEventImp::getMetaData()));
    MouseEventInitImp::setStaticPrivate(new NativeClass(global, MouseEventInitImp::getMetaData()));
    MutationCallbackImp::setStaticPrivate(new NativeClass(global, MutationCallbackImp::getMetaData()));
    MutationEventImp::setStaticPrivate(new NativeClass(global, MutationEventImp::getMetaData()));
    MutationObserverImp::setStaticPrivate(new NativeClass(global, MutationObserverImp::getMetaData()));
    MutationObserverInitImp::setStaticPrivate(new NativeClass(global, MutationObserverInitImp::getMetaData()));
    MutationRecordImp::setStaticPrivate(new NativeClass(global, MutationRecordImp::getMetaData()));
    OnErrorEventHandlerNonNullImp::setStaticPrivate(new NativeClass(global, OnErrorEventHandlerNonNullImp::getMetaData()));
    ProgressEventImp::setStaticPrivate(new NativeClass(global, ProgressEventImp::getMetaData()));
//    UIEventImp::setStaticPrivate(new NativeClass(global, UIEventImp::getMetaData()));
    UIEventInitImp::setStaticPrivate(new NativeClass(global, UIEventInitImp::getMetaData()));
    WheelEventImp::setStaticPrivate(new NativeClass(global, WheelEventImp::getMetaData()));
    WheelEventInitImp::setStaticPrivate(new NativeClass(global, WheelEventInitImp::getMetaData()));
    EventSourceImp::setStaticPrivate(new NativeClass(global, EventSourceImp::getMetaData()));
    BlobImp::setStaticPrivate(new NativeClass(global, BlobImp::getMetaData()));
    BlobPropertyBagImp::setStaticPrivate(new NativeClass(global, BlobPropertyBagImp::getMetaData()));
    FileCallbackImp::setStaticPrivate(new NativeClass(global, FileCallbackImp::getMetaData()));
    FileImp::setStaticPrivate(new NativeClass(global, FileImp::getMetaData()));
    FileListImp::setStaticPrivate(new NativeClass(global, FileListImp::getMetaData()));
    FileReaderImp::setStaticPrivate(new NativeClass(global, FileReaderImp::getMetaData()));
    FileReaderSyncImp::setStaticPrivate(new NativeClass(global, FileReaderSyncImp::getMetaData()));
    LineEndingsImp::setStaticPrivate(new NativeClass(global, LineEndingsImp::getMetaData()));
    ApplicationCacheImp::setStaticPrivate(new NativeClass(global, ApplicationCacheImp::getMetaData()));
    AudioTrackImp::setStaticPrivate(new NativeClass(global, AudioTrackImp::getMetaData()));
    AudioTrackListImp::setStaticPrivate(new NativeClass(global, AudioTrackListImp::getMetaData()));
    BarPropImp::setStaticPrivate(new NativeClass(global, BarPropImp::getMetaData()));
    BeforeUnloadEventImp::setStaticPrivate(new NativeClass(global, BeforeUnloadEventImp::getMetaData()));
    CanvasDrawingStylesImp::setStaticPrivate(new NativeClass(global, CanvasDrawingStylesImp::getMetaData()));
    CanvasGradientImp::setStaticPrivate(new NativeClass(global, CanvasGradientImp::getMetaData()));
    CanvasPathMethodsImp::setStaticPrivate(new NativeClass(global, CanvasPathMethodsImp::getMetaData()));
    CanvasPatternImp::setStaticPrivate(new NativeClass(global, CanvasPatternImp::getMetaData()));
    CanvasRenderingContext2DImp::setStaticPrivate(new NativeClass(global, CanvasRenderingContext2DImp::getMetaData()));
    DataTransferImp::setStaticPrivate(new NativeClass(global, DataTransferImp::getMetaData()));
    DataTransferItemImp::setStaticPrivate(new NativeClass(global, DataTransferItemImp::getMetaData()));
    DataTransferItemListImp::setStaticPrivate(new NativeClass(global, DataTransferItemListImp::getMetaData()));
    DragEventImp::setStaticPrivate(new NativeClass(global, DragEventImp::getMetaData()));
    DragEventInitImp::setStaticPrivate(new NativeClass(global, DragEventInitImp::getMetaData()));
    DrawingStyleImp::setStaticPrivate(new NativeClass(global, DrawingStyleImp::getMetaData()));
    ExternalImp::setStaticPrivate(new NativeClass(global, ExternalImp::getMetaData()));
    FunctionStringCallbackImp::setStaticPrivate(new NativeClass(global, FunctionStringCallbackImp::getMetaData()));
    HashChangeEventImp::setStaticPrivate(new NativeClass(global, HashChangeEventImp::getMetaData()));
    HashChangeEventInitImp::setStaticPrivate(new NativeClass(global, HashChangeEventInitImp::getMetaData()));
    HistoryImp::setStaticPrivate(new NativeClass(global, HistoryImp::getMetaData()));
    HitRegionOptionsImp::setStaticPrivate(new NativeClass(global, HitRegionOptionsImp::getMetaData()));
    HTMLAllCollectionImp::setStaticPrivate(new NativeClass(global, HTMLAllCollectionImp::getMetaData()));
    HTMLAnchorElementImp::setStaticPrivate(new NativeClass(global, HTMLAnchorElementImp::getMetaData()));
    HTMLAppletElementImp::setStaticPrivate(new NativeClass(global, HTMLAppletElementImp::getMetaData()));
    HTMLAreaElementImp::setStaticPrivate(new NativeClass(global, HTMLAreaElementImp::getMetaData()));
    HTMLAudioElementImp::setStaticPrivate(new NativeClass(global, HTMLAudioElementImp::getMetaData()));
    HTMLBaseElementImp::setStaticPrivate(new NativeClass(global, HTMLBaseElementImp::getMetaData()));
    HTMLBaseFontElementImp::setStaticPrivate(new NativeClass(global, HTMLBaseFontElementImp::getMetaData()));
    HTMLBindingElementImp::setStaticPrivate(new NativeClass(global, HTMLBindingElementImp::getMetaData()));
    HTMLBodyElementImp::setStaticPrivate(new NativeClass(global, HTMLBodyElementImp::getMetaData()));
    HTMLBRElementImp::setStaticPrivate(new NativeClass(global, HTMLBRElementImp::getMetaData()));
    HTMLButtonElementImp::setStaticPrivate(new NativeClass(global, HTMLButtonElementImp::getMetaData()));
    HTMLCanvasElementImp::setStaticPrivate(new NativeClass(global, HTMLCanvasElementImp::getMetaData()));
//    HTMLCollectionImp::setStaticPrivate(new NativeClass(global, HTMLCollectionImp::getMetaData()));
    HTMLCommandElementImp::setStaticPrivate(new NativeClass(global, HTMLCommandElementImp::getMetaData()));
    HTMLDataListElementImp::setStaticPrivate(new NativeClass(global, HTMLDataListElementImp::getMetaData()));
    HTMLDetailsElementImp::setStaticPrivate(new NativeClass(global, HTMLDetailsElementImp::getMetaData()));
    HTMLDialogElementImp::setStaticPrivate(new NativeClass(global, HTMLDialogElementImp::getMetaData()));
    HTMLDirectoryElementImp::setStaticPrivate(new NativeClass(global, HTMLDirectoryElementImp::getMetaData()));
    HTMLDivElementImp::setStaticPrivate(new NativeClass(global, HTMLDivElementImp::getMetaData()));
    HTMLDListElementImp::setStaticPrivate(new NativeClass(global, HTMLDListElementImp::getMetaData()));
//    HTMLElementImp::setStaticPrivate(new NativeClass(global, HTMLElementImp::getMetaData()));
    HTMLEmbedElementImp::setStaticPrivate(new NativeClass(global, HTMLEmbedElementImp::getMetaData()));
    HTMLFieldSetElementImp::setStaticPrivate(new NativeClass(global, HTMLFieldSetElementImp::getMetaData()));
    HTMLFontElementImp::setStaticPrivate(new NativeClass(global, HTMLFontElementImp::getMetaData()));
    HTMLFormControlsCollectionImp::setStaticPrivate(new NativeClass(global, HTMLFormControlsCollectionImp::getMetaData()));
    HTMLFormElementImp::setStaticPrivate(new NativeClass(global, HTMLFormElementImp::getMetaData()));
    HTMLFrameElementImp::setStaticPrivate(new NativeClass(global, HTMLFrameElementImp::getMetaData()));
    HTMLFrameSetElementImp::setStaticPrivate(new NativeClass(global, HTMLFrameSetElementImp::getMetaData()));
    HTMLHeadElementImp::setStaticPrivate(new NativeClass(global, HTMLHeadElementImp::getMetaData()));
    HTMLHeadingElementImp::setStaticPrivate(new NativeClass(global, HTMLHeadingElementImp::getMetaData()));
    HTMLHRElementImp::setStaticPrivate(new NativeClass(global, HTMLHRElementImp::getMetaData()));
    HTMLHtmlElementImp::setStaticPrivate(new NativeClass(global, HTMLHtmlElementImp::getMetaData()));
    HTMLIFrameElementImp::setStaticPrivate(new NativeClass(global, HTMLIFrameElementImp::getMetaData()));
    HTMLImageElementImp::setStaticPrivate(new NativeClass(global, HTMLImageElementImp::getMetaData()));
    HTMLInputElementImp::setStaticPrivate(new NativeClass(global, HTMLInputElementImp::getMetaData()));
    HTMLKeygenElementImp::setStaticPrivate(new NativeClass(global, HTMLKeygenElementImp::getMetaData()));
    HTMLLabelElementImp::setStaticPrivate(new NativeClass(global, HTMLLabelElementImp::getMetaData()));
    HTMLLegendElementImp::setStaticPrivate(new NativeClass(global, HTMLLegendElementImp::getMetaData()));
    HTMLLIElementImp::setStaticPrivate(new NativeClass(global, HTMLLIElementImp::getMetaData()));
    HTMLLinkElementImp::setStaticPrivate(new NativeClass(global, HTMLLinkElementImp::getMetaData()));
    HTMLMapElementImp::setStaticPrivate(new NativeClass(global, HTMLMapElementImp::getMetaData()));
    HTMLMarqueeElementImp::setStaticPrivate(new NativeClass(global, HTMLMarqueeElementImp::getMetaData()));
//    HTMLMediaElementImp::setStaticPrivate(new NativeClass(global, HTMLMediaElementImp::getMetaData()));
    HTMLMenuElementImp::setStaticPrivate(new NativeClass(global, HTMLMenuElementImp::getMetaData()));
    HTMLMetaElementImp::setStaticPrivate(new NativeClass(global, HTMLMetaElementImp::getMetaData()));
    HTMLMeterElementImp::setStaticPrivate(new NativeClass(global, HTMLMeterElementImp::getMetaData()));
    HTMLModElementImp::setStaticPrivate(new NativeClass(global, HTMLModElementImp::getMetaData()));
    HTMLObjectElementImp::setStaticPrivate(new NativeClass(global, HTMLObjectElementImp::getMetaData()));
    HTMLOListElementImp::setStaticPrivate(new NativeClass(global, HTMLOListElementImp::getMetaData()));
    HTMLOptGroupElementImp::setStaticPrivate(new NativeClass(global, HTMLOptGroupElementImp::getMetaData()));
    HTMLOptionElementImp::setStaticPrivate(new NativeClass(global, HTMLOptionElementImp::getMetaData()));
    HTMLOptionsCollectionImp::setStaticPrivate(new NativeClass(global, HTMLOptionsCollectionImp::getMetaData()));
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
    HTMLTrackElementImp::setStaticPrivate(new NativeClass(global, HTMLTrackElementImp::getMetaData()));
    HTMLUListElementImp::setStaticPrivate(new NativeClass(global, HTMLUListElementImp::getMetaData()));
    HTMLUnknownElementImp::setStaticPrivate(new NativeClass(global, HTMLUnknownElementImp::getMetaData()));
    HTMLVideoElementImp::setStaticPrivate(new NativeClass(global, HTMLVideoElementImp::getMetaData()));
    ImageDataImp::setStaticPrivate(new NativeClass(global, ImageDataImp::getMetaData()));
//    LocationImp::setStaticPrivate(new NativeClass(global, LocationImp::getMetaData()));
    MediaControllerImp::setStaticPrivate(new NativeClass(global, MediaControllerImp::getMetaData()));
    MediaErrorImp::setStaticPrivate(new NativeClass(global, MediaErrorImp::getMetaData()));
    MediaQueryListImp::setStaticPrivate(new NativeClass(global, MediaQueryListImp::getMetaData()));
    MediaQueryListListenerImp::setStaticPrivate(new NativeClass(global, MediaQueryListListenerImp::getMetaData()));
    MessageChannelImp::setStaticPrivate(new NativeClass(global, MessageChannelImp::getMetaData()));
    MessageEventImp::setStaticPrivate(new NativeClass(global, MessageEventImp::getMetaData()));
    MessageEventInitImp::setStaticPrivate(new NativeClass(global, MessageEventInitImp::getMetaData()));
    MessagePortImp::setStaticPrivate(new NativeClass(global, MessagePortImp::getMetaData()));
    NavigatorContentUtilsImp::setStaticPrivate(new NativeClass(global, NavigatorContentUtilsImp::getMetaData()));
    NavigatorImp::setStaticPrivate(new NativeClass(global, NavigatorImp::getMetaData()));
    NavigatorIDImp::setStaticPrivate(new NativeClass(global, NavigatorIDImp::getMetaData()));
    NavigatorOnLineImp::setStaticPrivate(new NativeClass(global, NavigatorOnLineImp::getMetaData()));
    NavigatorStorageUtilsImp::setStaticPrivate(new NativeClass(global, NavigatorStorageUtilsImp::getMetaData()));
    PageTransitionEventImp::setStaticPrivate(new NativeClass(global, PageTransitionEventImp::getMetaData()));
    PageTransitionEventInitImp::setStaticPrivate(new NativeClass(global, PageTransitionEventInitImp::getMetaData()));
    PathImp::setStaticPrivate(new NativeClass(global, PathImp::getMetaData()));
    PopStateEventImp::setStaticPrivate(new NativeClass(global, PopStateEventImp::getMetaData()));
    PopStateEventInitImp::setStaticPrivate(new NativeClass(global, PopStateEventInitImp::getMetaData()));
    RadioNodeListImp::setStaticPrivate(new NativeClass(global, RadioNodeListImp::getMetaData()));
    ScreenImp::setStaticPrivate(new NativeClass(global, ScreenImp::getMetaData()));
    TextMetricsImp::setStaticPrivate(new NativeClass(global, TextMetricsImp::getMetaData()));
    TextTrackCueImp::setStaticPrivate(new NativeClass(global, TextTrackCueImp::getMetaData()));
    TextTrackCueListImp::setStaticPrivate(new NativeClass(global, TextTrackCueListImp::getMetaData()));
    TextTrackImp::setStaticPrivate(new NativeClass(global, TextTrackImp::getMetaData()));
    TextTrackListImp::setStaticPrivate(new NativeClass(global, TextTrackListImp::getMetaData()));
    TimeRangesImp::setStaticPrivate(new NativeClass(global, TimeRangesImp::getMetaData()));
    TrackEventImp::setStaticPrivate(new NativeClass(global, TrackEventImp::getMetaData()));
    TrackEventInitImp::setStaticPrivate(new NativeClass(global, TrackEventInitImp::getMetaData()));
    TransferableImp::setStaticPrivate(new NativeClass(global, TransferableImp::getMetaData()));
    ValidityStateImp::setStaticPrivate(new NativeClass(global, ValidityStateImp::getMetaData()));
    VideoTrackImp::setStaticPrivate(new NativeClass(global, VideoTrackImp::getMetaData()));
    VideoTrackListImp::setStaticPrivate(new NativeClass(global, VideoTrackListImp::getMetaData()));
    WindowProxy::setStaticPrivate(new NativeClass(global, WindowProxy::getMetaData()));
    RangeImp::setStaticPrivate(new NativeClass(global, RangeImp::getMetaData()));
//    MediaListImp::setStaticPrivate(new NativeClass(global, MediaListImp::getMetaData()));
//    StyleSheetImp::setStaticPrivate(new NativeClass(global, StyleSheetImp::getMetaData()));
    SVGMatrixImp::setStaticPrivate(new NativeClass(global, SVGMatrixImp::getMetaData()));
    NodeFilterImp::setStaticPrivate(new NativeClass(global, NodeFilterImp::getMetaData()));
    NodeIteratorImp::setStaticPrivate(new NativeClass(global, NodeIteratorImp::getMetaData()));
    TreeWalkerImp::setStaticPrivate(new NativeClass(global, TreeWalkerImp::getMetaData()));
    ArrayBufferImp::setStaticPrivate(new NativeClass(global, ArrayBufferImp::getMetaData()));
    ArrayBufferViewImp::setStaticPrivate(new NativeClass(global, ArrayBufferViewImp::getMetaData()));
    DataViewImp::setStaticPrivate(new NativeClass(global, DataViewImp::getMetaData()));
    Float32ArrayImp::setStaticPrivate(new NativeClass(global, Float32ArrayImp::getMetaData()));
    Float64ArrayImp::setStaticPrivate(new NativeClass(global, Float64ArrayImp::getMetaData()));
    Int16ArrayImp::setStaticPrivate(new NativeClass(global, Int16ArrayImp::getMetaData()));
    Int32ArrayImp::setStaticPrivate(new NativeClass(global, Int32ArrayImp::getMetaData()));
    Int8ArrayImp::setStaticPrivate(new NativeClass(global, Int8ArrayImp::getMetaData()));
    Uint16ArrayImp::setStaticPrivate(new NativeClass(global, Uint16ArrayImp::getMetaData()));
    Uint32ArrayImp::setStaticPrivate(new NativeClass(global, Uint32ArrayImp::getMetaData()));
    Uint8ArrayImp::setStaticPrivate(new NativeClass(global, Uint8ArrayImp::getMetaData()));
    Uint8ClampedArrayImp::setStaticPrivate(new NativeClass(global, Uint8ClampedArrayImp::getMetaData()));
    ClientRectImp::setStaticPrivate(new NativeClass(global, ClientRectImp::getMetaData()));
    ClientRectListImp::setStaticPrivate(new NativeClass(global, ClientRectListImp::getMetaData()));
    XBLContentElementImp::setStaticPrivate(new NativeClass(global, XBLContentElementImp::getMetaData()));
    XBLImplementationImp::setStaticPrivate(new NativeClass(global, XBLImplementationImp::getMetaData()));
    XBLImplementationListImp::setStaticPrivate(new NativeClass(global, XBLImplementationListImp::getMetaData()));
    FormDataImp::setStaticPrivate(new NativeClass(global, FormDataImp::getMetaData()));
    XMLHttpRequestEventTargetImp::setStaticPrivate(new NativeClass(global, XMLHttpRequestEventTargetImp::getMetaData()));
    XMLHttpRequestImp::setStaticPrivate(new NativeClass(global, XMLHttpRequestImp::getMetaData()));
    XMLHttpRequestOptionsImp::setStaticPrivate(new NativeClass(global, XMLHttpRequestOptionsImp::getMetaData()));
    XMLHttpRequestUploadImp::setStaticPrivate(new NativeClass(global, XMLHttpRequestUploadImp::getMetaData()));
}

}  // namespace

v8::Persistent<v8::ObjectTemplate> ECMAScriptContext::Impl::getGlobalTemplate() {
    static bool initilized = false;
    static v8::Persistent<v8::ObjectTemplate> globalTemplate;
    if (initilized)
        return globalTemplate;
#ifdef HAVE_V8_ISOLATE
    globalTemplate = v8::Persistent<v8::ObjectTemplate>::New(v8::Isolate::GetCurrent(), v8::ObjectTemplate::New());
#else
    globalTemplate = v8::Persistent<v8::ObjectTemplate>::New(v8::ObjectTemplate::New());
#endif
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

    v8::V8::AddMessageListener(reportError);

    v8::ExtensionConfiguration extensions(1, extensionNames);
    context = v8::Context::New(&extensions, getGlobalTemplate());
    v8::Context::Scope scope(context);
    v8::HandleScope handleScope;
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
    if (v8::Context::InContext() && v8::Context::GetCurrent() == context)
        context->Exit();
#ifdef HAVE_V8_ISOLATE
    context.Dispose(v8::Isolate::GetCurrent());
#else
    context.Dispose();
#endif
    context.Clear();
}

void ECMAScriptContext::Impl::enter(ObjectImp* windowProxy)
{
    context->Enter();

    v8::HandleScope handleScope;

    v8::Handle<v8::Object> globalProxy = context->Global();
    v8::Handle<v8::Object> global = globalProxy->GetPrototype().As<v8::Object>();
    assert(global->InternalFieldCount() == 1);
    global->SetInternalField(0, v8::External::New(windowProxy));
}

void ECMAScriptContext::Impl::exit(ObjectImp* windowProxy)
{
    assert(v8::Context::GetCurrent() == context);
    context->Exit();
}

void ECMAScriptContext::Impl::shutDown()
{
#ifdef HAVE_V8_ISOLATE
    getGlobalTemplate().Dispose(v8::Isolate::GetCurrent());
#else
    getGlobalTemplate().Dispose();
#endif
    getGlobalTemplate().Clear();
}

ECMAScriptContext::ECMAScriptContext()
  : pimpl(new Impl())
{
}

ECMAScriptContext::~ECMAScriptContext()
{
}

void ECMAScriptContext::enter(ObjectImp* window)
{
    pimpl->enter(window);
    current = window;
}

void ECMAScriptContext::exit(ObjectImp* window)
{
    pimpl->exit(window);
}

Object ECMAScriptContext::compileFunction(const std::u16string& body)
{
    assert(getCurrentContext() == this);
    return pimpl->compileFunction(body);
}

Object ECMAScriptContext::xblCreateImplementation(Object object, Object prototype, Object boundElement, Object shadowTree)
{
    assert(getCurrentContext() == this);
    return pimpl->xblCreateImplementation(object, prototype, boundElement, shadowTree);
}

void ECMAScriptContext::shutDown()
{
    Impl::shutDown();
}

ObjectImp* ECMAScriptContext::current;
