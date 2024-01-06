#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include "lib/lexer.hpp"
#include "lib/parser.hpp"
#include "lib/runner.hpp"
#include "lib/console.hpp"

using namespace std;

static bool DEBUG = false;

int main(int argc, char** args)
{
    for (int i = 0; i < argc; ++i)
    {
        if (strcmp(args[i], "-d") == 0)
            DEBUG = true;
    }

    if (argc > 1)
    {
        Console::init();

        /**
         * Lexing ...
        */

        lex::Lexer lexer(args[1]);
        std::vector<lex::Token> tokens = lexer.get_tokens();

        if (DEBUG)
        {
            printf("========== TOKENS ==========\n");
            for (const auto& token : tokens)
                print_token(token);
        }

        /**
         * Parsing ...
        */

        prs::Parser parser(tokens);
        ast::AST* ast = parser.parse_ast();

        if (DEBUG)
        {
            printf("\n========== AST ==========\n");
            ast->print();
        }

        /**
         * Running ...
        */
        
        if (DEBUG)
            printf("\n========== OUTPUT ==========\n\n");

        run::Runner runner;
        runner.run(*ast);

        if (DEBUG)
            printf("\n============================\n\n");

        if (DEBUG)
        {
            runner.dump_runner();
        }

        if (ast != NULL)
            delete ast;
    }
}