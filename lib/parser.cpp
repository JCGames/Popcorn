#include <stdexcept>
#include <string>
#include "console.hpp"
#include "parser.hpp"
#include "diagnostics.hpp"

using namespace prs;
using namespace ast;
using namespace lex;

Parser::Parser()
{
    _index = -1;
    _currentToken = ERR_TOKEN;

    Diagnostics::info = DiagnosticInfo(DiagnosticState::_PARSER);
}

Parser::Parser(const std::vector<Token>& tokens)
{
    _tokens = tokens;
    _index = -1;
    _currentToken = ERR_TOKEN;
    _isInChildBlock = false;

    Diagnostics::info = DiagnosticInfo(DiagnosticState::_PARSER);
}

/// @brief Moves to the next token. Next token could be whitespace.
void Parser::move_next()
{
    if (_index + 1 >= (int)_tokens.size())
        return;

    ++_index;
    _currentToken = _tokens[_index];
    Diagnostics::info.lineNumber = _currentToken.lineNumber;
    Diagnostics::info.columnNumber = _currentToken.columnNumber;
    Diagnostics::info.currentToken = _currentToken;
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

Token Parser::peek_next_non_wspace_pass_eols()
{
    int prevIndex = _index;

    move_next_non_wspace_pass_eols();

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
Node* Parser::parse_expression()
{
    return new Node(NodeType::EXPRESSION, _currentToken.lineNumber, { parse_condition() });
}

Node* Parser::parse_condition()
{
    Node* left = parse_equality();

    while (_currentToken.type == TokenType::AND_CONDITION || _currentToken.type == TokenType::OR_CONDITION)
    {
        if (_currentToken.type == TokenType::AND_CONDITION)
        {
            move_next_non_wspace();
            Node* right = parse_equality();
            left = new Node(NodeType::AND_CONDITION, _currentToken.lineNumber, { left, right });
        }
        else if (_currentToken.type == TokenType::OR_CONDITION)
        {
            move_next_non_wspace();
            Node* right = parse_equality();
            left = new Node(NodeType::OR_CONDITION, _currentToken.lineNumber, { left, right });
        }
    }

    return left;
}

Node* Parser::parse_equality()
{
    Node* left = parse_addend();

    while (_currentToken.type == TokenType::EQUALS || _currentToken.type == TokenType::NOT_EQUALS ||
            _currentToken.type == TokenType::GREATER_THAN || _currentToken.type == TokenType::LESS_THAN ||
            _currentToken.type == TokenType::GREATER_THAN_EQUALS || _currentToken.type == TokenType::LESS_THAN_EQUALS)
    {
        if (_currentToken.type == TokenType::EQUALS)
        {
            move_next_non_wspace();
            Node* right = parse_addend();
            left = new Node(NodeType::EQUALS_OPERATOR, _currentToken.lineNumber, { left, right });
        }
        else if (_currentToken.type == TokenType::NOT_EQUALS)
        {
            move_next_non_wspace();
            Node* right = parse_addend();
            left = new Node(NodeType::NOT_EQUALS_OPERATOR, _currentToken.lineNumber, { left, right });
        }
        else if (_currentToken.type == TokenType::GREATER_THAN)
        {
            move_next_non_wspace();
            Node* right = parse_addend();
            left = new Node(NodeType::GREATER_THAN_OPERATOR, _currentToken.lineNumber, { left, right });
        }
        else if (_currentToken.type == TokenType::LESS_THAN)
        {
            move_next_non_wspace();
            Node* right = parse_addend();
            left = new Node(NodeType::LESS_THAN_OPERATOR, _currentToken.lineNumber, { left, right });
        }
        else if (_currentToken.type == TokenType::GREATER_THAN_EQUALS)
        {
            move_next_non_wspace();
            Node* right = parse_addend();
            left = new Node(NodeType::GREATER_THAN_EQUALS_OPERATOR, _currentToken.lineNumber, { left, right });
        }
        else if (_currentToken.type == TokenType::LESS_THAN_EQUALS)
        {
            move_next_non_wspace();
            Node* right = parse_addend();
            left = new Node(NodeType::LESS_THAN_EQUALS_OPERATOR, _currentToken.lineNumber, { left, right });
        }
    }

    return left;
}

Node* Parser::parse_addend()
{
    Node* left = parse_term();

    while (_currentToken.type == TokenType::ADD || _currentToken.type == TokenType::SUB)
    {
        if (_currentToken.type == TokenType::ADD)
        {
            move_next_non_wspace();
            Node* right = parse_term();
            left = new Node(NodeType::ADD_OPERATOR, _currentToken.lineNumber, { left, right });
        }
        else if (_currentToken.type == TokenType::SUB)
        {
            move_next_non_wspace();
            Node* right = parse_term();
            left = new Node(NodeType::SUB_OPERATOR, _currentToken.lineNumber, { left, right });
        }
    }

    return left;
}

Node* Parser::parse_term()
{
    Node* left = parse_power();

    while (_currentToken.type == TokenType::MUL || _currentToken.type == TokenType::DIV || _currentToken.type == TokenType::MODULUS)
    {
        if (_currentToken.type == TokenType::MUL)
        {
            move_next_non_wspace();
            Node* right = parse_power();
            left = new Node(NodeType::MUL_OPERATOR, _currentToken.lineNumber, { left, right });
        }
        else if (_currentToken.type == TokenType::DIV)
        {
            move_next_non_wspace();
            Node* right = parse_power();
            left = new Node(NodeType::DIV_OPERATOR, _currentToken.lineNumber, { left, right });
        }
        else if (_currentToken.type == TokenType::MODULUS)
        {
            move_next_non_wspace();
            Node* right = parse_power();
            left = new Node(NodeType::MODULUS_OPERATOR, _currentToken.lineNumber, { left, right });
        }
    }

    return left;
}

Node* Parser::parse_power()
{
    Node* left = parse_factor();

    while (_currentToken.type == TokenType::POWER)
    {
        if (_currentToken.type == TokenType::POWER)
        {
            move_next_non_wspace();
            Node* right = parse_factor();
            left = new Node(NodeType::POWER_OPERATOR, _currentToken.lineNumber, { left, right });
        }
    }

    return left;
}

Node* Parser::parse_factor()
{
    Node* result = NULL;

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
            Diagnostics::log_error("Could not cast string to double.");
        }   

        result = new Node(NodeType::DOUBLE, new double(value), _currentToken.lineNumber);
    }
    // INTEGER
    else if (_currentToken.type == TokenType::INTEGER)
    {
        int value;

        try {
            value = std::stoi(_currentToken.value);
        } catch (std::exception& e) {
            Diagnostics::log_error("Could not cast string to integer.");
        }   

        result = new Node(NodeType::INTEGER, new int(value), _currentToken.lineNumber);
    }
    // STRING
    else if (_currentToken.type == TokenType::STRING)
    {
        result = new Node(NodeType::STRING, new std::string(_currentToken.value), _currentToken.lineNumber);
    }
    // VARIABLE
    else if (_currentToken.type == TokenType::WORD)
    {
        result = new Node(NodeType::VARIABLE, new std::string(_currentToken.value), _currentToken.lineNumber);
    }
    // NEGATE
    else if (_currentToken.type == TokenType::SUB)
    {
        move_next_non_wspace();
        result = new Node(NodeType::NEGATE, _currentToken.lineNumber, { parse_expression() });
    }
    // BOOLEAN
    else if (_currentToken.type == TokenType::BOOLEAN)
    {
        result = new Node(NodeType::BOOLEAN, new bool(_currentToken.value == "true" ? true : false), _currentToken.lineNumber);
    }
    // ERROR
    else
    {
        Diagnostics::log_error("Not a term.");
    }

    move_next_non_wspace();
    return result;
}

/**
 * Statements
*/

Node* Parser::parse_if()
{
    auto _if = new Node(NodeType::IF, _currentToken.lineNumber);

    move_next_non_wspace();

    _if->add_child(parse_expression());

    if (is_end_of_statement() && _currentToken.type != TokenType::CLOSED_BRACKET)
        move_next_non_wspace_pass_eols();

    if (_currentToken.type != TokenType::OPEN_BRACKET)
        Diagnostics::log_error("If statement did not have an open bracket.");

    _if->add_child(parse_block());

    if (peek_next_non_wspace_pass_eols().type == TokenType::ELSE)
    {
        move_next_non_wspace_pass_eols();

        if (peek_next_non_wspace().type == TokenType::IF)
        {
            move_next_non_wspace();

            _if->add_child(parse_if());
        }
        else
        {
            _if->add_child(parse_else());
        }
    }

    return _if;
}

Node* Parser::parse_else()
{
    auto _else = new Node(NodeType::ELSE, _currentToken.lineNumber);

    move_next_non_wspace();

    if (is_end_of_statement() && _currentToken.type != TokenType::CLOSED_BRACKET)
        move_next_non_wspace_pass_eols();

    if (_currentToken.type != TokenType::OPEN_BRACKET)
        Diagnostics::log_error("Else statement did not have an open bracket.");

    _else->add_child(parse_block());

    return _else;
}

/**
 * Meat and potatos of parsing below
*/

/// @brief Should always end at the end of its own statement. Not the next!
Node* Parser::parse_next_statement()
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
        auto va = new Node(NodeType::VARIABLE_ASSIGNMENT, new std::string(_currentToken.value), _currentToken.lineNumber);

        move_next_non_wspace();
        move_next_non_wspace();

        va->add_child(parse_expression());

        if (!is_end_of_statement())
            Diagnostics::log_error("Incorrect variable assignment.");
        
        return va;
    }
    // FUNCITON CALLS
    else if (_currentToken.type == TokenType::WORD && peek_next_non_wspace().type == TokenType::OPEN_PARAN)
    {
        auto fc = parse_function_call();
        
        move_next_non_wspace();

        if (!is_end_of_statement())
            Diagnostics::log_error("Incorrect function call " + *fc->get_value<std::string>() + ".");

        return fc;
    }
    // FUNCITON
    else if (_currentToken.type == TokenType::FUNCTION)
    {
        move_next_non_wspace();

        if (_currentToken.type != TokenType::WORD)
            Diagnostics::log_error("Function definition is lacking a name.");

        auto func = new Node(NodeType::FUNCTION, new FunctionData{ _currentToken.value }, _currentToken.lineNumber);
        auto funcInfo = func->get_value<FunctionData>();
        
        move_next_non_wspace();

        if (_currentToken.type != TokenType::OPEN_PARAN)
            Diagnostics::log_error("Function definition is missing an open parenthesis (.");

        move_next_non_wspace();

        if (_currentToken.type != TokenType::CLOSE_PARAN) 
        {
            while (_currentToken.type != TokenType::CLOSE_PARAN && _currentToken.type != TokenType::END_OF_FILE)
            {
                if (_currentToken.type != TokenType::WORD)
                    Diagnostics::log_error("Incorrect parameter in function definition.");

                funcInfo->paramNames.push_back(_currentToken.value);

                move_next_non_wspace();

                if (_currentToken.type == TokenType::COMMA && peek_next_non_wspace().type != TokenType::CLOSE_PARAN)
                {
                    move_next_non_wspace();
                    continue;
                }

                break;
            }

            if (_currentToken.type != TokenType::CLOSE_PARAN)
                Diagnostics::log_error("Function definition missing a closing parenthesis ).");
        }

        move_next_non_wspace_pass_eols();

        if (_currentToken.type != TokenType::OPEN_BRACKET)
            Diagnostics::log_error("Function definition missing its body {.");

        func->add_child(parse_block());

        return func;
    }
    // RETURN
    else if (_currentToken.type == TokenType::RETURN)
    {
        auto* r = new Node(NodeType::RETURN, _currentToken.lineNumber);

        move_next_non_wspace();

        r->add_child(parse_expression());

        if (!is_end_of_statement())
            Diagnostics::log_error("Return not formatted properly.");

        return r;
    }
    // WHILE
    else if (_currentToken.type == TokenType::WHILE)
    {
        auto* w = new Node(NodeType::WHILE, _currentToken.lineNumber);

        move_next_non_wspace();

        w->add_child(parse_expression());

        if (is_end_of_statement() && _currentToken.type != TokenType::CLOSED_BRACKET)
            move_next_non_wspace_pass_eols();

        if (_currentToken.type != TokenType::OPEN_BRACKET)
            Diagnostics::log_error("While statement did not have an open bracket.");

        w->add_child(parse_block());

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
            Diagnostics::log_error("Variable increment was not properly formatted.");

        return new Node(NodeType::VARIABLE_ASSIGNMENT, new std::string(variableName), _currentToken.lineNumber, {
            new Node(NodeType::EXPRESSION, _currentToken.lineNumber, { 
                new Node(NodeType::ADD_OPERATOR, _currentToken.lineNumber, { 
                    new Node(NodeType::VARIABLE, new std::string(variableName), _currentToken.lineNumber), new Node(NodeType::INTEGER, new int(1), _currentToken.lineNumber) 
                }) 
            }) 
        });
    }
    // DECREMENT
    else if (_currentToken.type == TokenType::WORD && peek_next_non_wspace().type == TokenType::DECREMENT)
    {
        std::string variableName = _currentToken.value;
        move_next_non_wspace();
        move_next_non_wspace();

        if (!is_end_of_statement())
            Diagnostics::log_error("Variable decrement was not properly formatted.");

        return new Node(NodeType::VARIABLE_ASSIGNMENT, new std::string(variableName), _currentToken.lineNumber, {
            new Node(NodeType::EXPRESSION, _currentToken.lineNumber, { 
                new Node(NodeType::SUB_OPERATOR, _currentToken.lineNumber, { 
                    new Node(NodeType::VARIABLE, new std::string(variableName), _currentToken.lineNumber), new Node(NodeType::INTEGER, new int(1), _currentToken.lineNumber) 
                })
            }) 
        });
    }
    // BREAK
    else if (_currentToken.type == TokenType::BREAK)
    {
        move_next_non_wspace();

        if (!is_end_of_statement())
            Diagnostics::log_error("Break statement was not properly formatted.");

        return new Node(NodeType::BREAK, _currentToken.lineNumber);
    }

    Diagnostics::log_error("Could not read statement.");

    return nullptr;
}

Node* Parser::parse_function_call()
{
    auto* fc = new Node(NodeType::FUNCTION_CALL, new std::string(_currentToken.value), _currentToken.lineNumber);

    move_next_non_wspace();
    move_next_non_wspace();

    if (_currentToken.type != TokenType::CLOSE_PARAN)
    {
        while (_currentToken.type != TokenType::CLOSE_PARAN && _currentToken.type != TokenType::END_OF_FILE)
        {
            auto* expression = parse_expression();

            if (expression != nullptr)
                fc->add_child(expression);

            if (_currentToken.type == TokenType::COMMA && peek_next_non_wspace().type != TokenType::CLOSE_PARAN)
            {
                move_next_non_wspace();
                continue;
            }

            break;
        }

        if (_currentToken.type == TokenType::COMMA)
            Diagnostics::log_error("Function call should not be ended with a comma.");

        if (_currentToken.type != TokenType::CLOSE_PARAN)
            Diagnostics::log_error("Missing a closed parenthesis ).");
    }

    return fc;
}

/// @brief Must start on an OPEN_BRACKET to start an inner block and nothing to start a file block
Node* Parser::parse_block()
{
    auto* block = new Node(NodeType::BLOCK, _currentToken.lineNumber);

    if (_currentToken.type == TokenType::OPEN_BRACKET)
    {
        _isInChildBlock = true;

        move_next_non_wspace_pass_eols();

        while (_currentToken.type != TokenType::CLOSED_BRACKET && _currentToken.type != TokenType::END_OF_FILE)
        {
            Node* stmt = parse_next_statement();

            if (stmt != nullptr)
                block->add_child(stmt);

            move_next_non_wspace_pass_eols();
        }

        if (_currentToken.type != TokenType::CLOSED_BRACKET)
            Diagnostics::log_error("Missing a closed bracket }.");

        _isInChildBlock = false;
    }
    else
    {
        if (_currentToken.type == TokenType::WHITESPACE || is_end_of_statement())
            move_next_non_wspace_pass_eols();

        while (_currentToken.type != TokenType::END_OF_FILE)
        {
            Node* stmt = parse_next_statement();

            if (stmt != nullptr)
                block->add_child(stmt);

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