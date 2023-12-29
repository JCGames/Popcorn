#include "parser.hpp"
#include <stdexcept>
#include <string>

using namespace ast;

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

/// @brief Moves to the next token. Next token could be whitespace.
void Parser::move_next()
{
    if (_index >= (int)_tokens.size())
        throw std::runtime_error("Cannot read another token because the end of the file has been reached on token type: " + get_token_type_name(_currentToken.type));

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

/// @brief Moves to the first token of the next line.
void Parser::move_next_line()
{
    int nextLine = _currentToken.lineNumber + 1;

    while (_currentToken.lineNumber < nextLine && _currentToken.type != TokenType::END_OF_FILE)
        move_next();
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

/// @brief Ends on the next non-whitespace token.
Expression* Parser::parse_expression()
{
    Expression* expression = new Expression(NULL);
    Statement* left = parse_term();

    while (_currentToken.type == TokenType::ADD || _currentToken.type == TokenType::SUB)
    {
        if (_currentToken.type == TokenType::ADD)
        {
            move_next_non_wspace();
            Statement* right = parse_term();
            left = new AddOperator(left, right);
        }
        else if (_currentToken.type == TokenType::SUB)
        {
            move_next_non_wspace();
            Statement* right = parse_term();
            left = new SubtractOperator(left, right);
        }
    }
    
    expression->root = left;
    return expression;
}
 
Statement* Parser::parse_term()
{
    Statement* left = parse_factor();

    while (_currentToken.type == TokenType::MUL || _currentToken.type == TokenType::DIV)
    {
        if (_currentToken.type == TokenType::MUL)
        {
            move_next_non_wspace();
            Statement* right = parse_factor();
            left = new MultiplyOperator(left, right);
        }
        else if (_currentToken.type == TokenType::DIV)
        {
            move_next_non_wspace();
            Statement* right = parse_factor();
            left = new DivideOperator(left, right);
        }
    }

    return left;
}

Statement* Parser::parse_factor()
{
    Statement* result = NULL;

    if (_currentToken.type == TokenType::OPEN_PARAN)
    {
        move_next_non_wspace();

        result = parse_expression();

        if (_currentToken.type != TokenType::CLOSE_PARAN)
            throw std::runtime_error("Missing a closed paranthesis on line: " + std::to_string(_currentToken.lineNumber));
    }
    else if (_currentToken.type == TokenType::WORD && peek_next_non_wspace().type == TokenType::OPEN_PARAN)
    {
        result = parse_function_call();
    }
    // NUMBER
    else if (_currentToken.type == TokenType::NUM)
    {
        double value;

        try {
            value = std::stod(_currentToken.value);
        } catch (std::exception& e) {
            throw std::runtime_error(e.what());
        }   

        result = new Number(value);
    }
    // STRING
    else if (_currentToken.type == TokenType::STRING)
    {
        result = new String(_currentToken.value);
    }
    // VARIABLE
    else if (_currentToken.type == TokenType::WORD)
    {
        result = new Variable(_currentToken.value);
    }
    // ERROR
    else
    {
        throw std::runtime_error("The token with the value of [Token Type:" + get_token_type_name(_currentToken.type) + ", Token Value |" + _currentToken.value + "|] on line " + std::to_string(_currentToken.lineNumber) + " was not a term.");
    }

    move_next_non_wspace();
    return result;
}

Statement* Parser::get_next_statement()
{
    // move to the next non whitespace token if current token is a whitespace
    if (_currentToken.type == TokenType::WHITESPACE || _currentToken.type == TokenType::END_OF_LINE)
        move_next_non_wspace_pass_eols();

    // COMMENT
    if (_currentToken.type == TokenType::COMMENT)
    {
        move_next_line();
        return NULL;
    }
    // VARIABLE
    else if (_currentToken.type == TokenType::WORD && peek_next_non_wspace().type == TokenType::ASSIGNMENT)
    {
        VariableAssignment* vd = new VariableAssignment(_currentToken.value, NULL);

        move_next_non_wspace();
        move_next_non_wspace();

        Expression* expression = parse_expression();

        if (expression == NULL)
            throw std::runtime_error("Cannot parse expression on line: " + std::to_string(_currentToken.lineNumber));

        vd->expression = expression;

        move_next_line();

        return vd;
    }
    // BLOCK
    else if (_currentToken.type == TokenType::OPEN_BRACKET)
    {
        move_next();
        
        Block* block = get_block();

        move_next_line();

        return block;
    }
    // FUNCTION CALL
    else if (_currentToken.type == TokenType::WORD && peek_next_non_wspace().type == TokenType::OPEN_PARAN)
    {
        FunctionCall* funcCall = parse_function_call();

        move_next_line();

        return funcCall;
    }

    return NULL;
}

FunctionCall* Parser::parse_function_call()
{
    FunctionCall* funcCall = new FunctionCall(_currentToken.value);

    move_next_non_wspace();
    move_next_non_wspace();

    while (_currentToken.type != TokenType::CLOSE_PARAN && !is_end_of_statement())
    {
        Expression* expression = parse_expression();

        funcCall->parameterList.push_back(expression);

        if (_currentToken.type != TokenType::COMMA && _currentToken.type != TokenType::CLOSE_PARAN)
            throw std::runtime_error("Parameters for function call were not passed in correctly on line: " + std::to_string(_currentToken.lineNumber));
        
        if (_currentToken.type == TokenType::CLOSE_PARAN)
            break;

        move_next_non_wspace();
    }

    if (is_end_of_statement())
        throw std::runtime_error("Did not close function call on line: " + std::to_string(_currentToken.lineNumber));
    
    return funcCall;
}

Block* Parser::get_block()
{
    Block* result = new Block();

    int lastIndex = _index - 1;

    while (_currentToken.type != TokenType::CLOSED_BRACKET && _currentToken.type != TokenType::END_OF_FILE)
    {
        if (lastIndex == _index)
            throw std::runtime_error("Unable to parse statement on line: " + std::to_string(_currentToken.lineNumber));

        lastIndex = _index;

        Statement* statement = get_next_statement();

        if (statement != NULL)
            result->statements.push_back(statement);
    }

    if (_currentToken.type != TokenType::CLOSED_BRACKET)
        throw std::runtime_error("Body was not closed correctly on line: " + std::to_string(_currentToken.lineNumber));

    return result;
}

/// @brief Parses a list of tokens and returns its Abstract Syntax Tree
/// @return Abstract Syntax Tree (AST*)
AST* Parser::parse_ast()
{
    AST* result = new AST();
    move_next(); // moves to the first token

    int lastIndex = _index - 1;

    while (_currentToken.type != TokenType::END_OF_FILE)
    {
        if (lastIndex == _index)
            throw std::runtime_error("Unable to parse statement on line: " + std::to_string(_currentToken.lineNumber));

        lastIndex = _index;

        Statement* statement = get_next_statement();

        if (statement != NULL)
            result->statements.push_back(statement);
    }

    // result->block = get_block(TokenType::END_OF_FILE);
    return result;
}