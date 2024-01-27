#ifndef PARSER
#define PARSER

#include <string>
#include <vector>
#include "lexer.hpp"
#include "ast.hpp"

namespace popcorn::parser
{
    #define ERR_TOKEN { TokenType::ERR, "ERROR", -1 }

    class Parser
    {
        popcorn::parser::AST* _ast;

        std::vector<popcorn::lexer::Token> _tokens;
        int _index;
        popcorn::lexer::Token _currentToken;
        bool _isInChildBlock;

        void move_next();
        popcorn::lexer::Token peek_next();
        popcorn::lexer::Token peek_next_non_wspace();
        void move_next_non_wspace();
        void move_next_non_wspace_pass_eols();
        popcorn::lexer::Token peek_next_non_wspace_pass_eols();
        bool is_end_of_statement();

        popcorn::parser::Node* parse_expression();
        popcorn::parser::Node* parse_condition();
        popcorn::parser::Node* parse_equality();
        popcorn::parser::Node* parse_addend();
        popcorn::parser::Node* parse_term();
        popcorn::parser::Node* parse_power();
        popcorn::parser::Node* parse_factor();

        popcorn::parser::Node* parse_if();
        popcorn::parser::Node* parse_else();

        popcorn::parser::Node* parse_next_statement();
        popcorn::parser::Node* parse_function_call();
        popcorn::parser::Node* parse_function();
        popcorn::parser::Node* parse_block();

        public:
            Parser();
            Parser(const std::vector<popcorn::lexer::Token>& tokens);

            popcorn::parser::AST* parse_ast();
    };
}

#endif // PARSER