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

void TableWrapperBox::renderTableBorders(ViewCSSImp* view)
{
    if (borderRows.empty() || borderColumns.empty())
        return;

    glDisable(GL_TEXTURE_2D);
    float h = tableBox->getY();
    for (unsigned y = 0; y < yHeight + 1; h += heights[y], ++y) {
        float w = tableBox->getX();
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
