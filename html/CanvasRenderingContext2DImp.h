// Generated by esidl (r1745).
// This file is expected to be modified for the Web IDL interface
// implementation.  Permission to use, copy, modify and distribute
// this file in any software license is hereby granted.

#ifndef ORG_W3C_DOM_BOOTSTRAP_CANVASRENDERINGCONTEXT2DIMP_H_INCLUDED
#define ORG_W3C_DOM_BOOTSTRAP_CANVASRENDERINGCONTEXT2DIMP_H_INCLUDED

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <org/w3c/dom/html/CanvasRenderingContext2D.h>

#include <org/w3c/dom/html/CanvasGradient.h>
#include <org/w3c/dom/html/CanvasPattern.h>
#include <org/w3c/dom/html/TextMetrics.h>
#include <org/w3c/dom/html/ImageData.h>
#include <org/w3c/dom/Element.h>
#include <org/w3c/dom/html/HTMLImageElement.h>
#include <org/w3c/dom/html/HTMLVideoElement.h>
#include <org/w3c/dom/html/HTMLCanvasElement.h>

namespace org
{
namespace w3c
{
namespace dom
{
namespace bootstrap
{
class CanvasRenderingContext2DImp : public ObjectMixin<CanvasRenderingContext2DImp>
{
public:
    // CanvasRenderingContext2D
    html::HTMLCanvasElement getCanvas() __attribute__((weak));
    void save() __attribute__((weak));
    void restore() __attribute__((weak));
    void scale(float x, float y) __attribute__((weak));
    void rotate(float angle) __attribute__((weak));
    void translate(float x, float y) __attribute__((weak));
    void transform(float m11, float m12, float m21, float m22, float dx, float dy) __attribute__((weak));
    void setTransform(float m11, float m12, float m21, float m22, float dx, float dy) __attribute__((weak));
    float getGlobalAlpha() __attribute__((weak));
    void setGlobalAlpha(float globalAlpha) __attribute__((weak));
    std::u16string getGlobalCompositeOperation() __attribute__((weak));
    void setGlobalCompositeOperation(std::u16string globalCompositeOperation) __attribute__((weak));
    Any getStrokeStyle() __attribute__((weak));
    void setStrokeStyle(Any strokeStyle) __attribute__((weak));
    Any getFillStyle() __attribute__((weak));
    void setFillStyle(Any fillStyle) __attribute__((weak));
    html::CanvasGradient createLinearGradient(float x0, float y0, float x1, float y1) __attribute__((weak));
    html::CanvasGradient createRadialGradient(float x0, float y0, float r0, float x1, float y1, float r1) __attribute__((weak));
    html::CanvasPattern createPattern(html::HTMLImageElement image, std::u16string repetition) __attribute__((weak));
    html::CanvasPattern createPattern(html::HTMLCanvasElement image, std::u16string repetition) __attribute__((weak));
    html::CanvasPattern createPattern(html::HTMLVideoElement image, std::u16string repetition) __attribute__((weak));
    float getLineWidth() __attribute__((weak));
    void setLineWidth(float lineWidth) __attribute__((weak));
    std::u16string getLineCap() __attribute__((weak));
    void setLineCap(std::u16string lineCap) __attribute__((weak));
    std::u16string getLineJoin() __attribute__((weak));
    void setLineJoin(std::u16string lineJoin) __attribute__((weak));
    float getMiterLimit() __attribute__((weak));
    void setMiterLimit(float miterLimit) __attribute__((weak));
    float getShadowOffsetX() __attribute__((weak));
    void setShadowOffsetX(float shadowOffsetX) __attribute__((weak));
    float getShadowOffsetY() __attribute__((weak));
    void setShadowOffsetY(float shadowOffsetY) __attribute__((weak));
    float getShadowBlur() __attribute__((weak));
    void setShadowBlur(float shadowBlur) __attribute__((weak));
    std::u16string getShadowColor() __attribute__((weak));
    void setShadowColor(std::u16string shadowColor) __attribute__((weak));
    void clearRect(float x, float y, float w, float h) __attribute__((weak));
    void fillRect(float x, float y, float w, float h) __attribute__((weak));
    void strokeRect(float x, float y, float w, float h) __attribute__((weak));
    void beginPath() __attribute__((weak));
    void closePath() __attribute__((weak));
    void moveTo(float x, float y) __attribute__((weak));
    void lineTo(float x, float y) __attribute__((weak));
    void quadraticCurveTo(float cpx, float cpy, float x, float y) __attribute__((weak));
    void bezierCurveTo(float cp1x, float cp1y, float cp2x, float cp2y, float x, float y) __attribute__((weak));
    void arcTo(float x1, float y1, float x2, float y2, float radius) __attribute__((weak));
    void rect(float x, float y, float w, float h) __attribute__((weak));
    void arc(float x, float y, float radius, float startAngle, float endAngle, bool anticlockwise) __attribute__((weak));
    void fill() __attribute__((weak));
    void stroke() __attribute__((weak));
    void clip() __attribute__((weak));
    bool isPointInPath(float x, float y) __attribute__((weak));
    bool drawFocusRing(Element element, float xCaret, float yCaret) __attribute__((weak));
    bool drawFocusRing(Element element, float xCaret, float yCaret, bool canDrawCustom) __attribute__((weak));
    std::u16string getFont() __attribute__((weak));
    void setFont(std::u16string font) __attribute__((weak));
    std::u16string getTextAlign() __attribute__((weak));
    void setTextAlign(std::u16string textAlign) __attribute__((weak));
    std::u16string getTextBaseline() __attribute__((weak));
    void setTextBaseline(std::u16string textBaseline) __attribute__((weak));
    void fillText(std::u16string text, float x, float y) __attribute__((weak));
    void fillText(std::u16string text, float x, float y, float maxWidth) __attribute__((weak));
    void strokeText(std::u16string text, float x, float y) __attribute__((weak));
    void strokeText(std::u16string text, float x, float y, float maxWidth) __attribute__((weak));
    html::TextMetrics measureText(std::u16string text) __attribute__((weak));
    void drawImage(html::HTMLImageElement image, float dx, float dy) __attribute__((weak));
    void drawImage(html::HTMLImageElement image, float dx, float dy, float dw, float dh) __attribute__((weak));
    void drawImage(html::HTMLImageElement image, float sx, float sy, float sw, float sh, float dx, float dy, float dw, float dh) __attribute__((weak));
    void drawImage(html::HTMLCanvasElement image, float dx, float dy) __attribute__((weak));
    void drawImage(html::HTMLCanvasElement image, float dx, float dy, float dw, float dh) __attribute__((weak));
    void drawImage(html::HTMLCanvasElement image, float sx, float sy, float sw, float sh, float dx, float dy, float dw, float dh) __attribute__((weak));
    void drawImage(html::HTMLVideoElement image, float dx, float dy) __attribute__((weak));
    void drawImage(html::HTMLVideoElement image, float dx, float dy, float dw, float dh) __attribute__((weak));
    void drawImage(html::HTMLVideoElement image, float sx, float sy, float sw, float sh, float dx, float dy, float dw, float dh) __attribute__((weak));
    html::ImageData createImageData(float sw, float sh) __attribute__((weak));
    html::ImageData createImageData(html::ImageData imagedata) __attribute__((weak));
    html::ImageData getImageData(float sx, float sy, float sw, float sh) __attribute__((weak));
    void putImageData(html::ImageData imagedata, float dx, float dy) __attribute__((weak));
    void putImageData(html::ImageData imagedata, float dx, float dy, float dirtyX, float dirtyY, float dirtyWidth, float dirtyHeight) __attribute__((weak));
    // Object
    virtual Any message_(uint32_t selector, const char* id, int argc, Any* argv)
    {
        return html::CanvasRenderingContext2D::dispatch(this, selector, id, argc, argv);
    }
    static const char* const getMetaData()
    {
        return html::CanvasRenderingContext2D::getMetaData();
    }
};

}
}
}
}

#endif  // ORG_W3C_DOM_BOOTSTRAP_CANVASRENDERINGCONTEXT2DIMP_H_INCLUDED
