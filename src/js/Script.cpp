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

// #define DEBUG 1 // for JS_DumpHeap

#include "Script.h"

#include <iostream>

#include "AttrImp.h"
#include "CaretPositionImp.h"
#include "CharacterDataImp.h"
#include "CommentImp.h"
#include "DocumentFragmentImp.h"
#include "DocumentImp.h"
#include "DocumentTypeImp.h"
#include "DOMElementMapImp.h"
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
#include "WindowImp.h"
// #include "WindowModalImp.h"
// #include "WindowTimersImp.h"

// ranges
#include "DocumentRangeImp.h"
// #include "RangeExceptionImp.h"
#include "RangeImp.h"

// stylesheets
#include "LinkStyleImp.h"
#include "MediaListImp.h"
#include "StyleSheetImp.h"

// svg
#include "svg/SVGMatrixImp.h"

// traversal
#include "DocumentTraversalImp.h"
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
    std::cout << (report->filename ? report->filename : "<no filename>") << report->lineno << ' ' << message << '\n';
}

void registerClasses(JSContext* cx, JSObject* global)
{
    // prototype roots
    AttrImp::setStaticPrivate(new NativeClass(cx, global, AttrImp::getMetaData()));
    CSSRuleImp::setStaticPrivate(new NativeClass(cx, global, CSSRuleImp::getMetaData()));
    CSSStyleDeclarationImp::setStaticPrivate(new NativeClass(cx, global, CSSStyleDeclarationImp::getMetaData()));
    CSSValueImp::setStaticPrivate(new NativeClass(cx, global, CSSValueImp::getMetaData()));
    DOMImplementationImp::setStaticPrivate(new NativeClass(cx, global, DOMImplementationImp::getMetaData()));
    DOMTokenListImp::setStaticPrivate(new NativeClass(cx, global, DOMTokenListImp::getMetaData()));
    EventImp::setStaticPrivate(new NativeClass(cx, global, EventImp::getMetaData()));
    EventInitImp::setStaticPrivate(new NativeClass(cx, global, EventInitImp::getMetaData()));
    EventTargetImp::setStaticPrivate(new NativeClass(cx, global, EventTargetImp::getMetaData()));
    HTMLCollectionImp::setStaticPrivate(new NativeClass(cx, global, HTMLCollectionImp::getMetaData()));
    LocationImp::setStaticPrivate(new NativeClass(cx, global, LocationImp::getMetaData()));
    MediaListImp::setStaticPrivate(new NativeClass(cx, global, MediaListImp::getMetaData()));
    NodeImp::setStaticPrivate(new NativeClass(cx, global, NodeImp::getMetaData()));
    NodeListImp::setStaticPrivate(new NativeClass(cx, global, NodeListImp::getMetaData()));
    StyleSheetImp::setStaticPrivate(new NativeClass(cx, global, StyleSheetImp::getMetaData()));
    UIEventImp::setStaticPrivate(new NativeClass(cx, global, UIEventImp::getMetaData()));

    ElementImp::setStaticPrivate(new NativeClass(cx, global, ElementImp::getMetaData()));
    HTMLElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLElementImp::getMetaData()));
    HTMLMediaElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLMediaElementImp::getMetaData()));

//    AttrImp::setStaticPrivate(new NativeClass(cx, global, AttrImp::getMetaData()));
    CaretPositionImp::setStaticPrivate(new NativeClass(cx, global, CaretPositionImp::getMetaData()));
    CharacterDataImp::setStaticPrivate(new NativeClass(cx, global, CharacterDataImp::getMetaData()));
    CommentImp::setStaticPrivate(new NativeClass(cx, global, CommentImp::getMetaData()));
    DocumentFragmentImp::setStaticPrivate(new NativeClass(cx, global, DocumentFragmentImp::getMetaData()));
    DocumentImp::setStaticPrivate(new NativeClass(cx, global, DocumentImp::getMetaData()));
    DocumentTypeImp::setStaticPrivate(new NativeClass(cx, global, DocumentTypeImp::getMetaData()));
    DOMElementMapImp::setStaticPrivate(new NativeClass(cx, global, DOMElementMapImp::getMetaData()));
    DOMErrorImp::setStaticPrivate(new NativeClass(cx, global, DOMErrorImp::getMetaData()));
//    DOMImplementationImp::setStaticPrivate(new NativeClass(cx, global, DOMImplementationImp::getMetaData()));
    DOMSettableTokenListImp::setStaticPrivate(new NativeClass(cx, global, DOMSettableTokenListImp::getMetaData()));
    DOMStringListImp::setStaticPrivate(new NativeClass(cx, global, DOMStringListImp::getMetaData()));
    DOMStringMapImp::setStaticPrivate(new NativeClass(cx, global, DOMStringMapImp::getMetaData()));
//    DOMTokenListImp::setStaticPrivate(new NativeClass(cx, global, DOMTokenListImp::getMetaData()));
//    ElementImp::setStaticPrivate(new NativeClass(cx, global, ElementImp::getMetaData()));
//    NodeImp::setStaticPrivate(new NativeClass(cx, global, NodeImp::getMetaData()));
//    NodeListImp::setStaticPrivate(new NativeClass(cx, global, NodeListImp::getMetaData()));
    ProcessingInstructionImp::setStaticPrivate(new NativeClass(cx, global, ProcessingInstructionImp::getMetaData()));
    TextImp::setStaticPrivate(new NativeClass(cx, global, TextImp::getMetaData()));
    CounterImp::setStaticPrivate(new NativeClass(cx, global, CounterImp::getMetaData()));
    CSS2PropertiesImp::setStaticPrivate(new NativeClass(cx, global, CSS2PropertiesImp::getMetaData()));
    CSSCharsetRuleImp::setStaticPrivate(new NativeClass(cx, global, CSSCharsetRuleImp::getMetaData()));
    CSSFontFaceRuleImp::setStaticPrivate(new NativeClass(cx, global, CSSFontFaceRuleImp::getMetaData()));
    CSSImportRuleImp::setStaticPrivate(new NativeClass(cx, global, CSSImportRuleImp::getMetaData()));
    CSSMediaRuleImp::setStaticPrivate(new NativeClass(cx, global, CSSMediaRuleImp::getMetaData()));
    CSSNamespaceRuleImp::setStaticPrivate(new NativeClass(cx, global, CSSNamespaceRuleImp::getMetaData()));
    CSSPageRuleImp::setStaticPrivate(new NativeClass(cx, global, CSSPageRuleImp::getMetaData()));
    CSSPrimitiveValueImp::setStaticPrivate(new NativeClass(cx, global, CSSPrimitiveValueImp::getMetaData()));
//    CSSRuleImp::setStaticPrivate(new NativeClass(cx, global, CSSRuleImp::getMetaData()));
//    CSSStyleDeclarationImp::setStaticPrivate(new NativeClass(cx, global, CSSStyleDeclarationImp::getMetaData()));
    CSSStyleRuleImp::setStaticPrivate(new NativeClass(cx, global, CSSStyleRuleImp::getMetaData()));
    CSSStyleSheetImp::setStaticPrivate(new NativeClass(cx, global, CSSStyleSheetImp::getMetaData()));
    CSSUnknownRuleImp::setStaticPrivate(new NativeClass(cx, global, CSSUnknownRuleImp::getMetaData()));
//    CSSValueImp::setStaticPrivate(new NativeClass(cx, global, CSSValueImp::getMetaData()));
    CSSValueListImp::setStaticPrivate(new NativeClass(cx, global, CSSValueListImp::getMetaData()));
    RectImp::setStaticPrivate(new NativeClass(cx, global, RectImp::getMetaData()));
    RGBColorImp::setStaticPrivate(new NativeClass(cx, global, RGBColorImp::getMetaData()));
    CompositionEventImp::setStaticPrivate(new NativeClass(cx, global, CompositionEventImp::getMetaData()));
    CompositionEventInitImp::setStaticPrivate(new NativeClass(cx, global, CompositionEventInitImp::getMetaData()));
    CustomEventImp::setStaticPrivate(new NativeClass(cx, global, CustomEventImp::getMetaData()));
    CustomEventInitImp::setStaticPrivate(new NativeClass(cx, global, CustomEventInitImp::getMetaData()));
//    EventImp::setStaticPrivate(new NativeClass(cx, global, EventImp::getMetaData()));
    EventHandlerNonNullImp::setStaticPrivate(new NativeClass(cx, global, EventHandlerNonNullImp::getMetaData()));
//    EventInitImp::setStaticPrivate(new NativeClass(cx, global, EventInitImp::getMetaData()));
    EventListenerImp::setStaticPrivate(new NativeClass(cx, global, EventListenerImp::getMetaData()));
//    EventTargetImp::setStaticPrivate(new NativeClass(cx, global, EventTargetImp::getMetaData()));
    FocusEventImp::setStaticPrivate(new NativeClass(cx, global, FocusEventImp::getMetaData()));
    FocusEventInitImp::setStaticPrivate(new NativeClass(cx, global, FocusEventInitImp::getMetaData()));
    KeyboardEventImp::setStaticPrivate(new NativeClass(cx, global, KeyboardEventImp::getMetaData()));
    KeyboardEventInitImp::setStaticPrivate(new NativeClass(cx, global, KeyboardEventInitImp::getMetaData()));
    MouseEventImp::setStaticPrivate(new NativeClass(cx, global, MouseEventImp::getMetaData()));
    MouseEventInitImp::setStaticPrivate(new NativeClass(cx, global, MouseEventInitImp::getMetaData()));
    MutationCallbackImp::setStaticPrivate(new NativeClass(cx, global, MutationCallbackImp::getMetaData()));
    MutationEventImp::setStaticPrivate(new NativeClass(cx, global, MutationEventImp::getMetaData()));
    MutationObserverImp::setStaticPrivate(new NativeClass(cx, global, MutationObserverImp::getMetaData()));
    MutationObserverInitImp::setStaticPrivate(new NativeClass(cx, global, MutationObserverInitImp::getMetaData()));
    MutationRecordImp::setStaticPrivate(new NativeClass(cx, global, MutationRecordImp::getMetaData()));
    OnErrorEventHandlerNonNullImp::setStaticPrivate(new NativeClass(cx, global, OnErrorEventHandlerNonNullImp::getMetaData()));
    ProgressEventImp::setStaticPrivate(new NativeClass(cx, global, ProgressEventImp::getMetaData()));
//    UIEventImp::setStaticPrivate(new NativeClass(cx, global, UIEventImp::getMetaData()));
    UIEventInitImp::setStaticPrivate(new NativeClass(cx, global, UIEventInitImp::getMetaData()));
    WheelEventImp::setStaticPrivate(new NativeClass(cx, global, WheelEventImp::getMetaData()));
    WheelEventInitImp::setStaticPrivate(new NativeClass(cx, global, WheelEventInitImp::getMetaData()));
    EventSourceImp::setStaticPrivate(new NativeClass(cx, global, EventSourceImp::getMetaData()));
    BlobImp::setStaticPrivate(new NativeClass(cx, global, BlobImp::getMetaData()));
    BlobPropertyBagImp::setStaticPrivate(new NativeClass(cx, global, BlobPropertyBagImp::getMetaData()));
    FileCallbackImp::setStaticPrivate(new NativeClass(cx, global, FileCallbackImp::getMetaData()));
    FileImp::setStaticPrivate(new NativeClass(cx, global, FileImp::getMetaData()));
    FileListImp::setStaticPrivate(new NativeClass(cx, global, FileListImp::getMetaData()));
    FileReaderImp::setStaticPrivate(new NativeClass(cx, global, FileReaderImp::getMetaData()));
    FileReaderSyncImp::setStaticPrivate(new NativeClass(cx, global, FileReaderSyncImp::getMetaData()));
    LineEndingsImp::setStaticPrivate(new NativeClass(cx, global, LineEndingsImp::getMetaData()));
    ApplicationCacheImp::setStaticPrivate(new NativeClass(cx, global, ApplicationCacheImp::getMetaData()));
    AudioTrackImp::setStaticPrivate(new NativeClass(cx, global, AudioTrackImp::getMetaData()));
    AudioTrackListImp::setStaticPrivate(new NativeClass(cx, global, AudioTrackListImp::getMetaData()));
    BarPropImp::setStaticPrivate(new NativeClass(cx, global, BarPropImp::getMetaData()));
    BeforeUnloadEventImp::setStaticPrivate(new NativeClass(cx, global, BeforeUnloadEventImp::getMetaData()));
    CanvasDrawingStylesImp::setStaticPrivate(new NativeClass(cx, global, CanvasDrawingStylesImp::getMetaData()));
    CanvasGradientImp::setStaticPrivate(new NativeClass(cx, global, CanvasGradientImp::getMetaData()));
    CanvasPathMethodsImp::setStaticPrivate(new NativeClass(cx, global, CanvasPathMethodsImp::getMetaData()));
    CanvasPatternImp::setStaticPrivate(new NativeClass(cx, global, CanvasPatternImp::getMetaData()));
    CanvasRenderingContext2DImp::setStaticPrivate(new NativeClass(cx, global, CanvasRenderingContext2DImp::getMetaData()));
    DataTransferImp::setStaticPrivate(new NativeClass(cx, global, DataTransferImp::getMetaData()));
    DataTransferItemImp::setStaticPrivate(new NativeClass(cx, global, DataTransferItemImp::getMetaData()));
    DataTransferItemListImp::setStaticPrivate(new NativeClass(cx, global, DataTransferItemListImp::getMetaData()));
    DragEventImp::setStaticPrivate(new NativeClass(cx, global, DragEventImp::getMetaData()));
    DragEventInitImp::setStaticPrivate(new NativeClass(cx, global, DragEventInitImp::getMetaData()));
    DrawingStyleImp::setStaticPrivate(new NativeClass(cx, global, DrawingStyleImp::getMetaData()));
    ExternalImp::setStaticPrivate(new NativeClass(cx, global, ExternalImp::getMetaData()));
    FunctionStringCallbackImp::setStaticPrivate(new NativeClass(cx, global, FunctionStringCallbackImp::getMetaData()));
    HashChangeEventImp::setStaticPrivate(new NativeClass(cx, global, HashChangeEventImp::getMetaData()));
    HashChangeEventInitImp::setStaticPrivate(new NativeClass(cx, global, HashChangeEventInitImp::getMetaData()));
    HistoryImp::setStaticPrivate(new NativeClass(cx, global, HistoryImp::getMetaData()));
    HitRegionOptionsImp::setStaticPrivate(new NativeClass(cx, global, HitRegionOptionsImp::getMetaData()));
    HTMLAllCollectionImp::setStaticPrivate(new NativeClass(cx, global, HTMLAllCollectionImp::getMetaData()));
    HTMLAnchorElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLAnchorElementImp::getMetaData()));
    HTMLAppletElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLAppletElementImp::getMetaData()));
    HTMLAreaElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLAreaElementImp::getMetaData()));
    HTMLAudioElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLAudioElementImp::getMetaData()));
    HTMLBaseElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLBaseElementImp::getMetaData()));
    HTMLBaseFontElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLBaseFontElementImp::getMetaData()));
    HTMLBindingElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLBindingElementImp::getMetaData()));
    HTMLBodyElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLBodyElementImp::getMetaData()));
    HTMLBRElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLBRElementImp::getMetaData()));
    HTMLButtonElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLButtonElementImp::getMetaData()));
    HTMLCanvasElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLCanvasElementImp::getMetaData()));
//    HTMLCollectionImp::setStaticPrivate(new NativeClass(cx, global, HTMLCollectionImp::getMetaData()));
    HTMLCommandElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLCommandElementImp::getMetaData()));
    HTMLDataListElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLDataListElementImp::getMetaData()));
    HTMLDetailsElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLDetailsElementImp::getMetaData()));
    HTMLDialogElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLDialogElementImp::getMetaData()));
    HTMLDirectoryElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLDirectoryElementImp::getMetaData()));
    HTMLDivElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLDivElementImp::getMetaData()));
    HTMLDListElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLDListElementImp::getMetaData()));
//    HTMLElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLElementImp::getMetaData()));
    HTMLEmbedElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLEmbedElementImp::getMetaData()));
    HTMLFieldSetElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLFieldSetElementImp::getMetaData()));
    HTMLFontElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLFontElementImp::getMetaData()));
    HTMLFormControlsCollectionImp::setStaticPrivate(new NativeClass(cx, global, HTMLFormControlsCollectionImp::getMetaData()));
    HTMLFormElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLFormElementImp::getMetaData()));
    HTMLFrameElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLFrameElementImp::getMetaData()));
    HTMLFrameSetElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLFrameSetElementImp::getMetaData()));
    HTMLHeadElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLHeadElementImp::getMetaData()));
    HTMLHeadingElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLHeadingElementImp::getMetaData()));
    HTMLHRElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLHRElementImp::getMetaData()));
    HTMLHtmlElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLHtmlElementImp::getMetaData()));
    HTMLIFrameElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLIFrameElementImp::getMetaData()));
    HTMLImageElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLImageElementImp::getMetaData()));
    HTMLInputElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLInputElementImp::getMetaData()));
    HTMLKeygenElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLKeygenElementImp::getMetaData()));
    HTMLLabelElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLLabelElementImp::getMetaData()));
    HTMLLegendElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLLegendElementImp::getMetaData()));
    HTMLLIElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLLIElementImp::getMetaData()));
    HTMLLinkElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLLinkElementImp::getMetaData()));
    HTMLMapElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLMapElementImp::getMetaData()));
    HTMLMarqueeElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLMarqueeElementImp::getMetaData()));
//    HTMLMediaElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLMediaElementImp::getMetaData()));
    HTMLMenuElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLMenuElementImp::getMetaData()));
    HTMLMetaElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLMetaElementImp::getMetaData()));
    HTMLMeterElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLMeterElementImp::getMetaData()));
    HTMLModElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLModElementImp::getMetaData()));
    HTMLObjectElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLObjectElementImp::getMetaData()));
    HTMLOListElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLOListElementImp::getMetaData()));
    HTMLOptGroupElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLOptGroupElementImp::getMetaData()));
    HTMLOptionElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLOptionElementImp::getMetaData()));
    HTMLOptionsCollectionImp::setStaticPrivate(new NativeClass(cx, global, HTMLOptionsCollectionImp::getMetaData()));
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
    HTMLTemplateElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLTemplateElementImp::getMetaData()));
    HTMLTextAreaElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLTextAreaElementImp::getMetaData()));
    HTMLTimeElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLTimeElementImp::getMetaData()));
    HTMLTitleElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLTitleElementImp::getMetaData()));
    HTMLTrackElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLTrackElementImp::getMetaData()));
    HTMLUListElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLUListElementImp::getMetaData()));
    HTMLUnknownElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLUnknownElementImp::getMetaData()));
    HTMLVideoElementImp::setStaticPrivate(new NativeClass(cx, global, HTMLVideoElementImp::getMetaData()));
    ImageDataImp::setStaticPrivate(new NativeClass(cx, global, ImageDataImp::getMetaData()));
//    LocationImp::setStaticPrivate(new NativeClass(cx, global, LocationImp::getMetaData()));
    MediaControllerImp::setStaticPrivate(new NativeClass(cx, global, MediaControllerImp::getMetaData()));
    MediaErrorImp::setStaticPrivate(new NativeClass(cx, global, MediaErrorImp::getMetaData()));
    MediaQueryListImp::setStaticPrivate(new NativeClass(cx, global, MediaQueryListImp::getMetaData()));
    MediaQueryListListenerImp::setStaticPrivate(new NativeClass(cx, global, MediaQueryListListenerImp::getMetaData()));
    MessageChannelImp::setStaticPrivate(new NativeClass(cx, global, MessageChannelImp::getMetaData()));
    MessageEventImp::setStaticPrivate(new NativeClass(cx, global, MessageEventImp::getMetaData()));
    MessageEventInitImp::setStaticPrivate(new NativeClass(cx, global, MessageEventInitImp::getMetaData()));
    MessagePortImp::setStaticPrivate(new NativeClass(cx, global, MessagePortImp::getMetaData()));
    NavigatorContentUtilsImp::setStaticPrivate(new NativeClass(cx, global, NavigatorContentUtilsImp::getMetaData()));
    NavigatorImp::setStaticPrivate(new NativeClass(cx, global, NavigatorImp::getMetaData()));
    NavigatorIDImp::setStaticPrivate(new NativeClass(cx, global, NavigatorIDImp::getMetaData()));
    NavigatorOnLineImp::setStaticPrivate(new NativeClass(cx, global, NavigatorOnLineImp::getMetaData()));
    NavigatorStorageUtilsImp::setStaticPrivate(new NativeClass(cx, global, NavigatorStorageUtilsImp::getMetaData()));
    PageTransitionEventImp::setStaticPrivate(new NativeClass(cx, global, PageTransitionEventImp::getMetaData()));
    PageTransitionEventInitImp::setStaticPrivate(new NativeClass(cx, global, PageTransitionEventInitImp::getMetaData()));
    PathImp::setStaticPrivate(new NativeClass(cx, global, PathImp::getMetaData()));
    PopStateEventImp::setStaticPrivate(new NativeClass(cx, global, PopStateEventImp::getMetaData()));
    PopStateEventInitImp::setStaticPrivate(new NativeClass(cx, global, PopStateEventInitImp::getMetaData()));
    RadioNodeListImp::setStaticPrivate(new NativeClass(cx, global, RadioNodeListImp::getMetaData()));
    ScreenImp::setStaticPrivate(new NativeClass(cx, global, ScreenImp::getMetaData()));
    TextMetricsImp::setStaticPrivate(new NativeClass(cx, global, TextMetricsImp::getMetaData()));
    TextTrackCueImp::setStaticPrivate(new NativeClass(cx, global, TextTrackCueImp::getMetaData()));
    TextTrackCueListImp::setStaticPrivate(new NativeClass(cx, global, TextTrackCueListImp::getMetaData()));
    TextTrackImp::setStaticPrivate(new NativeClass(cx, global, TextTrackImp::getMetaData()));
    TextTrackListImp::setStaticPrivate(new NativeClass(cx, global, TextTrackListImp::getMetaData()));
    TimeRangesImp::setStaticPrivate(new NativeClass(cx, global, TimeRangesImp::getMetaData()));
    TrackEventImp::setStaticPrivate(new NativeClass(cx, global, TrackEventImp::getMetaData()));
    TrackEventInitImp::setStaticPrivate(new NativeClass(cx, global, TrackEventInitImp::getMetaData()));
    TransferableImp::setStaticPrivate(new NativeClass(cx, global, TransferableImp::getMetaData()));
    ValidityStateImp::setStaticPrivate(new NativeClass(cx, global, ValidityStateImp::getMetaData()));
    VideoTrackImp::setStaticPrivate(new NativeClass(cx, global, VideoTrackImp::getMetaData()));
    VideoTrackListImp::setStaticPrivate(new NativeClass(cx, global, VideoTrackListImp::getMetaData()));
    WindowImp::setStaticPrivate(new NativeClass(cx, global, WindowImp::getMetaData()));
    DocumentRangeImp::setStaticPrivate(new NativeClass(cx, global, DocumentRangeImp::getMetaData()));
    RangeImp::setStaticPrivate(new NativeClass(cx, global, RangeImp::getMetaData()));
    LinkStyleImp::setStaticPrivate(new NativeClass(cx, global, LinkStyleImp::getMetaData()));
//    MediaListImp::setStaticPrivate(new NativeClass(cx, global, MediaListImp::getMetaData()));
//    StyleSheetImp::setStaticPrivate(new NativeClass(cx, global, StyleSheetImp::getMetaData()));
    SVGMatrixImp::setStaticPrivate(new NativeClass(cx, global, SVGMatrixImp::getMetaData()));
    DocumentTraversalImp::setStaticPrivate(new NativeClass(cx, global, DocumentTraversalImp::getMetaData()));
    NodeFilterImp::setStaticPrivate(new NativeClass(cx, global, NodeFilterImp::getMetaData()));
    NodeIteratorImp::setStaticPrivate(new NativeClass(cx, global, NodeIteratorImp::getMetaData()));
    TreeWalkerImp::setStaticPrivate(new NativeClass(cx, global, TreeWalkerImp::getMetaData()));
#if 0
    ArrayBufferImp::setStaticPrivate(new NativeClass(cx, global, ArrayBufferImp::getMetaData()));
    ArrayBufferViewImp::setStaticPrivate(new NativeClass(cx, global, ArrayBufferViewImp::getMetaData()));
    DataViewImp::setStaticPrivate(new NativeClass(cx, global, DataViewImp::getMetaData()));
    Float32ArrayImp::setStaticPrivate(new NativeClass(cx, global, Float32ArrayImp::getMetaData()));
    Float64ArrayImp::setStaticPrivate(new NativeClass(cx, global, Float64ArrayImp::getMetaData()));
    Int16ArrayImp::setStaticPrivate(new NativeClass(cx, global, Int16ArrayImp::getMetaData()));
    Int32ArrayImp::setStaticPrivate(new NativeClass(cx, global, Int32ArrayImp::getMetaData()));
    Int8ArrayImp::setStaticPrivate(new NativeClass(cx, global, Int8ArrayImp::getMetaData()));
    Uint16ArrayImp::setStaticPrivate(new NativeClass(cx, global, Uint16ArrayImp::getMetaData()));
    Uint32ArrayImp::setStaticPrivate(new NativeClass(cx, global, Uint32ArrayImp::getMetaData()));
    Uint8ArrayImp::setStaticPrivate(new NativeClass(cx, global, Uint8ArrayImp::getMetaData()));
    Uint8ClampedArrayImp::setStaticPrivate(new NativeClass(cx, global, Uint8ClampedArrayImp::getMetaData()));
#endif
    ClientRectImp::setStaticPrivate(new NativeClass(cx, global, ClientRectImp::getMetaData()));
    ClientRectListImp::setStaticPrivate(new NativeClass(cx, global, ClientRectListImp::getMetaData()));
    XBLContentElementImp::setStaticPrivate(new NativeClass(cx, global, XBLContentElementImp::getMetaData()));
    XBLImplementationImp::setStaticPrivate(new NativeClass(cx, global, XBLImplementationImp::getMetaData()));
    XBLImplementationListImp::setStaticPrivate(new NativeClass(cx, global, XBLImplementationListImp::getMetaData()));
    FormDataImp::setStaticPrivate(new NativeClass(cx, global, FormDataImp::getMetaData()));
    XMLHttpRequestEventTargetImp::setStaticPrivate(new NativeClass(cx, global, XMLHttpRequestEventTargetImp::getMetaData()));
    XMLHttpRequestImp::setStaticPrivate(new NativeClass(cx, global, XMLHttpRequestImp::getMetaData()));
    XMLHttpRequestOptionsImp::setStaticPrivate(new NativeClass(cx, global, XMLHttpRequestOptionsImp::getMetaData()));
    XMLHttpRequestUploadImp::setStaticPrivate(new NativeClass(cx, global, XMLHttpRequestUploadImp::getMetaData()));
}

}  // namespace

ECMAScriptContext::Impl::Impl() :
    global(0)
{
    JSContext* context = getContext();
    if (!context)
        return;
    global = JS_NewCompartmentAndGlobalObject(context, &globalClass, NULL);
    if (!global)
        return;
    if (!JS_InitStandardClasses(context, global))
        return;

    registerClasses(context, global);

    Reflect::Interface globalMeta(html::Window::getMetaData());
    std::string name = Reflect::getIdentifier(globalMeta.getName());
    if (0 < name.length()) {
        jsval val;
        if (JS_GetProperty(context, global, name.c_str(), &val) && JSVAL_IS_OBJECT(val)) {
            JSObject* parent = JSVAL_TO_OBJECT(val);
            if (JS_GetProperty(context, parent, "prototype", &val) && JSVAL_IS_OBJECT(val)) {
                JSObject* proto = JSVAL_TO_OBJECT(val);
                JS_SetPrototype(context, global, proto);
            }
        }
    }

    JS_AddObjectRoot(context, &global);
}

ECMAScriptContext::Impl::~Impl()
{
    if (global) {
        JS_RemoveObjectRoot(getContext(), &global);
        global = 0;
    }
}

JSContext* ECMAScriptContext::Impl::getContext()
{
    static JSContext* context = 0;
    if (context)
        return context;
    context = JS_NewContext(getRuntime(), 8192);
    if (!context)
        return 0;
    JS_SetOptions(context, JSOPTION_VAROBJFIX | JSOPTION_JIT | JSOPTION_METHODJIT);
    JS_SetVersion(context, JSVERSION_LATEST);
    JS_SetErrorReporter(context, reportError);
    return context;
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
    current = window;
    pimpl->activate(window);
}

Any ECMAScriptContext::evaluate(const std::u16string& script)
{
    return pimpl->evaluate(script);
}

Object* ECMAScriptContext::compileFunction(const std::u16string& body)
{
    return pimpl->compileFunction(body);
}

Any ECMAScriptContext::callFunction(Object thisObject, Object functionObject, int argc, Any* argv)
{
    return pimpl->callFunction(thisObject, functionObject, argc, argv);
}

Object* ECMAScriptContext::xblCreateImplementation(Object object, Object prototype, Object boundElement, Object shadowTree)
{
    return pimpl->xblCreateImplementation(object, prototype, boundElement, shadowTree);
}

void ECMAScriptContext::shutDown()
{
    Impl::shutDown();
}

ObjectImp* ECMAScriptContext::current;
