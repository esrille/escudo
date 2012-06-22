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

#include "css/Box.h"
#include "css/Ico.h"

#include <assert.h>
#include <stdio.h>

#include <iostream>

using namespace org::w3c::dom::bootstrap;

int main(int argc, char* argv[])
{
    IcoImage ico;

    assert(2 <= argc);
    FILE* file = fopen(argv[1], "rb");
    assert(file);

    bool result = ico.open(file);
    assert(result);

    for (size_t i = 0; i < ico.getPlaneCount(); ++i) {
        const IconDirectoryEntry& ent(ico.getEntry(i));
        std::cout << ent.getWidth() << ", " << ent.getHeight() << ",\n";
        BoxImage* image = ico.open(file, i);
        assert(image);
        assert(image->getState() == BoxImage::CompletelyAvailable);
        uint32_t* pixels = reinterpret_cast<uint32_t*>(image->getPixels());
        std::cout << std::hex;
        for (size_t y = 0; y < ent.getHeight(); ++y) {
            for (size_t x = 0; x < ent.getWidth(); ++x)
                std::cout << pixels[y * ent.getWidth() + x] << ", ";
            std::cout << '\n';
        }
        std::cout << std::dec;
    }
}
