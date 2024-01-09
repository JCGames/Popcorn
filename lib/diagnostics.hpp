#ifndef ERROR
#define ERROR

#include <string>
#include "lexer.hpp"

enum class DiagnosticState 
{
    _LEXER,
    _PARSER,
    _RUNNER,
};

struct DiagnosticInfo
{
    DiagnosticState state;
    int lineNumber;
    int columnNumber;
    lex::Token currentToken;

    DiagnosticInfo() 
    {
        state = DiagnosticState::_LEXER;
        lineNumber = 0;
        columnNumber = 0;
        currentToken = lex::Token();
    }

    DiagnosticInfo(DiagnosticState state)
    {
        this->state = state;
        lineNumber = 0;
        columnNumber = 0;
        currentToken = lex::Token();
    }
};

class Diagnostics 
{
    public:
        static DiagnosticInfo info;

        static void log_error(std::string message);
        static void log_warning(std::string message);
        static void log_information(std::string message);
};

#endif // ERROR