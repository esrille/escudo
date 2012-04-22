SUFFIXES = .idl .re

%.cpp : %.re
	mkdir -p $(*D);
	re2c -u -i -o $@ $<

vpath %.idl = $(srcdir)/idl

idl_files = \
	cssom.idl \
	cssomview.idl \
	dom2css.idl \
	dom2ranges.idl \
	dom2traversal.idl \
	dom3events.idl \
	domcore.idl \
	eventsource.idl \
	file.idl \
	html5.idl \
	progress-events.idl \
	selectors.idl \
	typedarray.idl \
	xbl2.idl \
	xhr2.idl

generated_headers = \
	org/w3c/dom/AnonXMLHttpRequest.h \
	org/w3c/dom/Attr.h \
	org/w3c/dom/CaretPosition.h \
	org/w3c/dom/CharacterData.h \
	org/w3c/dom/Comment.h \
	org/w3c/dom/DOMElementMap.h \
	org/w3c/dom/DOMException.h \
	org/w3c/dom/DOMImplementation.h \
	org/w3c/dom/DOMSettableTokenList.h \
	org/w3c/dom/DOMStringList.h \
	org/w3c/dom/DOMStringMap.h \
	org/w3c/dom/DOMTokenList.h \
	org/w3c/dom/Document.h \
	org/w3c/dom/DocumentFragment.h \
	org/w3c/dom/DocumentType.h \
	org/w3c/dom/Element.h \
	org/w3c/dom/FormData.h \
	org/w3c/dom/Node.h \
	org/w3c/dom/NodeList.h \
	org/w3c/dom/ProcessingInstruction.h \
	org/w3c/dom/Text.h \
	org/w3c/dom/XMLHttpRequest.h \
	org/w3c/dom/XMLHttpRequestEventTarget.h \
	org/w3c/dom/XMLHttpRequestUpload.h \
	org/w3c/dom/css/CSS2Properties.h \
	org/w3c/dom/css/CSSCharsetRule.h \
	org/w3c/dom/css/CSSColorComponentValue.h \
	org/w3c/dom/css/CSSComponentValue.h \
	org/w3c/dom/css/CSSFontFaceRule.h \
	org/w3c/dom/css/CSSIdentifierComponentValue.h \
	org/w3c/dom/css/CSSImportRule.h \
	org/w3c/dom/css/CSSKeywordComponentValue.h \
	org/w3c/dom/css/CSSLengthComponentValue.h \
	org/w3c/dom/css/CSSMapValue.h \
	org/w3c/dom/css/CSSMediaRule.h \
	org/w3c/dom/css/CSSNamespaceRule.h \
	org/w3c/dom/css/CSSPageRule.h \
	org/w3c/dom/css/CSSPercentageComponentValue.h \
	org/w3c/dom/css/CSSPrimitiveValue.h \
	org/w3c/dom/css/CSSPropertyValue.h \
	org/w3c/dom/css/CSSPropertyValueList.h \
	org/w3c/dom/css/CSSRule.h \
	org/w3c/dom/css/CSSStringComponentValue.h \
	org/w3c/dom/css/CSSStyleDeclaration.h \
	org/w3c/dom/css/CSSStyleDeclarationValue.h \
	org/w3c/dom/css/CSSStyleRule.h \
	org/w3c/dom/css/CSSStyleSheet.h \
	org/w3c/dom/css/CSSURLComponentValue.h \
	org/w3c/dom/css/CSSUnknownRule.h \
	org/w3c/dom/css/CSSValue.h \
	org/w3c/dom/css/CSSValueList.h \
	org/w3c/dom/css/Counter.h \
	org/w3c/dom/css/DOMImplementationCSS.h \
	org/w3c/dom/css/DocumentCSS.h \
	org/w3c/dom/css/ElementCSSInlineStyle.h \
	org/w3c/dom/css/RGBColor.h \
	org/w3c/dom/css/Rect.h \
	org/w3c/dom/events/CompositionEvent.h \
	org/w3c/dom/events/CustomEvent.h \
	org/w3c/dom/events/CustomEventInit.h \
	org/w3c/dom/events/Event.h \
	org/w3c/dom/events/EventException.h \
	org/w3c/dom/events/EventInit.h \
	org/w3c/dom/events/EventListener.h \
	org/w3c/dom/events/EventTarget.h \
	org/w3c/dom/events/FocusEvent.h \
	org/w3c/dom/events/KeyboardEvent.h \
	org/w3c/dom/events/MouseEvent.h \
	org/w3c/dom/events/MutationEvent.h \
	org/w3c/dom/events/MutationNameEvent.h \
	org/w3c/dom/events/ProgressEvent.h \
	org/w3c/dom/events/TextEvent.h \
	org/w3c/dom/events/UIEvent.h \
	org/w3c/dom/events/WheelEvent.h \
	org/w3c/dom/eventsource/EventSource.h \
	org/w3c/dom/file/Blob.h \
	org/w3c/dom/file/File.h \
	org/w3c/dom/file/FileCallback.h \
	org/w3c/dom/file/FileError.h \
	org/w3c/dom/file/FileException.h \
	org/w3c/dom/file/FileList.h \
	org/w3c/dom/file/FileReader.h \
	org/w3c/dom/file/FileReaderSync.h \
	org/w3c/dom/html/ApplicationCache.h \
	org/w3c/dom/html/BarProp.h \
	org/w3c/dom/html/BeforeUnloadEvent.h \
	org/w3c/dom/html/BlobCallback.h \
	org/w3c/dom/html/CanvasGradient.h \
	org/w3c/dom/html/CanvasPattern.h \
	org/w3c/dom/html/CanvasPixelArray.h \
	org/w3c/dom/html/CanvasRenderingContext2D.h \
	org/w3c/dom/html/DataTransfer.h \
	org/w3c/dom/html/DataTransferItem.h \
	org/w3c/dom/html/DataTransferItemList.h \
	org/w3c/dom/html/DragEvent.h \
	org/w3c/dom/html/Function.h \
	org/w3c/dom/html/HTMLAllCollection.h \
	org/w3c/dom/html/HTMLAnchorElement.h \
	org/w3c/dom/html/HTMLAppletElement.h \
	org/w3c/dom/html/HTMLAreaElement.h \
	org/w3c/dom/html/HTMLAudioElement.h \
	org/w3c/dom/html/HTMLBRElement.h \
	org/w3c/dom/html/HTMLBaseElement.h \
	org/w3c/dom/html/HTMLBaseFontElement.h \
	org/w3c/dom/html/HTMLBindingElement.h \
	org/w3c/dom/html/HTMLBodyElement.h \
	org/w3c/dom/html/HTMLButtonElement.h \
	org/w3c/dom/html/HTMLCanvasElement.h \
	org/w3c/dom/html/HTMLCollection.h \
	org/w3c/dom/html/HTMLCommandElement.h \
	org/w3c/dom/html/HTMLDListElement.h \
	org/w3c/dom/html/HTMLDataListElement.h \
	org/w3c/dom/html/HTMLDetailsElement.h \
	org/w3c/dom/html/HTMLDirectoryElement.h \
	org/w3c/dom/html/HTMLDivElement.h \
	org/w3c/dom/html/HTMLDocument.h \
	org/w3c/dom/html/HTMLElement.h \
	org/w3c/dom/html/HTMLEmbedElement.h \
	org/w3c/dom/html/HTMLFieldSetElement.h \
	org/w3c/dom/html/HTMLFontElement.h \
	org/w3c/dom/html/HTMLFormControlsCollection.h \
	org/w3c/dom/html/HTMLFormElement.h \
	org/w3c/dom/html/HTMLFrameElement.h \
	org/w3c/dom/html/HTMLFrameSetElement.h \
	org/w3c/dom/html/HTMLHRElement.h \
	org/w3c/dom/html/HTMLHeadElement.h \
	org/w3c/dom/html/HTMLHeadingElement.h \
	org/w3c/dom/html/HTMLHtmlElement.h \
	org/w3c/dom/html/HTMLIFrameElement.h \
	org/w3c/dom/html/HTMLImageElement.h \
	org/w3c/dom/html/HTMLInputElement.h \
	org/w3c/dom/html/HTMLKeygenElement.h \
	org/w3c/dom/html/HTMLLIElement.h \
	org/w3c/dom/html/HTMLLabelElement.h \
	org/w3c/dom/html/HTMLLegendElement.h \
	org/w3c/dom/html/HTMLLinkElement.h \
	org/w3c/dom/html/HTMLMapElement.h \
	org/w3c/dom/html/HTMLMarqueeElement.h \
	org/w3c/dom/html/HTMLMediaElement.h \
	org/w3c/dom/html/HTMLMenuElement.h \
	org/w3c/dom/html/HTMLMetaElement.h \
	org/w3c/dom/html/HTMLMeterElement.h \
	org/w3c/dom/html/HTMLModElement.h \
	org/w3c/dom/html/HTMLOListElement.h \
	org/w3c/dom/html/HTMLObjectElement.h \
	org/w3c/dom/html/HTMLOptGroupElement.h \
	org/w3c/dom/html/HTMLOptionElement.h \
	org/w3c/dom/html/HTMLOptionsCollection.h \
	org/w3c/dom/html/HTMLOutputElement.h \
	org/w3c/dom/html/HTMLParagraphElement.h \
	org/w3c/dom/html/HTMLParamElement.h \
	org/w3c/dom/html/HTMLPreElement.h \
	org/w3c/dom/html/HTMLProgressElement.h \
	org/w3c/dom/html/HTMLPropertiesCollection.h \
	org/w3c/dom/html/HTMLQuoteElement.h \
	org/w3c/dom/html/HTMLScriptElement.h \
	org/w3c/dom/html/HTMLSelectElement.h \
	org/w3c/dom/html/HTMLSourceElement.h \
	org/w3c/dom/html/HTMLSpanElement.h \
	org/w3c/dom/html/HTMLStyleElement.h \
	org/w3c/dom/html/HTMLTableCaptionElement.h \
	org/w3c/dom/html/HTMLTableCellElement.h \
	org/w3c/dom/html/HTMLTableColElement.h \
	org/w3c/dom/html/HTMLTableDataCellElement.h \
	org/w3c/dom/html/HTMLTableElement.h \
	org/w3c/dom/html/HTMLTableHeaderCellElement.h \
	org/w3c/dom/html/HTMLTableRowElement.h \
	org/w3c/dom/html/HTMLTableSectionElement.h \
	org/w3c/dom/html/HTMLTemplateElement.h \
	org/w3c/dom/html/HTMLTextAreaElement.h \
	org/w3c/dom/html/HTMLTimeElement.h \
	org/w3c/dom/html/HTMLTitleElement.h \
	org/w3c/dom/html/HTMLUListElement.h \
	org/w3c/dom/html/HTMLUnknownElement.h \
	org/w3c/dom/html/HTMLVideoElement.h \
	org/w3c/dom/html/HashChangeEvent.h \
	org/w3c/dom/html/History.h \
	org/w3c/dom/html/ImageData.h \
	org/w3c/dom/html/Location.h \
	org/w3c/dom/html/MediaQueryList.h \
	org/w3c/dom/html/MediaQueryListListener.h \
	org/w3c/dom/html/MessageChannel.h \
	org/w3c/dom/html/MessageEvent.h \
	org/w3c/dom/html/MessagePort.h \
	org/w3c/dom/html/Navigator.h \
	org/w3c/dom/html/PageTransitionEvent.h \
	org/w3c/dom/html/PopStateEvent.h \
	org/w3c/dom/html/PropertyNodeList.h \
	org/w3c/dom/html/RadioNodeList.h \
	org/w3c/dom/html/Screen.h \
	org/w3c/dom/html/TextMetrics.h \
	org/w3c/dom/html/TimeRanges.h \
	org/w3c/dom/html/Transferable.h \
	org/w3c/dom/html/UndoManager.h \
	org/w3c/dom/html/UndoManagerEvent.h \
	org/w3c/dom/html/ValidityState.h \
	org/w3c/dom/html/Window.h \
	org/w3c/dom/ranges/DocumentRange.h \
	org/w3c/dom/ranges/Range.h \
	org/w3c/dom/ranges/RangeException.h \
	org/w3c/dom/stylesheets/LinkStyle.h \
	org/w3c/dom/stylesheets/MediaList.h \
	org/w3c/dom/stylesheets/StyleSheet.h \
	org/w3c/dom/traversal/DocumentTraversal.h \
	org/w3c/dom/traversal/NodeFilter.h \
	org/w3c/dom/traversal/NodeIterator.h \
	org/w3c/dom/traversal/TreeWalker.h \
	org/w3c/dom/typedarray/ArrayBuffer.h \
	org/w3c/dom/typedarray/ArrayBufferView.h \
	org/w3c/dom/typedarray/DataView.h \
	org/w3c/dom/typedarray/Float32Array.h \
	org/w3c/dom/typedarray/Float64Array.h \
	org/w3c/dom/typedarray/Int16Array.h \
	org/w3c/dom/typedarray/Int32Array.h \
	org/w3c/dom/typedarray/Int8Array.h \
	org/w3c/dom/typedarray/Uint16Array.h \
	org/w3c/dom/typedarray/Uint32Array.h \
	org/w3c/dom/typedarray/Uint8Array.h \
	org/w3c/dom/views/ClientRect.h \
	org/w3c/dom/views/ClientRectList.h \
	org/w3c/dom/xbl2/XBLContentElement.h \
	org/w3c/dom/xbl2/XBLImplementation.h \
	org/w3c/dom/xbl2/XBLImplementationList.h

#	org/w3c/dom/html/AudioTrack.h \
#	org/w3c/dom/html/AudioTrackList.h \
#	org/w3c/dom/html/External.h \
#	org/w3c/dom/html/FunctionStringCallback.h \
#	org/w3c/dom/html/HTMLTrackElement.h \
#	org/w3c/dom/html/LocalMediaStream.h \
#	org/w3c/dom/html/MediaController.h \
#	org/w3c/dom/html/MediaError.h \
#	org/w3c/dom/html/MediaStream.h \
#	org/w3c/dom/html/MediaStreamRecorder.h \
#	org/w3c/dom/html/MutableTextTrack.h \
#	org/w3c/dom/html/NavigatorUserMediaError.h \
#	org/w3c/dom/html/NavigatorUserMediaErrorCallback.h \
#	org/w3c/dom/html/NavigatorUserMediaSuccessCallback.h \
#	org/w3c/dom/html/PeerConnection.h \
#	org/w3c/dom/html/SignalingCallback.h \
#	org/w3c/dom/html/StreamEvent.h \
#	org/w3c/dom/html/StreamTrack.h \
#	org/w3c/dom/html/TextTrack.h \
#	org/w3c/dom/html/TextTrackCue.h \
#	org/w3c/dom/html/TextTrackCueList.h \
#	org/w3c/dom/html/VideoTrack.h \
#	org/w3c/dom/html/VideoTrackList.h \
#	org/w3c/dom/html/WindowModal.h

generated_sources = \
	org/w3c/dom/AnonXMLHttpRequest.cpp \
	org/w3c/dom/Attr.cpp \
	org/w3c/dom/CaretPosition.cpp \
	org/w3c/dom/CharacterData.cpp \
	org/w3c/dom/Comment.cpp \
	org/w3c/dom/DOMElementMap.cpp \
	org/w3c/dom/DOMImplementation.cpp \
	org/w3c/dom/DOMSettableTokenList.cpp \
	org/w3c/dom/DOMStringList.cpp \
	org/w3c/dom/DOMStringMap.cpp \
	org/w3c/dom/DOMTokenList.cpp \
	org/w3c/dom/Document.cpp \
	org/w3c/dom/DocumentFragment.cpp \
	org/w3c/dom/DocumentType.cpp \
	org/w3c/dom/Element.cpp \
	org/w3c/dom/FormData.cpp \
	org/w3c/dom/Node.cpp \
	org/w3c/dom/NodeList.cpp \
	org/w3c/dom/ProcessingInstruction.cpp \
	org/w3c/dom/Text.cpp \
	org/w3c/dom/XMLHttpRequest.cpp \
	org/w3c/dom/XMLHttpRequestEventTarget.cpp \
	org/w3c/dom/XMLHttpRequestUpload.cpp \
	org/w3c/dom/css/CSS2Properties.cpp \
	org/w3c/dom/css/CSSCharsetRule.cpp \
	org/w3c/dom/css/CSSColorComponentValue.cpp \
	org/w3c/dom/css/CSSComponentValue.cpp \
	org/w3c/dom/css/CSSFontFaceRule.cpp \
	org/w3c/dom/css/CSSIdentifierComponentValue.cpp \
	org/w3c/dom/css/CSSImportRule.cpp \
	org/w3c/dom/css/CSSKeywordComponentValue.cpp \
	org/w3c/dom/css/CSSLengthComponentValue.cpp \
	org/w3c/dom/css/CSSMapValue.cpp \
	org/w3c/dom/css/CSSMediaRule.cpp \
	org/w3c/dom/css/CSSNamespaceRule.cpp \
	org/w3c/dom/css/CSSPageRule.cpp \
	org/w3c/dom/css/CSSPercentageComponentValue.cpp \
	org/w3c/dom/css/CSSPrimitiveValue.cpp \
	org/w3c/dom/css/CSSPropertyValue.cpp \
	org/w3c/dom/css/CSSPropertyValueList.cpp \
	org/w3c/dom/css/CSSRule.cpp \
	org/w3c/dom/css/CSSStringComponentValue.cpp \
	org/w3c/dom/css/CSSStyleDeclaration.cpp \
	org/w3c/dom/css/CSSStyleDeclarationValue.cpp \
	org/w3c/dom/css/CSSStyleRule.cpp \
	org/w3c/dom/css/CSSStyleSheet.cpp \
	org/w3c/dom/css/CSSURLComponentValue.cpp \
	org/w3c/dom/css/CSSUnknownRule.cpp \
	org/w3c/dom/css/CSSValue.cpp \
	org/w3c/dom/css/CSSValueList.cpp \
	org/w3c/dom/css/Counter.cpp \
	org/w3c/dom/css/DOMImplementationCSS.cpp \
	org/w3c/dom/css/DocumentCSS.cpp \
	org/w3c/dom/css/ElementCSSInlineStyle.cpp \
	org/w3c/dom/css/RGBColor.cpp \
	org/w3c/dom/css/Rect.cpp \
	org/w3c/dom/events/CompositionEvent.cpp \
	org/w3c/dom/events/CustomEvent.cpp \
	org/w3c/dom/events/CustomEventInit.cpp \
	org/w3c/dom/events/Event.cpp \
	org/w3c/dom/events/EventInit.cpp \
	org/w3c/dom/events/EventListener.cpp \
	org/w3c/dom/events/EventTarget.cpp \
	org/w3c/dom/events/FocusEvent.cpp \
	org/w3c/dom/events/KeyboardEvent.cpp \
	org/w3c/dom/events/MouseEvent.cpp \
	org/w3c/dom/events/MutationEvent.cpp \
	org/w3c/dom/events/MutationNameEvent.cpp \
	org/w3c/dom/events/ProgressEvent.cpp \
	org/w3c/dom/events/TextEvent.cpp \
	org/w3c/dom/events/UIEvent.cpp \
	org/w3c/dom/events/WheelEvent.cpp \
	org/w3c/dom/eventsource/EventSource.cpp \
	org/w3c/dom/file/Blob.cpp \
	org/w3c/dom/file/File.cpp \
	org/w3c/dom/file/FileCallback.cpp \
	org/w3c/dom/file/FileError.cpp \
	org/w3c/dom/file/FileList.cpp \
	org/w3c/dom/file/FileReader.cpp \
	org/w3c/dom/file/FileReaderSync.cpp \
	org/w3c/dom/html/ApplicationCache.cpp \
	org/w3c/dom/html/BarProp.cpp \
	org/w3c/dom/html/BeforeUnloadEvent.cpp \
	org/w3c/dom/html/BlobCallback.cpp \
	org/w3c/dom/html/CanvasGradient.cpp \
	org/w3c/dom/html/CanvasPattern.cpp \
	org/w3c/dom/html/CanvasPixelArray.cpp \
	org/w3c/dom/html/CanvasRenderingContext2D.cpp \
	org/w3c/dom/html/DataTransfer.cpp \
	org/w3c/dom/html/DataTransferItem.cpp \
	org/w3c/dom/html/DataTransferItemList.cpp \
	org/w3c/dom/html/DragEvent.cpp \
	org/w3c/dom/html/Function.cpp \
	org/w3c/dom/html/HTMLAllCollection.cpp \
	org/w3c/dom/html/HTMLAnchorElement.cpp \
	org/w3c/dom/html/HTMLAppletElement.cpp \
	org/w3c/dom/html/HTMLAreaElement.cpp \
	org/w3c/dom/html/HTMLAudioElement.cpp \
	org/w3c/dom/html/HTMLBRElement.cpp \
	org/w3c/dom/html/HTMLBaseElement.cpp \
	org/w3c/dom/html/HTMLBaseFontElement.cpp \
	org/w3c/dom/html/HTMLBindingElement.cpp \
	org/w3c/dom/html/HTMLBodyElement.cpp \
	org/w3c/dom/html/HTMLButtonElement.cpp \
	org/w3c/dom/html/HTMLCanvasElement.cpp \
	org/w3c/dom/html/HTMLCollection.cpp \
	org/w3c/dom/html/HTMLCommandElement.cpp \
	org/w3c/dom/html/HTMLDListElement.cpp \
	org/w3c/dom/html/HTMLDataListElement.cpp \
	org/w3c/dom/html/HTMLDetailsElement.cpp \
	org/w3c/dom/html/HTMLDirectoryElement.cpp \
	org/w3c/dom/html/HTMLDivElement.cpp \
	org/w3c/dom/html/HTMLDocument.cpp \
	org/w3c/dom/html/HTMLElement.cpp \
	org/w3c/dom/html/HTMLEmbedElement.cpp \
	org/w3c/dom/html/HTMLFieldSetElement.cpp \
	org/w3c/dom/html/HTMLFontElement.cpp \
	org/w3c/dom/html/HTMLFormControlsCollection.cpp \
	org/w3c/dom/html/HTMLFormElement.cpp \
	org/w3c/dom/html/HTMLFrameElement.cpp \
	org/w3c/dom/html/HTMLFrameSetElement.cpp \
	org/w3c/dom/html/HTMLHRElement.cpp \
	org/w3c/dom/html/HTMLHeadElement.cpp \
	org/w3c/dom/html/HTMLHeadingElement.cpp \
	org/w3c/dom/html/HTMLHtmlElement.cpp \
	org/w3c/dom/html/HTMLIFrameElement.cpp \
	org/w3c/dom/html/HTMLImageElement.cpp \
	org/w3c/dom/html/HTMLInputElement.cpp \
	org/w3c/dom/html/HTMLKeygenElement.cpp \
	org/w3c/dom/html/HTMLLIElement.cpp \
	org/w3c/dom/html/HTMLLabelElement.cpp \
	org/w3c/dom/html/HTMLLegendElement.cpp \
	org/w3c/dom/html/HTMLLinkElement.cpp \
	org/w3c/dom/html/HTMLMapElement.cpp \
	org/w3c/dom/html/HTMLMarqueeElement.cpp \
	org/w3c/dom/html/HTMLMediaElement.cpp \
	org/w3c/dom/html/HTMLMenuElement.cpp \
	org/w3c/dom/html/HTMLMetaElement.cpp \
	org/w3c/dom/html/HTMLMeterElement.cpp \
	org/w3c/dom/html/HTMLModElement.cpp \
	org/w3c/dom/html/HTMLOListElement.cpp \
	org/w3c/dom/html/HTMLObjectElement.cpp \
	org/w3c/dom/html/HTMLOptGroupElement.cpp \
	org/w3c/dom/html/HTMLOptionElement.cpp \
	org/w3c/dom/html/HTMLOptionsCollection.cpp \
	org/w3c/dom/html/HTMLOutputElement.cpp \
	org/w3c/dom/html/HTMLParagraphElement.cpp \
	org/w3c/dom/html/HTMLParamElement.cpp \
	org/w3c/dom/html/HTMLPreElement.cpp \
	org/w3c/dom/html/HTMLProgressElement.cpp \
	org/w3c/dom/html/HTMLPropertiesCollection.cpp \
	org/w3c/dom/html/HTMLQuoteElement.cpp \
	org/w3c/dom/html/HTMLScriptElement.cpp \
	org/w3c/dom/html/HTMLSelectElement.cpp \
	org/w3c/dom/html/HTMLSourceElement.cpp \
	org/w3c/dom/html/HTMLSpanElement.cpp \
	org/w3c/dom/html/HTMLStyleElement.cpp \
	org/w3c/dom/html/HTMLTableCaptionElement.cpp \
	org/w3c/dom/html/HTMLTableCellElement.cpp \
	org/w3c/dom/html/HTMLTableColElement.cpp \
	org/w3c/dom/html/HTMLTableDataCellElement.cpp \
	org/w3c/dom/html/HTMLTableElement.cpp \
	org/w3c/dom/html/HTMLTableHeaderCellElement.cpp \
	org/w3c/dom/html/HTMLTableRowElement.cpp \
	org/w3c/dom/html/HTMLTableSectionElement.cpp \
	org/w3c/dom/html/HTMLTemplateElement.cpp \
	org/w3c/dom/html/HTMLTextAreaElement.cpp \
	org/w3c/dom/html/HTMLTimeElement.cpp \
	org/w3c/dom/html/HTMLTitleElement.cpp \
	org/w3c/dom/html/HTMLUListElement.cpp \
	org/w3c/dom/html/HTMLUnknownElement.cpp \
	org/w3c/dom/html/HTMLVideoElement.cpp \
	org/w3c/dom/html/HashChangeEvent.cpp \
	org/w3c/dom/html/History.cpp \
	org/w3c/dom/html/ImageData.cpp \
	org/w3c/dom/html/Location.cpp \
	org/w3c/dom/html/MediaQueryList.cpp \
	org/w3c/dom/html/MediaQueryListListener.cpp \
	org/w3c/dom/html/MessageChannel.cpp \
	org/w3c/dom/html/MessageEvent.cpp \
	org/w3c/dom/html/MessagePort.cpp \
	org/w3c/dom/html/Navigator.cpp \
	org/w3c/dom/html/PageTransitionEvent.cpp \
	org/w3c/dom/html/PopStateEvent.cpp \
	org/w3c/dom/html/PropertyNodeList.cpp \
	org/w3c/dom/html/RadioNodeList.cpp \
	org/w3c/dom/html/Screen.cpp \
	org/w3c/dom/html/TextMetrics.cpp \
	org/w3c/dom/html/TimeRanges.cpp \
	org/w3c/dom/html/Transferable.cpp \
	org/w3c/dom/html/UndoManager.cpp \
	org/w3c/dom/html/UndoManagerEvent.cpp \
	org/w3c/dom/html/ValidityState.cpp \
	org/w3c/dom/html/Window.cpp \
	org/w3c/dom/ranges/DocumentRange.cpp \
	org/w3c/dom/ranges/Range.cpp \
	org/w3c/dom/stylesheets/LinkStyle.cpp \
	org/w3c/dom/stylesheets/MediaList.cpp \
	org/w3c/dom/stylesheets/StyleSheet.cpp \
	org/w3c/dom/traversal/DocumentTraversal.cpp \
	org/w3c/dom/traversal/NodeFilter.cpp \
	org/w3c/dom/traversal/NodeIterator.cpp \
	org/w3c/dom/traversal/TreeWalker.cpp \
	org/w3c/dom/typedarray/ArrayBuffer.cpp \
	org/w3c/dom/typedarray/ArrayBufferView.cpp \
	org/w3c/dom/typedarray/DataView.cpp \
	org/w3c/dom/typedarray/Float32Array.cpp \
	org/w3c/dom/typedarray/Float64Array.cpp \
	org/w3c/dom/typedarray/Int16Array.cpp \
	org/w3c/dom/typedarray/Int32Array.cpp \
	org/w3c/dom/typedarray/Int8Array.cpp \
	org/w3c/dom/typedarray/Uint16Array.cpp \
	org/w3c/dom/typedarray/Uint32Array.cpp \
	org/w3c/dom/typedarray/Uint8Array.cpp \
	org/w3c/dom/views/ClientRect.cpp \
	org/w3c/dom/views/ClientRectList.cpp \
	org/w3c/dom/xbl2/XBLContentElement.cpp \
	org/w3c/dom/xbl2/XBLImplementation.cpp \
	org/w3c/dom/xbl2/XBLImplementationList.cpp

#	org/w3c/dom/html/AudioTrack.cpp \
#	org/w3c/dom/html/AudioTrackList.cpp \
#	org/w3c/dom/html/External.cpp \
#	org/w3c/dom/html/FunctionStringCallback.cpp \
#	org/w3c/dom/html/HTMLTrackElement.cpp \
#	org/w3c/dom/html/LocalMediaStream.cpp \
#	org/w3c/dom/html/MediaController.cpp \
#	org/w3c/dom/html/MediaError.cpp \
#	org/w3c/dom/html/MediaStream.cpp \
#	org/w3c/dom/html/MediaStreamRecorder.cpp \
#	org/w3c/dom/html/MutableTextTrack.cpp \
#	org/w3c/dom/html/NavigatorUserMediaError.cpp \
#	org/w3c/dom/html/NavigatorUserMediaErrorCallback.cpp \
#	org/w3c/dom/html/NavigatorUserMediaSuccessCallback.cpp \
#	org/w3c/dom/html/PeerConnection.cpp \
#	org/w3c/dom/html/SignalingCallback.cpp \
#	org/w3c/dom/html/StreamEvent.cpp \
#	org/w3c/dom/html/StreamTrack.cpp \
#	org/w3c/dom/html/TextTrack.cpp \
#	org/w3c/dom/html/TextTrackCue.cpp \
#	org/w3c/dom/html/TextTrackCueList.cpp \
#	org/w3c/dom/html/VideoTrack.cpp \
#	org/w3c/dom/html/VideoTrackList.cpp \
#	org/w3c/dom/html/WindowModal.cpp

generated_scanners = \
	css/CSSPropertyNames.cpp \
	css/CSSTokenizer.cpp \
	css/CSSColor.cpp \
	CSSGrammar.cc CSSGrammar.h

BUILT_SOURCES = $(generated_headers) $(generated_scanners) $(generated_sources)

nobase_nodist_include_HEADERS = $(generated_headers)

$(generated_headers) : org/w3c/dom/Node.h

org/w3c/dom/Node.h : $(idl_files)
	esidl -messenger -string std::u16string -object Object -fno-multipleinheritance $^

lib_LIBRARIES = libeshtml5.a libesfontmanager.a libesjsapi.a libesv8api.a

LDADD = libeshtml5.a libesfontmanager.a

AM_CPPFLAGS = \
	-std=gnu++0x \
	-I $(srcdir) \
	-isystem $(builddir) \
	$(ICU_CPPFLAGS) \
	$(FREETYPE_CPPFLAGS) \
	-DXP_UNIX \
	-DYYERROR_VERBOSE \
	-Wall

AM_LDFLAGS = $(ICU_LIBS) $(FREETYPE_LIBS) -ljpeg -lpng -lglut -lGLU -lGL -lXext -lX11 -lm -lrt -lboost_regex -lboost_system -lboost_iostreams

AM_YFLAGS = -d -v -t

libesfontmanager_a_CPPFLAGS = \
	$(AM_CPPFLAGS) \
	-DLIBERATON_TTF=\"$(LIBERATON_TTF)\" \
	-DDEJAVU_TTF=\"$(DEJAVU_TTF)\" \
	-DHAVE_AHEM=\"$(HAVE_AHEM)\" \
	-DHAVE_IPA_GOTHIC=\"$(HAVE_IPA_GOTHIC)\" \
	-DHAVE_IPA_MINCHO=\"$(HAVE_IPA_MINCHO)\" \
	-DHAVE_IPA_PGOTHIC=\"$(HAVE_IPA_PGOTHIC)\" \
	-DHAVE_IPA_PMINCHO=\"$(HAVE_IPA_PMINCHO)\" \
	-DHAVE_AEGEAN=\"$(HAVE_AEGEAN)\" \
	-DTEST_FONTS=\"$(TEST_FONTS)\"

libesfontmanager_a_SOURCES = \
	font/FontManager.cpp \
	font/FontManager.h \
	font/FontManagerBackEndGL.h \
	font/FontDatabase.h \
	font/FontDatabase.cpp

libesjsapi_a_SOURCES = \
	js/esjsapi.h \
	js/bridge.cpp \
	js/Script.cpp

libesv8api_a_SOURCES = \
	v8/esv8api.h \
	v8/bridgeV8.cpp \
	v8/ScriptV8.cpp

org/w3c/dom/Node.cpp : $(idl_files)
	esidl -messenger-src -string std::u16string -object Object -fno-multipleinheritance $^

$(generated_sources) : org/w3c/dom/Node.cpp

nodist_libeshtml5_a_SOURCES = $(generated_sources) $(generated_headers) $(generated_scanners)

libeshtml5_a_SOURCES = \
	Object.h \
	ObjectArrayImp.h \
	Any.cpp \
	Any.h \
	nullable.h \
	sequence.h \
	ECMAScript.h \
	utf.h \
	utf.cpp \
	TextIterator.h \
	U16InputStream.cpp \
	U16InputStream.h \
	DocumentWindow.cpp \
	DocumentWindow.h \
	url/URI.h \
	url/URI.cpp \
	url/URL.h \
	url/URL.cpp \
	http/HTTPUtil.h \
	http/HTTPUtil.cpp \
	http/HTTPHeader.h \
	http/HTTPHeader.cpp \
	http/HTTPRequestMessage.h \
	http/HTTPRequestMessage.cpp \
	http/HTTPResponseMessage.h \
	http/HTTPResponseMessage.cpp \
	http/HTTPHeader.cpp \
	http/HTTPRequest.h \
	http/HTTPRequest.cpp \
	http/HTTPConnection.h \
	http/HTTPConnection.cpp \
	http/HTTPCache.h \
	http/HTTPCache.cpp \
	html/HTMLInputStream.cpp \
	html/HTMLInputStream.h \
	html/HTMLParser.cpp \
	html/HTMLParser.h \
	html/HTMLTokenizer.cpp \
	html/HTMLTokenizer.h \
	css/Box.cpp \
	css/Box.h \
	css/BoxGL.cpp \
	css/BoxImage.cpp \
	css/FormattingContext.cpp \
	css/LineBox.cpp \
	css/StackingContext.cpp \
	css/StackingContext.h \
	css/CSSColor.re \
	css/CSSPropertyNames.re \
	css/CSSPropertyValueImp.cpp \
	css/CSSPropertyValueImp.h \
	css/CSSSerialize.cpp \
	css/CSSSerialize.h \
	css/CSSGrammar.yy \
	css/CSSSelector.cpp \
	css/CSSSelector.h \
	css/CSSTokenizer.re \
	css/CSSParser.cpp \
	css/CSSParser.h \
	css/CSSValueParser.cpp \
	css/CSSValueParser.h \
	css/CSSInputStream.cpp \
	css/CSSInputStream.h \
	css/Replaced.cpp \
	css/Table.cpp \
	css/Table.h \
	css/TableGL.cpp \
	css/ViewCSSImp.cpp \
	css/ViewCSSImp.h \
	css/ViewCSSImpGL.cpp \
	Test.util.h \
	Test.util.cpp

# implementation files originally generated by esidl
libeshtml5_a_SOURCES += \
	ApplicationCacheImp.cpp \
	ApplicationCacheImp.h \
	AttrImp.cpp \
	AttrImp.h \
	BarPropImp.cpp \
	BarPropImp.h \
	BeforeUnloadEventImp.cpp \
	BeforeUnloadEventImp.h \
	BlobImp.cpp \
	BlobImp.h \
	CaretPositionImp.cpp \
	CaretPositionImp.h \
	CharacterDataImp.cpp \
	CharacterDataImp.h \
	ClientRectImp.cpp \
	ClientRectImp.h \
	ClientRectListImp.cpp \
	ClientRectListImp.h \
	CommentImp.cpp \
	CommentImp.h \
	CompositionEventImp.cpp \
	CompositionEventImp.h \
	CustomEventImp.cpp \
	CustomEventImp.h \
	CustomEventInitImp.cpp \
	CustomEventInitImp.h \
	DOMImplementationImp.cpp \
	DOMImplementationImp.h \
	DOMSettableTokenListImp.cpp \
	DOMSettableTokenListImp.h \
	DOMStringListImp.cpp \
	DOMStringListImp.h \
	DOMStringMapImp.cpp \
	DOMStringMapImp.h \
	DOMTokenListImp.cpp \
	DOMTokenListImp.h \
	DocumentFragmentImp.cpp \
	DocumentFragmentImp.h \
	DocumentImp.cpp \
	DocumentImp.h \
	DocumentRangeImp.cpp \
	DocumentRangeImp.h \
	DocumentTraversalImp.cpp \
	DocumentTraversalImp.h \
	DocumentTypeImp.cpp \
	DocumentTypeImp.h \
	DragEventImp.cpp \
	DragEventImp.h \
	ElementImp.cpp \
	ElementImp.h \
	EventImp.cpp \
	EventImp.h \
	EventInitImp.cpp \
	EventInitImp.h \
	EventListenerImp.cpp \
	EventListenerImp.h \
	EventSourceImp.cpp \
	EventSourceImp.h \
	EventTargetImp.cpp \
	EventTargetImp.h \
	FocusEventImp.cpp \
	FocusEventImp.h \
	FunctionImp.cpp \
	FunctionImp.h \
	HashChangeEventImp.cpp \
	HashChangeEventImp.h \
	HistoryImp.cpp \
	HistoryImp.h \
	KeyboardEventImp.cpp \
	KeyboardEventImp.h \
	LinkStyleImp.cpp \
	LinkStyleImp.h \
	LocationImp.cpp \
	LocationImp.h \
	MediaErrorImp.cpp \
	MediaErrorImp.h \
	MediaListImp.cpp \
	MediaListImp.h \
	MediaQueryListImp.cpp \
	MediaQueryListImp.h \
	MediaQueryListListenerImp.cpp \
	MediaQueryListListenerImp.h \
	MessageChannelImp.cpp \
	MessageChannelImp.h \
	MessageEventImp.cpp \
	MessageEventImp.h \
	MessagePortImp.cpp \
	MessagePortImp.h \
	MouseEventImp.cpp \
	MouseEventImp.h \
	MutationEventImp.cpp \
	MutationEventImp.h \
	MutationNameEventImp.cpp \
	MutationNameEventImp.h \
	NavigatorImp.cpp \
	NavigatorImp.h \
	NodeFilterImp.cpp \
	NodeFilterImp.h \
	NodeImp.cpp \
	NodeImp.h \
	NodeIteratorImp.cpp \
	NodeIteratorImp.h \
	NodeListImp.cpp \
	NodeListImp.h \
	ObjectArrayImp.h \
	PageTransitionEventImp.cpp \
	PageTransitionEventImp.h \
	PopStateEventImp.cpp \
	PopStateEventImp.h \
	ProcessingInstructionImp.cpp \
	ProcessingInstructionImp.h \
	ProgressEventImp.cpp \
	ProgressEventImp.h \
	PropertyNodeListImp.cpp \
	PropertyNodeListImp.h \
	RangeImp.cpp \
	RangeImp.h \
	ScreenImp.cpp \
	ScreenImp.h \
	StyleSheetImp.cpp \
	StyleSheetImp.h \
	TextEventImp.cpp \
	TextEventImp.h \
	TextImp.cpp \
	TextImp.h \
	TreeWalkerImp.cpp \
	TreeWalkerImp.h \
	UIEventImp.cpp \
	UIEventImp.h \
	UndoManagerEventImp.cpp \
	UndoManagerEventImp.h \
	UndoManagerImp.cpp \
	UndoManagerImp.h \
	WheelEventImp.cpp \
	WheelEventImp.h \
	WindowImp.cpp \
	WindowImp.h \
	css/CSS2PropertiesImp.cpp \
	css/CSS2PropertiesImp.h \
	css/CSSCharsetRuleImp.cpp \
	css/CSSCharsetRuleImp.h \
	css/CSSColorComponentValueImp.cpp \
	css/CSSColorComponentValueImp.h \
	css/CSSComponentValueImp.cpp \
	css/CSSComponentValueImp.h \
	css/CSSFontFaceRuleImp.cpp \
	css/CSSFontFaceRuleImp.h \
	css/CSSIdentifierComponentValueImp.cpp \
	css/CSSIdentifierComponentValueImp.h \
	css/CSSImportRuleImp.cpp \
	css/CSSImportRuleImp.h \
	css/CSSKeywordComponentValueImp.cpp \
	css/CSSKeywordComponentValueImp.h \
	css/CSSLengthComponentValueImp.cpp \
	css/CSSLengthComponentValueImp.h \
	css/CSSMapValueImp.cpp \
	css/CSSMapValueImp.h \
	css/CSSMediaRuleImp.cpp \
	css/CSSMediaRuleImp.h \
	css/CSSNamespaceRuleImp.cpp \
	css/CSSNamespaceRuleImp.h \
	css/CSSPageRuleImp.cpp \
	css/CSSPageRuleImp.h \
	css/CSSPercentageComponentValueImp.cpp \
	css/CSSPercentageComponentValueImp.h \
	css/CSSPrimitiveValueImp.cpp \
	css/CSSPrimitiveValueImp.h \
	css/CSSPropertyValueListImp.cpp \
	css/CSSPropertyValueListImp.h \
	css/CSSRuleImp.cpp \
	css/CSSRuleImp.h \
	css/CSSRuleListImp.cpp \
	css/CSSRuleListImp.h \
	css/CSSStringComponentValueImp.cpp \
	css/CSSStringComponentValueImp.h \
	css/CSSStyleDeclarationImp.cpp \
	css/CSSStyleDeclarationImp.h \
	css/CSSStyleDeclarationValueImp.cpp \
	css/CSSStyleDeclarationValueImp.h \
	css/CSSStyleRuleImp.cpp \
	css/CSSStyleRuleImp.h \
	css/CSSStyleSheetImp.cpp \
	css/CSSStyleSheetImp.h \
	css/CSSURLComponentValueImp.cpp \
	css/CSSURLComponentValueImp.h \
	css/CSSUnknownRuleImp.cpp \
	css/CSSUnknownRuleImp.h \
	css/CSSValueImp.cpp \
	css/CSSValueImp.h \
	css/CSSValueListImp.cpp \
	css/CSSValueListImp.h \
	css/CounterImp.cpp \
	css/CounterImp.h \
	css/DOMImplementationCSSImp.cpp \
	css/DOMImplementationCSSImp.h \
	css/DocumentCSSImp.cpp \
	css/DocumentCSSImp.h \
	css/ElementCSSInlineStyleImp.cpp \
	css/ElementCSSInlineStyleImp.h \
	css/RGBColorImp.cpp \
	css/RGBColorImp.h \
	css/RectImp.cpp \
	css/RectImp.h \
	css/ViewCSSImp.cpp \
	css/ViewCSSImp.h \
	file/FileErrorImp.cpp \
	file/FileErrorImp.h \
	file/FileImp.cpp \
	file/FileImp.h \
	file/FileListImp.cpp \
	file/FileListImp.h \
	file/FileReaderImp.cpp \
	file/FileReaderImp.h \
	file/FileReaderSyncImp.cpp \
	file/FileReaderSyncImp.h \
	html/CanvasGradientImp.cpp \
	html/CanvasGradientImp.h \
	html/CanvasPatternImp.cpp \
	html/CanvasPatternImp.h \
	html/CanvasPixelArrayImp.cpp \
	html/CanvasPixelArrayImp.h \
	html/CanvasRenderingContext2DImp.cpp \
	html/CanvasRenderingContext2DImp.h \
	html/DataTransferImp.cpp \
	html/DataTransferImp.h \
	html/DataTransferItemImp.cpp \
	html/DataTransferItemImp.h \
	html/DataTransferItemListImp.cpp \
	html/DataTransferItemListImp.h \
	html/HTMLAllCollectionImp.cpp \
	html/HTMLAllCollectionImp.h \
	html/HTMLAnchorElementImp.cpp \
	html/HTMLAnchorElementImp.h \
	html/HTMLAppletElementImp.cpp \
	html/HTMLAppletElementImp.h \
	html/HTMLAreaElementImp.cpp \
	html/HTMLAreaElementImp.h \
	html/HTMLAudioElementImp.cpp \
	html/HTMLAudioElementImp.h \
	html/HTMLBindingElementImp.cpp \
	html/HTMLBindingElementImp.h \
	html/HTMLBRElementImp.cpp \
	html/HTMLBRElementImp.h \
	html/HTMLBaseElementImp.cpp \
	html/HTMLBaseElementImp.h \
	html/HTMLBaseFontElementImp.cpp \
	html/HTMLBaseFontElementImp.h \
	html/HTMLBodyElementImp.cpp \
	html/HTMLBodyElementImp.h \
	html/HTMLButtonElementImp.cpp \
	html/HTMLButtonElementImp.h \
	html/HTMLCanvasElementImp.cpp \
	html/HTMLCanvasElementImp.h \
	html/HTMLCollectionImp.cpp \
	html/HTMLCollectionImp.h \
	html/HTMLCommandElementImp.cpp \
	html/HTMLCommandElementImp.h \
	html/HTMLDListElementImp.cpp \
	html/HTMLDListElementImp.h \
	html/HTMLDataListElementImp.cpp \
	html/HTMLDataListElementImp.h \
	html/HTMLDetailsElementImp.cpp \
	html/HTMLDetailsElementImp.h \
	html/HTMLDirectoryElementImp.cpp \
	html/HTMLDirectoryElementImp.h \
	html/HTMLDivElementImp.cpp \
	html/HTMLDivElementImp.h \
	html/HTMLDocumentImp.cpp \
	html/HTMLDocumentImp.h \
	html/HTMLElementImp.cpp \
	html/HTMLElementImp.h \
	html/HTMLEmbedElementImp.cpp \
	html/HTMLEmbedElementImp.h \
	html/HTMLFieldSetElementImp.cpp \
	html/HTMLFieldSetElementImp.h \
	html/HTMLFontElementImp.cpp \
	html/HTMLFontElementImp.h \
	html/HTMLFormControlsCollectionImp.cpp \
	html/HTMLFormControlsCollectionImp.h \
	html/HTMLFormElementImp.cpp \
	html/HTMLFormElementImp.h \
	html/HTMLFrameElementImp.cpp \
	html/HTMLFrameElementImp.h \
	html/HTMLFrameSetElementImp.cpp \
	html/HTMLFrameSetElementImp.h \
	html/HTMLHRElementImp.cpp \
	html/HTMLHRElementImp.h \
	html/HTMLHeadElementImp.cpp \
	html/HTMLHeadElementImp.h \
	html/HTMLHeadingElementImp.cpp \
	html/HTMLHeadingElementImp.h \
	html/HTMLHtmlElementImp.cpp \
	html/HTMLHtmlElementImp.h \
	html/HTMLIFrameElementImp.cpp \
	html/HTMLIFrameElementImp.h \
	html/HTMLImageElementImp.cpp \
	html/HTMLImageElementImp.h \
	html/HTMLInputElementImp.cpp \
	html/HTMLInputElementImp.h \
	html/HTMLKeygenElementImp.cpp \
	html/HTMLKeygenElementImp.h \
	html/HTMLLIElementImp.cpp \
	html/HTMLLIElementImp.h \
	html/HTMLLabelElementImp.cpp \
	html/HTMLLabelElementImp.h \
	html/HTMLLegendElementImp.cpp \
	html/HTMLLegendElementImp.h \
	html/HTMLLinkElementImp.cpp \
	html/HTMLLinkElementImp.h \
	html/HTMLMapElementImp.cpp \
	html/HTMLMapElementImp.h \
	html/HTMLMarqueeElementImp.cpp \
	html/HTMLMarqueeElementImp.h \
	html/HTMLMediaElementImp.cpp \
	html/HTMLMediaElementImp.h \
	html/HTMLMenuElementImp.cpp \
	html/HTMLMenuElementImp.h \
	html/HTMLMetaElementImp.cpp \
	html/HTMLMetaElementImp.h \
	html/HTMLMeterElementImp.cpp \
	html/HTMLMeterElementImp.h \
	html/HTMLModElementImp.cpp \
	html/HTMLModElementImp.h \
	html/HTMLOListElementImp.cpp \
	html/HTMLOListElementImp.h \
	html/HTMLObjectElementImp.cpp \
	html/HTMLObjectElementImp.h \
	html/HTMLOptGroupElementImp.cpp \
	html/HTMLOptGroupElementImp.h \
	html/HTMLOptionElementImp.cpp \
	html/HTMLOptionElementImp.h \
	html/HTMLOptionsCollectionImp.cpp \
	html/HTMLOptionsCollectionImp.h \
	html/HTMLOutputElementImp.cpp \
	html/HTMLOutputElementImp.h \
	html/HTMLParagraphElementImp.cpp \
	html/HTMLParagraphElementImp.h \
	html/HTMLParamElementImp.cpp \
	html/HTMLParamElementImp.h \
	html/HTMLPreElementImp.cpp \
	html/HTMLPreElementImp.h \
	html/HTMLProgressElementImp.cpp \
	html/HTMLProgressElementImp.h \
	html/HTMLPropertiesCollectionImp.cpp \
	html/HTMLPropertiesCollectionImp.h \
	html/HTMLQuoteElementImp.cpp \
	html/HTMLQuoteElementImp.h \
	html/HTMLReplacedElementImp.h \
	html/HTMLScriptElementImp.cpp \
	html/HTMLScriptElementImp.h \
	html/HTMLSelectElementImp.cpp \
	html/HTMLSelectElementImp.h \
	html/HTMLSourceElementImp.cpp \
	html/HTMLSourceElementImp.h \
	html/HTMLSpanElementImp.cpp \
	html/HTMLSpanElementImp.h \
	html/HTMLStyleElementImp.cpp \
	html/HTMLStyleElementImp.h \
	html/HTMLTableCaptionElementImp.cpp \
	html/HTMLTableCaptionElementImp.h \
	html/HTMLTableCellElementImp.cpp \
	html/HTMLTableCellElementImp.h \
	html/HTMLTableColElementImp.cpp \
	html/HTMLTableColElementImp.h \
	html/HTMLTableDataCellElementImp.cpp \
	html/HTMLTableDataCellElementImp.h \
	html/HTMLTableElementImp.cpp \
	html/HTMLTableElementImp.h \
	html/HTMLTableHeaderCellElementImp.cpp \
	html/HTMLTableHeaderCellElementImp.h \
	html/HTMLTableRowElementImp.cpp \
	html/HTMLTableRowElementImp.h \
	html/HTMLTableSectionElementImp.cpp \
	html/HTMLTableSectionElementImp.h \
	html/HTMLTemplateElementImp.cpp \
	html/HTMLTemplateElementImp.h \
	html/HTMLTextAreaElementImp.cpp \
	html/HTMLTextAreaElementImp.h \
	html/HTMLTimeElementImp.cpp \
	html/HTMLTimeElementImp.h \
	html/HTMLTitleElementImp.cpp \
	html/HTMLTitleElementImp.h \
	html/HTMLUListElementImp.cpp \
	html/HTMLUListElementImp.h \
	html/HTMLUnknownElementImp.cpp \
	html/HTMLUnknownElementImp.h \
	html/HTMLVideoElementImp.cpp \
	html/HTMLVideoElementImp.h \
	html/ImageDataImp.cpp \
	html/ImageDataImp.h \
	html/RadioNodeListImp.cpp \
	html/RadioNodeListImp.h \
	html/TextMetricsImp.cpp \
	html/TextMetricsImp.h \
	html/TimeRangesImp.cpp \
	html/TimeRangesImp.h \
	html/ValidityStateImp.cpp \
	html/ValidityStateImp.h \
	typedarray/ArrayBufferImp.cpp \
	typedarray/ArrayBufferImp.h \
	typedarray/ArrayBufferViewImp.cpp \
	typedarray/ArrayBufferViewImp.h \
	typedarray/DataViewImp.cpp \
	typedarray/DataViewImp.h \
	typedarray/Float32ArrayImp.cpp \
	typedarray/Float32ArrayImp.h \
	typedarray/Float64ArrayImp.cpp \
	typedarray/Float64ArrayImp.h \
	typedarray/Int16ArrayImp.cpp \
	typedarray/Int16ArrayImp.h \
	typedarray/Int32ArrayImp.cpp \
	typedarray/Int32ArrayImp.h \
	typedarray/Int8ArrayImp.cpp \
	typedarray/Int8ArrayImp.h \
	typedarray/Uint16ArrayImp.cpp \
	typedarray/Uint16ArrayImp.h \
	typedarray/Uint32ArrayImp.cpp \
	typedarray/Uint32ArrayImp.h \
	typedarray/Uint8ArrayImp.cpp \
	typedarray/Uint8ArrayImp.h \
	xbl/XBLContentElementImp.cpp \
	xbl/XBLContentElementImp.h \
	xbl/XBLImplementationImp.cpp \
	xbl/XBLImplementationImp.h \
	xbl/XBLImplementationListImp.cpp \
	xbl/XBLImplementationListImp.h \
	xhr/AnonXMLHttpRequestImp.cpp \
	xhr/AnonXMLHttpRequestImp.h \
	xhr/FormDataImp.cpp \
	xhr/FormDataImp.h \
	xhr/XMLHttpRequestEventTargetImp.cpp \
	xhr/XMLHttpRequestEventTargetImp.h \
	xhr/XMLHttpRequestImp.cpp \
	xhr/XMLHttpRequestImp.h \
	xhr/XMLHttpRequestUploadImp.cpp \
	xhr/XMLHttpRequestUploadImp.h

bin_PROGRAMS = \
	harness \
	Any.test \
	FontManager.test \
	URL.test \
	HTTPHeader.test \
	HTTPRequest.test \
	HTMLInputStream.test \
	HTMLInputStream.test.getChar \
	HTMLTokenizer.test \
	HTMLParser.test \
	CSSTokenizer.test \
	CSSParser.test \
	CSSStyle.test \
	Box.test \
	Script.test \
	ScriptV8.test \
	Navigator.test

bin_PROGRAMS : $(LDADD)

js_LDADD = libeshtml5.a libesjsapi.a libeshtml5.a libesjsapi.a libesfontmanager.a
v8_LDADD = libeshtml5.a libesv8api.a libeshtml5.a libesv8api.a libesfontmanager.a -lv8

harness_SOURCES = harness/harness.cpp
harness_LDADD = $(js_LDADD)

Any_test_SOURCES = Any.test.cpp
Any_test_LDADD = $(js_LDADD)

HTMLInputStream_test_SOURCES = HTMLInputStream.test.cpp
HTMLInputStream_test_LDADD = $(js_LDADD)

HTMLInputStream_test_getChar_SOURCES = HTMLInputStream.test.getChar.cpp
HTMLInputStream_test_getChar_LDADD = $(js_LDADD)

HTMLTokenizer_test_SOURCES = HTMLTokenizer.test.cpp
HTMLTokenizer_test_LDADD = $(js_LDADD)

HTMLParser_test_SOURCES = HTMLParser.test.cpp
HTMLParser_test_LDADD = $(js_LDADD)

CSSTokenizer_test_SOURCES = CSSTokenizer.test.cpp
CSSTokenizer_test_LDADD = $(js_LDADD)

CSSParser_test_SOURCES = CSSParser.test.cpp
CSSParser_test_LDADD = $(js_LDADD)

CSSStyle_test_SOURCES = CSSStyle.test.cpp
CSSStyle_test_LDADD = $(js_LDADD)

Box_test_SOURCES = Box.test.cpp
Box_test_LDADD = $(js_LDADD)

FontManager_test_SOURCES = FontManager.test.cpp
FontManager_test_LDADD = $(js_LDADD)

URL_test_SOURCES = URL.test.cpp
URL_test_LDADD = $(js_LDADD)

HTTPHeader_test_SOURCES = HTTPHeader.test.cpp
HTTPHeader_test_LDADD = $(js_LDADD)

HTTPRequest_test_SOURCES = HTTPRequest.test.cpp
HTTPRequest_test_LDADD = $(js_LDADD)

Script_test_SOURCES = Script.test.cpp Test.glut.cpp
Script_test_LDADD = $(js_LDADD)

ScriptV8_test_SOURCES = ScriptV8.test.cpp Test.glut.cpp
ScriptV8_test_LDADD = $(v8_LDADD)

Navigator_test_SOURCES = Navigator.test.cpp Test.glut.cpp
Navigator_test_LDADD = $(js_LDADD)

clean-local:
	rm -rf org css html $(generated_sources) $(generated_headers) $(generated_scanners) *.output

EXTRA_DIST = \
	config.h \
	idl/COPYRIGHT.html \
	idl/2dcontext.idl \
	idl/arrays.idl \
	idl/console.idl \
	idl/cssom.idl \
	idl/cssomview.idl \
	idl/dom2css.idl \
	idl/dom2events.idl \
	idl/dom2ranges.idl \
	idl/dom2stylesheets.idl \
	idl/dom2traversal.idl \
	idl/dom2views.idl \
	idl/dom3core.idl \
	idl/dom3events.idl \
	idl/dom3ls.idl \
	idl/dom3validation.idl \
	idl/dom3xpath.idl \
	idl/domcore.idl \
	idl/elementtraversal.idl \
	idl/eventsource.idl \
	idl/file.idl \
	idl/geolocation.idl \
	idl/html5.idl \
	idl/idl_scraper.pl \
	idl/indexeddb.idl \
	idl/microdata.idl \
	idl/NOTICE \
	idl/postmsg.idl \
	idl/progress-events.idl \
	idl/selectors.idl \
	idl/smil.idl \
	idl/svg.idl \
	idl/typedarray.idl \
	idl/url.idl \
	idl/webdatabase.idl \
	idl/webgl.idl \
	idl/webnotifications.idl \
	idl/webstorage.idl \
	idl/websocket.idl \
	idl/wget_webidl_dom \
	idl/workers.idl \
	idl/xbl2.idl \
	idl/xhr2.idl
