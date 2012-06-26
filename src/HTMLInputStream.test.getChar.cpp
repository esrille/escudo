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
    while (htmlInputStream.getChar(c)) {
        char utf8[5];
        char* end = utf32to8(c, utf8);
        *end = '\0';
        std::cout << utf8;
    }
    return rc;
}
