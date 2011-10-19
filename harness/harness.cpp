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

#include <unistd.h>
#include <sys/wait.h>

#include <cstring>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

// A test harness for the implementation report of
//       the CSS2.1 Conformance Test Suite
// http://test.csswg.org/suites/css2.1/20110323/
int main(int argc,  char* argv[])
{
    if (argc < 3) {
        std::cout << "usage: " << argv[0] << " report.data command [argument ...]\n";
        return EXIT_FAILURE;
    }

    std::ifstream data(argv[1]);
    if (!data) {
        std::cerr << "error: " << argv[1] << ": no such file\n";
        return EXIT_FAILURE;
    }

    std::ofstream result("report.data", std::ios_base::out | std::ios_base::trunc);
    if (!result) {
        std::cerr << "error: failed to open the report file\n";
        return EXIT_FAILURE;
    }

    char* args[argc];
    for (int i = 2; i < argc; ++i)
        args[i - 2] = argv[i];
    args[argc - 1] = 0;

    while (data) {
        std::string line;
        std::getline(data, line);
        if (line.empty() || line[0] == '#' || line == "testname    result  comment") {
            result << line << '\n';
            continue;
        }

        std::stringstream s(line, std::stringstream::in);
        std::string url;
        s >> url;
        if (url.empty())
            continue;

        pid_t pid = fork();
        if (pid == -1) {
            std::cerr << "error: no more process to create\n";
            break;
        }
        if (pid == 0) {
            url = "http://localhost:8000/" + url;
            args[argc - 2] = strdup(url.c_str());
            execvp(args[0], args);
            exit(EXIT_FAILURE);
        }
        int status;
        if (wait(&status) == -1) {
            std::cerr << "error: failed to wait for a test process to complete\n";
            break;
        }

        std::cout << '[' << url << "]? ";
        std::string input;
        std::getline(std::cin, input);
        if (input.empty() || input == "p")
            input = "pass";
        else if (input == "f")
            input = "fail";
        else if (input == "i")
            input = "invalid";
        else if (input == "n")
            input = "na";
        else if (input == "s")
            input = "skip";
        else if (input == "u")
            input = "uncertain";
        else if (input == "q" || input == "quit")
            break;

        result << url << '\t' << input << '\n';
    }
    result.close();
}
