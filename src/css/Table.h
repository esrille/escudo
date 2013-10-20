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

#ifndef ES_CSSTABLE_H
#define ES_CSSTABLE_H

#include "css/Box.h"
#include "Box.h"

#include <vector>

namespace org { namespace w3c { namespace dom { namespace bootstrap {

class CellBox;
class TableWrapperBox;

typedef std::shared_ptr<CellBox> CellBoxPtr;
typedef std::shared_ptr<TableWrapperBox> TableWrapperBoxPtr;

class CellBox : public Block
{
    friend class TableWrapperBox;

    bool fixedLayout;
    unsigned col;
    unsigned row;
    unsigned colSpan;
    unsigned rowSpan;
    unsigned verticalAlign;
    float intrinsicHeight;
    float columnWidth;

    float getBaseline(const BoxPtr& box) const;

public:
    CellBox(Element element = nullptr, const CSSStyleDeclarationPtr& style = nullptr);

    virtual void fit(float w, FormattingContext* context);

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
    bool isLeftSpanned(unsigned x) const {
        return col != x;
    }
    bool isTopSpanned(unsigned y) const {
        return row != y;
    }
    bool isRightSpanned(unsigned x) const {
        return col + colSpan != x;
    }
    bool isBottomSpanned(unsigned y) const {
        return row + rowSpan != y;
    }

    void separateBorders(const CSSStyleDeclarationPtr& style, unsigned xWidth, unsigned yHeight);
    void collapseBorder(const TableWrapperBoxPtr& wrapper);

    unsigned getVerticalAlign() const {
        return verticalAlign;
    }
    float getBaseline() const;

    bool isEmptyCell() const;

    float getColumnWidth() const {
        return columnWidth;
    }
    float adjustWidth();
    void resetWidth() {
        columnWidth = NAN;
        fixedLayout = false;
    }

    virtual float shrinkTo();
    virtual float resolveWidth(float w, FormattingContext* context, float r);
    virtual void render(ViewCSSImp* view, StackingContext* stackingContext);
    void renderNonInline(ViewCSSImp* view, StackingContext* stackingContext);
};

typedef std::shared_ptr<CellBox> CellBoxPtr;

class TableWrapperBox : public Block
{
    typedef BlockPtr Caption;

    typedef std::deque<CellBoxPtr> Row;
    typedef std::deque<Row> Grid;

    struct BorderValue
    {
        CSSBorderColorValueImp color;
        CSSBorderStyleValueImp style;
        float width;
    public:
        BorderValue() :
            width(0.0f)
        {}

        bool resolveBorderConflict(CSSBorderColorValueImp& c, CSSBorderStyleValueImp& s, CSSBorderWidthValueImp& w);
        void resolveBorderConflict(CSSStyleDeclarationPtr s, unsigned trbl);
        float getWidth() const {
            return width;
        }
        unsigned getStyle() const {
            unsigned value = style.getValue();
            switch (value) {
            case CSSBorderStyleValueImp::Inset:
                value = CSSBorderStyleValueImp::Ridge;
                break;
            case CSSBorderStyleValueImp::Outset:
                value = CSSBorderStyleValueImp::Groove;
                break;
            default:
                break;
            }
            return value;
        }
    };

    ViewCSSImp* view;
    Element table;

    std::deque<Caption> topCaptions;
    std::deque<Caption> bottomCaptions;

    std::deque<CSSStyleDeclarationPtr> rows;
    std::deque<CSSStyleDeclarationPtr> rowGroups;
    std::deque<CSSStyleDeclarationPtr> columns;
    std::deque<CSSStyleDeclarationPtr> columnGroups;

    Grid grid;
    unsigned xWidth;
    unsigned yHeight;

    BlockPtr tableBox;
    std::vector<BorderValue> borderRows;
    std::vector<BorderValue> borderColumns;

    std::vector<float> widths;
    std::vector<float> fixedWidths;
    std::vector<float> percentages;
    std::vector<float> heights;
    std::vector<float> baselines;
    std::vector<BoxImage*> rowImages;
    std::vector<BoxImage*> rowGroupImages;
    std::vector<BoxImage*> columnImages;
    std::vector<BoxImage*> columnGroupImages;

    bool isAnonymousTable;
    bool isHtmlTable;

    // CSS table model
    bool inRow;
    unsigned xCurrent;
    unsigned yCurrent;
    CellBoxPtr anonymousCell;
    TableWrapperBoxPtr anonymousTable;
    Element pendingTheadElement;
    unsigned yTheadBegin;
    unsigned yTheadEnd;
    Element pendingTfootElement;
    unsigned yTfootBegin;
    unsigned yTfootEnd;

    BlockPtr constructTablePart(Node node);

    unsigned appendRow();
    unsigned appendColumn();

    void processCol(Element col, const CSSStyleDeclarationPtr& colStyle, Element colgroup);
    void processColGroup(Element colgroup);
    void processRow(Element row);
    void processRowChild(Node node, const CSSStyleDeclarationPtr& rowStyle);
    void endRow();
    void processRowGroup(Element section);
    void processRowGroupChild(Node node, const CSSStyleDeclarationPtr& sectionStyle);
    void endRowGroup();
    void processHeader();
    void processFooter();
    void growDownwardGrowingCells();

    void resolveHorizontalBorderConflict(unsigned x, unsigned y, BorderValue* b, const CellBoxPtr& top, const CellBoxPtr& bottom);
    void resolveVerticalBorderConflict(unsigned x, unsigned y, BorderValue* b, const CellBoxPtr& left, const CellBoxPtr& right);
    bool resolveBorderConflict();
    void computeTableBorders();

    void formCSSTable();
    CellBoxPtr processCell(Element current, const BlockPtr& parentBox, const CSSStyleDeclarationPtr& style, const CSSStyleDeclarationPtr& rowStyle);

    void layOutFixed(ViewCSSImp* view, const ContainingBlockPtr& containingBlock, bool collapsingModel);
    void layOutAuto(ViewCSSImp* view, const ContainingBlockPtr& containingBlock);
    void layOutAutoColgroup(ViewCSSImp* view, const ContainingBlockPtr& containingBlock);
    void layOutTableBox(ViewCSSImp* view, FormattingContext* context, const ContainingBlockPtr& containingBlock, bool collapsingModel, bool fixedLayout);
    void layOutTableParts();
    void resetCellBoxWidths();

    void renderBackground(ViewCSSImp* view, const CSSStyleDeclarationPtr& style, float x, float y, float left, float top, float right, float bottom, float width, float height, unsigned backgroundColor, BoxImage* backgroundImage);
    void renderLayers(ViewCSSImp* view);

public:
    TableWrapperBox(ViewCSSImp* view, Element element, const CSSStyleDeclarationPtr& style);
    ~TableWrapperBox();

    void constructBlocks();
    void clearGrid();
    void revertTablePart(Node node);
    void reconstructBlocks();

    BorderValue* getRowBorderValue(unsigned x, unsigned y) {
        assert(x < xWidth);
        return &borderRows[xWidth * y + x];
    }
    BorderValue* getColumnBorderValue(unsigned x, unsigned y) {
        assert(x < xWidth + 1);
        return &borderColumns[(xWidth + 1) * y + x];
    }

    BlockPtr getTableBox() const {
        return tableBox;
    }
    bool isTableBox(const BlockPtr& box) const {
        if (BlockPtr table = getTableBox())
            return table == box;
        return false;
    }

    bool isAnonymousTableObject() const {
        return isAnonymousTable;
    }

    unsigned getColumnCount() const {
        return xWidth;
    }
    unsigned getRowCount() const {
        return yHeight;
    }

    bool processTableChild(Node node, const CSSStyleDeclarationPtr& style);

    float getBaseline() const;

    virtual void fit(float w, FormattingContext* context);
    virtual bool layOut(ViewCSSImp* view, FormattingContext* context);
    virtual float shrinkTo();

    virtual void layOutAbsolute(ViewCSSImp* view);  // 2nd pass

    void renderTableBorders(ViewCSSImp* view);
    void renderTableOutlines(ViewCSSImp* view);

    virtual void dump(std::string indent = "");
};

typedef std::shared_ptr<TableWrapperBox> TableWrapperBoxPtr;

}}}}  // org::w3c::dom::bootstrap

#endif  // ES_CSSTABLE_H
