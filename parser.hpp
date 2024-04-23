#ifndef PARSER
#define PARSER

#include <string>
#include <vector>
#include <memory>

#include "tokenizer.hpp"

namespace pop
{
    /**
     * The type of statement.
    */
    enum class StatementType : char
    {
        VARIABLE,
        NUMBER,
        CHAR,
        BLOCK,
        ASSIGN,
        ADD_OP,
        SUB_OP,
        MULT_OP,
        DIV_OP,
        MOD_OP,
        ERROR,
        EXP,
        EQUALS_OP,
        NEQUALS_OP,
        GTHANE_OP,
        LTHANE_OP,
        GTHAN_OP,
        LTHAN_OP,
        BOOLEAN,
        IF,
        ELSE,
        ELSEIF,
        WHILE,
        STRING,
        NEGATE_OP,
        FUNCTION,
        FUNCTION_CALL
    };

    /**
     * Converts statement types into strings.
    */
    static std::string statement_type_as_str(const StatementType& type)
    {
        switch (type)
        {
        case StatementType::VARIABLE:
            return "VARIABLE";
        case StatementType::NUMBER:
            return "NUMBER";
        case StatementType::CHAR:
            return "CHARACTER";
        case StatementType::BLOCK:
            return "BLOCK";
        case StatementType::ASSIGN:
            return "ASSIGNMENT";
        case StatementType::ADD_OP:
            return "ADD OPERATOR";
        case StatementType::SUB_OP:
            return "SUBTRACT OPERATOR";
        case StatementType::MULT_OP:
            return "MULTIPLY OPERATOR";
        case StatementType::DIV_OP:
            return "DIVIDE OPERATOR";
        case StatementType::MOD_OP:
            return "MODULUS OPERATOR";
        case StatementType::ERROR:
            return "ERROR";
        case StatementType::EXP:
            return "EXPRESSION";
        case StatementType::EQUALS_OP:
            return "EQUALS";
        case StatementType::NEQUALS_OP:
            return "NOT EQUALS";
        case StatementType::GTHANE_OP:
            return "GREATER THAN OR EQUAL TO";
        case StatementType::LTHANE_OP:
            return "LESS THAN OR EQUAL TO";
        case StatementType::GTHAN_OP:
            return "GREATER THAN";
        case StatementType::LTHAN_OP:
            return "LESS THAN";
        case StatementType::BOOLEAN:
            return "BOOLEAN";
        case StatementType::IF:
            return "IF";
        case StatementType::ELSE:
            return "ELSE";
        case StatementType::ELSEIF:
            return "ELSE IF";
        case StatementType::WHILE:
            return "WHILE";
        case StatementType::NEGATE_OP:
            return "NEGATE OPERATOR";
        case StatementType::FUNCTION:
            return "FUNCTION";
        case StatementType::FUNCTION_CALL:
            return "FUNCTION CALL";
        }

        return "NOT A TYPE";
    }

    #pragma region Data Structures for Statements

    struct StatementInfo { };

    struct SI_Function : public StatementInfo
    {
        std::string functionName;
        std::vector<std::string> parameterNames;
    };

    struct SI_String : public StatementInfo
    {
        std::string value;
    };

    struct SI_Boolean : public StatementInfo
    {
        bool value;
    };

    #pragma endregion

    /**
     * A coded statement.
    */
    struct Statement
    {
        std::shared_ptr<StatementInfo> info;
        StatementType type;
        std::vector<Statement> children;
        std::string line;
        unsigned int lineColumn;
        unsigned int lineNumber;

        Statement();
        Statement(StatementType type, std::string line, unsigned int lineColumn, unsigned int lineNumber);
    };

    /**
     * The parser takes a list of tokens and turns 
     * them into statements.
    */
    class Parser
    {
        std::vector<Token>* tokens;
        Diagnostics* diagnostics;
        Statement root;
        unsigned int currentToken;

        /**
         * Checks to see if we are at the end of the token stream.
        */
        bool eof() const;

        /**
         * Increments the current token index.
        */
        void move_next();

        /**
         * Gets the current token.
         * Returns an ERROR token if failure.
        */
        Token get();

        /**
         * Gets the previous token.
         * Returns an ERROR token if failure.
        */
        Token prev();

        /**
         * Gets the next token.
         * Returns an ERROR token if failure.
        */
        Token next();

        /**
         * Parses a single statement.
        */
        Statement parse_statement();

        Statement parse_function();

        Statement parse_function_call();

        Statement parse_if();

        Statement parse_else();

        Statement parse_expression();

        Statement parse_boolean_operators();

        Statement parse_add_sub();

        Statement parse_mult_div_mod();

        Statement parse_term();

        Statement parse_block();

        /**
         * Prints a single statement.
        */
        void print_statement(const Statement& statement, std::string padding);

    public:
        Parser();

        Statement* get_root();

        /**
         * Parses all of the statements in a given list of tokens.
        */
        void parse_statements(std::vector<Token>* tokens, Diagnostics* diagnostics);

        /**
         * Prints the abstract syntax tree.
        */
        void print_ast();
    };
}

#endif