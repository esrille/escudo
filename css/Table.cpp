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

#include "Table.h"

#include <org/w3c/dom/html/HTMLTableElement.h>
#include <org/w3c/dom/html/HTMLTableCaptionElement.h>
#include <org/w3c/dom/html/HTMLTableCellElement.h>
#include <org/w3c/dom/html/HTMLTableColElement.h>
#include <org/w3c/dom/html/HTMLTableRowElement.h>
#include <org/w3c/dom/html/HTMLTableSectionElement.h>

#include "css/Box.h"
#include "css/ViewCSSImp.h"
#include "ViewCSSImp.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

namespace {

int isOneOf(const std::u16string& s, std::initializer_list<const char16_t*> list)
{
    int index = 0;
    for (auto i = list.begin(); i != list.end(); ++i, ++index) {
        if (s.compare(*i) == 0)
            return index;
    }
    return -1;
}

}  // namespace

TableWrapperBox::TableWrapperBox(ViewCSSImp* view, Element element, CSSStyleDeclarationImp* style) :
    BlockLevelBox(element, style),
    view(view),
    xWidth(0),
    yHeight(0),
    table(element),
    tableBox(0)
{
    // TODO: Support cases where display is not table or inline-table.
    assert(style->display == CSSDisplayValueImp::Table || style->display == CSSDisplayValueImp::InlineTable);
    formTable();

    // Top caption boxes
    for (auto i = topCaptions.begin(); i != topCaptions.end(); ++i)
        appendChild(i->get());

    // Table box
    tableBox = new(std::nothrow) BlockLevelBox;
    if (tableBox) {
        for (unsigned y = 0; y < yHeight; ++y) {
            LineBox* lineBox = new(std::nothrow) LineBox(0);
            if (!lineBox)
                continue;
            tableBox->appendChild(lineBox);
            for (unsigned x = 0; x < xWidth; ++x) {
                CellBox* cellBox = grid[y][x].get();
                if (!cellBox || *cellBox == occupied)
                    continue;
                lineBox->appendChild(cellBox);
            }
        }
        appendChild(tableBox);
    }

    // Bottom caption boxes
    for (auto i = bottomCaptions.begin(); i != bottomCaptions.end(); ++i)
        appendChild(i->get());
}

unsigned TableWrapperBox::appendRow()
{
    ++yHeight;
    grid.resize(yHeight);
    grid.back().resize(xWidth);
    return yHeight;
}

unsigned TableWrapperBox::appendColumn()
{
    ++xWidth;
    for (auto r = grid.begin(); r != grid.end(); ++r)
        r->resize(xWidth);
    return xWidth;
}

void TableWrapperBox::formTable()
{
    if (table.getChildElementCount() == 0)
        return;

    // 10.
    unsigned yCurrent = 0;
    // 11.
    std::list<Element> pendingTfootElements;
    // std::list<> downwardGrowingCells;
    for (Element current = table.getFirstElementChild(); current; current = current.getNextElementSibling()) {
        CSSStyleDeclarationImp* currentStyle = view->getStyle(current);
        assert(currentStyle);
        if (!currentStyle->display.isProperTableChild()) {
            yCurrent = endRowGroup(yCurrent);
            continue;
        }
        unsigned display = currentStyle->display.getValue();
        if (display == CSSDisplayValueImp::TableCaption) {
            BlockLevelBox* caption = view->layOutBlockBoxes(current, this, 0, currentStyle, false);
            if (!caption)
                continue;
            if (currentStyle->captionSide.getValue() == CSSCaptionSideValueImp::Top)
                topCaptions.push_back(caption);
            else
                bottomCaptions.push_back(caption);
            continue;
        }
        if (display == CSSDisplayValueImp::TableColumnGroup) {
            yCurrent = endRowGroup(yCurrent);
            processColGruop(current);
            continue;
        }
        if (display == CSSDisplayValueImp::TableColumn)  // TODO HTML doesn't need this though
            continue;
        // 12.
        // TODO: ?
        // 13.
        if (display == CSSDisplayValueImp::TableRow) {
            // TODO
            yCurrent = processRow(current, yCurrent);
            continue;
        }
        if (display == CSSDisplayValueImp::TableFooterGroup) {
            pendingTfootElements.push_back(current);
            continue;
        }
        assert(display == CSSDisplayValueImp::TableHeaderGroup || display == CSSDisplayValueImp::TableRowGroup);
        yCurrent = processRowGruop(current, yCurrent);
    }
    while (!pendingTfootElements.empty()) {
        Element tfoot = pendingTfootElements.front();
        yCurrent = processRowGruop(tfoot, yCurrent);
        pendingTfootElements.pop_front();
    }
}

unsigned TableWrapperBox::processRow(Element row, unsigned yCurrent)
{
    if (yHeight == yCurrent)
        appendRow();
    unsigned xCurrent = 0;
    growDownwardGrowingCells();
    for (Element child = row.getFirstElementChild(); child; child = child.getNextElementSibling()) {
        CSSStyleDeclarationImp* childStyle = view->getStyle(child);
        assert(childStyle);
        if (childStyle->display.getValue() != CSSDisplayValueImp::TableCell)
            continue;
        while (xCurrent < xWidth && grid[yCurrent][xCurrent])
            ++xCurrent;
        if (xCurrent == xWidth)
            appendColumn();
        unsigned colspan = 1;
        unsigned rowspan = 1;
        if (html::HTMLTableCellElement::hasInstance(child)) {
            html::HTMLTableCellElement cell(interface_cast<html::HTMLTableCellElement>(child));
            colspan = cell.getColSpan();
            rowspan = cell.getRowSpan();
        }
        // TODO: 10 ?
        bool cellGrowsDownward = false;
        while (xWidth < xCurrent + colspan)
            appendColumn();
        while (yHeight < yCurrent + rowspan)
            appendRow();
        for (unsigned x = xCurrent; x < xCurrent + colspan; ++x) {
            for (unsigned y = yCurrent; y < yCurrent + rowspan; ++y)
                grid[y][x] = &occupied;
        }
        CellBox* cellBox = static_cast<CellBox*>(view->layOutBlockBoxes(child, 0, 0, childStyle, true));
        if (cellBox) {
            cellBox->setColSpan(colspan);
            cellBox->setRowSpan(rowspan);
            grid[yCurrent][xCurrent] = cellBox;
        }
        // TODO: 13
        if (cellGrowsDownward)
            ;  // TODO: 14
        xCurrent += colspan;
    }
    return ++yCurrent;
}

unsigned TableWrapperBox::processRowGruop(Element section, unsigned yCurrent)
{
    unsigned yStart = yHeight;
    for (Element child = section.getFirstElementChild(); child; child = child.getNextElementSibling()) {
        CSSStyleDeclarationImp* childStyle = view->getStyle(child);
        assert(childStyle);
        if (childStyle->display.getValue() != CSSDisplayValueImp::TableRow)
            continue;
        yCurrent = processRow(child, yCurrent);
    }
    // TODO 3.
    return endRowGroup(yCurrent);
}

unsigned TableWrapperBox::endRowGroup(int yCurrent)
{
    while (yCurrent < yHeight) {
        growDownwardGrowingCells();
        ++yCurrent;
    }
    // downwardGrowingCells.clear();
    return yCurrent;
}

void TableWrapperBox::processColGruop(Element colgroup)
{
    bool hasCol = false;
    int xStart = xWidth;
    for (Element child = colgroup.getFirstElementChild(); child; child = child.getNextElementSibling()) {
        CSSStyleDeclarationImp* childStyle = view->getStyle(child);
        assert(childStyle);
        if (childStyle->display.getValue() != CSSDisplayValueImp::TableColumn)
            continue;
        hasCol = true;
        unsigned int span = 1;
        if (html::HTMLTableColElement::hasInstance(child)) {
            html::HTMLTableColElement col(interface_cast<html::HTMLTableColElement>(child));
            span = col.getSpan();
        }
        while (0 < span--)
            appendColumn();
        // TODO 5.
    }
    if (hasCol) {
        // TODO. 7.
    } else {
        unsigned int span = 1;
        if (html::HTMLTableColElement::hasInstance(colgroup)) {
            html::HTMLTableColElement cg(interface_cast<html::HTMLTableColElement>(colgroup));
            span = cg.getSpan();
        }
        while (0 < span--)
            appendColumn();
        // TODO 3.
    }
}

void TableWrapperBox::growDownwardGrowingCells()
{
}

void TableWrapperBox::fit(float w)
{
    float diff = w - getTotalWidth();
    if (0.0f < diff) {
        unsigned autoMask = Left | Right;
        if (style) {
            if (!style->marginLeft.isAuto())
                autoMask &= ~Left;
            if (!style->marginRight.isAuto())
                autoMask &= ~Right;
        }
        switch (autoMask) {
        case Left | Right:
            diff /= 2.0f;
            marginLeft += diff;
            marginRight += diff;
            break;
        case Left:
            marginLeft += diff;
            break;
        case Right:
            marginRight += diff;
            break;
        default:
            break;
        }
    }
}

void TableWrapperBox::layOut(ViewCSSImp* view, FormattingContext* context)
{
    const ContainingBlock* containingBlock = getContainingBlock(view);
    style = view->getStyle(table);
    if (!style)
        return;  // TODO error

    style->resolve(view, containingBlock, table);

    // The computed values of properties 'position', 'float', 'margin-*', 'top', 'right', 'bottom',
    // and 'left' on the table element are used on the table wrapper box and not the table box;
    backgroundColor = 0x00000000;
    paddingTop = paddingRight = paddingBottom = paddingLeft = 0.0f;
    borderTop = borderRight = borderBottom = borderLeft = 0.0f;
    resolveMargin(view, containingBlock, 0.0f);
    stackingContext = style->getStackingContext();

    context = collapseMargins(context);

    if (tableBox) {
        tableBox->setStyle(style.get());
        tableBox->resolveBackground(view);
        tableBox->updatePadding();
        tableBox->updateBorderWidth();
        tableBox->resolveMargin(view, containingBlock, 0.0f);

        float widths[xWidth];
        float heights[yHeight];
        for (unsigned x = 0; x < xWidth; ++x)
            widths[x] = 0.0f;
        for (unsigned y = 0; y < yHeight; ++y)
            heights[y] = 0.0f;
        for (unsigned x = 0; x < xWidth; ++x) {
            for (unsigned y = 0; y < yHeight; ++y) {
                CellBox* cellBox = grid[y][x].get();
                if (!cellBox || *cellBox == occupied)
                    continue;
                cellBox->layOut(view, context);
                if (cellBox->getColSpan() == 1)
                    widths[x] = std::max(widths[x], cellBox->getBorderWidth());
                if (cellBox->getRowSpan() == 1)
                    heights[y] = std::max(heights[y], cellBox->getBorderHeight());
            }
        }
        for (unsigned x = 0; x < xWidth; ++x) {
            for (unsigned y = 0; y < yHeight; ++y) {
                CellBox* cellBox = grid[y][x].get();
                if (!cellBox || *cellBox == occupied)
                    continue;
                unsigned span = cellBox->getColSpan();
                if (1 < span) {
                    float sum = 0.0f;
                    for (unsigned c = 0; c < span; ++c)
                        sum += widths[x + c];
                    if (sum < cellBox->getBorderWidth()) {
                        float diff = (cellBox->getBorderWidth() - sum) / span;
                        for (unsigned c = 0; c < span; ++c)
                            widths[x + c] += diff;
                    }
                }
                span = cellBox->getRowSpan();
                if (1 < span) {
                    float sum = 0.0f;
                    for (unsigned r = 0; r < span; ++r)
                        sum += heights[y + r];
                    if (sum < cellBox->getBorderHeight()) {
                        float diff = (cellBox->getBorderHeight() - sum) / span;
                        for (unsigned r = 0; r < span; ++r)
                            heights[y + r] += diff;
                    }
                }
            }
        }

        for (unsigned x = 0; x < xWidth; ++x) {
            for (unsigned y = 0; y < yHeight; ++y) {
                CellBox* cellBox = grid[y][x].get();
                if (!cellBox || *cellBox == occupied)
                    continue;
                float w = 0.0f;
                for (unsigned c = 0; c < cellBox->getColSpan(); ++c)
                    w += widths[x + c];
                cellBox->fit(w);
                float h = 0.0f;
                for (unsigned r = 0; r < cellBox->getRowSpan(); ++r)
                    h += heights[y + r];
                cellBox->height = h;
            }
        }

        tableBox->width = 0.0f;
        for (unsigned x = 0; x < xWidth; ++x)
            tableBox->width += widths[x];

        tableBox->height = 0.0f;
        Box* lineBox = tableBox->getFirstChild();
        for (unsigned y = 0; y < yHeight; ++y)  {
            tableBox->height += heights[y];
            assert(lineBox);
            lineBox->width = tableBox->width;
            lineBox->height = heights[y];
            lineBox = lineBox->getNextSibling();

            float xOffset = 0.0f;
            for (unsigned x = 0; x < xWidth; ++x) {
                CellBox* cellBox = grid[y][x].get();
                if (!cellBox || *cellBox == occupied) {
                    xOffset += widths[x];
                    continue;
                }
                cellBox->offsetH += xOffset;
            }
        }
    }

    width = 0.0f;
    for (Box* child = getFirstChild(); child; child = child->getNextSibling())
        width = std::max(width, child->getTotalWidth());
    height = 0.0f;
    for (Box* child = getFirstChild(); child; child = child->getNextSibling())
        height += child->getTotalHeight();

    style->width.setValue();

    collapseMarginBottom();
    if (!isAnonymous()) {
        width = std::max(width, style->minWidth.getPx());
        height = std::max(height, style->minHeight.getPx());
    }
}

float TableWrapperBox::shrinkTo()
{
    return getTotalWidth();
}

void TableWrapperBox::dump(ViewCSSImp* view, std::string indent)
{
    std::cout << indent << "* table wrapper box: (" << xWidth << ", " << yHeight << ")\n";
    indent += "    ";
    for (Box* child = getFirstChild(); child; child = child->getNextSibling())
        child->dump(view, indent);
}

}}}}  // org::w3c::dom::bootstrap
