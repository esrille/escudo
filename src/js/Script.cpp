/*
 * Copyright 2011-2015 Esrille Inc.
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

// geometry
#include "DOMRectImp.h"
#include "DOMRectReadOnlyImp.h"
#include "DOMRectListImp.h"

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
    std::cerr << (report->filename ? report->filename : "<no filename>") << report->lineno << ' ' << message << '\n';
}

void initializeClasses()
{
    static bool initialized(false);

    if (initialized)
        return;
    initialized = true;

    // prototype roots
    AttrImp::setStaticPrivate(new NativeClass(AttrImp::getMetaData()));
    CSSRuleImp::setStaticPrivate(new NativeClass(CSSRuleImp::getMetaData()));
    CSSStyleDeclarationImp::setStaticPrivate(new NativeClass(CSSStyleDeclarationImp::getMetaData()));
    CSSValueImp::setStaticPrivate(new NativeClass(CSSValueImp::getMetaData()));
    DOMImplementationImp::setStaticPrivate(new NativeClass(DOMImplementationImp::getMetaData()));
    DOMTokenListImp::setStaticPrivate(new NativeClass(DOMTokenListImp::getMetaData()));
    EventImp::setStaticPrivate(new NativeClass(EventImp::getMetaData()));
    EventInitImp::setStaticPrivate(new NativeClass(EventInitImp::getMetaData()));
    EventTargetImp::setStaticPrivate(new NativeClass(EventTargetImp::getMetaData()));
    HTMLCollectionImp::setStaticPrivate(new NativeClass(HTMLCollectionImp::getMetaData()));
    LocationImp::setStaticPrivate(new NativeClass(LocationImp::getMetaData()));
    MediaListImp::setStaticPrivate(new NativeClass(MediaListImp::getMetaData()));
    NodeImp::setStaticPrivate(new NativeClass(NodeImp::getMetaData()));
    NodeListImp::setStaticPrivate(new NativeClass(NodeListImp::getMetaData()));
    StyleSheetImp::setStaticPrivate(new NativeClass(StyleSheetImp::getMetaData()));
    UIEventImp::setStaticPrivate(new NativeClass(UIEventImp::getMetaData()));

    ElementImp::setStaticPrivate(new NativeClass(ElementImp::getMetaData()));
    HTMLElementImp::setStaticPrivate(new NativeClass(HTMLElementImp::getMetaData()));
    HTMLMediaElementImp::setStaticPrivate(new NativeClass(HTMLMediaElementImp::getMetaData()));

//    AttrImp::setStaticPrivate(new NativeClass(AttrImp::getMetaData()));
    CaretPositionImp::setStaticPrivate(new NativeClass(CaretPositionImp::getMetaData()));
    CharacterDataImp::setStaticPrivate(new NativeClass(CharacterDataImp::getMetaData()));
    CommentImp::setStaticPrivate(new NativeClass(CommentImp::getMetaData()));
    DocumentFragmentImp::setStaticPrivate(new NativeClass(DocumentFragmentImp::getMetaData()));
    DocumentImp::setStaticPrivate(new NativeClass(DocumentImp::getMetaData()));
    DocumentTypeImp::setStaticPrivate(new NativeClass(DocumentTypeImp::getMetaData()));
    DOMErrorImp::setStaticPrivate(new NativeClass(DOMErrorImp::getMetaData()));
//    DOMImplementationImp::setStaticPrivate(new NativeClass(DOMImplementationImp::getMetaData()));
    DOMSettableTokenListImp::setStaticPrivate(new NativeClass(DOMSettableTokenListImp::getMetaData()));
    DOMStringListImp::setStaticPrivate(new NativeClass(DOMStringListImp::getMetaData()));
    DOMStringMapImp::setStaticPrivate(new NativeClass(DOMStringMapImp::getMetaData()));
//    DOMTokenListImp::setStaticPrivate(new NativeClass(DOMTokenListImp::getMetaData()));
//    ElementImp::setStaticPrivate(new NativeClass(ElementImp::getMetaData()));
//    NodeImp::setStaticPrivate(new NativeClass(NodeImp::getMetaData()));
//    NodeListImp::setStaticPrivate(new NativeClass(NodeListImp::getMetaData()));
    ProcessingInstructionImp::setStaticPrivate(new NativeClass(ProcessingInstructionImp::getMetaData()));
    TextImp::setStaticPrivate(new NativeClass(TextImp::getMetaData()));
    XMLDocumentImp::setStaticPrivate(new NativeClass(XMLDocumentImp::getMetaData()));
    CounterImp::setStaticPrivate(new NativeClass(CounterImp::getMetaData()));
    CSS2PropertiesImp::setStaticPrivate(new NativeClass(CSS2PropertiesImp::getMetaData()));
    CSSCharsetRuleImp::setStaticPrivate(new NativeClass(CSSCharsetRuleImp::getMetaData()));
    CSSFontFaceRuleImp::setStaticPrivate(new NativeClass(CSSFontFaceRuleImp::getMetaData()));
    CSSImportRuleImp::setStaticPrivate(new NativeClass(CSSImportRuleImp::getMetaData()));
    CSSMediaRuleImp::setStaticPrivate(new NativeClass(CSSMediaRuleImp::getMetaData()));
    CSSNamespaceRuleImp::setStaticPrivate(new NativeClass(CSSNamespaceRuleImp::getMetaData()));
    CSSPageRuleImp::setStaticPrivate(new NativeClass(CSSPageRuleImp::getMetaData()));
    CSSPrimitiveValueImp::setStaticPrivate(new NativeClass(CSSPrimitiveValueImp::getMetaData()));
//    CSSRuleImp::setStaticPrivate(new NativeClass(CSSRuleImp::getMetaData()));
    CSSRuleListImp::setStaticPrivate(new NativeClass(CSSRuleListImp::getMetaData()));
//    CSSStyleDeclarationImp::setStaticPrivate(new NativeClass(CSSStyleDeclarationImp::getMetaData()));
    CSSStyleRuleImp::setStaticPrivate(new NativeClass(CSSStyleRuleImp::getMetaData()));
    CSSStyleSheetImp::setStaticPrivate(new NativeClass(CSSStyleSheetImp::getMetaData()));
    CSSUnknownRuleImp::setStaticPrivate(new NativeClass(CSSUnknownRuleImp::getMetaData()));
//    CSSValueImp::setStaticPrivate(new NativeClass(CSSValueImp::getMetaData()));
    CSSValueListImp::setStaticPrivate(new NativeClass(CSSValueListImp::getMetaData()));
    RectImp::setStaticPrivate(new NativeClass(RectImp::getMetaData()));
    RGBColorImp::setStaticPrivate(new NativeClass(RGBColorImp::getMetaData()));
    CompositionEventImp::setStaticPrivate(new NativeClass(CompositionEventImp::getMetaData()));
    CompositionEventInitImp::setStaticPrivate(new NativeClass(CompositionEventInitImp::getMetaData()));
    CustomEventImp::setStaticPrivate(new NativeClass(CustomEventImp::getMetaData()));
    CustomEventInitImp::setStaticPrivate(new NativeClass(CustomEventInitImp::getMetaData()));
//    EventImp::setStaticPrivate(new NativeClass(EventImp::getMetaData()));
    EventHandlerNonNullImp::setStaticPrivate(new NativeClass(EventHandlerNonNullImp::getMetaData()));
//    EventInitImp::setStaticPrivate(new NativeClass(EventInitImp::getMetaData()));
    EventListenerImp::setStaticPrivate(new NativeClass(EventListenerImp::getMetaData()));
//    EventTargetImp::setStaticPrivate(new NativeClass(EventTargetImp::getMetaData()));
    FocusEventImp::setStaticPrivate(new NativeClass(FocusEventImp::getMetaData()));
    FocusEventInitImp::setStaticPrivate(new NativeClass(FocusEventInitImp::getMetaData()));
    KeyboardEventImp::setStaticPrivate(new NativeClass(KeyboardEventImp::getMetaData()));
    KeyboardEventInitImp::setStaticPrivate(new NativeClass(KeyboardEventInitImp::getMetaData()));
    MouseEventImp::setStaticPrivate(new NativeClass(MouseEventImp::getMetaData()));
    MouseEventInitImp::setStaticPrivate(new NativeClass(MouseEventInitImp::getMetaData()));
    MutationCallbackImp::setStaticPrivate(new NativeClass(MutationCallbackImp::getMetaData()));
    MutationEventImp::setStaticPrivate(new NativeClass(MutationEventImp::getMetaData()));
    MutationObserverImp::setStaticPrivate(new NativeClass(MutationObserverImp::getMetaData()));
    MutationObserverInitImp::setStaticPrivate(new NativeClass(MutationObserverInitImp::getMetaData()));
    MutationRecordImp::setStaticPrivate(new NativeClass(MutationRecordImp::getMetaData()));
    OnErrorEventHandlerNonNullImp::setStaticPrivate(new NativeClass(OnErrorEventHandlerNonNullImp::getMetaData()));
    ProgressEventImp::setStaticPrivate(new NativeClass(ProgressEventImp::getMetaData()));
//    UIEventImp::setStaticPrivate(new NativeClass(UIEventImp::getMetaData()));
    UIEventInitImp::setStaticPrivate(new NativeClass(UIEventInitImp::getMetaData()));
    WheelEventImp::setStaticPrivate(new NativeClass(WheelEventImp::getMetaData()));
    WheelEventInitImp::setStaticPrivate(new NativeClass(WheelEventInitImp::getMetaData()));
    EventSourceImp::setStaticPrivate(new NativeClass(EventSourceImp::getMetaData()));
    BlobImp::setStaticPrivate(new NativeClass(BlobImp::getMetaData()));
    BlobPropertyBagImp::setStaticPrivate(new NativeClass(BlobPropertyBagImp::getMetaData()));
    FileCallbackImp::setStaticPrivate(new NativeClass(FileCallbackImp::getMetaData()));
    FileImp::setStaticPrivate(new NativeClass(FileImp::getMetaData()));
    FileListImp::setStaticPrivate(new NativeClass(FileListImp::getMetaData()));
    FileReaderImp::setStaticPrivate(new NativeClass(FileReaderImp::getMetaData()));
    FileReaderSyncImp::setStaticPrivate(new NativeClass(FileReaderSyncImp::getMetaData()));
    LineEndingsImp::setStaticPrivate(new NativeClass(LineEndingsImp::getMetaData()));
    ApplicationCacheImp::setStaticPrivate(new NativeClass(ApplicationCacheImp::getMetaData()));
    AudioTrackImp::setStaticPrivate(new NativeClass(AudioTrackImp::getMetaData()));
    AudioTrackListImp::setStaticPrivate(new NativeClass(AudioTrackListImp::getMetaData()));
    BarPropImp::setStaticPrivate(new NativeClass(BarPropImp::getMetaData()));
    BeforeUnloadEventImp::setStaticPrivate(new NativeClass(BeforeUnloadEventImp::getMetaData()));
    CanvasDrawingStylesImp::setStaticPrivate(new NativeClass(CanvasDrawingStylesImp::getMetaData()));
    CanvasGradientImp::setStaticPrivate(new NativeClass(CanvasGradientImp::getMetaData()));
    CanvasPathMethodsImp::setStaticPrivate(new NativeClass(CanvasPathMethodsImp::getMetaData()));
    CanvasPatternImp::setStaticPrivate(new NativeClass(CanvasPatternImp::getMetaData()));
    CanvasRenderingContext2DImp::setStaticPrivate(new NativeClass(CanvasRenderingContext2DImp::getMetaData()));
    DataTransferImp::setStaticPrivate(new NativeClass(DataTransferImp::getMetaData()));
    DataTransferItemImp::setStaticPrivate(new NativeClass(DataTransferItemImp::getMetaData()));
    DataTransferItemListImp::setStaticPrivate(new NativeClass(DataTransferItemListImp::getMetaData()));
    DragEventImp::setStaticPrivate(new NativeClass(DragEventImp::getMetaData()));
    DragEventInitImp::setStaticPrivate(new NativeClass(DragEventInitImp::getMetaData()));
    DrawingStyleImp::setStaticPrivate(new NativeClass(DrawingStyleImp::getMetaData()));
    ExternalImp::setStaticPrivate(new NativeClass(ExternalImp::getMetaData()));
    FunctionStringCallbackImp::setStaticPrivate(new NativeClass(FunctionStringCallbackImp::getMetaData()));
    HashChangeEventImp::setStaticPrivate(new NativeClass(HashChangeEventImp::getMetaData()));
    HashChangeEventInitImp::setStaticPrivate(new NativeClass(HashChangeEventInitImp::getMetaData()));
    HistoryImp::setStaticPrivate(new NativeClass(HistoryImp::getMetaData()));
    HitRegionOptionsImp::setStaticPrivate(new NativeClass(HitRegionOptionsImp::getMetaData()));
    HTMLAllCollectionImp::setStaticPrivate(new NativeClass(HTMLAllCollectionImp::getMetaData()));
    HTMLAnchorElementImp::setStaticPrivate(new NativeClass(HTMLAnchorElementImp::getMetaData()));
    HTMLAppletElementImp::setStaticPrivate(new NativeClass(HTMLAppletElementImp::getMetaData()));
    HTMLAreaElementImp::setStaticPrivate(new NativeClass(HTMLAreaElementImp::getMetaData()));
    HTMLAudioElementImp::setStaticPrivate(new NativeClass(HTMLAudioElementImp::getMetaData()));
    HTMLBaseElementImp::setStaticPrivate(new NativeClass(HTMLBaseElementImp::getMetaData()));
    HTMLBaseFontElementImp::setStaticPrivate(new NativeClass(HTMLBaseFontElementImp::getMetaData()));
    HTMLBindingElementImp::setStaticPrivate(new NativeClass(HTMLBindingElementImp::getMetaData()));
    HTMLBodyElementImp::setStaticPrivate(new NativeClass(HTMLBodyElementImp::getMetaData()));
    HTMLBRElementImp::setStaticPrivate(new NativeClass(HTMLBRElementImp::getMetaData()));
    HTMLButtonElementImp::setStaticPrivate(new NativeClass(HTMLButtonElementImp::getMetaData()));
    HTMLCanvasElementImp::setStaticPrivate(new NativeClass(HTMLCanvasElementImp::getMetaData()));
//    HTMLCollectionImp::setStaticPrivate(new NativeClass(HTMLCollectionImp::getMetaData()));
    HTMLCommandElementImp::setStaticPrivate(new NativeClass(HTMLCommandElementImp::getMetaData()));
    HTMLDataListElementImp::setStaticPrivate(new NativeClass(HTMLDataListElementImp::getMetaData()));
    HTMLDetailsElementImp::setStaticPrivate(new NativeClass(HTMLDetailsElementImp::getMetaData()));
    HTMLDialogElementImp::setStaticPrivate(new NativeClass(HTMLDialogElementImp::getMetaData()));
    HTMLDirectoryElementImp::setStaticPrivate(new NativeClass(HTMLDirectoryElementImp::getMetaData()));
    HTMLDivElementImp::setStaticPrivate(new NativeClass(HTMLDivElementImp::getMetaData()));
    HTMLDListElementImp::setStaticPrivate(new NativeClass(HTMLDListElementImp::getMetaData()));
//    HTMLElementImp::setStaticPrivate(new NativeClass(HTMLElementImp::getMetaData()));
    HTMLEmbedElementImp::setStaticPrivate(new NativeClass(HTMLEmbedElementImp::getMetaData()));
    HTMLFieldSetElementImp::setStaticPrivate(new NativeClass(HTMLFieldSetElementImp::getMetaData()));
    HTMLFontElementImp::setStaticPrivate(new NativeClass(HTMLFontElementImp::getMetaData()));
    HTMLFormControlsCollectionImp::setStaticPrivate(new NativeClass(HTMLFormControlsCollectionImp::getMetaData()));
    HTMLFormElementImp::setStaticPrivate(new NativeClass(HTMLFormElementImp::getMetaData()));
    HTMLFrameElementImp::setStaticPrivate(new NativeClass(HTMLFrameElementImp::getMetaData()));
    HTMLFrameSetElementImp::setStaticPrivate(new NativeClass(HTMLFrameSetElementImp::getMetaData()));
    HTMLHeadElementImp::setStaticPrivate(new NativeClass(HTMLHeadElementImp::getMetaData()));
    HTMLHeadingElementImp::setStaticPrivate(new NativeClass(HTMLHeadingElementImp::getMetaData()));
    HTMLHRElementImp::setStaticPrivate(new NativeClass(HTMLHRElementImp::getMetaData()));
    HTMLHtmlElementImp::setStaticPrivate(new NativeClass(HTMLHtmlElementImp::getMetaData()));
    HTMLIFrameElementImp::setStaticPrivate(new NativeClass(HTMLIFrameElementImp::getMetaData()));
    HTMLImageElementImp::setStaticPrivate(new NativeClass(HTMLImageElementImp::getMetaData()));
    HTMLInputElementImp::setStaticPrivate(new NativeClass(HTMLInputElementImp::getMetaData()));
    HTMLKeygenElementImp::setStaticPrivate(new NativeClass(HTMLKeygenElementImp::getMetaData()));
    HTMLLabelElementImp::setStaticPrivate(new NativeClass(HTMLLabelElementImp::getMetaData()));
    HTMLLegendElementImp::setStaticPrivate(new NativeClass(HTMLLegendElementImp::getMetaData()));
    HTMLLIElementImp::setStaticPrivate(new NativeClass(HTMLLIElementImp::getMetaData()));
    HTMLLinkElementImp::setStaticPrivate(new NativeClass(HTMLLinkElementImp::getMetaData()));
    HTMLMapElementImp::setStaticPrivate(new NativeClass(HTMLMapElementImp::getMetaData()));
    HTMLMarqueeElementImp::setStaticPrivate(new NativeClass(HTMLMarqueeElementImp::getMetaData()));
//    HTMLMediaElementImp::setStaticPrivate(new NativeClass(HTMLMediaElementImp::getMetaData()));
    HTMLMenuElementImp::setStaticPrivate(new NativeClass(HTMLMenuElementImp::getMetaData()));
    HTMLMetaElementImp::setStaticPrivate(new NativeClass(HTMLMetaElementImp::getMetaData()));
    HTMLMeterElementImp::setStaticPrivate(new NativeClass(HTMLMeterElementImp::getMetaData()));
    HTMLModElementImp::setStaticPrivate(new NativeClass(HTMLModElementImp::getMetaData()));
    HTMLObjectElementImp::setStaticPrivate(new NativeClass(HTMLObjectElementImp::getMetaData()));
    HTMLOListElementImp::setStaticPrivate(new NativeClass(HTMLOListElementImp::getMetaData()));
    HTMLOptGroupElementImp::setStaticPrivate(new NativeClass(HTMLOptGroupElementImp::getMetaData()));
    HTMLOptionElementImp::setStaticPrivate(new NativeClass(HTMLOptionElementImp::getMetaData()));
    HTMLOptionsCollectionImp::setStaticPrivate(new NativeClass(HTMLOptionsCollectionImp::getMetaData()));
    HTMLOutputElementImp::setStaticPrivate(new NativeClass(HTMLOutputElementImp::getMetaData()));
    HTMLParagraphElementImp::setStaticPrivate(new NativeClass(HTMLParagraphElementImp::getMetaData()));
    HTMLParamElementImp::setStaticPrivate(new NativeClass(HTMLParamElementImp::getMetaData()));
    HTMLPreElementImp::setStaticPrivate(new NativeClass(HTMLPreElementImp::getMetaData()));
    HTMLProgressElementImp::setStaticPrivate(new NativeClass(HTMLProgressElementImp::getMetaData()));
    HTMLQuoteElementImp::setStaticPrivate(new NativeClass(HTMLQuoteElementImp::getMetaData()));
    HTMLScriptElementImp::setStaticPrivate(new NativeClass(HTMLScriptElementImp::getMetaData()));
    HTMLSelectElementImp::setStaticPrivate(new NativeClass(HTMLSelectElementImp::getMetaData()));
    HTMLSourceElementImp::setStaticPrivate(new NativeClass(HTMLSourceElementImp::getMetaData()));
    HTMLSpanElementImp::setStaticPrivate(new NativeClass(HTMLSpanElementImp::getMetaData()));
    HTMLStyleElementImp::setStaticPrivate(new NativeClass(HTMLStyleElementImp::getMetaData()));
    HTMLTableCaptionElementImp::setStaticPrivate(new NativeClass(HTMLTableCaptionElementImp::getMetaData()));
    HTMLTableCellElementImp::setStaticPrivate(new NativeClass(HTMLTableCellElementImp::getMetaData()));
    HTMLTableColElementImp::setStaticPrivate(new NativeClass(HTMLTableColElementImp::getMetaData()));
    HTMLTableDataCellElementImp::setStaticPrivate(new NativeClass(HTMLTableDataCellElementImp::getMetaData()));
    HTMLTableElementImp::setStaticPrivate(new NativeClass(HTMLTableElementImp::getMetaData()));
    HTMLTableHeaderCellElementImp::setStaticPrivate(new NativeClass(HTMLTableHeaderCellElementImp::getMetaData()));
    HTMLTableRowElementImp::setStaticPrivate(new NativeClass(HTMLTableRowElementImp::getMetaData()));
    HTMLTableSectionElementImp::setStaticPrivate(new NativeClass(HTMLTableSectionElementImp::getMetaData()));
    HTMLTemplateElementImp::setStaticPrivate(new NativeClass(HTMLTemplateElementImp::getMetaData()));
    HTMLTextAreaElementImp::setStaticPrivate(new NativeClass(HTMLTextAreaElementImp::getMetaData()));
    HTMLTimeElementImp::setStaticPrivate(new NativeClass(HTMLTimeElementImp::getMetaData()));
    HTMLTitleElementImp::setStaticPrivate(new NativeClass(HTMLTitleElementImp::getMetaData()));
    HTMLTrackElementImp::setStaticPrivate(new NativeClass(HTMLTrackElementImp::getMetaData()));
    HTMLUListElementImp::setStaticPrivate(new NativeClass(HTMLUListElementImp::getMetaData()));
    HTMLUnknownElementImp::setStaticPrivate(new NativeClass(HTMLUnknownElementImp::getMetaData()));
    HTMLVideoElementImp::setStaticPrivate(new NativeClass(HTMLVideoElementImp::getMetaData()));
    ImageDataImp::setStaticPrivate(new NativeClass(ImageDataImp::getMetaData()));
//    LocationImp::setStaticPrivate(new NativeClass(LocationImp::getMetaData()));
    MediaControllerImp::setStaticPrivate(new NativeClass(MediaControllerImp::getMetaData()));
    MediaErrorImp::setStaticPrivate(new NativeClass(MediaErrorImp::getMetaData()));
    MediaQueryListImp::setStaticPrivate(new NativeClass(MediaQueryListImp::getMetaData()));
    MediaQueryListListenerImp::setStaticPrivate(new NativeClass(MediaQueryListListenerImp::getMetaData()));
    MessageChannelImp::setStaticPrivate(new NativeClass(MessageChannelImp::getMetaData()));
    MessageEventImp::setStaticPrivate(new NativeClass(MessageEventImp::getMetaData()));
    MessageEventInitImp::setStaticPrivate(new NativeClass(MessageEventInitImp::getMetaData()));
    MessagePortImp::setStaticPrivate(new NativeClass(MessagePortImp::getMetaData()));
    NavigatorContentUtilsImp::setStaticPrivate(new NativeClass(NavigatorContentUtilsImp::getMetaData()));
    NavigatorImp::setStaticPrivate(new NativeClass(NavigatorImp::getMetaData()));
    NavigatorIDImp::setStaticPrivate(new NativeClass(NavigatorIDImp::getMetaData()));
    NavigatorOnLineImp::setStaticPrivate(new NativeClass(NavigatorOnLineImp::getMetaData()));
    NavigatorStorageUtilsImp::setStaticPrivate(new NativeClass(NavigatorStorageUtilsImp::getMetaData()));
    PageTransitionEventImp::setStaticPrivate(new NativeClass(PageTransitionEventImp::getMetaData()));
    PageTransitionEventInitImp::setStaticPrivate(new NativeClass(PageTransitionEventInitImp::getMetaData()));
    PathImp::setStaticPrivate(new NativeClass(PathImp::getMetaData()));
    PopStateEventImp::setStaticPrivate(new NativeClass(PopStateEventImp::getMetaData()));
    PopStateEventInitImp::setStaticPrivate(new NativeClass(PopStateEventInitImp::getMetaData()));
    RadioNodeListImp::setStaticPrivate(new NativeClass(RadioNodeListImp::getMetaData()));
    ScreenImp::setStaticPrivate(new NativeClass(ScreenImp::getMetaData()));
    TextMetricsImp::setStaticPrivate(new NativeClass(TextMetricsImp::getMetaData()));
    TextTrackCueImp::setStaticPrivate(new NativeClass(TextTrackCueImp::getMetaData()));
    TextTrackCueListImp::setStaticPrivate(new NativeClass(TextTrackCueListImp::getMetaData()));
    TextTrackImp::setStaticPrivate(new NativeClass(TextTrackImp::getMetaData()));
    TextTrackListImp::setStaticPrivate(new NativeClass(TextTrackListImp::getMetaData()));
    TimeRangesImp::setStaticPrivate(new NativeClass(TimeRangesImp::getMetaData()));
    TrackEventImp::setStaticPrivate(new NativeClass(TrackEventImp::getMetaData()));
    TrackEventInitImp::setStaticPrivate(new NativeClass(TrackEventInitImp::getMetaData()));
    TransferableImp::setStaticPrivate(new NativeClass(TransferableImp::getMetaData()));
    ValidityStateImp::setStaticPrivate(new NativeClass(ValidityStateImp::getMetaData()));
    VideoTrackImp::setStaticPrivate(new NativeClass(VideoTrackImp::getMetaData()));
    VideoTrackListImp::setStaticPrivate(new NativeClass(VideoTrackListImp::getMetaData()));
    WindowProxy::setStaticPrivate(new NativeClass(WindowProxy::getMetaData()));
    RangeImp::setStaticPrivate(new NativeClass(RangeImp::getMetaData()));
//    MediaListImp::setStaticPrivate(new NativeClass(MediaListImp::getMetaData()));
//    StyleSheetImp::setStaticPrivate(new NativeClass(StyleSheetImp::getMetaData()));
    SVGMatrixImp::setStaticPrivate(new NativeClass(SVGMatrixImp::getMetaData()));
    NodeFilterImp::setStaticPrivate(new NativeClass(NodeFilterImp::getMetaData()));
    NodeIteratorImp::setStaticPrivate(new NativeClass(NodeIteratorImp::getMetaData()));
    TreeWalkerImp::setStaticPrivate(new NativeClass(TreeWalkerImp::getMetaData()));
#if 0
    ArrayBufferImp::setStaticPrivate(new NativeClass(ArrayBufferImp::getMetaData()));
    ArrayBufferViewImp::setStaticPrivate(new NativeClass(ArrayBufferViewImp::getMetaData()));
    DataViewImp::setStaticPrivate(new NativeClass(DataViewImp::getMetaData()));
    Float32ArrayImp::setStaticPrivate(new NativeClass(Float32ArrayImp::getMetaData()));
    Float64ArrayImp::setStaticPrivate(new NativeClass(Float64ArrayImp::getMetaData()));
    Int16ArrayImp::setStaticPrivate(new NativeClass(Int16ArrayImp::getMetaData()));
    Int32ArrayImp::setStaticPrivate(new NativeClass(Int32ArrayImp::getMetaData()));
    Int8ArrayImp::setStaticPrivate(new NativeClass(Int8ArrayImp::getMetaData()));
    Uint16ArrayImp::setStaticPrivate(new NativeClass(Uint16ArrayImp::getMetaData()));
    Uint32ArrayImp::setStaticPrivate(new NativeClass(Uint32ArrayImp::getMetaData()));
    Uint8ArrayImp::setStaticPrivate(new NativeClass(Uint8ArrayImp::getMetaData()));
    Uint8ClampedArrayImp::setStaticPrivate(new NativeClass(Uint8ClampedArrayImp::getMetaData()));
#endif
    DOMRectReadOnlyImp::setStaticPrivate(new NativeClass(DOMRectReadOnlyImp::getMetaData()));
    DOMRectImp::setStaticPrivate(new NativeClass(DOMRectImp::getMetaData()));
    DOMRectListImp::setStaticPrivate(new NativeClass(DOMRectListImp::getMetaData()));
    XBLContentElementImp::setStaticPrivate(new NativeClass(XBLContentElementImp::getMetaData()));
    XBLImplementationImp::setStaticPrivate(new NativeClass(XBLImplementationImp::getMetaData()));
    XBLImplementationListImp::setStaticPrivate(new NativeClass(XBLImplementationListImp::getMetaData()));
    FormDataImp::setStaticPrivate(new NativeClass(FormDataImp::getMetaData()));
    XMLHttpRequestEventTargetImp::setStaticPrivate(new NativeClass(XMLHttpRequestEventTargetImp::getMetaData()));
    XMLHttpRequestImp::setStaticPrivate(new NativeClass(XMLHttpRequestImp::getMetaData()));
    XMLHttpRequestOptionsImp::setStaticPrivate(new NativeClass(XMLHttpRequestOptionsImp::getMetaData()));
    XMLHttpRequestUploadImp::setStaticPrivate(new NativeClass(XMLHttpRequestUploadImp::getMetaData()));
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

    initializeClasses();
    NativeClass::registerClasses(context, global);

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

void ECMAScriptContext::enter(ObjectImp* window)
{
    pimpl->activate(window);
    current = window;
}

void ECMAScriptContext::exit(ObjectImp* window)
{
}

Any ECMAScriptContext::evaluate(const std::u16string& script)
{
    return pimpl->evaluate(script);
}

Object ECMAScriptContext::compileFunction(const std::u16string& body)
{
    return pimpl->compileFunction(body);
}

Any ECMAScriptContext::callFunction(Object thisObject, Object functionObject, int argc, Any* argv)
{
    return pimpl->callFunction(thisObject, functionObject, argc, argv);
}

Object ECMAScriptContext::xblCreateImplementation(Object object, Object prototype, Object boundElement, Object shadowTree)
{
    return pimpl->xblCreateImplementation(object, prototype, boundElement, shadowTree);
}

void ECMAScriptContext::shutDown()
{
    Impl::shutDown();
}

ObjectImp* ECMAScriptContext::current;
