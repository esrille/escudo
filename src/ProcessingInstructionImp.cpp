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

#include "ProcessingInstructionImp.h"

namespace org
{
namespace w3c
{
namespace dom
{
namespace bootstrap
{

bool ProcessingInstructionImp::isEqualNode(Node arg)
{
    ProcessingInstructionImp* processingInstruction = dynamic_cast<ProcessingInstructionImp*>(arg.self());
    if (this == processingInstruction)
        return true;
    if (!processingInstruction)
        return false;
    if (getTarget() != processingInstruction->getTarget())
        return false;
    if (getData() != processingInstruction->getData())
        return false;
    return NodeImp::isEqualNode(arg);
}

std::u16string ProcessingInstructionImp::getTarget()
{
    // TODO: implement me!
    return u"";
}

std::u16string ProcessingInstructionImp::getData()
{
    // TODO: implement me!
    return u"";
}

void ProcessingInstructionImp::setData(const std::u16string& data)
{
    // TODO: implement me!
}

stylesheets::StyleSheet ProcessingInstructionImp::getSheet()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

}
}
}
}
