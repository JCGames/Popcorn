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
    _isInChildBlock = false;
}

void Parser::error(std::string message)
{
    printf("\n");

    #ifdef _WIN32
        Console::set_console_color(12);
    #elif __linux__
        printf("\x1B[31m");
    #endif
    
    printf("[LINE: %i, COLUMN: %i] [TOKEN: (%s, \"%s\")]: %s", _currentToken.lineNumber, _currentToken.columnNumber, get_token_type_name(_currentToken.type).c_str(), _currentToken.value.c_str(), message.c_str());

    #ifdef _WIN32
        Console::set_console_to_default();
    #elif __linux__
        printf("\033[0m");
    #endif

    printf("\n");

    delete _ast;
    exit(1);
}

/// @brief Moves to the next token. Next token could be whitespace.
void Parser::move_next()
{
    if (_index + 1 >= (int)_tokens.size())
        error("End of file has been reached!");

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
    return _currentToken.type == TokenType::END_OF_LINE || 
        _currentToken.type == TokenType::END_OF_FILE || 
        (_currentToken.type == TokenType::CLOSED_BRACKET && _isInChildBlock);
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

Statement* Parser::parse_power()
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
        error("Not a term."); 
    }

    move_next_non_wspace();
    return result;
}

/**
 * Statements
*/

If* Parser::parse_if()
{
    If* _if = new If(nullptr, nullptr, _currentToken.lineNumber);

    move_next_non_wspace();

    _if->condition = parse_expression();

    if (is_end_of_statement() && _currentToken.type != TokenType::CLOSED_BRACKET)
        move_next_non_wspace_pass_eols();

    if (_currentToken.type != TokenType::OPEN_BRACKET)
        error("If statement did not have an open bracket.");

    _if->body = parse_block();

    if (peek_next_non_wspace().type == TokenType::ELSE)
    {
        move_next_non_wspace();

        if (peek_next_non_wspace().type == TokenType::IF)
        {
            move_next_non_wspace();

            _if->elseOrIf = parse_if();
        }
        else
        {
            _if->elseOrIf = parse_else();
        }
    }

    return _if;
}

Else* Parser::parse_else()
{
    Else* _else = new Else(nullptr, _currentToken.lineNumber);

    move_next_non_wspace();

    if (is_end_of_statement() && _currentToken.type != TokenType::CLOSED_BRACKET)
        move_next_non_wspace_pass_eols();

    if (_currentToken.type != TokenType::OPEN_BRACKET)
        error("Else statement did not have an open bracket.");

    _else->body = parse_block();

    return _else;
}

/**
 * Meat and potatos of parsing below
*/

/// @brief Should always end at the end of its own statement. Not the next!
Statement* Parser::parse_next_statement()
{
    // printf("Reading a statement beginning with: %s\n", get_token_type_name(_currentToken.type).c_str());

    // COMMENTS
    if (_currentToken.type == TokenType::COMMENT)
    {
        int nextLine = _currentToken.lineNumber + 1;

        while (_currentToken.lineNumber < nextLine && _currentToken.type != TokenType::END_OF_FILE)
            move_next();

        return nullptr;
    }
    // VARIABLE ASSIGNMENT
    else if (_currentToken.type == TokenType::WORD && peek_next_non_wspace().type == TokenType::ASSIGNMENT)
    {
        VariableAssignment* va = new VariableAssignment(_currentToken.value, nullptr, _currentToken.lineNumber);

        move_next_non_wspace();
        move_next_non_wspace();

        va->expression = parse_expression();

        if (!is_end_of_statement())
            error("Incorrect variable assignment.");
        
        return va;
    }
    // FUNCITON CALLS
    else if (_currentToken.type == TokenType::WORD && peek_next_non_wspace().type == TokenType::OPEN_PARAN)
    {
        FunctionCall* fc = parse_function_call();
        
        move_next_non_wspace();

        if (!is_end_of_statement())
            error("Incorrect function call " + fc->functionName + ".");

        return fc;
    }
    // RETURN
    else if (_currentToken.type == TokenType::RETURN)
    {
        Return* r = new Return(nullptr, _currentToken.lineNumber);

        move_next_non_wspace();

        r->expression = parse_expression();

        if (!is_end_of_statement())
            error("Return not formatted properly.");

        return r;
    }
    // WHILE
    else if (_currentToken.type == TokenType::WHILE)
    {
        While* w = new While(nullptr, nullptr, _currentToken.lineNumber);

        move_next_non_wspace();

        w->condition = parse_expression();

        if (is_end_of_statement() && _currentToken.type != TokenType::CLOSED_BRACKET)
            move_next_non_wspace_pass_eols();

        if (_currentToken.type != TokenType::OPEN_BRACKET)
            error("While statement did not have an open bracket.");

        w->body = parse_block();

        return w;
    }
    // If
    else if (_currentToken.type == TokenType::IF)
    {
        return parse_if();
    }
    // INCREMENT
    else if (_currentToken.type == TokenType::WORD && peek_next_non_wspace().type == TokenType::INCREMENT)
    {
        std::string variableName = _currentToken.value;
        move_next_non_wspace();
        move_next_non_wspace();

        if (!is_end_of_statement())
            error("Variable increment was not properly formatted.");

        return new VariableAssignment(variableName, new Expression(new AddOperator(new Variable(variableName, _currentToken.lineNumber), new Integer(1, _currentToken.lineNumber), _currentToken.lineNumber), _currentToken.lineNumber), _currentToken.lineNumber);
    }
    // DECREMENT
    else if (_currentToken.type == TokenType::WORD && peek_next_non_wspace().type == TokenType::DECREMENT)
    {
        std::string variableName = _currentToken.value;
        move_next_non_wspace();
        move_next_non_wspace();

        if (!is_end_of_statement())
            error("Variable decrement was not properly formatted.");

        return new VariableAssignment(variableName, new Expression(new SubtractOperator(new Variable(variableName, _currentToken.lineNumber), new Integer(1, _currentToken.lineNumber), _currentToken.lineNumber), _currentToken.lineNumber), _currentToken.lineNumber);
    }

    error("Could not read statement.");

    return nullptr;
}

FunctionCall* Parser::parse_function_call()
{
    FunctionCall* fc = new FunctionCall(_currentToken.value, _currentToken.lineNumber);

    move_next_non_wspace();
    move_next_non_wspace();

    if (_currentToken.type != TokenType::CLOSE_PARAN)
    {
        while (_currentToken.type != TokenType::CLOSE_PARAN && _currentToken.type != TokenType::END_OF_FILE)
        {
            Expression* e = parse_expression();

            if (e != nullptr)
                fc->parameterList.push_back(e);

            if (_currentToken.type == TokenType::COMMA && peek_next_non_wspace().type != TokenType::CLOSE_PARAN)
            {
                move_next_non_wspace();
                continue;
            }

            break;
        }

        if (_currentToken.type == TokenType::COMMA)
            error("Function call should not be ended with a comma.");

        if (_currentToken.type != TokenType::CLOSE_PARAN)
            error("Missing a closed parenthesis ).");
    }

    return fc;
}

/// @brief Must start on an OPEN_BRACKET to start an inner block and nothing to start a file block
Block* Parser::parse_block()
{
    Block* block = new Block(_currentToken.lineNumber);

    if (_currentToken.type == TokenType::OPEN_BRACKET)
    {
        _isInChildBlock = true;

        move_next_non_wspace_pass_eols();

        while (_currentToken.type != TokenType::CLOSED_BRACKET && _currentToken.type != TokenType::END_OF_FILE)
        {
            Statement* stmt = parse_next_statement();

            if (stmt != nullptr)
                block->statements.push_back(stmt);

            move_next_non_wspace_pass_eols();
        }

        if (_currentToken.type != TokenType::CLOSED_BRACKET)
            error("Missing a closed bracket }.");

        _isInChildBlock = false;
    }
    else
    {
        if (_currentToken.type == TokenType::WHITESPACE)
            move_next_non_wspace_pass_eols();

        while (_currentToken.type != TokenType::END_OF_FILE)
        {
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
    move_next();
    _ast->root = parse_block(); 
    return _ast;
}