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

// A test harness for the implementation report of
//       the CSS2.1 Conformance Test Suite
// http://test.csswg.org/suites/css2.1/20110323/

#include <unistd.h>
#include <sys/wait.h>

#include <cstring>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include <boost/version.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/file_descriptor.hpp>

int processOutput(std::istream& stream, std::string& result)
{
    std::string output;
    bool completed = false;
    while (std::getline(stream, output)) {
        if (!completed) {
            if (output == "## complete")
                completed = true;
            continue;
        }
        if (output == "##")
            break;
        result += output + '\n';
    }
    return 0;
}

bool loadLog(const std::string& path, std::string& result, std::string& log)
{
    std::ifstream file(path.c_str());
    if (!file) {
        result = "?";
        return false;
    }
    std::string line;
    std::getline(file, line);
    size_t pos = line.find('\t');
    if (pos != std::string::npos)
        result = line.substr(pos + 1);
    else {
        result = "?";
        return false;
    }
    log.clear();
    while (std::getline(file, line))
        log += line + '\n';
    return true;
}

bool saveLog(const std::string& path, const std::string& url, const std::string& result, const std::string& log)
{
    std::ofstream file(path.c_str(), std::ios_base::out | std::ios_base::trunc);
    if (!file) {
        std::cerr << "error: failed to open the report file\n";
        return false;
    }
    file << "# " << url.c_str() << '\t' << result << '\n' << log;
    file.flush();
    file.close();
    return true;
}

int main(int argc, char* argv[])
{
    bool headless = true;

    int argi = 1;
    while (*argv[argi] == '-') {
        switch (argv[argi][1]) {
        case 'i':
            headless = false;
            break;
        default:
            break;
        }
        ++argi;
    }

    if (argc < argi + 2) {
        std::cout << "usage: " << argv[0] << " [-i] report.data command [argument ...]\n";
        return EXIT_FAILURE;
    }

    std::ifstream data(argv[argi]);
    if (!data) {
        std::cerr << "error: " << argv[argi] << ": no such file\n";
        return EXIT_FAILURE;
    }

    std::ofstream report("report.data", std::ios_base::out | std::ios_base::trunc);
    if (!report) {
        std::cerr << "error: failed to open the report file\n";
        return EXIT_FAILURE;
    }

    char* args[argc - argi + 1];
    for (int i = 2; i < argc; ++i)
        args[i - 2] = argv[i + argi - 1];
    args[argc - argi] = 0;

    while (data) {
        std::string line;
        std::getline(data, line);
        if (line.empty() || line[0] == '#' || line == "testname    result  comment") {
            report << line << '\n';
            continue;
        }

        std::stringstream s(line, std::stringstream::in);
        std::string url;
        s >> url;
        if (url.empty())
            continue;

        int pipefd[2];
        pipe(pipefd);

        pid_t pid = fork();
        if (pid == -1) {
            std::cerr << "error: no more process to create\n";
            break;
        }
        if (pid == 0) {
            close(1);
            dup(pipefd[1]);
            close(pipefd[0]);

            url = "http://localhost:8000/" + url;
            args[argc - argi - 1] = strdup(url.c_str());
            execvp(args[0], args);
            exit(EXIT_FAILURE);
        }
        close(pipefd[1]);

        std::string output;
#if 104400 <= BOOST_VERSION
        boost::iostreams::stream<boost::iostreams::file_descriptor_source> stream(pipefd[0], boost::iostreams::close_handle);
#else
        boost::iostreams::stream<boost::iostreams::file_descriptor_source> stream(pipefd[0], true);
#endif
        processOutput(stream, output);

        std::string path(url);
        size_t pos = path.rfind('.');
        if (pos != std::string::npos) {
            path.erase(pos);
            path += ".log";
        }

        std::string evaluation;
        std::string log;
        loadLog(path, evaluation, log);

        std::string result;
        if (output.empty())
            result = "fatal";
        else if (!headless) {
            std::cout << "## complete\n" << output;
            std::cout << '[' << url << " : " << evaluation << "]? ";
            std::getline(std::cin, result);
            if (result.empty() || result == "p" || result == "\x1b")
                result = "pass";
            else if (result == "f")
                result = "fail";
            else if (result == "i")
                result = "invalid";
            else if (result == "k") // keep
                result = evaluation;
            else if (result == "n")
                result = "na";
            else if (result == "s")
                result = "skip";
            else if (result == "u")
                result = "uncertain";
            else if (result == "q" || result == "quit")
                break;
            if (!saveLog(path, url, result, output)) {
                std::cerr << "error: failed to open the report file\n";
                return EXIT_FAILURE;
            }
        } else {
            if (evaluation != "?" && output != log)
                result = "uncertain";
            else
                result = evaluation;
            std::cout << url << '\t' << result << '\n';
        }

        int status;
        kill(pid, SIGTERM);
        if (wait(&status) == -1) {
            std::cerr << "error: failed to wait for a test process to complete\n";
            break;
        }

        report << url << '\t' << result << '\n';
    }
    report.close();
}
