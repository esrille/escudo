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

#ifndef ES_CSSTABLE_H
#define ES_CSSTABLE_H

#include "css/Box.h"
#include "Box.h"

#include <vector>

namespace org { namespace w3c { namespace dom { namespace bootstrap {

class TableWrapperBox;

class CellBox : public BlockLevelBox
{
    friend class TableWrapperBox;

    unsigned col;
    unsigned row;
    unsigned colSpan;
    unsigned rowSpan;
public:
    CellBox(Element element = 0, CSSStyleDeclarationImp* style = 0) :
        BlockLevelBox(element, style),
        col(0),
        row(0),
        colSpan(1),
        rowSpan(1)
    {
    }

    virtual void fit(float w);

    unsigned getColSpan() const {
        return colSpan;
    }
    void setColSpan(unsigned span) {
        colSpan = span;
    }
    unsigned getRowSpan() const {
        return rowSpan;
    }
    void setRowSpan(unsigned span) {
        rowSpan = span;
    }

    void setPosition(unsigned x, unsigned y) {
        col = x;
        row = y;
    }
    bool isSpanned(unsigned x, unsigned y) const {
        return col != x || row != y;
    }

    void collapseBorder(TableWrapperBox* wrapper);
};

typedef boost::intrusive_ptr<CellBox> CellBoxPtr;

class TableWrapperBox : public BlockLevelBox
{
    typedef BlockLevelBoxPtr Caption;

    typedef std::deque<CellBoxPtr> Row;
    typedef std::deque<Row> Grid;

    struct BorderValue {
        CSSBorderColorValueImp color;
        CSSBorderStyleValueImp style;
        CSSBorderWidthValueImp width;
    public:
        BorderValue() :
            width(0.0f)
        {}

        bool resolveBorderConflict(CSSBorderColorValueImp& c, CSSBorderStyleValueImp& s, CSSBorderWidthValueImp& w);
        void resolveBorderConflict(CSSStyleDeclarationPtr s, unsigned trbl);
        float getWidth() const {
            return width.getPx();
        }
    };

    ViewCSSImp* view;

    std::deque<Caption> topCaptions;
    std::deque<Caption> bottomCaptions;

    std::deque<CSSStyleDeclarationPtr> rows;
    std::deque<CSSStyleDeclarationPtr> rowGroups;
    std::deque<CSSStyleDeclarationPtr> columns;
    std::deque<CSSStyleDeclarationPtr> columnGroups;

    Grid grid;
    unsigned xWidth;
    unsigned yHeight;

    Element table;
    BlockLevelBox* tableBox;
    std::vector<BorderValue> borderRows;
    std::vector<BorderValue> borderColumns;

    std::vector<float> widths;
    std::vector<float> heights;

    unsigned appendRow();
    unsigned appendColumn();

    void formTable(ViewCSSImp* view);
    void processColGruop(ViewCSSImp* view, Element colgroup);
    unsigned processRow(ViewCSSImp* view, Element row, unsigned yCurrent);
    unsigned processRowGruop(ViewCSSImp* view, Element section, unsigned yCurrent);
    unsigned endRowGroup(int yCurrent);
    void growDownwardGrowingCells();

    void resolveHorizontalBorderConflict(unsigned x, unsigned y, BorderValue* b, CellBox* c, unsigned mask);
    void resolveVerticalBorderConflict(unsigned x, unsigned y, BorderValue* b, CellBox* c, unsigned mask);
    bool resolveBorderConflict();

public:
    TableWrapperBox(ViewCSSImp* view, Element element, CSSStyleDeclarationImp* style);

    BorderValue* getRowBorderValue(unsigned x, unsigned y) {
        assert(x < xWidth);
        return &borderRows[xWidth * y + x];
    }
    BorderValue* getColumnBorderValue(unsigned x, unsigned y) {
        assert(x < xWidth + 1);
        return &borderColumns[(xWidth + 1) * y + x];
    }

    virtual void fit(float w);
    virtual bool layOut(ViewCSSImp* view, FormattingContext* context);
    virtual float shrinkTo();

    virtual void renderTableBorders(ViewCSSImp* view);

    virtual void dump(std::string indent = "");
};

typedef boost::intrusive_ptr<TableWrapperBox> TableWrapperBoxPtr;

}}}}  // org::w3c::dom::bootstrap

#endif  // ES_CSSTABLE_H
