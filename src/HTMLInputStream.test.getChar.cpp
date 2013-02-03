/*
 * Copyright 2010-2013 Esrille Inc.
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

#include "html/HTMLInputStream.h"

#include <iostream>
#include <fstream>

#include <stdlib.h>

#include "utf.h"

int main(int argc, char* argv[])
{
    if (argc != 2) {
        std::cout << "usage: " << argv[0] << " html_file\n";
        exit(EXIT_FAILURE);
    }
    int rc = EXIT_SUCCESS;
    std::ifstream stream(argv[1]);
    if (!stream) {
        std::cerr << "error: cannot open " << argv[1] << ".\n";
        return EXIT_FAILURE;
    }
    HTMLInputStream htmlInputStream(stream);
    char16_t c;
    while (htmlInputStream.get(c)) {
        char utf8[5];
        char* end = utf32to8(c, utf8);
        *end = '\0';
        std::cout << utf8;
    }
    return rc;
}
