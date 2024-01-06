#include <stdexcept>
#include <string>
#include "console.hpp"
#include "parser.hpp"

using namespace prs;
using namespace ast;
using namespace lex;

Parser::Parser()
{
    _index = -1;
    _currentToken = ERR_TOKEN;
}

Parser::Parser(const std::vector<Token>& tokens)
{
    _tokens = tokens;
    _index = -1;
    _currentToken = ERR_TOKEN;
}

void Parser::parse_error(std::string message)
{
    std::string result = "\nPARSING ERROR [LINE: " + std::to_string(_currentToken.lineNumber) + "]: " + message;
    
    Console::set_console_color(12);
    printf(result.c_str());
    Console::set_console_to_default();

    delete _ast;
    exit(1);
}

/// @brief Moves to the next token. Next token could be whitespace.
void Parser::move_next()
{
    if (_index >= (int)_tokens.size())
        parse_error("End of file has been reached!");

    ++_index;
    _currentToken = _tokens[_index];
}

/// @brief Looks at the next token. Ignores whitespace.
/// @return The next token.
Token Parser::peek_next()
{
    Token result = ERR_TOKEN;

    if (_index + 1 < (int)_tokens.size())
        result = _tokens[_index + 1];

    return result;
}

/// @brief Looks for tokens that are not whitespace (END_OF_LINEs are not whitespace!).
void Parser::move_next_non_wspace()
{
    move_next();

    while (_currentToken.type == TokenType::WHITESPACE)
        move_next();
}

Token Parser::peek_next_non_wspace()
{
    int prevIndex = _index;

    move_next_non_wspace();

    Token result = _currentToken;
    _index = prevIndex;
    _currentToken = _tokens[prevIndex];
    return result;
}

/// @brief Moves to the first non-whitespace tokens and advances through each line to find it
void Parser::move_next_non_wspace_pass_eols()
{
    move_next();

    while (_currentToken.type == TokenType::WHITESPACE || _currentToken.type == TokenType::END_OF_LINE)
        move_next();
}

/// @brief Checks _currentToken to see if it is an end of statement token.
bool Parser::is_end_of_statement()
{
    return _currentToken.type == TokenType::END_OF_LINE || _currentToken.type == TokenType::END_OF_FILE;
}

/**
 * Expression stuff below
*/

/// @brief Ends on the next non-whitespace token.
Expression* Parser::parse_expression()
{
    return new Expression(parse_condition(), _currentToken.lineNumber);
}

Statement* Parser::parse_condition()
{
    Statement* left = parse_equality();

    while (_currentToken.type == TokenType::AND_CONDITION || _currentToken.type == TokenType::OR_CONDITION)
    {
        if (_currentToken.type == TokenType::AND_CONDITION)
        {
            move_next_non_wspace();
            Statement* right = parse_equality();
            left = new AndCondition(left, right, _currentToken.lineNumber);
        }
        else if (_currentToken.type == TokenType::OR_CONDITION)
        {
            move_next_non_wspace();
            Statement* right = parse_equality();
            left = new OrCondition(left, right, _currentToken.lineNumber);
        }
    }

    return left;
}

Statement* Parser::parse_equality()
{
    Statement* left = parse_addend();

    while (_currentToken.type == TokenType::EQUALS || _currentToken.type == TokenType::NOT_EQUALS ||
            _currentToken.type == TokenType::GREATER_THAN || _currentToken.type == TokenType::LESS_THAN ||
            _currentToken.type == TokenType::GREATER_THAN_EQUALS || _currentToken.type == TokenType::LESS_THAN_EQUALS)
    {
        if (_currentToken.type == TokenType::EQUALS)
        {
            move_next_non_wspace();
            Statement* right = parse_addend();
            left = new EqualsOperator(left, right, _currentToken.lineNumber);
        }
        else if (_currentToken.type == TokenType::NOT_EQUALS)
        {
            move_next_non_wspace();
            Statement* right = parse_addend();
            left = new NotEqualsOperator(left, right, _currentToken.lineNumber);
        }
        else if (_currentToken.type == TokenType::GREATER_THAN)
        {
            move_next_non_wspace();
            Statement* right = parse_addend();
            left = new GreaterThanOperator(left, right, _currentToken.lineNumber);
        }
        else if (_currentToken.type == TokenType::LESS_THAN)
        {
            move_next_non_wspace();
            Statement* right = parse_addend();
            left = new LessThanOperator(left, right, _currentToken.lineNumber);
        }
        else if (_currentToken.type == TokenType::GREATER_THAN_EQUALS)
        {
            move_next_non_wspace();
            Statement* right = parse_addend();
            left = new GreaterThanEqualsOperator(left, right, _currentToken.lineNumber);
        }
        else if (_currentToken.type == TokenType::LESS_THAN_EQUALS)
        {
            move_next_non_wspace();
            Statement* right = parse_addend();
            left = new LessThanEqualsOperator(left, right, _currentToken.lineNumber);
        }
    }

    return left;
}

Statement* Parser::parse_addend()
{
    Statement* left = parse_term();

    while (_currentToken.type == TokenType::ADD || _currentToken.type == TokenType::SUB)
    {
        if (_currentToken.type == TokenType::ADD)
        {
            move_next_non_wspace();
            Statement* right = parse_term();
            left = new AddOperator(left, right, _currentToken.lineNumber);
        }
        else if (_currentToken.type == TokenType::SUB)
        {
            move_next_non_wspace();
            Statement* right = parse_term();
            left = new SubtractOperator(left, right, _currentToken.lineNumber);
        }
    }

    return left;
}

Statement* Parser::parse_term()
{
    Statement* left = parse_power();

    while (_currentToken.type == TokenType::MUL || _currentToken.type == TokenType::DIV || _currentToken.type == TokenType::MODULUS)
    {
        if (_currentToken.type == TokenType::MUL)
        {
            move_next_non_wspace();
            Statement* right = parse_power();
            left = new MultiplyOperator(left, right, _currentToken.lineNumber);
        }
        else if (_currentToken.type == TokenType::DIV)
        {
            move_next_non_wspace();
            Statement* right = parse_power();
            left = new DivideOperator(left, right, _currentToken.lineNumber);
        }
        else if (_currentToken.type == TokenType::MODULUS)
        {
            move_next_non_wspace();
            Statement* right = parse_power();
            left = new ModulusOperator(left, right, _currentToken.lineNumber);
        }
    }

    return left;
}

ast::Statement* Parser::parse_power()
{
    Statement* left = parse_factor();

    while (_currentToken.type == TokenType::POWER)
    {
        if (_currentToken.type == TokenType::POWER)
        {
            move_next_non_wspace();
            Statement* right = parse_factor();
            left = new PowerOperator(left, right, _currentToken.lineNumber);
        }
    }

    return left;
}

Statement* Parser::parse_factor()
{
    Statement* result = NULL;

    // PARSE AN EXPRESSION
    if (_currentToken.type == TokenType::OPEN_PARAN)
    {
        move_next_non_wspace();
        result = parse_expression();

        if (_currentToken.type != TokenType::CLOSE_PARAN)
            throw std::runtime_error("Missing a closed paranthesis on line: " + std::to_string(_currentToken.lineNumber));
    }
    // FUNCTION CALLS
    else if (_currentToken.type == TokenType::WORD && peek_next_non_wspace().type == TokenType::OPEN_PARAN)
    {
        result = parse_function_call();
    }
    // DOUBLE
    else if (_currentToken.type == TokenType::DOUBLE)
    {
        double value;

        try {
            value = std::stod(_currentToken.value);
        } catch (std::exception& e) {
            throw std::runtime_error(e.what());
        }   

        result = new Double(value, _currentToken.lineNumber);
    }
    // INTEGER
    else if (_currentToken.type == TokenType::INTEGER)
    {
        int value;

        try {
            value = std::stoi(_currentToken.value);
        } catch (std::exception& e) {
            throw std::runtime_error(e.what());
        }   

        result = new Integer(value, _currentToken.lineNumber);
    }
    // STRING
    else if (_currentToken.type == TokenType::STRING)
    {
        result = new String(_currentToken.value, _currentToken.lineNumber);
    }
    // VARIABLE
    else if (_currentToken.type == TokenType::WORD)
    {
        result = new Variable(_currentToken.value, _currentToken.lineNumber);
    }
    // NEGATE
    else if (_currentToken.type == TokenType::SUB)
    {
        result = new Negate(parse_factor(), _currentToken.lineNumber);
    }
    // BOOLEAN
    else if (_currentToken.type == TokenType::BOOLEAN)
    {
        result = new Boolean(_currentToken.value == "true" ? true : false, _currentToken.lineNumber);
    }
    // ERROR
    else
    {
        throw std::runtime_error("The token with the value of [Token Type:" + get_token_type_name(_currentToken.type) + ", Token Value |" + _currentToken.value + "|] on line " + std::to_string(_currentToken.lineNumber) + " was not a term.");
    }

    move_next_non_wspace();
    return result;
}

/**
 * Statements
*/

If* Parser::parse_if()
{
    return nullptr;
}

Else* Parser::parse_else()
{
    return nullptr;
}

/**
 * Meat and potatos of parsing below
*/

/// @brief Should always end at the end of a statement line
Statement* Parser::parse_next_statement()
{
    return nullptr;
}

FunctionCall* Parser::parse_function_call()
{
    return nullptr;
}

/// @brief Must start on an OPEN_BRACKET to start an inner block and nothing to start a file block
Block* Parser::parse_block()
{
    Block* block = new Block(_currentToken.lineNumber);

    move_next_non_wspace_pass_eols();

    if (_currentToken.type == TokenType::OPEN_BRACKET)
    {
        while (_currentToken.type != TokenType::CLOSED_BRACKET && _currentToken.type != TokenType::END_OF_FILE)
        {
            // for comments
            if (_currentToken.type == TokenType::COMMENT)
            {
                int nextLine = _currentToken.lineNumber + 1;

                while (_currentToken.lineNumber < nextLine && _currentToken.type != TokenType::END_OF_FILE)
                    move_next();
            }

            Statement* stmt = parse_next_statement();

            if (stmt != nullptr)
                block->statements.push_back(stmt);

            move_next_non_wspace_pass_eols();
        }

        if (_currentToken.type != TokenType::CLOSED_BRACKET)
            parse_error("Missing a closed bracket }.");
    }
    else
    {
        while (_currentToken.type != TokenType::END_OF_FILE)
        {
            // for comments
            if (_currentToken.type == TokenType::COMMENT)
            {
                int nextLine = _currentToken.lineNumber + 1;

                while (_currentToken.lineNumber < nextLine && _currentToken.type != TokenType::END_OF_FILE)
                    move_next();
            }

            Statement* stmt = parse_next_statement();

            if (stmt != nullptr)
                block->statements.push_back(stmt);

            move_next_non_wspace_pass_eols();
        }
    }

    return block;
}

/// @brief Parses a list of tokens and returns its Abstract Syntax Tree
/// @return Abstract Syntax Tree (AST*)
AST* Parser::parse_ast()
{
    _ast = new AST();
    _ast->root = parse_block();
    return _ast;
}