/*
 * Copyright 2012 Esrille Inc.
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

#ifndef ORG_W3C_DOM_BOOTSTRAP_PROCESSINGINSTRUCTIONIMP_H_INCLUDED
#define ORG_W3C_DOM_BOOTSTRAP_PROCESSINGINSTRUCTIONIMP_H_INCLUDED

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <org/w3c/dom/ProcessingInstruction.h>
#include "NodeImp.h"

#include <org/w3c/dom/stylesheets/StyleSheet.h>
#include <org/w3c/dom/Node.h>

namespace org
{
namespace w3c
{
namespace dom
{
namespace bootstrap
{
class ProcessingInstructionImp : public ObjectMixin<ProcessingInstructionImp, NodeImp>
{
public:
    // Node - override
    virtual bool isEqualNode(Node arg);
    // ProcessingInstruction
    std::u16string getTarget();
    std::u16string getData();
    void setData(const std::u16string& data);
    // LinkStyle
    stylesheets::StyleSheet getSheet();
    // Object
    virtual Any message_(uint32_t selector, const char* id, int argc, Any* argv)
    {
        return ProcessingInstruction::dispatch(this, selector, id, argc, argv);
    }
    static const char* const getMetaData()
    {
        return ProcessingInstruction::getMetaData();
    }
};

}
}
}
}

#endif  // ORG_W3C_DOM_BOOTSTRAP_PROCESSINGINSTRUCTIONIMP_H_INCLUDED
