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

#include "Table.h"

#include <GL/gl.h>
#include <GL/glu.h>

#include "ViewCSSImp.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

namespace {

enum
{
    TOP,
    RIGHT,
    BOTTOM,
    LEFT
};

}

void TableWrapperBox::renderBackground(ViewCSSImp* view, CSSStyleDeclarationImp* style, float x, float y, float left, float top, float right, float bottom, float width, float height, unsigned backgroundColor, BoxImage* backgroundImage)
{
    glDisable(GL_TEXTURE_2D);

    if (backgroundColor) {
        glColor4ub(backgroundColor >> 16, backgroundColor >> 8, backgroundColor, backgroundColor >> 24);
        glBegin(GL_QUADS);
            glVertex2f(left, top);
            glVertex2f(right, top);
            glVertex2f(right, bottom);
            glVertex2f(left, bottom);
        glEnd();
    }

    if (backgroundImage && backgroundImage->getState() == BoxImage::CompletelyAvailable) {
        // TODO: Check style->backgroundAttachment.isFixed()
        style->backgroundPosition.resolve(view, backgroundImage, style, width, height);
        backgroundLeft = style->backgroundPosition.getLeftPx();
        backgroundTop = style->backgroundPosition.getTopPx();
        GLfloat border[] = { ((backgroundColor >> 16) & 0xff) / 255.0f,
                             ((backgroundColor >> 8) & 0xff) / 255.0f,
                             ((backgroundColor) & 0xff) / 255.0f,
                             ((backgroundColor >> 24) & 0xff) / 255.0f };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border);
        glPushMatrix();
            glTranslatef(x, y, 0.0f);
            backgroundImage->render(view, left - x, top - y, right - left, bottom - top, backgroundLeft, backgroundTop);
        glPopMatrix();
    }

    glEnable(GL_TEXTURE_2D);
}

void TableWrapperBox::renderLayers(ViewCSSImp* view)
{
    float h;
    float w;
    float left;
    float top;
    float right;
    float bottom;

    // row groups
    h = tableBox->getY() + tableBox->getMarginTop();
    for (unsigned y = 0; y < yHeight;) {
        CSSStyleDeclarationImp* rowGroupStyle = rowGroups[y].get();
        if (rowGroupStyle && (rowGroupImages[y] || rowGroupStyle->backgroundColor.getARGB())) {
            BoxImage* image = rowGroupImages[y];
            w = tableBox->getX() + tableBox->getMarginLeft();
            float w0 = w;
            float h0 = h;
            float wN = w0 + tableBox->width;
            float hN = h0 + heights[y];
            size_t elements = 1;
            while (rowGroupStyle == rowGroups[y + elements].get()) {
                hN += heights[y + elements];
                ++elements;
            }
            if (CellBox* cellBox = grid[y][0].get()) {
                w0 = cellBox->x + cellBox->getMarginLeft();
                if (!cellBox->isTopSpanned(y)) // TODO: else look up for a single row spanning cell.
                    h0 = cellBox->y + cellBox->getMarginTop();
            }
            if (CellBox* cellBox = grid[y + elements - 1][xWidth - 1].get()) {
                wN = cellBox->x + cellBox->getTotalWidth() - cellBox->getMarginRight();
                if (!cellBox->isBottomSpanned(y + elements)) // TODO: else look up for a single row spanning cell.
                    hN = cellBox->y + cellBox->getTotalHeight() - cellBox->getMarginBottom();
            }
            if (style->borderCollapse.getValue() == style->borderCollapse.Collapse) {
                renderBackground(view, rowGroupStyle, w0, h0, w0, h0, wN, hN, wN - w0, hN - h0, rowGroupStyle->backgroundColor.getARGB(), image);
                for (unsigned end = y + elements; y < end; h += heights[y], ++y)
                    ;
            } else {
                for (unsigned end = y + elements; y < end; h += heights[y], ++y) {
                    for (unsigned x = 0; x < xWidth; w += widths[x], ++x) {
                        CellBox* cellBox = grid[y][x].get();
                        if (!cellBox || cellBox->isEmptyCell())
                            continue;
                        if (!cellBox->isSpanned(x, y)) {
                            left = cellBox->x;
                            top = cellBox->y;
                            right = left + cellBox->getTotalWidth();
                            bottom = top + cellBox->getTotalHeight();
                            left += cellBox->getMarginLeft();
                            right -= cellBox->getMarginRight();
                            top += cellBox->getMarginTop();
                            bottom -= cellBox->getMarginBottom();
                        } else {
                            left = cellBox->isLeftSpanned(x) ? w : (cellBox->x + cellBox->getMarginLeft());
                            top = cellBox->isTopSpanned(y) ? h : (cellBox->y + cellBox->getMarginTop());
                            right = cellBox->isRightSpanned(x + 1) ? w + widths[x] : (cellBox->x + cellBox->getTotalWidth() - cellBox->getMarginRight());
                            bottom = cellBox->isBottomSpanned(y + 1) ? h + heights[y] : (cellBox->y + cellBox->getTotalHeight() - cellBox->getMarginBottom());
                        }
                        renderBackground(view, rowGroupStyle, w0, h0, left, top, right, bottom, wN - w0, hN - h0, rowGroupStyle->backgroundColor.getARGB(), image);
                    }
                }
            }
        } else {
            h += heights[y];
            ++y;
        }
    }

    // rows
    h = tableBox->getY() + tableBox->getMarginTop();
    for (unsigned y = 0; y < yHeight; h += heights[y], ++y) {
        CSSStyleDeclarationImp* rowStyle = rows[y].get();
        if (rowStyle && (rowImages[y] || rowStyle->backgroundColor.getARGB())) {
            w = tableBox->getX() + tableBox->getMarginLeft();
            float w0 = w;
            float h0 = h;
            float wN = w0 + tableBox->width;
            float hN = h0 + heights[y];
            if (CellBox* cellBox = grid[y][0].get()) {
                w0 = cellBox->x + cellBox->getMarginLeft();
                if (!cellBox->isTopSpanned(y)) // TODO: else look up for a single row spanning cell.
                    h0 = cellBox->y + cellBox->getMarginTop();
            }
            if (CellBox* cellBox = grid[y][xWidth - 1].get()) {
                wN = cellBox->x + cellBox->getTotalWidth() - cellBox->getMarginRight();
                if (!cellBox->isBottomSpanned(y + 1)) // TODO: else look up for a single row spanning cell.
                    hN = cellBox->y + cellBox->getTotalHeight() - cellBox->getMarginBottom();
            }
            if (style->borderCollapse.getValue() == style->borderCollapse.Collapse)
                renderBackground(view, rowStyle, w0, h0, w0, h0, wN, hN, wN - w0, hN - h0, rowStyle->backgroundColor.getARGB(), rowImages[y]);
            else {
                for (unsigned x = 0; x < xWidth; w += widths[x], ++x) {
                    CellBox* cellBox = grid[y][x].get();
                    if (!cellBox || cellBox->isEmptyCell())
                        continue;
                    if (!cellBox->isSpanned(x, y)) {
                        left = cellBox->x;
                        top = cellBox->y;
                        right = left + cellBox->getTotalWidth();
                        bottom = top + cellBox->getTotalHeight();
                        left += cellBox->getMarginLeft();
                        right -= cellBox->getMarginRight();
                        top += cellBox->getMarginTop();
                        bottom -= cellBox->getMarginBottom();
                    } else {
                        left = cellBox->isLeftSpanned(x) ? w : (cellBox->x + cellBox->getMarginLeft());
                        top = cellBox->isTopSpanned(y) ? h : (cellBox->y + cellBox->getMarginTop());
                        right = cellBox->isRightSpanned(x + 1) ? w + widths[x] : (cellBox->x + cellBox->getTotalWidth() - cellBox->getMarginRight());
                        bottom = cellBox->isBottomSpanned(y + 1) ? h + heights[y] : (cellBox->y + cellBox->getTotalHeight() - cellBox->getMarginBottom());
                    }
                    renderBackground(view, rowStyle, w0, h0, left, top, right, bottom, wN - w0, hN - h0, rowStyle->backgroundColor.getARGB(), rowImages[y]);
                }
            }
        }
    }
}

void TableWrapperBox::renderTableBorders(ViewCSSImp* view)
{
    renderLayers(view);

    if (borderRows.empty() || borderColumns.empty())
        return;

    glDisable(GL_TEXTURE_2D);
    float h = tableBox->getY() + tableBox->getMarginTop();
    for (unsigned y = 0; y < yHeight + 1; h += heights[y], ++y) {
        float w = tableBox->getX() + tableBox->getMarginLeft();
        for (unsigned x = 0; x < xWidth + 1; w += widths[x], ++x) {
            if (x < xWidth) {
                BorderValue* br = getRowBorderValue(x, y);
                if (0.0f < br->getWidth()) {
                    float m = br->getWidth() / 2.0f;
                    float l = w - m;
                    float r = w + widths[x] + m;
                    float t = h - m;
                    float b = h + m;
                    renderBorderEdge(view, TOP,
                                     br->style.getValue(),
                                     br->color.getARGB(),
                                     l, t, r, t, r, b, l, b);
                }
            }
            if (y < yHeight) {
                BorderValue* bc = getColumnBorderValue(x, y);
                if (0.0f < bc->getWidth()) {
                    float m = bc->getWidth() / 2.0f;
                    float l = w - m;
                    float r = w + m;
                    float t = h - m;
                    float b = h + heights[y] + m;
                    renderBorderEdge(view, LEFT,
                                     bc->style.getValue(),
                                     bc->color.getARGB(),
                                     l, b, l, t, r, t, r, b);
                }
            }
        }
    }
    glEnable(GL_TEXTURE_2D);
}

}}}}  // org::w3c::dom::bootstrap
