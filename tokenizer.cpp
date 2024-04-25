#include "tokenizer.hpp"

using namespace pop;

#pragma region Token

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

/**
 * Self printing method for a token.
*/
void Token::print_token()
{
    std::cout << "Value: |" << value << "| Type: |" << static_cast<int>(type) << "| Line Column: " << lineColumn << " Line Number: " << lineNumber << " Line: " << line << "\n"; 
}

#pragma endregion

#pragma region Public Methods

/**
 * Prints all of the current tokens to the console.
*/
void Tokenizer::print_tokens()
{
    for (auto& token : tokens)
        token.print_token();
}

/**
 * Parses a File.
*/
void Tokenizer::parse_file(File* file, Diagnostics* diagnostics)
{
    this->file = file;
    this->diagnostics = diagnostics;
    lineColumn = 0;
    lineNumber = 0;
    parse_lines();
}

/**
 * Returns the tokens array as a pointer.
*/
std::vector<Token>* Tokenizer::get_tokens()
{
    return &tokens;
}

#pragma endregion

#pragma region Private Methods

/**
 * Returns true if current character is an end of line character.
*/
bool Tokenizer::eol() const
{
    return lineColumn >= currentLine.size();
}

/**
 * Looks at the current character.
*/
char Tokenizer::get()
{
    if (lineColumn >= currentLine.size())
        return '\0';
    return currentLine[lineColumn];
}

/**
 * Looks ahead at the next character.
*/
char Tokenizer::next()
{
    if (lineColumn + 1 >= currentLine.size())
        return '\0';
    return currentLine[lineColumn + 1];
}

/**
 * Parses the lines of a File.
*/
void Tokenizer::parse_lines()
{
    for (lineNumber = 0; lineNumber < file->get_length(); ++lineNumber)
        parse_tokens();

    tokens.push_back(Token("EOF", TokenType::_EOF, currentLine, lineColumn, lineNumber));    
}

/**
 * Extracts tokens from a line.
*/
void Tokenizer::parse_tokens()
{
    currentLine = file->get_line(lineNumber);
    skipLine = false;
    
    for (lineColumn = 0; lineColumn < currentLine.length() && !skipLine; ++lineColumn)
        parse_token();

    tokens.push_back(Token("EOL", TokenType::EOL, currentLine, lineColumn, lineNumber));
}

/**
 * Parses the current character into a token.
*/
void Tokenizer::parse_token()
{
    // SKIP COMMENT
    if (get() == '/' && next() == '/')
    {
        skipLine = true;
        return;
    }
    // PARSE STRING
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
    // PARSE WORD
    else if (isalpha(get()) || get() == '_')
    {
        std::string value = EMPTY_STRING;

        while (!eol() && (isalnum(get()) || get() == '_'))
        {
            value += get();
            ++lineColumn;
        }

        --lineColumn;

        // DETERMINE KEYWORD
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
        else if (value == "ret")
        {
            tokens.push_back(Token(value, TokenType::RETURN, currentLine, lineColumn, lineNumber));
        }
        else if (value == "break")
        {
            tokens.push_back(Token(value, TokenType::BREAK, currentLine, lineColumn, lineNumber));
        }
        else if (value == "cont")
        {
            tokens.push_back(Token(value, TokenType::CONTINUE, currentLine, lineColumn, lineNumber));
        }
        // JUST A WORD
        else
        {
            tokens.push_back(Token(value, TokenType::WORD, currentLine, lineColumn, lineNumber));
        }
    }
    // PARSE NUMBER
    else if (isdigit(get()) || (get() == '.' && isdigit(next())))
    {
        std::string value = EMPTY_STRING;
        bool hasDecimalPoint = false;

        while (!eol() && (isdigit(get()) || get() == '.'))
        {
            if (hasDecimalPoint && get() == '.')
                diagnostics->add_error("To many decimal points.", currentLine, lineColumn, lineNumber);
            else if (get() == '.')
                hasDecimalPoint = true;

            value += get();
            ++lineColumn;
        }

        --lineColumn;

        tokens.push_back(Token(value, TokenType::NUMBER, currentLine, lineColumn, lineNumber));
    }
    // PARSE CHARACTER
    else if (get() == '\'')
    {
        ++lineColumn;
        char value = get();
        ++lineColumn;

        if (get() != '\'')
            diagnostics->add_error("Missing closing single quotes!", currentLine, lineColumn, lineNumber);
        
        tokens.push_back(Token(std::string(1, value), TokenType::CHAR, currentLine, lineColumn, lineNumber));
    }
    // TWO CHARACTER OPERATORS
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
    // ONE CHARACTER OPERATORS
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
    else if (get() == '=')
    {
        tokens.push_back(Token("=", TokenType::ASSIGNMENT, currentLine, lineColumn, lineNumber));
    }
    // SPECIAL CHARACTERS
    else if (get() == ':')
    {
        tokens.push_back(Token(":", TokenType::COLON, currentLine, lineColumn, lineNumber));
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
        diagnostics->add_error("It's really not that hard to get right.", currentLine, lineColumn, lineNumber);
    }
}

#pragma endregion