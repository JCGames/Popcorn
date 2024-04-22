#ifndef TOKENIZER
#define TOKENIZER

#include <string>
#include <vector>
#include <iostream>

#include "file.hpp"
#include "diagnostics.hpp"

const std::string EMPTY_STRING = "";

namespace pop 
{
    /**
     * Determines the type of token.
    */
    enum class TokenType : char
    {
        _EOF,
        EOL,
        PLUS,
        SUB,
        MULT,
        DIV,
        MOD,
        WORD,
        COLON,
        ASSIGNMENT,
        NUMBER,
        CHAR,
        ERROR,
        OPEN_PARAN,
        CLOSE_PARAN,
        OPEN_CURL,
        CLOSE_CURL,
        FUNC,
        IF,
        ELSE,
        WHILE,
        TRUE,
        FALSE,
        EQUALS,
        NEQUALS,
        GTHAN,
        LTHAN,
        GTHANE,
        LTHANE,
        STRING,
        COMMA
    };

    /**
     * The data structure for holding a token.
    */
    struct Token
    {
        std::string value;
        TokenType type;
        std::string line;
        unsigned int lineColumn;
        unsigned int lineNumber;

        Token(std::string value,
            TokenType type,
            std::string line,
            unsigned int lineColumn,
            unsigned int lineNumber);

        void print_token();
    };

    /**
     * The data structure responsible for compiling a list of tokens.
    */
    class Tokenizer
    {
        std::vector<Token> tokens;
        File* file;
        Diagnostics* diagnostics;
        std::string currentLine;
        unsigned int lineColumn;
        unsigned int lineNumber;
        bool skipLine; // flag is raised whenever a line should be skipped

        bool eol() const;
        char get();
        char next();
        void parse_lines();
        void parse_line();
        void parse_token();

    public:
        void print_tokens();
        void parse_file(File* file, Diagnostics* diagnostics);
        std::vector<Token>* get_tokens();
    };
}

#endif