#ifndef PARSER
#define PARSER

#include <string>
#include <vector>
#include "lexer.hpp"
#include "ast.hpp"

namespace prs
{
    #define ERR_TOKEN { TokenType::ERR, "ERROR", -1 }

    class Parser
    {
        ast::AST* _ast;

        std::vector<lex::Token> _tokens;
        int _index;
        lex::Token _currentToken;
        bool _isInChildBlock;

        

        void move_next();
        lex::Token peek_next();
        lex::Token peek_next_non_wspace();
        void move_next_non_wspace();
        void move_next_non_wspace_pass_eols();
        lex::Token peek_next_non_wspace_pass_eols();
        bool is_end_of_statement();

        ast::Node* parse_expression();
        ast::Node* parse_condition();
        ast::Node* parse_equality();
        ast::Node* parse_addend();
        ast::Node* parse_term();
        ast::Node* parse_power();
        ast::Node* parse_factor();

        ast::Node* parse_if();
        ast::Node* parse_else();

        ast::Node* parse_next_statement();
        ast::Node* parse_function_call();
        ast::Node* parse_block();

        public:
            Parser();
            Parser(const std::vector<lex::Token>& tokens);

            ast::AST* parse_ast();
    };
}

#endif // PARSER