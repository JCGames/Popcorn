
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

    File file = File(std::string(argv[1]));

    Diagnostics diagnostics;

    Tokenizer tokenizer;
    tokenizer.parse_file(&file, &diagnostics);

    if (DEBUG_MODE)
        tokenizer.print_tokens();

    // display diagnostics
    if (diagnostics.has_errors() || diagnostics.has_warnings())
        diagnostics.dump();

    if (diagnostics.has_errors()) return 0;

    Parser parser;
    parser.parse_statements(tokenizer.get_tokens(), &diagnostics);

    if (DEBUG_MODE)
        parser.print_ast();

    // display diagnostics
    if (diagnostics.has_errors() || diagnostics.has_warnings())
        diagnostics.dump();

    if (diagnostics.has_errors()) return 0;
    
    Runner runner;
    runner.run(parser.get_root(), &diagnostics);

    // display diagnostics
    if (diagnostics.has_errors() || diagnostics.has_warnings())
        diagnostics.dump();

    return 0;
}