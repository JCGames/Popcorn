#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include "lib/lexer.hpp"
#include "lib/parser.hpp"
#include "lib/runner.hpp"
#include "lib/console.hpp"
#include "lib/diagnostics.hpp"
#include "lib/any.hpp"

using namespace std;

static bool DEBUG = false;

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
        if (strcmp(args[i], "-d") == 0)
        {
            DEBUG = true;
            DO_ANY_DEBUG = true;
        }
    }

    if (argc > 1)
    {
        #ifdef _WIN32
            Console::init();
        #endif

        // make sure the file is a .pop file
        std::string fileName(args[1]);
        std::string fileExt = get_file_ext(fileName);

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

        lex::Lexer lexer(fileName);
        std::vector<lex::Token> tokens = lexer.get_tokens();

        if (DEBUG)
        {
            printf("========== TOKENS ==========\n");
            for (const auto& token : tokens)
                print_token(token);
        }

        if (DEBUG)
            printf("\n=============================");

        /**
         * Parsing ...
        */

        prs::Parser parser(tokens);
        ast::AST* ast = parser.parse_ast();

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

        run::Runner runner;
        runner.run(*ast);

        if (DEBUG)
            printf("\n============================\n\n");

        if (ast != NULL)
            delete ast;
    }
}