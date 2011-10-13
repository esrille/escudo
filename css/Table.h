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

namespace org { namespace w3c { namespace dom { namespace bootstrap {

class CellBox : public BlockLevelBox
{
    friend class TableWrapperBox;

    unsigned colSpan;
    unsigned rowSpan;
public:
    CellBox(Element element = 0, CSSStyleDeclarationImp* style = 0) :
        BlockLevelBox(element, style),
        colSpan(1),
        rowSpan(1)
    {
    }
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
};

typedef boost::intrusive_ptr<CellBox> CellBoxPtr;

class TableWrapperBox : public BlockLevelBox
{
    typedef BlockLevelBoxPtr Caption;

    typedef std::deque<CellBoxPtr> Row;
    typedef std::deque<Row> Grid;

    ViewCSSImp* view;

    std::deque<Caption> topCaptions;
    std::deque<Caption> bottomCaptions;

    Grid grid;
    unsigned xWidth;
    unsigned yHeight;

    Element table;
    BlockLevelBox* tableBox;

    Retained<CellBox> occupied;

    unsigned appendRow();
    unsigned appendColumn();

    void formTable();
    void processColGruop(Element colgroup);
    unsigned processRow(Element row, unsigned yCurrent);
    unsigned processRowGruop(Element section, unsigned yCurrent);
    unsigned endRowGroup(int yCurrent);
    void growDownwardGrowingCells();

public:
    TableWrapperBox(ViewCSSImp* view, Element element, CSSStyleDeclarationImp* style);
    virtual void fit(float w);
    virtual bool layOut(ViewCSSImp* view, FormattingContext* context);
    virtual float shrinkTo();
    virtual void dump(std::string indent = "");
};

typedef boost::intrusive_ptr<TableWrapperBox> TableWrapperBoxPtr;

}}}}  // org::w3c::dom::bootstrap

#endif  // ES_CSSTABLE_H
