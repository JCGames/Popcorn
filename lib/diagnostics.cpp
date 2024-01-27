#include "diagnostics.hpp"

#include "../includes/console.hpp"

using namespace popcorn::diagnostics;

DiagnosticInfo Diagnostics::info = DiagnosticInfo();

void Diagnostics::log_error(std::string message)
{
    #ifdef _WIN32
        helpers::Console::set_console_color(12);
    #elif __linux__
        printf("\x1B[31m");
    #endif
    
    switch (info.state)
    {
        case DiagnosticState::_LEXER:
            printf("Lexing !: [LINE: %i, COLUMN: %i]: %s", info.lineNumber, info.columnNumber, message.c_str());
            break;
        case DiagnosticState::_PARSER:
            printf("Parsing !: [LINE: %i, COLUMN: %i] [TOKEN: (%s, \"%s\")]: %s", info.lineNumber, info.columnNumber, get_token_type_name(info.currentToken.type).c_str(), info.currentToken.value.c_str(), message.c_str());
            break;
        case DiagnosticState::_RUNNER:
            printf("Runtime !: [LINE: %i]: %s", info.lineNumber, message.c_str());
            break;
    }

    #ifdef _WIN32
        helpers::Console::set_console_to_default();
    #elif __linux__
        printf("\033[0m");
    #endif

    printf("\n");

    exit(1);
}

void Diagnostics::log_warning(std::string message)
{
    #ifdef _WIN32
        helpers::Console::set_console_color(14);
    #elif __linux__
        printf("\x1B[31m");
    #endif

    switch (info.state)
    {
        case DiagnosticState::_LEXER:
            printf("Lexing ?: [LINE: %i, COLUMN: %i]: %s", info.lineNumber, info.columnNumber, message.c_str());
            break;
        case DiagnosticState::_PARSER:
            printf("Parsing ?: [LINE: %i, COLUMN: %i] [TOKEN: (%s, \"%s\")]: %s", info.lineNumber, info.columnNumber, get_token_type_name(info.currentToken.type).c_str(), info.currentToken.value.c_str(), message.c_str());
            break;
        case DiagnosticState::_RUNNER:
            printf("Runtime ?: [LINE: %i]: %s", info.lineNumber, message.c_str());
            break;
    }    

    #ifdef _WIN32
        helpers::Console::set_console_to_default();
    #elif __linux__
        printf("\033[0m");
    #endif

    printf("\n");
}

void Diagnostics::log_information(std::string message)
{
    switch (info.state)
    {
        case DiagnosticState::_LEXER:
            printf("Lexing: [LINE: %i, COLUMN: %i]: %s", info.lineNumber, info.columnNumber, message.c_str());
            break;
        case DiagnosticState::_PARSER:
            printf("Parsing: [LINE: %i, COLUMN: %i] [TOKEN: (%s, \"%s\")]: %s", info.lineNumber, info.columnNumber, get_token_type_name(info.currentToken.type).c_str(), info.currentToken.value.c_str(), message.c_str());
            break;
        case DiagnosticState::_RUNNER:
            printf("Runtime: [LINE: %i]: %s", info.lineNumber, message.c_str());
            break;
    }    

    printf("\n");
}