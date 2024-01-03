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
        std::vector<lex::Token> _tokens;
        int _index;
        lex::Token _currentToken;

        void move_next();
        lex::Token peek_next();
        lex::Token peek_next_non_wspace();
        void move_next_non_wspace();
        void move_next_line();
        void move_next_non_wspace_pass_eols();
        bool is_end_of_statement();
        bool is_current_token_number();

        ast::Expression* parse_expression();
        ast::Statement* parse_condition();
        ast::Statement* parse_addend();
        ast::Statement* parse_term();
        ast::Statement* parse_equality();
        ast::Statement* parse_factor();

        ast::If* parse_if();
        ast::Else* parse_else();

        ast::Statement* get_next_statement();
        ast::FunctionCall* parse_function_call();
        ast::Block* get_block();

        public:
            Parser();
            Parser(const std::vector<lex::Token>& tokens);

            ast::AST* parse_ast();
    };
}

#endif // PARSER