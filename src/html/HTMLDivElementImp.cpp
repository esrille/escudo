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

#include "HTMLDivElementImp.h"

#include "css/CSSStyleDeclarationImp.h"

namespace org
{
namespace w3c
{
namespace dom
{
namespace bootstrap
{

void HTMLDivElementImp::eval()
{
    HTMLElementImp::eval();

    // TODO: MVC violation
    std::u16string align = getAlign();
    if (!align.empty()) {
        css::CSSStyleDeclaration style = getStyle();
        CSSStyleDeclarationImp* imp = dynamic_cast<CSSStyleDeclarationImp*>(style.self());
        if (imp)
            imp->setHTMLAlign(align);
    }
}

std::u16string HTMLDivElementImp::getAlign()
{
    return getAttribute(u"align");
}

void HTMLDivElementImp::setAlign(std::u16string align)
{
    setAttribute(u"align", align);
    assert(getAlign() == align);
}

}
}
}
}
