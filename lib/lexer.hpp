#ifndef LEXER
#define LEXER

#include <string>
#include <vector>
#include <fstream>

namespace popcorn::lexer
{
    enum class TokenType 
    {
        DOUBLE,
        INTEGER,
        MUL,
        DIV,
        SUB,
        ADD,
        END_OF_LINE,
        END_OF_FILE,
        WHITESPACE,
        ERR,
        WORD,
        ASSIGNMENT,
        OPEN_BRACKET,
        CLOSED_BRACKET,
        STRING,
        OPEN_PARAN,
        CLOSE_PARAN,
        COMMA,
        COMMENT,
        BOOLEAN,
        EQUALS,
        NOT_EQUALS,
        GREATER_THAN,
        LESS_THAN,
        GREATER_THAN_EQUALS,
        LESS_THAN_EQUALS,
        IF,
        ELSE,
        WHILE,
        FOR,
        AND_CONDITION,
        OR_CONDITION,
        MODULUS,
        INCREMENT,
        DECREMENT,
        FUNCTION,
        RETURN,
        POWER,
        BREAK,
        DOT,
    };

    std::string get_token_type_name(TokenType type);

    struct Token
    {
        TokenType type;
        std::string value;
        int lineNumber;
        int columnNumber;
    };

    void print_token(const Token& token);

    class Lexer
    {
        std::ifstream* _ifs;
        std::vector<Token> _tokens;
        char _current;
        int _currentLineNumber;
        int _currentColumnNumber;

        void get_next();
        char peek_next();
        Token get_next_token();
        bool is_end_of_file();

        public:
            Lexer(const std::string& fileName);
            ~Lexer();

            std::vector<Token> get_tokens();
    };
}

#endif // LEXER