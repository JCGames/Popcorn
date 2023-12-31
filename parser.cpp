#include <stdexcept>
#include <string>
#include "parser.hpp"

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
    if (is_end_of_statement())
        throw std::runtime_error("Tried to finish reading a statement that was not complete on line: " + std::to_string(_currentToken.lineNumber));

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
    bool hasFoundEndOfLineToken = false;

    while (_currentToken.lineNumber < nextLine && _currentToken.type != TokenType::END_OF_FILE)
    {
        move_next();
        
        if (_currentToken.type == TokenType::END_OF_LINE)
            hasFoundEndOfLineToken = true;

        if (hasFoundEndOfLineToken && _currentToken.type != TokenType::WHITESPACE && _currentToken.type != TokenType::END_OF_LINE)
            break;
    }
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

bool Parser::is_current_token_number()
{
    return _currentToken.type == TokenType::DOUBLE || _currentToken.type == TokenType::INTEGER;
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
    Statement* left = parse_factor();

    while (_currentToken.type == TokenType::MUL || _currentToken.type == TokenType::DIV || _currentToken.type == TokenType::MODULUS)
    {
        if (_currentToken.type == TokenType::MUL)
        {
            move_next_non_wspace();
            Statement* right = parse_factor();
            left = new MultiplyOperator(left, right, _currentToken.lineNumber);
        }
        else if (_currentToken.type == TokenType::DIV)
        {
            move_next_non_wspace();
            Statement* right = parse_factor();
            left = new DivideOperator(left, right, _currentToken.lineNumber);
        }
        else if (_currentToken.type == TokenType::MODULUS)
        {
            move_next_non_wspace();
            Statement* right = parse_factor();
            left = new ModulusOperator(left, right, _currentToken.lineNumber);
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
    move_next_non_wspace();

    Expression* condition = parse_expression();

    if (is_end_of_statement())
        move_next_non_wspace_pass_eols();

    if (_currentToken.type == TokenType::OPEN_BRACKET)
    {
        Block* body = get_block();
        move_next_non_wspace_pass_eols();

        If* result = new If(condition, body, _currentToken.lineNumber);

        // ELSE IF CASE
        if (_currentToken.type == TokenType::ELSE && peek_next_non_wspace().type == TokenType::IF)
        {
            move_next_non_wspace();
            result->elseOrIf = parse_if();
        }
        // ELSE CASE
        else if (_currentToken.type == TokenType::ELSE)
        {
            result->elseOrIf = parse_else();
        }

        return result;
    }
    else throw std::runtime_error("Could not find body for if statement on line: " + std::to_string(_currentToken.lineNumber));
}

Else* Parser::parse_else()
{
    move_next_non_wspace_pass_eols();

    if (_currentToken.type == TokenType::OPEN_BRACKET)
    {
        Block* body = get_block();
        move_next_non_wspace_pass_eols();

        return new Else(body, _currentToken.lineNumber);
    }
    else throw std::runtime_error("Could not find body for else statement on line: " + std::to_string(_currentToken.lineNumber));
}

/**
 * Meat and potatos of parsing below
*/

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
        VariableAssignment* vd = new VariableAssignment(_currentToken.value, NULL, _currentToken.lineNumber);

        move_next_non_wspace();
        move_next_non_wspace();

        Expression* expression = parse_expression();

        if (expression == NULL)
            throw std::runtime_error("Cannot parse expression on line: " + std::to_string(_currentToken.lineNumber));

        vd->expression = expression;

        move_next_non_wspace_pass_eols();

        return vd;
    }
    // BLOCK
    else if (_currentToken.type == TokenType::OPEN_BRACKET)
    {
        move_next();
        
        Block* block = get_block();

        move_next_non_wspace_pass_eols();

        return block;
    }
    // FUNCTION CALL
    else if (_currentToken.type == TokenType::WORD && peek_next_non_wspace().type == TokenType::OPEN_PARAN)
    {
        FunctionCall* funcCall = parse_function_call();

        move_next_non_wspace_pass_eols();

        return funcCall;
    }
    // IF STATEMENT
    else if (_currentToken.type == TokenType::IF)
    {
        return parse_if();
    }
    // WHILE STATEMENT
    else if (_currentToken.type == TokenType::WHILE)
    {
        move_next_non_wspace();

        Expression* condition = parse_expression();

        if (is_end_of_statement())
            move_next_non_wspace_pass_eols();

        if (_currentToken.type == TokenType::OPEN_BRACKET)
        {
            Block* body = get_block();
            move_next_non_wspace_pass_eols();

            return new While(condition, body, _currentToken.lineNumber);
        }
        
        throw std::runtime_error("Could not find body for while statement on line: " + std::to_string(_currentToken.lineNumber));
    }
    else if (_currentToken.type == TokenType::WORD && peek_next_non_wspace().type == TokenType::INCREMENT)
    {
        std::string variableName = _currentToken.value;

        move_next_non_wspace();
        move_next_non_wspace_pass_eols();

        return new VariableAssignment(variableName, new Expression(new AddOperator(new Variable(variableName, _currentToken.lineNumber), new Integer(1, _currentToken.lineNumber), _currentToken.lineNumber), _currentToken.lineNumber), _currentToken.lineNumber);
    }
    else if (_currentToken.type == TokenType::WORD && peek_next_non_wspace().type == TokenType::DECREMENT)
    {
        std::string variableName = _currentToken.value;

        move_next_non_wspace();
        move_next_non_wspace_pass_eols();

        return new VariableAssignment(variableName, new Expression(new SubtractOperator(new Variable(variableName, _currentToken.lineNumber), new Integer(1, _currentToken.lineNumber), _currentToken.lineNumber), _currentToken.lineNumber), _currentToken.lineNumber);
    }

    return NULL;
}

FunctionCall* Parser::parse_function_call()
{
    FunctionCall* funcCall = new FunctionCall(_currentToken.value, _currentToken.lineNumber);

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
    Block* result = new Block(_currentToken.lineNumber);

    if (_currentToken.type == TokenType::OPEN_BRACKET)
        move_next();
    
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
        throw std::runtime_error("Body was not closed correctly on line: " + std::to_string(_currentToken.lineNumber) + "\nToken found was: " + get_token_type_name(_currentToken.type).c_str());

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