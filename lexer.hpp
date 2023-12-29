#ifndef LEXER
#define LEXER

#include <string>
#include <vector>
#include <fstream>

enum class TokenType 
{
    NUM,
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
};

static std::string get_token_type_name(TokenType type)
{
    switch (type)
    {
        case TokenType::NUM: return "NUMBER";
        case TokenType::MUL: return "MULTIPLICATION";
        case TokenType::DIV: return "DIVISION";
        case TokenType::SUB: return "SUBTRACTION";
        case TokenType::ADD: return "ADDITION";
        case TokenType::END_OF_LINE: return "END OF LINE";
        case TokenType::END_OF_FILE: return "END OF FILE";
        case TokenType::WHITESPACE: return "WHITESPACE";
        case TokenType::ERR: return "ERROR";
        case TokenType::WORD: return "WORD";
        case TokenType::ASSIGNMENT: return "ASSIGNMENT";
        case TokenType::OPEN_BRACKET: return "OPEN BRACKET";
        case TokenType::CLOSED_BRACKET: return "CLOSED BRACKET";
        case TokenType::STRING: return "STRING";
        case TokenType::OPEN_PARAN: return "OPEN PARANTHESIS";
        case TokenType::CLOSE_PARAN: return "CLOSED PARANTHESIS";
        case TokenType::COMMA: return "COMMA";
        case TokenType::COMMENT: return "COMMENT";
        default: return "NONE";
    }
}

struct Token 
{
    TokenType type;
    std::string value;
    int lineNumber;
};

static void print_token(const Token& token)
{
    printf("Token Type: %s,\tToken Value: |%s|,\tLine Number: %i\n", get_token_type_name(token.type).c_str(), token.value.c_str(), token.lineNumber);
}

class Lexer
{
    std::ifstream* _ifs;
    std::vector<Token> _tokens;
    char _current;
    int _currentLineNumber;

    void get_next();
    char peek_next();
    Token get_next_token();
    bool is_end_of_file();

    public:
        Lexer(const std::string& fileName);
        ~Lexer();

        std::vector<Token> get_tokens();
};

#endif // LEXER