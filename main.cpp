#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include "lexer.hpp"
#include "parser.hpp"
#include "runner.hpp"

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
        Lexer lexer(args[1]);
        std::vector<Token> tokens = lexer.get_tokens();

        if (DEBUG)
        {
            printf("========== TOKENS ==========\n");

            for (const auto& token : tokens)
                print_token(token);
        }

        Parser parser(tokens);
        AST* ast = parser.parse_ast();

        if (DEBUG)
        {
            printf("\n========== AST ==========\n");
            ast->print();
        }
        
        Runner runner;
        runner.run(*ast);

        if (ast != NULL)
            delete ast;
    }
}