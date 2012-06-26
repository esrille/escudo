SUFFIXES = .idl .re
SUBDIRS = data

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
	src/css/CSSPropertyNames.cpp \
	src/css/CSSTokenizer.cpp \
	src/css/CSSColor.cpp \
	CSSGrammar.cc CSSGrammar.h

BUILT_SOURCES = $(generated_headers) $(generated_scanners) $(generated_sources)

nobase_nodist_noinst_HEADERS = $(generated_headers)

$(generated_headers) : org/w3c/dom/Node.h

org/w3c/dom/Node.h : $(idl_files)
	esidl -messenger -string std::u16string -object Object -fno-multipleinheritance $^

noinst_LIBRARIES = libeshtml5.a libesfontmanager.a libesjsapi.a libesv8api.a

LDADD = libeshtml5.a libesfontmanager.a \
	$(ICU_LIBS) $(FREETYPE_LIBS) \
	-lboost_regex -lboost_system -lboost_iostreams \
	-ljpeg -lpng -lgif -lglut -lGLU -lGL -lXext -lX11 -lXmu -lm -lrt

AM_CPPFLAGS = \
	-std=gnu++0x \
	-I $(srcdir) -I $(srcdir)/src \
	-isystem $(builddir) \
	$(ICU_CPPFLAGS) \
	$(FREETYPE_CPPFLAGS) \
	-DXP_UNIX \
	-DYYERROR_VERBOSE \
	-Wall

AM_YFLAGS = -d -v -t

libesfontmanager_a_CPPFLAGS = $(AM_CPPFLAGS)

if HAVE_LIBERATON
libesfontmanager_a_CPPFLAGS += -DLIBERATON_TTF=\"$(LIBERATON_TTF)\"
endif	
if HAVE_DEJAVU
libesfontmanager_a_CPPFLAGS += -DDEJAVU_TTF=\"$(DEJAVU_TTF)\"
endif	
if HAVE_IPA_GOTHIC
libesfontmanager_a_CPPFLAGS += -DHAVE_IPA_GOTHIC=\"$(HAVE_IPA_GOTHIC)\"
endif	
if HAVE_IPA_MINCHO
libesfontmanager_a_CPPFLAGS += -DHAVE_IPA_MINCHO=\"$(HAVE_IPA_MINCHO)\"
endif	
if HAVE_IPA_PGOTHIC
libesfontmanager_a_CPPFLAGS += -DHAVE_IPA_PGOTHIC=\"$(HAVE_IPA_PGOTHIC)\"
endif	
if HAVE_IPA_PMINCHO
libesfontmanager_a_CPPFLAGS += -DHAVE_IPA_PMINCHO=\"$(HAVE_IPA_PMINCHO)\"
endif	
if HAVE_AEGEAN
libesfontmanager_a_CPPFLAGS += -DHAVE_AEGEAN=\"$(HAVE_AEGEAN)\"
endif	
if HAVE_AHEM
libesfontmanager_a_CPPFLAGS += -DHAVE_AHEM=\"$(HAVE_AHEM)\"
endif	
if TEST_FONTS
libesfontmanager_a_CPPFLAGS += -DTEST_FONTS=\"$(TEST_FONTS)\"
endif	
	
libesfontmanager_a_SOURCES = \
	src/font/FontManager.cpp \
	src/font/FontManager.h \
	src/font/FontManagerBackEndGL.h \
	src/font/FontDatabase.h \
	src/font/FontDatabase.cpp

libesjsapi_a_SOURCES = \
	src/js/bridge.cpp \
	src/js/esjsapi.h \
	src/js/Script.cpp \
	src/js/Script.h

libesv8api_a_SOURCES = \
	src/v8/bridgeV8.cpp \
	src/v8/esv8api.h \
	src/v8/ScriptV8.cpp \
	src/v8/ScriptV8.h

org/w3c/dom/Node.cpp : $(idl_files)
	esidl -messenger-src -string std::u16string -object Object -fno-multipleinheritance $^

$(generated_sources) : org/w3c/dom/Node.cpp

nodist_libeshtml5_a_SOURCES = $(generated_sources) $(generated_headers) $(generated_scanners)

libeshtml5_a_SOURCES = \
	org/w3c/dom/ObjectArray.h \
	src/Object.h \
	src/ObjectArrayImp.h \
	src/Reflect.h \
	src/Any.cpp \
	src/Any.h \
	src/Variadic.h \
	src/nullable.h \
	src/sequence.h \
	src/ECMAScript.h \
	src/utf.h \
	src/utf.cpp \
	src/TextIterator.h \
	src/U16InputStream.cpp \
	src/U16InputStream.h \
	src/BackgroundTask.cpp \
	src/DocumentWindow.cpp \
	src/DocumentWindow.h \
	src/Profile.cpp \
	src/Profile.h \
	src/Test.util.h \
	src/Test.util.cpp \
	src/Test.glut.cpp \
	src/Test.x11.cpp \
	src/url/URI.h \
	src/url/URI.cpp \
	src/url/URL.h \
	src/url/URL.cpp \
	src/http/HTTPCache.h \
	src/http/HTTPCache.cpp \
	src/http/HTTPConnection.h \
	src/http/HTTPConnection.cpp \
	src/http/HTTPHeader.h \
	src/http/HTTPHeader.cpp \
	src/http/HTTPRequest.h \
	src/http/HTTPRequest.cpp \
	src/http/HTTPRequestMessage.h \
	src/http/HTTPRequestMessage.cpp \
	src/http/HTTPResponseMessage.h \
	src/http/HTTPResponseMessage.cpp \
	src/http/HTTPUtil.h \
	src/http/HTTPUtil.cpp \
	src/html/HTMLInputStream.cpp \
	src/html/HTMLInputStream.h \
	src/html/HTMLParser.cpp \
	src/html/HTMLParser.h \
	src/html/HTMLTokenizer.cpp \
	src/html/HTMLTokenizer.h \
	src/css/Bmp.cpp \
	src/css/Bmp.h \
	src/css/Box.cpp \
	src/css/Box.h \
	src/css/BoxGL.cpp \
	src/css/BoxImage.cpp \
	src/css/Ico.cpp \
	src/css/Ico.h \
	src/css/FormattingContext.cpp \
	src/css/LineBox.cpp \
	src/css/StackingContext.cpp \
	src/css/StackingContext.h \
	src/css/CSSColor.re \
	src/css/CSSPropertyNames.re \
	src/css/CSSPropertyValueImp.cpp \
	src/css/CSSPropertyValueImp.h \
	src/css/CSSSerialize.cpp \
	src/css/CSSSerialize.h \
	src/css/CSSGrammar.yy \
	src/css/CSSSelector.cpp \
	src/css/CSSSelector.h \
	src/css/CSSTokenizer.h \
	src/css/CSSTokenizer.re \
	src/css/CSSParser.cpp \
	src/css/CSSParser.h \
	src/css/CSSValueParser.cpp \
	src/css/CSSValueParser.h \
	src/css/CSSInputStream.cpp \
	src/css/CSSInputStream.h \
	src/css/Replaced.cpp \
	src/css/Table.cpp \
	src/css/Table.h \
	src/css/TableGL.cpp \
	src/css/ViewCSSImp.cpp \
	src/css/ViewCSSImp.h \
	src/css/ViewCSSImpGL.cpp
	
# third party
libeshtml5_a_SOURCES += \
	src/picojson.h

# implementation files originally generated by esidl
libeshtml5_a_SOURCES += \
	src/ApplicationCacheImp.cpp \
	src/ApplicationCacheImp.h \
	src/AttrImp.cpp \
	src/AttrImp.h \
	src/BarPropImp.cpp \
	src/BarPropImp.h \
	src/BeforeUnloadEventImp.cpp \
	src/BeforeUnloadEventImp.h \
	src/BlobImp.cpp \
	src/BlobImp.h \
	src/CaretPositionImp.cpp \
	src/CaretPositionImp.h \
	src/CharacterDataImp.cpp \
	src/CharacterDataImp.h \
	src/ClientRectImp.cpp \
	src/ClientRectImp.h \
	src/ClientRectListImp.cpp \
	src/ClientRectListImp.h \
	src/CommentImp.cpp \
	src/CommentImp.h \
	src/CompositionEventImp.cpp \
	src/CompositionEventImp.h \
	src/CustomEventImp.cpp \
	src/CustomEventImp.h \
	src/CustomEventInitImp.cpp \
	src/CustomEventInitImp.h \
	src/DOMImplementationImp.cpp \
	src/DOMImplementationImp.h \
	src/DOMSettableTokenListImp.cpp \
	src/DOMSettableTokenListImp.h \
	src/DOMStringListImp.cpp \
	src/DOMStringListImp.h \
	src/DOMStringMapImp.cpp \
	src/DOMStringMapImp.h \
	src/DOMTokenListImp.cpp \
	src/DOMTokenListImp.h \
	src/DocumentFragmentImp.cpp \
	src/DocumentFragmentImp.h \
	src/DocumentImp.cpp \
	src/DocumentImp.h \
	src/DocumentRangeImp.cpp \
	src/DocumentRangeImp.h \
	src/DocumentTraversalImp.cpp \
	src/DocumentTraversalImp.h \
	src/DocumentTypeImp.cpp \
	src/DocumentTypeImp.h \
	src/DragEventImp.cpp \
	src/DragEventImp.h \
	src/ElementImp.cpp \
	src/ElementImp.h \
	src/EventImp.cpp \
	src/EventImp.h \
	src/EventInitImp.cpp \
	src/EventInitImp.h \
	src/EventListenerImp.cpp \
	src/EventListenerImp.h \
	src/EventSourceImp.cpp \
	src/EventSourceImp.h \
	src/EventTargetImp.cpp \
	src/EventTargetImp.h \
	src/FocusEventImp.cpp \
	src/FocusEventImp.h \
	src/FunctionImp.cpp \
	src/FunctionImp.h \
	src/HashChangeEventImp.cpp \
	src/HashChangeEventImp.h \
	src/HistoryImp.cpp \
	src/HistoryImp.h \
	src/KeyboardEventImp.cpp \
	src/KeyboardEventImp.h \
	src/LinkStyleImp.cpp \
	src/LinkStyleImp.h \
	src/LocationImp.cpp \
	src/LocationImp.h \
	src/MediaErrorImp.cpp \
	src/MediaErrorImp.h \
	src/MediaListImp.cpp \
	src/MediaListImp.h \
	src/MediaQueryListImp.cpp \
	src/MediaQueryListImp.h \
	src/MediaQueryListListenerImp.cpp \
	src/MediaQueryListListenerImp.h \
	src/MessageChannelImp.cpp \
	src/MessageChannelImp.h \
	src/MessageEventImp.cpp \
	src/MessageEventImp.h \
	src/MessagePortImp.cpp \
	src/MessagePortImp.h \
	src/MouseEventImp.cpp \
	src/MouseEventImp.h \
	src/MutationEventImp.cpp \
	src/MutationEventImp.h \
	src/MutationNameEventImp.cpp \
	src/MutationNameEventImp.h \
	src/NavigatorImp.cpp \
	src/NavigatorImp.h \
	src/NodeFilterImp.cpp \
	src/NodeFilterImp.h \
	src/NodeImp.cpp \
	src/NodeImp.h \
	src/NodeIteratorImp.cpp \
	src/NodeIteratorImp.h \
	src/NodeListImp.cpp \
	src/NodeListImp.h \
	src/ObjectArrayImp.h \
	src/PageTransitionEventImp.cpp \
	src/PageTransitionEventImp.h \
	src/PopStateEventImp.cpp \
	src/PopStateEventImp.h \
	src/ProcessingInstructionImp.cpp \
	src/ProcessingInstructionImp.h \
	src/ProgressEventImp.cpp \
	src/ProgressEventImp.h \
	src/PropertyNodeListImp.cpp \
	src/PropertyNodeListImp.h \
	src/RangeImp.cpp \
	src/RangeImp.h \
	src/ScreenImp.cpp \
	src/ScreenImp.h \
	src/StyleSheetImp.cpp \
	src/StyleSheetImp.h \
	src/TextEventImp.cpp \
	src/TextEventImp.h \
	src/TextImp.cpp \
	src/TextImp.h \
	src/TreeWalkerImp.cpp \
	src/TreeWalkerImp.h \
	src/UIEventImp.cpp \
	src/UIEventImp.h \
	src/UndoManagerEventImp.cpp \
	src/UndoManagerEventImp.h \
	src/UndoManagerImp.cpp \
	src/UndoManagerImp.h \
	src/WheelEventImp.cpp \
	src/WheelEventImp.h \
	src/WindowImp.cpp \
	src/WindowImp.h \
	src/css/CSS2PropertiesImp.cpp \
	src/css/CSS2PropertiesImp.h \
	src/css/CSSCharsetRuleImp.cpp \
	src/css/CSSCharsetRuleImp.h \
	src/css/CSSColorComponentValueImp.cpp \
	src/css/CSSColorComponentValueImp.h \
	src/css/CSSComponentValueImp.cpp \
	src/css/CSSComponentValueImp.h \
	src/css/CSSFontFaceRuleImp.cpp \
	src/css/CSSFontFaceRuleImp.h \
	src/css/CSSIdentifierComponentValueImp.cpp \
	src/css/CSSIdentifierComponentValueImp.h \
	src/css/CSSImportRuleImp.cpp \
	src/css/CSSImportRuleImp.h \
	src/css/CSSKeywordComponentValueImp.cpp \
	src/css/CSSKeywordComponentValueImp.h \
	src/css/CSSLengthComponentValueImp.cpp \
	src/css/CSSLengthComponentValueImp.h \
	src/css/CSSMapValueImp.cpp \
	src/css/CSSMapValueImp.h \
	src/css/CSSMediaRuleImp.cpp \
	src/css/CSSMediaRuleImp.h \
	src/css/CSSNamespaceRuleImp.cpp \
	src/css/CSSNamespaceRuleImp.h \
	src/css/CSSPageRuleImp.cpp \
	src/css/CSSPageRuleImp.h \
	src/css/CSSPercentageComponentValueImp.cpp \
	src/css/CSSPercentageComponentValueImp.h \
	src/css/CSSPrimitiveValueImp.cpp \
	src/css/CSSPrimitiveValueImp.h \
	src/css/CSSPropertyValueListImp.cpp \
	src/css/CSSPropertyValueListImp.h \
	src/css/CSSRuleImp.cpp \
	src/css/CSSRuleImp.h \
	src/css/CSSRuleListImp.cpp \
	src/css/CSSRuleListImp.h \
	src/css/CSSStringComponentValueImp.cpp \
	src/css/CSSStringComponentValueImp.h \
	src/css/CSSStyleDeclarationImp.cpp \
	src/css/CSSStyleDeclarationImp.h \
	src/css/CSSStyleDeclarationValueImp.cpp \
	src/css/CSSStyleDeclarationValueImp.h \
	src/css/CSSStyleRuleImp.cpp \
	src/css/CSSStyleRuleImp.h \
	src/css/CSSStyleSheetImp.cpp \
	src/css/CSSStyleSheetImp.h \
	src/css/CSSURLComponentValueImp.cpp \
	src/css/CSSURLComponentValueImp.h \
	src/css/CSSUnknownRuleImp.cpp \
	src/css/CSSUnknownRuleImp.h \
	src/css/CSSValueImp.cpp \
	src/css/CSSValueImp.h \
	src/css/CSSValueListImp.cpp \
	src/css/CSSValueListImp.h \
	src/css/CounterImp.cpp \
	src/css/CounterImp.h \
	src/css/DOMImplementationCSSImp.cpp \
	src/css/DOMImplementationCSSImp.h \
	src/css/DocumentCSSImp.cpp \
	src/css/DocumentCSSImp.h \
	src/css/ElementCSSInlineStyleImp.cpp \
	src/css/ElementCSSInlineStyleImp.h \
	src/css/RGBColorImp.cpp \
	src/css/RGBColorImp.h \
	src/css/RectImp.cpp \
	src/css/RectImp.h \
	src/css/ViewCSSImp.cpp \
	src/css/ViewCSSImp.h \
	src/file/FileErrorImp.cpp \
	src/file/FileErrorImp.h \
	src/file/FileImp.cpp \
	src/file/FileImp.h \
	src/file/FileListImp.cpp \
	src/file/FileListImp.h \
	src/file/FileReaderImp.cpp \
	src/file/FileReaderImp.h \
	src/file/FileReaderSyncImp.cpp \
	src/file/FileReaderSyncImp.h \
	src/html/CanvasGradientImp.cpp \
	src/html/CanvasGradientImp.h \
	src/html/CanvasPatternImp.cpp \
	src/html/CanvasPatternImp.h \
	src/html/CanvasPixelArrayImp.cpp \
	src/html/CanvasPixelArrayImp.h \
	src/html/CanvasRenderingContext2DImp.cpp \
	src/html/CanvasRenderingContext2DImp.h \
	src/html/DataTransferImp.cpp \
	src/html/DataTransferImp.h \
	src/html/DataTransferItemImp.cpp \
	src/html/DataTransferItemImp.h \
	src/html/DataTransferItemListImp.cpp \
	src/html/DataTransferItemListImp.h \
	src/html/HTMLAllCollectionImp.cpp \
	src/html/HTMLAllCollectionImp.h \
	src/html/HTMLAnchorElementImp.cpp \
	src/html/HTMLAnchorElementImp.h \
	src/html/HTMLAppletElementImp.cpp \
	src/html/HTMLAppletElementImp.h \
	src/html/HTMLAreaElementImp.cpp \
	src/html/HTMLAreaElementImp.h \
	src/html/HTMLAudioElementImp.cpp \
	src/html/HTMLAudioElementImp.h \
	src/html/HTMLBindingElementImp.cpp \
	src/html/HTMLBindingElementImp.h \
	src/html/HTMLBRElementImp.cpp \
	src/html/HTMLBRElementImp.h \
	src/html/HTMLBaseElementImp.cpp \
	src/html/HTMLBaseElementImp.h \
	src/html/HTMLBaseFontElementImp.cpp \
	src/html/HTMLBaseFontElementImp.h \
	src/html/HTMLBodyElementImp.cpp \
	src/html/HTMLBodyElementImp.h \
	src/html/HTMLButtonElementImp.cpp \
	src/html/HTMLButtonElementImp.h \
	src/html/HTMLCanvasElementImp.cpp \
	src/html/HTMLCanvasElementImp.h \
	src/html/HTMLCollectionImp.cpp \
	src/html/HTMLCollectionImp.h \
	src/html/HTMLCommandElementImp.cpp \
	src/html/HTMLCommandElementImp.h \
	src/html/HTMLDListElementImp.cpp \
	src/html/HTMLDListElementImp.h \
	src/html/HTMLDataListElementImp.cpp \
	src/html/HTMLDataListElementImp.h \
	src/html/HTMLDetailsElementImp.cpp \
	src/html/HTMLDetailsElementImp.h \
	src/html/HTMLDirectoryElementImp.cpp \
	src/html/HTMLDirectoryElementImp.h \
	src/html/HTMLDivElementImp.cpp \
	src/html/HTMLDivElementImp.h \
	src/html/HTMLDocumentImp.cpp \
	src/html/HTMLDocumentImp.h \
	src/html/HTMLElementImp.cpp \
	src/html/HTMLElementImp.h \
	src/html/HTMLEmbedElementImp.cpp \
	src/html/HTMLEmbedElementImp.h \
	src/html/HTMLFieldSetElementImp.cpp \
	src/html/HTMLFieldSetElementImp.h \
	src/html/HTMLFontElementImp.cpp \
	src/html/HTMLFontElementImp.h \
	src/html/HTMLFormControlsCollectionImp.cpp \
	src/html/HTMLFormControlsCollectionImp.h \
	src/html/HTMLFormElementImp.cpp \
	src/html/HTMLFormElementImp.h \
	src/html/HTMLFrameElementImp.cpp \
	src/html/HTMLFrameElementImp.h \
	src/html/HTMLFrameSetElementImp.cpp \
	src/html/HTMLFrameSetElementImp.h \
	src/html/HTMLHRElementImp.cpp \
	src/html/HTMLHRElementImp.h \
	src/html/HTMLHeadElementImp.cpp \
	src/html/HTMLHeadElementImp.h \
	src/html/HTMLHeadingElementImp.cpp \
	src/html/HTMLHeadingElementImp.h \
	src/html/HTMLHtmlElementImp.cpp \
	src/html/HTMLHtmlElementImp.h \
	src/html/HTMLIFrameElementImp.cpp \
	src/html/HTMLIFrameElementImp.h \
	src/html/HTMLImageElementImp.cpp \
	src/html/HTMLImageElementImp.h \
	src/html/HTMLInputElementImp.cpp \
	src/html/HTMLInputElementImp.h \
	src/html/HTMLKeygenElementImp.cpp \
	src/html/HTMLKeygenElementImp.h \
	src/html/HTMLLIElementImp.cpp \
	src/html/HTMLLIElementImp.h \
	src/html/HTMLLabelElementImp.cpp \
	src/html/HTMLLabelElementImp.h \
	src/html/HTMLLegendElementImp.cpp \
	src/html/HTMLLegendElementImp.h \
	src/html/HTMLLinkElementImp.cpp \
	src/html/HTMLLinkElementImp.h \
	src/html/HTMLMapElementImp.cpp \
	src/html/HTMLMapElementImp.h \
	src/html/HTMLMarqueeElementImp.cpp \
	src/html/HTMLMarqueeElementImp.h \
	src/html/HTMLMediaElementImp.cpp \
	src/html/HTMLMediaElementImp.h \
	src/html/HTMLMenuElementImp.cpp \
	src/html/HTMLMenuElementImp.h \
	src/html/HTMLMetaElementImp.cpp \
	src/html/HTMLMetaElementImp.h \
	src/html/HTMLMeterElementImp.cpp \
	src/html/HTMLMeterElementImp.h \
	src/html/HTMLModElementImp.cpp \
	src/html/HTMLModElementImp.h \
	src/html/HTMLOListElementImp.cpp \
	src/html/HTMLOListElementImp.h \
	src/html/HTMLObjectElementImp.cpp \
	src/html/HTMLObjectElementImp.h \
	src/html/HTMLOptGroupElementImp.cpp \
	src/html/HTMLOptGroupElementImp.h \
	src/html/HTMLOptionElementImp.cpp \
	src/html/HTMLOptionElementImp.h \
	src/html/HTMLOptionsCollectionImp.cpp \
	src/html/HTMLOptionsCollectionImp.h \
	src/html/HTMLOutputElementImp.cpp \
	src/html/HTMLOutputElementImp.h \
	src/html/HTMLParagraphElementImp.cpp \
	src/html/HTMLParagraphElementImp.h \
	src/html/HTMLParamElementImp.cpp \
	src/html/HTMLParamElementImp.h \
	src/html/HTMLPreElementImp.cpp \
	src/html/HTMLPreElementImp.h \
	src/html/HTMLProgressElementImp.cpp \
	src/html/HTMLProgressElementImp.h \
	src/html/HTMLPropertiesCollectionImp.cpp \
	src/html/HTMLPropertiesCollectionImp.h \
	src/html/HTMLQuoteElementImp.cpp \
	src/html/HTMLQuoteElementImp.h \
	src/html/HTMLReplacedElementImp.h \
	src/html/HTMLScriptElementImp.cpp \
	src/html/HTMLScriptElementImp.h \
	src/html/HTMLSelectElementImp.cpp \
	src/html/HTMLSelectElementImp.h \
	src/html/HTMLSourceElementImp.cpp \
	src/html/HTMLSourceElementImp.h \
	src/html/HTMLSpanElementImp.cpp \
	src/html/HTMLSpanElementImp.h \
	src/html/HTMLStyleElementImp.cpp \
	src/html/HTMLStyleElementImp.h \
	src/html/HTMLTableCaptionElementImp.cpp \
	src/html/HTMLTableCaptionElementImp.h \
	src/html/HTMLTableCellElementImp.cpp \
	src/html/HTMLTableCellElementImp.h \
	src/html/HTMLTableColElementImp.cpp \
	src/html/HTMLTableColElementImp.h \
	src/html/HTMLTableDataCellElementImp.cpp \
	src/html/HTMLTableDataCellElementImp.h \
	src/html/HTMLTableElementImp.cpp \
	src/html/HTMLTableElementImp.h \
	src/html/HTMLTableHeaderCellElementImp.cpp \
	src/html/HTMLTableHeaderCellElementImp.h \
	src/html/HTMLTableRowElementImp.cpp \
	src/html/HTMLTableRowElementImp.h \
	src/html/HTMLTableSectionElementImp.cpp \
	src/html/HTMLTableSectionElementImp.h \
	src/html/HTMLTemplateElementImp.cpp \
	src/html/HTMLTemplateElementImp.h \
	src/html/HTMLTextAreaElementImp.cpp \
	src/html/HTMLTextAreaElementImp.h \
	src/html/HTMLTimeElementImp.cpp \
	src/html/HTMLTimeElementImp.h \
	src/html/HTMLTitleElementImp.cpp \
	src/html/HTMLTitleElementImp.h \
	src/html/HTMLUListElementImp.cpp \
	src/html/HTMLUListElementImp.h \
	src/html/HTMLUnknownElementImp.cpp \
	src/html/HTMLUnknownElementImp.h \
	src/html/HTMLVideoElementImp.cpp \
	src/html/HTMLVideoElementImp.h \
	src/html/ImageDataImp.cpp \
	src/html/ImageDataImp.h \
	src/html/RadioNodeListImp.cpp \
	src/html/RadioNodeListImp.h \
	src/html/TextMetricsImp.cpp \
	src/html/TextMetricsImp.h \
	src/html/TimeRangesImp.cpp \
	src/html/TimeRangesImp.h \
	src/html/ValidityStateImp.cpp \
	src/html/ValidityStateImp.h \
	src/typedarray/ArrayBufferImp.cpp \
	src/typedarray/ArrayBufferImp.h \
	src/typedarray/ArrayBufferViewImp.cpp \
	src/typedarray/ArrayBufferViewImp.h \
	src/typedarray/DataViewImp.cpp \
	src/typedarray/DataViewImp.h \
	src/typedarray/Float32ArrayImp.cpp \
	src/typedarray/Float32ArrayImp.h \
	src/typedarray/Float64ArrayImp.cpp \
	src/typedarray/Float64ArrayImp.h \
	src/typedarray/Int16ArrayImp.cpp \
	src/typedarray/Int16ArrayImp.h \
	src/typedarray/Int32ArrayImp.cpp \
	src/typedarray/Int32ArrayImp.h \
	src/typedarray/Int8ArrayImp.cpp \
	src/typedarray/Int8ArrayImp.h \
	src/typedarray/Uint16ArrayImp.cpp \
	src/typedarray/Uint16ArrayImp.h \
	src/typedarray/Uint32ArrayImp.cpp \
	src/typedarray/Uint32ArrayImp.h \
	src/typedarray/Uint8ArrayImp.cpp \
	src/typedarray/Uint8ArrayImp.h \
	src/xbl/XBLContentElementImp.cpp \
	src/xbl/XBLContentElementImp.h \
	src/xbl/XBLImplementationImp.cpp \
	src/xbl/XBLImplementationImp.h \
	src/xbl/XBLImplementationListImp.cpp \
	src/xbl/XBLImplementationListImp.h \
	src/xhr/AnonXMLHttpRequestImp.cpp \
	src/xhr/AnonXMLHttpRequestImp.h \
	src/xhr/FormDataImp.cpp \
	src/xhr/FormDataImp.h \
	src/xhr/XMLHttpRequestEventTargetImp.cpp \
	src/xhr/XMLHttpRequestEventTargetImp.h \
	src/xhr/XMLHttpRequestImp.cpp \
	src/xhr/XMLHttpRequestImp.h \
	src/xhr/XMLHttpRequestUploadImp.cpp \
	src/xhr/XMLHttpRequestUploadImp.h

noinst_PROGRAMS = \
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
	Ico.test \
	Script.test \
	ScriptV8.test \
	Navigator.test \
	NavigatorV8.test \
	Profile.test

noinst_PROGRAMS : $(LDADD) libesjsapi.a libesv8api.a

js_LDADD = $(LDADD) libesjsapi.a
v8_LDADD = $(LDADD) libesv8api.a -lv8

harness_SOURCES = src/harness.cpp
harness_LDADD = $(js_LDADD)

Any_test_SOURCES = src/Any.test.cpp
Any_test_LDADD = $(js_LDADD)

HTMLInputStream_test_SOURCES = src/HTMLInputStream.test.cpp
HTMLInputStream_test_LDADD = $(js_LDADD)

HTMLInputStream_test_getChar_SOURCES = src/HTMLInputStream.test.getChar.cpp
HTMLInputStream_test_getChar_LDADD = $(js_LDADD)

HTMLTokenizer_test_SOURCES = src/HTMLTokenizer.test.cpp
HTMLTokenizer_test_LDADD = $(js_LDADD)

HTMLParser_test_SOURCES = src/HTMLParser.test.cpp
HTMLParser_test_LDADD = $(js_LDADD)

CSSTokenizer_test_SOURCES = src/CSSTokenizer.test.cpp
CSSTokenizer_test_LDADD = $(js_LDADD)

CSSParser_test_SOURCES = src/CSSParser.test.cpp
CSSParser_test_LDADD = $(js_LDADD)

CSSStyle_test_SOURCES = src/CSSStyle.test.cpp
CSSStyle_test_LDADD = $(js_LDADD)

Box_test_SOURCES = src/Box.test.cpp
Box_test_LDADD = $(js_LDADD)

Ico_test_SOURCES = src/Ico.test.cpp
Ico_test_LDADD = $(js_LDADD)

FontManager_test_SOURCES = src/FontManager.test.cpp
FontManager_test_LDADD = $(js_LDADD)

URL_test_SOURCES = src/URL.test.cpp
URL_test_LDADD = $(js_LDADD)

HTTPHeader_test_SOURCES = src/HTTPHeader.test.cpp
HTTPHeader_test_LDADD = $(js_LDADD)

HTTPRequest_test_SOURCES = src/HTTPRequest.test.cpp
HTTPRequest_test_LDADD = $(js_LDADD)

Script_test_SOURCES = src/Script.test.cpp
Script_test_LDADD = $(js_LDADD)
Script_test_CXXFLAGS = $(AM_CFLAGS) -DUSE_JS

ScriptV8_test_SOURCES = src/Script.test.cpp
ScriptV8_test_LDADD = $(v8_LDADD)
ScriptV8_test_CXXFLAGS = $(AM_CFLAGS) -DUSE_V8

Navigator_test_SOURCES = src/Navigator.test.cpp
Navigator_test_LDADD = $(js_LDADD)
Navigator_test_CXXFLAGS = $(AM_CFLAGS) -DUSE_JS

NavigatorV8_test_SOURCES = src/Navigator.test.cpp
NavigatorV8_test_LDADD = $(v8_LDADD)
NavigatorV8_test_CXXFLAGS = $(AM_CFLAGS) -DUSE_V8

Profile_test_SOURCES = src/Profile.test.cpp

dist_bin_SCRIPTS = app/escort

esrillelibexecdir = $(libexecdir)/esrille
escortlibexecdir = $(esrillelibexecdir)/escort

escortlibexec_PROGRAMS = escort
escortlibexec_PROGRAMS : $(LDADD) libesjsapi.a libesv8api.a

escort_SOURCES = src/Escort.cpp
escort_LDADD = $(js_LDADD)
escort_CXXFLAGS = $(AM_CFLAGS) -DUSE_JS

desktopdir = $(datadir)/applications
dist_desktop_DATA = data/escort/escort.desktop

esrilledatadir = $(datadir)/esrille
escortdatadir = $(esrilledatadir)/escort
aboutdatadir = $(escortdatadir)/about

dist_escortdata_DATA =  \
	data/escort/default.css \
	data/escort/escort.ico \
	data/escort/escort.html \
	data/escort/Cancel_32x32.png \
	data/escort/Favorites_32x32.png \
	data/escort/Gear_32x32.gif \
	data/escort/Gear_32x32.png \
	data/escort/Next_32x32.png \
	data/escort/Previous_32x32.png \
	data/escort/Refresh_32x32.png \
	data/escort/Stop_32x32.png

dist_aboutdata_DATA =  \
	data/escort/about/blank \
	data/escort/about/index.html \
	data/escort/about/escort_24.png \
	data/escort/about/escort_128.png

clean-local:
	rm -rf org css html $(generated_sources) $(generated_headers) $(generated_scanners) *.output

dist-hook:
	-rm -f $(distdir)/CSSGrammar.cc $(distdir)/CSSGrammar.h

EXTRA_DIST = \
	escort.spec.in \
	escort.spec \
	data/escort/about/index.html.in \
	LICENSE \
	NOTICE \
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
