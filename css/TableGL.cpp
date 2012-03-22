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

    // column groups
    w = tableBox->getX() + tableBox->getMarginLeft();
    for (unsigned x = 0; x < xWidth;) {
        CSSStyleDeclarationImp* columnGroupStyle = columnGroups[x].get();
        if (columnGroupStyle && (columnGroupImages[x] || columnGroupStyle->backgroundColor.getARGB())) {
            BoxImage* image = columnGroupImages[x];
            h = tableBox->getY() + tableBox->getMarginTop();
            float w0 = w;
            float h0 = h;
            float wN = w0 + widths[x];
            float hN = h0 + tableBox->height;
            size_t elements = 1;
            while (columnGroupStyle == columnGroups[x + elements].get()) {
                wN += widths[x + elements];
                ++elements;
            }
            if (CellBox* cellBox = grid[0][x].get()) {
                h0 = cellBox->y + cellBox->getMarginTop();
                if (!cellBox->isLeftSpanned(x)) // TODO: else look up for a single column spanning cell.
                    w0 = cellBox->x + cellBox->getMarginLeft();
            }
            if (CellBox* cellBox = grid[yHeight - 1][x + elements - 1].get()) {
                hN = cellBox->y + cellBox->getTotalHeight() - cellBox->getMarginBottom();
                if (!cellBox->isRightSpanned(x + elements)) // TODO: else look up for a single column spanning cell.
                    wN = cellBox->x + cellBox->getTotalWidth() - cellBox->getMarginRight();
            }
            unsigned x0 = x;
            for (unsigned end = x + elements; x < end; w += widths[x], ++x) {
                for (unsigned y = 0; y < yHeight; h += heights[y], ++y) {
                    CellBox* cellBox = grid[y][x].get();
                    if (!cellBox || cellBox->isEmptyCell() || cellBox->isSpanned(x, y))
                        continue;
                    left = cellBox->x;
                    top = cellBox->y;
                    right = left + cellBox->getTotalWidth();
                    bottom = top + cellBox->getTotalHeight();
                    if (style->borderCollapse.getValue() == CSSBorderCollapseValueImp::Separate) {
                        left += cellBox->getMarginLeft();
                        right -= cellBox->getMarginRight();
                        top += cellBox->getMarginTop();
                        bottom -= cellBox->getMarginBottom();
                    } else {
                        if (x == x0)
                            left += cellBox->getMarginLeft();
                        if (x + 1 == end)
                            right -= cellBox->getMarginRight();
                        if (y == 0)
                            top += cellBox->getMarginTop();
                        if (y + 1 == yHeight)
                            bottom -= cellBox->getMarginBottom();
                    }
                    renderBackground(view, columnGroupStyle, w0, h0, left, top, right, bottom, wN - w0, hN - h0, columnGroupStyle->backgroundColor.getARGB(), image);
                }
            }
        } else {
            w += widths[x];
            ++x;
        }
    }

    // columns
    w = tableBox->getX() + tableBox->getMarginLeft();
    for (unsigned x = 0; x < xWidth; w += widths[x], ++x) {
        CSSStyleDeclarationImp* columnStyle = columns[x].get();
        if (columnStyle && (columnImages[x] || columnStyle->backgroundColor.getARGB())) {
            h = tableBox->getY() + tableBox->getMarginTop();
            float w0 = w;
            float h0 = h;
            float wN = w0 + widths[x];
            float hN = h0 + tableBox->height;
            if (CellBox* cellBox = grid[0][x].get()) {
                h0 = cellBox->y + cellBox->getMarginTop();
                if (!cellBox->isLeftSpanned(x)) // TODO: else look up for a single column spanning cell.
                    w0 = cellBox->x + cellBox->getMarginLeft();
            }
            if (CellBox* cellBox = grid[yHeight - 1][x].get()) {
                hN = cellBox->y + cellBox->getTotalHeight() - cellBox->getMarginBottom();
                if (!cellBox->isRightSpanned(x + 1)) // TODO: else look up for a single column spanning cell.
                    wN = cellBox->x + cellBox->getTotalWidth() - cellBox->getMarginRight();
            }
            for (unsigned y = 0; y < yHeight; h += heights[y], ++y) {
                CellBox* cellBox = grid[y][x].get();
                if (!cellBox || cellBox->isEmptyCell() || cellBox->isSpanned(x, y))
                    continue;
                left = cellBox->x;
                top = cellBox->y;
                right = left + cellBox->getTotalWidth();
                bottom = top + cellBox->getTotalHeight();
                if (style->borderCollapse.getValue() == CSSBorderCollapseValueImp::Separate) {
                    left += cellBox->getMarginLeft();
                    right -= cellBox->getMarginRight();
                    top += cellBox->getMarginTop();
                    bottom -= cellBox->getMarginBottom();
                } else {
                    left += cellBox->getMarginLeft();
                    right -= cellBox->getMarginRight();
                    if (y == 0)
                        top += cellBox->getMarginTop();
                    if (y + 1 == yHeight)
                        bottom -= cellBox->getMarginBottom();
                }
                renderBackground(view, columnStyle, w0, h0, left, top, right, bottom, wN - w0, hN - h0, columnStyle->backgroundColor.getARGB(), columnImages[x]);
            }
        }
    }

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
            unsigned y0 = y;
            for (unsigned end = y + elements; y < end; h += heights[y], ++y) {
                for (unsigned x = 0; x < xWidth; w += widths[x], ++x) {
                    CellBox* cellBox = grid[y][x].get();
                    if (!cellBox || cellBox->isEmptyCell() || cellBox->isSpanned(x, y))
                        continue;
                    left = cellBox->x;
                    top = cellBox->y;
                    right = left + cellBox->getTotalWidth();
                    bottom = top + cellBox->getTotalHeight();
                    if (style->borderCollapse.getValue() == CSSBorderCollapseValueImp::Separate) {
                        left += cellBox->getMarginLeft();
                        right -= cellBox->getMarginRight();
                        top += cellBox->getMarginTop();
                        bottom -= cellBox->getMarginBottom();
                    } else {
                        if (x == 0)
                            left += cellBox->getMarginLeft();
                        if (x + 1 == xWidth)
                            right -= cellBox->getMarginRight();
                        if (y == y0)
                            top += cellBox->getMarginTop();
                        if (y + 1 == end)
                            bottom -= cellBox->getMarginBottom();
                    }
                    renderBackground(view, rowGroupStyle, w0, h0, left, top, right, bottom, wN - w0, hN - h0, rowGroupStyle->backgroundColor.getARGB(), image);
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
            for (unsigned x = 0; x < xWidth; w += widths[x], ++x) {
                CellBox* cellBox = grid[y][x].get();
                if (!cellBox || cellBox->isEmptyCell() || cellBox->isSpanned(x, y))
                    continue;
                left = cellBox->x;
                top = cellBox->y;
                right = left + cellBox->getTotalWidth();
                bottom = top + cellBox->getTotalHeight();
                if (style->borderCollapse.getValue() == CSSBorderCollapseValueImp::Separate) {
                    left += cellBox->getMarginLeft();
                    right -= cellBox->getMarginRight();
                    top += cellBox->getMarginTop();
                    bottom -= cellBox->getMarginBottom();
                } else {
                    if (x == 0)
                        left += cellBox->getMarginLeft();
                    if (x + 1 == xWidth)
                        right -= cellBox->getMarginRight();
                    top += cellBox->getMarginTop();
                    bottom -= cellBox->getMarginBottom();
                }
                renderBackground(view, rowStyle, w0, h0, left, top, right, bottom, wN - w0, hN - h0, rowStyle->backgroundColor.getARGB(), rowImages[y]);
            }
        }
    }
}

void TableWrapperBox::renderTableBorders(ViewCSSImp* view)
{
    if (0 < xWidth && 0 < yHeight)
        renderLayers(view);

    if (borderRows.empty() || borderColumns.empty())
        return;

    glDisable(GL_TEXTURE_2D);
    float h = tableBox->getY() + tableBox->getMarginTop() + tableBox->getBorderTop() ;
    for (unsigned y = 0; y < yHeight + 1; h += heights[y], ++y) {
        float w = tableBox->getX() + tableBox->getMarginLeft() + tableBox->getBorderLeft();
        for (unsigned x = 0; x < xWidth + 1; w += widths[x], ++x) {
            if (x < xWidth) {
                BorderValue* br = getRowBorderValue(x, y);
                if (0.0f < br->getWidth()) {
                    float m = br->getWidth() / 2.0f;
                    float l = w;
                    float r = w + widths[x];
                    float t = h - m;
                    float b = h + m;
                    float ll = 0.0f;
                    float rr = 0.0f;
                    if (y == 0) {
                        if (x == 0)
                            ll = getColumnBorderValue(x, y)->getWidth() / 2.0f;
                        if (x == xWidth - 1)
                            rr = getColumnBorderValue(xWidth, y)->getWidth() / 2.0f;
                    } else if (y == yHeight) {
                        if (x == 0)
                            ll = -getColumnBorderValue(x, yHeight - 1)->getWidth() / 2.0f;
                        if (x == xWidth - 1)
                            rr = -getColumnBorderValue(xWidth, yHeight - 1)->getWidth() / 2.0f;
                    }
                    renderBorderEdge(view, TOP, br->getStyle(), br->color.getARGB(),
                                     l - ll, t, r + rr, t, r - rr, b, l + ll, b);
                }
            }
            if (y < yHeight) {
                BorderValue* bc = getColumnBorderValue(x, y);
                if (0.0f < bc->getWidth()) {
                    float m = bc->getWidth() / 2.0f;
                    float l = w - m;
                    float r = w + m;
                    float t = h;
                    float b = h + heights[y];
                    float tt = 0.0f;
                    float bb = 0.0f;
                    if (x == 0) {
                        if (y == 0)
                            tt = getRowBorderValue(x, y)->getWidth() / 2.0f;
                        if (y == yHeight - 1)
                            bb = getRowBorderValue(x, yHeight)->getWidth() / 2.0f;
                    } else if (x == xWidth) {
                        if (y == 0)
                            tt = -getRowBorderValue(xWidth - 1, y)->getWidth() / 2.0f;
                        if (y == yHeight - 1)
                            bb = -getRowBorderValue(xWidth - 1, yHeight)->getWidth() / 2.0f;
                    }
                    renderBorderEdge(view, LEFT, bc->getStyle(), bc->color.getARGB(),
                                     l, b + bb, l, t - tt, r, t + tt, r, b - bb);
                }
            }
        }
    }
    glEnable(GL_TEXTURE_2D);
}

void TableWrapperBox::renderTableOutlines(ViewCSSImp* view)
{
    // Note the 'outline' property does not apply to elements with a display
    // of table-column or table-column-group.

    // row groups
    float h = tableBox->getY() + tableBox->getMarginTop();
    for (unsigned y = 0; y < yHeight;) {
        CSSStyleDeclarationImp* rowGroupStyle = rowGroups[y].get();
        if (rowGroupStyle) {
            float h0 = h;
            h += heights[y];
            for (++y; rowGroupStyle == rowGroups[y].get(); ++y)
                h += heights[y];
            if (0.0f < rowGroupStyle->outlineWidth.getPx())
                renderOutline(view, tableBox->getX(), h0, tableBox->getX() + tableBox->width, h,
                              rowGroupStyle->outlineWidth.getPx(), rowGroupStyle->outlineStyle.getValue(), rowGroupStyle->outlineColor.getARGB());
        } else {
            h += heights[y];
            ++y;
        }
    }

    // rows
    h = tableBox->getY() + tableBox->getMarginTop();
    for (unsigned y = 0; y < yHeight; h += heights[y], ++y) {
        CSSStyleDeclarationImp* rowStyle = rows[y].get();
        if (rowStyle && 0.0f < rowStyle->outlineWidth.getPx())
            renderOutline(view, tableBox->getX(), h, tableBox->getX() + tableBox->width, h + heights[y],
                          rowStyle->outlineWidth.getPx(), rowStyle->outlineStyle.getValue(), rowStyle->outlineColor.getARGB());
    }

    // cells
    for (unsigned y = 0; y < yHeight; ++y) {
        for (unsigned x = 0; x < xWidth; ++x) {
            CellBox* cellBox = grid[y][x].get();
            if (!cellBox || cellBox->isEmptyCell() || cellBox->isSpanned(x, y))
                continue;
            cellBox->renderOutline(view, cellBox->x, cellBox->y);
        }
    }
}

}}}}  // org::w3c::dom::bootstrap
