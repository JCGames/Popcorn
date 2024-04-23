#include "tokenizer.hpp"

using namespace pop;

Token::Token(std::string value,
    TokenType type,
    std::string line,
    unsigned int lineColumn,
    unsigned int lineNumber)
{
    this->value = value;
    this->type = type;
    this->line = line;
    this->lineColumn = lineColumn;
    this->lineNumber = lineNumber;
}

void Token::print_token()
{
    std::cout << "Value: |" << value << "| Type: |" << static_cast<int>(type) << "| Line Column: " << lineColumn << " Line Number: " << lineNumber << " Line: " << line << "\n"; 
}

void Tokenizer::print_tokens()
{
    for (auto& token : tokens)
        token.print_token();
}

void Tokenizer::parse_file(File* file, Diagnostics* diagnostics)
{
    this->file = file;
    this->diagnostics = diagnostics;
    lineColumn = 0;
    lineNumber = 0;
    parse_lines();
}

std::vector<Token>* Tokenizer::get_tokens()
{
    return &tokens;
}

bool Tokenizer::eol() const
{
    return lineColumn >= currentLine.size();
}

char Tokenizer::get()
{
    if (lineColumn >= currentLine.size())
        return '\0';
    return currentLine[lineColumn];
}

char Tokenizer::next()
{
    if (lineColumn + 1 >= currentLine.size())
        return '\0';
    return currentLine[lineColumn + 1];
}

void Tokenizer::parse_lines()
{
    for (lineNumber = 0; lineNumber < file->get_length(); ++lineNumber)
        parse_line();

    tokens.push_back(Token("EOF", TokenType::_EOF, currentLine, lineColumn, lineNumber));    
}

void Tokenizer::parse_line()
{
    currentLine = file->get_line(lineNumber);
    skipLine = false;
    
    for (lineColumn = 0; lineColumn < currentLine.length() && !skipLine; ++lineColumn)
        parse_token();

    tokens.push_back(Token("EOL", TokenType::EOL, currentLine, lineColumn, lineNumber));
}

void Tokenizer::parse_token()
{
    if (get() == '/' && next() == '/')
    {
        skipLine = true;
        return;
    }
    else if (get() == '"')
    {
        std::string value = EMPTY_STRING;

        ++lineColumn;

        while (!eol() && get() != '"')
        {
            value += get();
            ++lineColumn;
        }

        if (get() != '"')
            diagnostics->add_error("Missing \"!", currentLine, lineColumn, lineNumber);

        tokens.push_back(Token(value, TokenType::STRING, currentLine, lineColumn, lineNumber));
    }
    // if is word
    else if (isalpha(get()) || get() == '_')
    {
        std::string value = EMPTY_STRING;

        while (!eol() && (isalnum(get()) || get() == '_'))
        {
            value += get();
            ++lineColumn;
        }

        --lineColumn;

        if (value == "func")
        {
            tokens.push_back(Token(value, TokenType::FUNC, currentLine, lineColumn, lineNumber));
        }
        else if (value == "if")
        {
            tokens.push_back(Token(value, TokenType::IF, currentLine, lineColumn, lineNumber));
        }
        else if (value == "else")
        {
            tokens.push_back(Token(value, TokenType::ELSE, currentLine, lineColumn, lineNumber));
        }
        else if (value == "while")
        {
            tokens.push_back(Token(value, TokenType::WHILE, currentLine, lineColumn, lineNumber));
        }
        else if (value == "true")
        {
            tokens.push_back(Token(value, TokenType::TRUE, currentLine, lineColumn, lineNumber));
        }   
        else if (value == "false")
        {
            tokens.push_back(Token(value, TokenType::FALSE, currentLine, lineColumn, lineNumber));
        }
        else
        {
            tokens.push_back(Token(value, TokenType::WORD, currentLine, lineColumn, lineNumber));
        }
    }
    // if is number
    else if (isdigit(get()) || (get() == '.' && isdigit(next())))
    {
        std::string value = EMPTY_STRING;
        bool hasDecimalPoint = false;

        while (!eol() && (isdigit(get()) || get() == '.'))
        {
            if (hasDecimalPoint && get() == '.')
                diagnostics->add_error("To many decimal points in number!", currentLine, lineColumn, lineNumber);
            else if (get() == '.')
                hasDecimalPoint = true;

            value += get();
            ++lineColumn;
        }

        --lineColumn;

        tokens.push_back(Token(value, TokenType::NUMBER, currentLine, lineColumn, lineNumber));
    }
    else if (get() == '\'')
    {
        ++lineColumn;
        char value = get();
        ++lineColumn;

        if (get() != '\'')
            diagnostics->add_error("Missing closing single quotes!", currentLine, lineColumn, lineNumber);
        
        tokens.push_back(Token(std::string(1, value), TokenType::CHAR, currentLine, lineColumn, lineNumber));
    }
    // two character operators
    else if (get() == '=' && next() == '=')
    {
        tokens.push_back(Token("==", TokenType::EQUALS, currentLine, lineColumn, lineNumber));
        ++lineColumn;
    }
    else if (get() == '!' && next() == '=')
    {
        tokens.push_back(Token("!=", TokenType::NEQUALS, currentLine, lineColumn, lineNumber));
        ++lineColumn;
    }
    else if (get() == '>' && next() == '=')
    {
        tokens.push_back(Token(">=", TokenType::GTHANE, currentLine, lineColumn, lineNumber));
        ++lineColumn;
    }
    else if (get() == '<' && next() == '=')
    {
        tokens.push_back(Token("<=", TokenType::LTHANE, currentLine, lineColumn, lineNumber));
        ++lineColumn;
    }
    // one character operators
    else if (get() == '>')
    {
        tokens.push_back(Token(">", TokenType::GTHAN, currentLine, lineColumn, lineNumber));
    }
    else if (get() == '<')
    {
        tokens.push_back(Token("<", TokenType::LTHAN, currentLine, lineColumn, lineNumber));
    }
    else if (get() == '+')
    {
        tokens.push_back(Token("+", TokenType::PLUS, currentLine, lineColumn, lineNumber));
    }
    else if (get() == '-')
    {
        tokens.push_back(Token("-", TokenType::SUB, currentLine, lineColumn, lineNumber));
    }
    else if (get() == '*')
    {
        tokens.push_back(Token("*", TokenType::MULT, currentLine, lineColumn, lineNumber));
    }
    else if (get() == '/')
    {
        tokens.push_back(Token("/", TokenType::DIV, currentLine, lineColumn, lineNumber));
    }
    else if (get() == '%')
    {
        tokens.push_back(Token("%", TokenType::MOD, currentLine, lineColumn, lineNumber));
    }
    else if (get() == ':')
    {
        tokens.push_back(Token(":", TokenType::COLON, currentLine, lineColumn, lineNumber));
    }
    else if (get() == '=')
    {
        tokens.push_back(Token("=", TokenType::ASSIGNMENT, currentLine, lineColumn, lineNumber));
    }
    else if (get() == '(')
    {
        tokens.push_back(Token("(", TokenType::OPEN_PARAN, currentLine, lineColumn, lineNumber));
    }
    else if (get() == ')')
    {
        tokens.push_back(Token(")", TokenType::CLOSE_PARAN, currentLine, lineColumn, lineNumber));
    }
    else if (get() == '{')
    {
        tokens.push_back(Token("{", TokenType::OPEN_CURL, currentLine, lineColumn, lineNumber));
    }
    else if (get() == '}')
    {
        tokens.push_back(Token("}", TokenType::CLOSE_CURL, currentLine, lineColumn, lineNumber));
    }
    else if (get() == ',')
    {
        tokens.push_back(Token(",", TokenType::COMMA, currentLine, lineColumn, lineNumber));
    }
    else if (!iswspace(get()))
    {
        diagnostics->add_error("You really just popped my pried. Ok ok... so I haven't used all of the characters on the keyboard yet. so what?", currentLine, lineColumn, lineNumber);
    }
}