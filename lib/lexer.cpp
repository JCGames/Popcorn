#include <stdexcept>
#include "lexer.hpp"
#include "diagnostics.hpp"

using namespace popcorn::lexer;
using namespace popcorn::diagnostics;

Lexer::Lexer(const std::string& fileName)
{
    Diagnostics::info = DiagnosticInfo(DiagnosticState::_LEXER);

    _ifs = new std::ifstream(fileName);

    if (!_ifs->is_open())
        Diagnostics::log_error("Could not open \"" + fileName + "\". File may not exist.");

    get_next(); // sets _current
    _currentLineNumber = 1;
    Diagnostics::info.lineNumber = 1;
    _currentColumnNumber = 0;
}

Lexer::~Lexer()
{
    _ifs->close();
    delete _ifs;
}

void Lexer::get_next()
{
    _current = _ifs->get();
    ++_currentColumnNumber;
    Diagnostics::info.columnNumber = _currentColumnNumber;
}

char Lexer::peek_next()
{
    return _ifs->peek();
}

bool Lexer::is_end_of_file()
{
    return _ifs->eof();
}

Token Lexer::get_next_token()
{
    Diagnostics::info.lineNumber = _currentLineNumber;

    std::string value;

    // END OF FILE
    if (is_end_of_file())
    {
        return { TokenType::END_OF_FILE, "", _currentLineNumber, _currentColumnNumber };
    }
    // END OF LINE
    else if (_current == '\n' || _current == ';')
    {
        int value = _currentLineNumber;

        if (_current != ';') 
        {
            ++_currentLineNumber;
            _currentColumnNumber = -1;
        }

        get_next();
        return { TokenType::END_OF_LINE, "", value };
    }
    // WHITESPACE
    else if (iswspace(_current) && _current != '\0' && _current != '\n')
    {
        while (iswspace(_current) && _current != '\0' && _current != '\n')
            get_next();

        return { TokenType::WHITESPACE, "", _currentLineNumber, _currentColumnNumber };
    }
    // NUMBERS
    else if (isdigit(_current) || (_current == '.' && isdigit(peek_next())))
    {
        bool hasDecimalPoint = false;

        while (isdigit(_current) || (_current == '.' && isdigit(peek_next())))
        {
            value += _current;

            if (!hasDecimalPoint && _current == '.')
                hasDecimalPoint = true;
            else if (_current == '.')
                Diagnostics::log_error("To many decimal points.");

            get_next();
        }

        if (_current == '.')
            Diagnostics::log_error("Number seems to but should not end with a decimal point.");

        if (hasDecimalPoint)
            return { TokenType::DOUBLE, value, _currentLineNumber, _currentColumnNumber };
        else
            return { TokenType::INTEGER, value, _currentLineNumber, _currentColumnNumber };
    }
    // WORDS
    else if (isalpha(_current) || _current == '_')
    {
        while (isalpha(_current) || isdigit(_current) || _current == '_')
        {
            value += _current;

            get_next();
        }

        if (value == "true" || value == "false")
            return { TokenType::BOOLEAN, value, _currentLineNumber, _currentColumnNumber };
        else if (value == "if")
            return { TokenType::IF, value, _currentLineNumber, _currentColumnNumber };
        else if (value == "else")
            return { TokenType::ELSE, value, _currentLineNumber, _currentColumnNumber };
        else if (value == "while")
            return { TokenType::WHILE, value, _currentLineNumber, _currentColumnNumber };
        else if (value == "FOR")
            return { TokenType::FOR, value, _currentLineNumber, _currentColumnNumber };
        else if (value == "and")
            return { TokenType::AND_CONDITION, "", _currentLineNumber, _currentColumnNumber };
        else if (value == "or")
            return { TokenType::OR_CONDITION, "", _currentLineNumber, _currentColumnNumber };
        else if (value == "func")
            return { TokenType::FUNCTION, "", _currentLineNumber, _currentColumnNumber };
        else if (value == "return")
            return { TokenType::RETURN, "", _currentLineNumber, _currentColumnNumber };
        else if (value == "break")
            return { TokenType::BREAK, "", _currentLineNumber, _currentColumnNumber };

        return { TokenType::WORD, value, _currentLineNumber, _currentColumnNumber };
    }
    // STRINGS
    else if (_current == '"')
    {
        get_next();

        while (_current != '"' && !is_end_of_file())
        {
            value += _current;
            get_next();
        }

        if (_current != '"')
            Diagnostics::log_error("String did not have a closing quotation.");

        get_next();

        return { TokenType::STRING, value, _currentLineNumber, _currentColumnNumber };
    }
    // TRUTH CONDITIONS
    else if (_current == '&' && peek_next() == '&')
    {
        get_next();
        get_next();
        return { TokenType::AND_CONDITION, "", _currentLineNumber, _currentColumnNumber };
    }
    else if (_current == '|' && peek_next() == '|')
    {
        get_next();
        get_next();
        return { TokenType::OR_CONDITION, "", _currentLineNumber, _currentColumnNumber };
    }
    // TRUTH OPERATORS
    else if (_current == '=' && peek_next() == '=')
    {
        get_next();
        get_next();
        return { TokenType::EQUALS, "", _currentLineNumber, _currentColumnNumber };
    }
    else if (_current == '>' && peek_next() == '=')
    {
        get_next();
        get_next();
        return { TokenType::GREATER_THAN_EQUALS, "", _currentLineNumber, _currentColumnNumber };
    }
    else if (_current == '<' && peek_next() == '=')
    {
        get_next();
        get_next();
        return { TokenType::LESS_THAN_EQUALS, "", _currentLineNumber, _currentColumnNumber };
    }
    else if (_current == '!' && peek_next() == '=')
    {
        get_next();
        get_next();
        return { TokenType::NOT_EQUALS, "", _currentLineNumber, _currentColumnNumber };
    }
    // OPERATORS
    else if (_current == '+' && peek_next() == '+')
    {
        get_next();
        get_next();
        return { TokenType::INCREMENT, "", _currentLineNumber, _currentColumnNumber };
    }
    else if (_current == '-' && peek_next() == '-')
    {
        get_next();
        get_next();
        return { TokenType::DECREMENT, "", _currentLineNumber, _currentColumnNumber };
    }
    // OPERATORS 2
    else if (_current == '^')
    {
        get_next();
        return { TokenType::POWER, "", _currentLineNumber, _currentColumnNumber };
    }
    else if (_current == '%')
    {
        get_next();
        return { TokenType::MODULUS, "", _currentLineNumber, _currentColumnNumber };
    }
    else if (_current == '>')
    {
        get_next();
        return { TokenType::GREATER_THAN, "", _currentLineNumber, _currentColumnNumber };
    }
    else if (_current == '<')
    {
        get_next();
        return { TokenType::LESS_THAN, "", _currentLineNumber, _currentColumnNumber };
    }
    else if (_current == '+')
    {
        get_next();
        return { TokenType::ADD, "", _currentLineNumber, _currentColumnNumber };
    }
    else if (_current == '-')
    {
        get_next();
        return { TokenType::SUB, "", _currentLineNumber, _currentColumnNumber };
    }
    else if (_current == '*')
    {
        get_next();
        return { TokenType::MUL, "", _currentLineNumber, _currentColumnNumber };
    }
    else if (_current == '/' && peek_next() == '/')
    {
        get_next();
        get_next();
        return { TokenType::COMMENT, "", _currentLineNumber, _currentColumnNumber };
    }
    else if (_current == '/')
    {
        get_next();
        return { TokenType::DIV, "", _currentLineNumber, _currentColumnNumber };
    }
    else if (_current == '=')
    {
        get_next();
        return { TokenType::ASSIGNMENT, "", _currentLineNumber, _currentColumnNumber };
    }
    // BODY
    else if (_current == '{')
    {
        get_next();
        return { TokenType::OPEN_BRACKET, "", _currentLineNumber, _currentColumnNumber };
    }
    else if (_current == '}')
    {
        get_next();
        return { TokenType::CLOSED_BRACKET, "", _currentLineNumber, _currentColumnNumber };
    }
    else if (_current == '(')
    {
        get_next();
        return { TokenType::OPEN_PARAN, "", _currentLineNumber, _currentColumnNumber };
    }
    else if (_current == ')')
    {
        get_next();
        return { TokenType::CLOSE_PARAN, "", _currentLineNumber, _currentColumnNumber };
    }
    else if (_current == ',')
    {
        get_next();
        return { TokenType::COMMA, "", _currentLineNumber, _currentColumnNumber };
    }
    else
    {
        value += _current;
        get_next();
    }

    return { TokenType::ERR, value, _currentLineNumber, _currentColumnNumber };
}

std::vector<Token> Lexer::get_tokens()
{
    Token token = get_next_token();

    while (token.type != TokenType::END_OF_FILE)
    {
        _tokens.push_back(token);
        token = get_next_token();
    }

    _tokens.push_back(token);

    return _tokens;
}