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

#include <fstream>
#include <iostream>
#include <sstream>

#include <stdlib.h>
#include <string.h>
#include <strings.h>

char data[64*1024];
char encoding[1024];

int test()
{
    std::cout << encoding << '\n';
    std::cout << data << '\n';
    std::istringstream stream(data);
    HTMLInputStream htmlInputStream(stream);
    if (strcasecmp(htmlInputStream.getEncoding().c_str(), encoding) == 0)
        std::cout << "PASS\n\n";
    else
        std::cout << "FAIL: '" << htmlInputStream.getEncoding() << "'\n\n";
    return EXIT_SUCCESS;
}

int load(const char* filename)
{
    int rc = EXIT_SUCCESS;
    std::ifstream stream(filename);
    if (!stream) {
        std::cerr << "error: cannot open " << filename << ".\n";
        return EXIT_FAILURE;
    }
    char c;
    char type[1024];
    char* p = data;
    while (stream.get(c)) {
        if (c == '#') {
            if (!stream.getline(type, sizeof type)) {
                std::cerr << "error: malformed input from " << filename << ".\n";
                return EXIT_FAILURE;
            }
            if (strcmp(type, "data") == 0)
                p = data;
            else if (strcmp(type, "encoding") == 0) {
                if (!stream.getline(encoding, sizeof encoding)) {
                    std::cerr << "error: malformed input from " << filename << ".\n";
                    return EXIT_FAILURE;
                }
                *p = '\0';
                rc |= test();
            }
        } else
            *p++ = c;
    }
    return rc;
}

int main(int argc, char* argv[])
{
    if (argc < 2) {
        std::cout << "usage: " << argv[0] << " [test.dat]...\n";
        exit(EXIT_FAILURE);
    }
    int rc = EXIT_SUCCESS;
    for (int i = 1; i < argc; ++i)
        rc |= load(argv[i]);
    return rc;
}
