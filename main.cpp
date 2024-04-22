
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <memory>
#include <cstring>

#include "file.hpp"
#include "diagnostics.hpp"
#include "tokenizer.hpp"
#include "parser.hpp"
#include "object.hpp"
#include "runner.hpp"

using namespace pop;

bool DEBUG_MODE = false;

int main(int argc, char** argv)
{
    for (int i = 0; i < argc; ++i)
    {
        if (strcmp("-d", argv[i]) == 0)
        {
            DEBUG_MODE = true;
        }
    }

    if (argc == 0)
        return 0;

    File file;
    file.load_file(std::string(argv[1]));

    Diagnostics diagnostics;

    Tokenizer tokenizer;
    tokenizer.parse_file(&file, &diagnostics);

    if (DEBUG_MODE)
        tokenizer.print_tokens();

    if (!diagnostics.has_errors())
    {
        Parser parser;
        parser.parse_statements(tokenizer.get_tokens(), &diagnostics);

        if (DEBUG_MODE)
            parser.print_ast();

        if (!diagnostics.has_errors())
        {
            Runner runner;
            runner.run(parser.get_root(), &diagnostics);
        }
    }

    // diagnostics dump
    std::cout << std::endl;
    diagnostics.dump();

    return 0;
}