#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include "lib/lexer.hpp"
#include "lib/parser.hpp"
#include "lib/runner.hpp"
#include "includes/console.hpp"
#include "lib/diagnostics.hpp"

using namespace std;
using namespace popcorn::diagnostics;

static bool DEBUG = false;
const char* VERSION = "1.0";

string get_file_ext(const string& s) 
{
    size_t i = s.rfind('.', s.length());
    if (i != string::npos)
        return(s.substr(i + 1, s.length() - i));

    return("");
}

int main(int argc, char** args)
{
    for (int i = 0; i < argc; ++i)
    {
        if (strcmp(args[i], "-d") == 0 || strcmp(args[i], "--debug") == 0)
        {
            DEBUG = true;
        }
        else if (strcmp(args[i], "-v") == 0 || strcmp(args[i], "--version") == 0)
        {
            printf("Popcorn Version: %s\n", VERSION);
            return 0;
        }
    }

    if (argc > 1)
    {
        // set up console output if on windows
        #ifdef _WIN32
            helpers::Console::init();
        #endif

        // make sure the file is a .pop file
        string fileName(args[1]);
        string fileExt = get_file_ext(fileName);

        if (fileExt.empty())
        {
            fileName += ".pop";
            fileExt = "pop";
        }

        if (fileExt != "pop")
            Diagnostics::log_error("Could not run the file " + std::string(args[1]) + " because it was not a .pop file.");

        /**
         * Lexing ...
        */

        popcorn::lexer::Lexer lexer(fileName);
        std::vector<popcorn::lexer::Token> tokens = lexer.get_tokens();

        if (DEBUG)
        {
            printf("========== TOKENS ==========\n");
            for (const auto& token : tokens)
                print_token(token);
        }

        if (DEBUG)
            printf("\n=============================\n");

        /**
         * Parsing ...
        */

        popcorn::parser::Parser parser(tokens);
        popcorn::parser::AST* ast = parser.parse_ast();

        if (DEBUG)
        {
            printf("\n============ AST ============\n");
            ast->print();
        }

        /**
         * Running ...
        */

        if (DEBUG)
            printf("\n============================\n");
        
        if (DEBUG)
            printf("========== OUTPUT ==========\n\n");

        popcorn::runner::Runner runner;
        runner.run(*ast);

        if (DEBUG)
            printf("\n============================\n\n");

        if (ast != NULL)
            delete ast;
    }
}