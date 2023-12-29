#include <stdexcept>
#include "lexer.hpp"

Lexer::Lexer(const std::string& fileName)
{
    _ifs = new std::ifstream(fileName);

    if (!_ifs->is_open())
        throw std::runtime_error("Could not open the requested file!");

    get_next(); // sets _current
    _currentLineNumber = 1;
}

Lexer::~Lexer()
{
    _ifs->close();
    delete _ifs;
}

void Lexer::get_next()
{
    _current = _ifs->get();
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
    std::string value;

    // END OF FILE
    if (is_end_of_file())
    {
        return { TokenType::END_OF_FILE, "", _currentLineNumber };
    }
    // END OF LINE
    else if (_current == '\n' || _current == ';')
    {
        int value = _currentLineNumber;

        if (_current != ';')
            ++_currentLineNumber;

        get_next();
        return { TokenType::END_OF_LINE, "", value };
    }
    // WHITESPACE
    else if (iswspace(_current) && _current != '\0' && _current != '\n')
    {
        while (iswspace(_current) && _current != '\0' && _current != '\n')
            get_next();

        return { TokenType::WHITESPACE, "", _currentLineNumber };
    }
    // NUMBERS
    else if (isdigit(_current) || (_current == '.' && isdigit(peek_next())))
    {
        while (isdigit(_current) || (_current == '.' && isdigit(peek_next())))
        {
            value += _current;

            get_next();
        }

        if (_current == '.')
            throw std::runtime_error("Number may not end with a period!");

        return { TokenType::NUM, value, _currentLineNumber };
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
            return { TokenType::BOOLEAN, value, _currentLineNumber };

        return { TokenType::WORD, value, _currentLineNumber };
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
            throw std::runtime_error("String did not have a closing quotation mark on line: " + _currentLineNumber);

        get_next();

        return { TokenType::STRING, value, _currentLineNumber };
    }
    // OPERATORS
    else if (_current == '+')
    {
        get_next();
        return { TokenType::ADD, "", _currentLineNumber };
    }
    else if (_current == '-')
    {
        get_next();
        return { TokenType::SUB, "", _currentLineNumber };
    }
    else if (_current == '*')
    {
        get_next();
        return { TokenType::MUL, "", _currentLineNumber };
    }
    else if (_current == '/' && peek_next() == '/')
    {
        get_next();
        get_next();
        return { TokenType::COMMENT, "", _currentLineNumber };
    }
    else if (_current == '/')
    {
        get_next();
        return { TokenType::DIV, "", _currentLineNumber };
    }
    else if (_current == '=')
    {
        get_next();
        return { TokenType::ASSIGNMENT, "", _currentLineNumber };
    }
    // BODY
    else if (_current == '{')
    {
        get_next();
        return { TokenType::OPEN_BRACKET, "", _currentLineNumber };
    }
    else if (_current == '}')
    {
        get_next();
        return { TokenType::CLOSED_BRACKET, "", _currentLineNumber };
    }
    else if (_current == '(')
    {
        get_next();
        return { TokenType::OPEN_PARAN, "", _currentLineNumber };
    }
    else if (_current == ')')
    {
        get_next();
        return { TokenType::CLOSE_PARAN, "", _currentLineNumber };
    }
    else if (_current == ',')
    {
        get_next();
        return { TokenType::COMMA, "", _currentLineNumber };
    }
    else
    {
        value += _current;
        get_next();
    }

    return { TokenType::ERR, value, _currentLineNumber };
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