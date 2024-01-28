#include <stdexcept>
#include <string>
#include "../includes/console.hpp"
#include "parser.hpp"
#include "diagnostics.hpp"

using namespace popcorn::parser;
using namespace popcorn::lexer;
using namespace popcorn::diagnostics;

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

bool Parser::is_function_call()
{
    return _currentToken.type == TokenType::WORD && peek_next_non_wspace().type == TokenType::OPEN_PARAN;
}

bool Parser::is_variable_assignment()
{
    return _currentToken.type == TokenType::WORD && peek_next_non_wspace().type == TokenType::ASSIGNMENT;
}

bool Parser::is_increment()
{
    return _currentToken.type == TokenType::WORD && peek_next_non_wspace().type == TokenType::INCREMENT;
}

bool Parser::is_decrement()
{
    return _currentToken.type == TokenType::WORD && peek_next_non_wspace().type == TokenType::DECREMENT;
}

/**
 * Expression stuff below
*/

/// @brief Ends on the next non-whitespace token.
Node* Parser::parse_expression()
{
    return new Node(NodeType::EXPRESSION, _currentToken.lineNumber, new Expression_S{ parse_condition() });
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
            left = new Node(NodeType::AND_CONDITION, _currentToken.lineNumber, new BinaryOperator_S{ left, right });
        }
        else if (_currentToken.type == TokenType::OR_CONDITION)
        {
            move_next_non_wspace();
            Node* right = parse_equality();
            left = new Node(NodeType::OR_CONDITION, _currentToken.lineNumber, new BinaryOperator_S{ left, right });
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
            left = new Node(NodeType::EQUALS_OPERATOR, _currentToken.lineNumber, new BinaryOperator_S{ left, right });
        }
        else if (_currentToken.type == TokenType::NOT_EQUALS)
        {
            move_next_non_wspace();
            Node* right = parse_addend();
            left = new Node(NodeType::NOT_EQUALS_OPERATOR, _currentToken.lineNumber, new BinaryOperator_S{ left, right });
        }
        else if (_currentToken.type == TokenType::GREATER_THAN)
        {
            move_next_non_wspace();
            Node* right = parse_addend();
            left = new Node(NodeType::GREATER_THAN_OPERATOR, _currentToken.lineNumber, new BinaryOperator_S{ left, right });
        }
        else if (_currentToken.type == TokenType::LESS_THAN)
        {
            move_next_non_wspace();
            Node* right = parse_addend();
            left = new Node(NodeType::LESS_THAN_OPERATOR, _currentToken.lineNumber, new BinaryOperator_S{ left, right });
        }
        else if (_currentToken.type == TokenType::GREATER_THAN_EQUALS)
        {
            move_next_non_wspace();
            Node* right = parse_addend();
            left = new Node(NodeType::GREATER_THAN_EQUALS_OPERATOR, _currentToken.lineNumber, new BinaryOperator_S{ left, right });
        }
        else if (_currentToken.type == TokenType::LESS_THAN_EQUALS)
        {
            move_next_non_wspace();
            Node* right = parse_addend();
            left = new Node(NodeType::LESS_THAN_EQUALS_OPERATOR, _currentToken.lineNumber, new BinaryOperator_S{ left, right });
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
            left = new Node(NodeType::ADD_OPERATOR, _currentToken.lineNumber, new BinaryOperator_S{ left, right });
        }
        else if (_currentToken.type == TokenType::SUB)
        {
            move_next_non_wspace();
            Node* right = parse_term();
            left = new Node(NodeType::SUB_OPERATOR, _currentToken.lineNumber, new BinaryOperator_S{ left, right });
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
            left = new Node(NodeType::MUL_OPERATOR, _currentToken.lineNumber, new BinaryOperator_S{ left, right });
        }
        else if (_currentToken.type == TokenType::DIV)
        {
            move_next_non_wspace();
            Node* right = parse_power();
            left = new Node(NodeType::DIV_OPERATOR, _currentToken.lineNumber, new BinaryOperator_S{ left, right });
        }
        else if (_currentToken.type == TokenType::MODULUS)
        {
            move_next_non_wspace();
            Node* right = parse_power();
            left = new Node(NodeType::MODULUS_OPERATOR, _currentToken.lineNumber, new BinaryOperator_S{ left, right });
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
            left = new Node(NodeType::POWER_OPERATOR, _currentToken.lineNumber, new BinaryOperator_S{ left, right });
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
    // IS VARIABLE MEMBER ACCESSOR
    else if (_currentToken.type == TokenType::WORD && peek_next_non_wspace_pass_eols().type == TokenType::DOT)
    {
        result = parse_member_accessor(nullptr);
    }
    // FUNCTION CALLS
    else if (is_function_call())
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

        result = new Node(NodeType::DOUBLE, _currentToken.lineNumber, new double(value));
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

        result = new Node(NodeType::INTEGER, _currentToken.lineNumber, new int(value));
    }
    // STRING
    else if (_currentToken.type == TokenType::STRING)
    {
        result = new Node(NodeType::STRING, _currentToken.lineNumber, new std::string(_currentToken.value));
    }
    // VARIABLE
    else if (_currentToken.type == TokenType::WORD)
    {
        result = new Node(NodeType::VARIABLE, _currentToken.lineNumber, new std::string(_currentToken.value));
    }
    // NEGATE
    else if (_currentToken.type == TokenType::SUB)
    {
        move_next_non_wspace();
        return new Node(NodeType::NEGATE, _currentToken.lineNumber, new UnaryOperator_S{ parse_expression() });
    }
    // BOOLEAN
    else if (_currentToken.type == TokenType::BOOLEAN)
    {
        result = new Node(NodeType::BOOLEAN, _currentToken.lineNumber, new bool(_currentToken.value == "true" ? true : false));
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
    auto _if = new Node(NodeType::IF, _currentToken.lineNumber, new If_S());

    move_next_non_wspace();

    _if->get_struct<If_S>()->condition = parse_expression();

    if (is_end_of_statement() && _currentToken.type != TokenType::CLOSED_BRACKET)
        move_next_non_wspace_pass_eols();

    if (_currentToken.type != TokenType::OPEN_BRACKET)
        Diagnostics::log_error("If statement did not have an open bracket.");

    _if->get_struct<If_S>()->body = parse_block();

    if (peek_next_non_wspace_pass_eols().type == TokenType::ELSE)
    {
        move_next_non_wspace_pass_eols();

        if (peek_next_non_wspace().type == TokenType::IF)
        {
            move_next_non_wspace();

            _if->get_struct<If_S>()->branchingElseOrIf = parse_if();
        }
        else
        {
            _if->get_struct<If_S>()->branchingElseOrIf = parse_else();
        }
    }

    return _if;
}

Node* Parser::parse_else()
{
    auto _else = new Node(NodeType::ELSE, _currentToken.lineNumber, new Else_S());

    move_next_non_wspace();

    if (is_end_of_statement() && _currentToken.type != TokenType::CLOSED_BRACKET)
        move_next_non_wspace_pass_eols();

    if (_currentToken.type != TokenType::OPEN_BRACKET)
        Diagnostics::log_error("Else statement did not have an open bracket.");

    _else->get_struct<Else_S>()->body = parse_block();

    return _else;
}

Node* Parser::parse_member_accessor(Node* functionCall)
{
    Node* result = new Node(NodeType::MEMBER_ACCESSOR, _currentToken.lineNumber, new MemberAccessor_S());
    auto memberAccessorPtr = result->get_struct<MemberAccessor_S>();

    if (functionCall != nullptr)
    {
        memberAccessorPtr->_class = functionCall;
        move_next_non_wspace_pass_eols(); // move to member accessor
        move_next_non_wspace_pass_eols(); // move to right side of the member accessor
    }
    else
    {
        memberAccessorPtr->_class = new Node(NodeType::VARIABLE, _currentToken.lineNumber, new std::string(_currentToken.value));
        move_next_non_wspace_pass_eols(); // move to member accessor
        move_next_non_wspace_pass_eols(); // move to right side of the member accessor
    }

    if (is_function_call())
    {
        memberAccessorPtr->member = parse_function_call();
    }
    else if (_currentToken.type == TokenType::WORD)
    {
        if (peek_next_non_wspace_pass_eols().type == TokenType::DOT)
        {
            memberAccessorPtr->member = parse_member_accessor(nullptr);
        }
        else
        {
            memberAccessorPtr->member = new Node(NodeType::VARIABLE, _currentToken.lineNumber, new std::string(_currentToken.value));
        }
    }
    else
    {
        Diagnostics::log_error("Could not read right side of member accessor.");
    }

    return result;
}

/// @brief Ends on the closed parathesis of the function call.
Node* Parser::parse_function_call()
{
    auto* fc = new Node(NodeType::FUNCTION_CALL, _currentToken.lineNumber, new FunctionCall_S());
    fc->get_struct<FunctionCall_S>()->functionName = _currentToken.value;

    move_next_non_wspace();
    move_next_non_wspace();

    if (_currentToken.type != TokenType::CLOSE_PARAN)
    {
        while (_currentToken.type != TokenType::CLOSE_PARAN && _currentToken.type != TokenType::END_OF_FILE)
        {
            auto* expression = parse_expression();

            if (expression != nullptr)
                fc->get_struct<FunctionCall_S>()->paramValues.push_back(expression);

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

    if (peek_next_non_wspace_pass_eols().type == TokenType::DOT)
        return parse_member_accessor(fc);

    return fc;
}

Node* Parser::parse_function()
{
    move_next_non_wspace();

    if (_currentToken.type != TokenType::WORD)
        Diagnostics::log_error("Function definition is lacking a name.");

    Node* func = new Node(NodeType::FUNCTION, _currentToken.lineNumber, new Funciton_S());
    auto function_struct = func->get_struct<Funciton_S>();
    function_struct->name = _currentToken.value;
    
    move_next_non_wspace();

    if (_currentToken.type != TokenType::OPEN_PARAN)
        Diagnostics::log_error("Function definition is missing an open parenthesis (.");

    move_next_non_wspace();

    if (_currentToken.type != TokenType::CLOSE_PARAN) 
    {
        // parse parameters
        while (_currentToken.type != TokenType::CLOSE_PARAN && _currentToken.type != TokenType::END_OF_FILE)
        {
            if (_currentToken.type != TokenType::WORD)
                Diagnostics::log_error("Incorrect parameter in function definition.");

            function_struct->paramList.push_back(_currentToken.value);

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

    function_struct->body = parse_block();

    return func;
}

/// @brief Must start on an OPEN_BRACKET to start an inner block and nothing to start a file block
Node* Parser::parse_block()
{
    auto* block = new Node(NodeType::BLOCK, _currentToken.lineNumber, new Block_S());

    if (_currentToken.type == TokenType::OPEN_BRACKET)
    {
        _isInChildBlock = true;

        move_next_non_wspace_pass_eols();

        while (_currentToken.type != TokenType::CLOSED_BRACKET && _currentToken.type != TokenType::END_OF_FILE)
        {
            Node* stmt = parse_next_statement();

            if (stmt != nullptr)
                block->get_struct<Block_S>()->statements.push_back(stmt);

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
                block->get_struct<Block_S>()->statements.push_back(stmt);

            move_next_non_wspace_pass_eols();
        }
    }

    return block;
}

/**
 * Meat and potatos of parsing below
*/

/// @brief Should always end at the end of its own statement. Not the next!
Node* Parser::parse_next_statement()
{
    // COMMENTS
    if (_currentToken.type == TokenType::COMMENT)
    {
        int nextLine = _currentToken.lineNumber + 1;

        while (peek_next().lineNumber < nextLine && _currentToken.type != TokenType::END_OF_FILE)
            move_next();

        return nullptr;
    }
    // MEMBER ACCESSOR
    else if (_currentToken.type == TokenType::WORD && peek_next_non_wspace_pass_eols().type == TokenType::DOT)
    {
        Node* result = parse_member_accessor(nullptr);
        move_next_non_wspace();

        if (!is_end_of_statement())
            Diagnostics::log_error("Invalid statement!");

        return result;
    }
    // VARIABLE ASSIGNMENT
    else if (is_variable_assignment())
    {
        Node* va = new Node(NodeType::VARIABLE_ASSIGNMENT, _currentToken.lineNumber, new VariableAssignment_S());
        auto va_struct = va->get_struct<VariableAssignment_S>();
        va_struct->variableName = _currentToken.value;

        move_next_non_wspace();
        move_next_non_wspace();

        va_struct->expression = parse_expression();

        if (!is_end_of_statement())
            Diagnostics::log_error("Incorrect variable assignment.");
        
        return va;
    }
    // FUNCITON CALLS
    else if (is_function_call())
    {
        auto fc = parse_function_call();
        
        move_next_non_wspace();

        if (!is_end_of_statement())
            Diagnostics::log_error("Incorrect function call " + fc->get_struct<FunctionCall_S>()->functionName + ".");

        return fc;
    }
    // FUNCITON
    else if (_currentToken.type == TokenType::FUNCTION)
    {
        return parse_function();
    }
    // RETURN
    else if (_currentToken.type == TokenType::RETURN)
    {
        auto r = new Node(NodeType::RETURN, _currentToken.lineNumber, new Return_S());

        move_next_non_wspace();

        r->get_struct<Return_S>()->expression = parse_expression();

        if (!is_end_of_statement())
            Diagnostics::log_error("Return not formatted properly.");

        return r;
    }
    // WHILE
    else if (_currentToken.type == TokenType::WHILE)
    {
        auto* w = new Node(NodeType::WHILE, _currentToken.lineNumber, new While_S());

        move_next_non_wspace();

        w->get_struct<While_S>()->condition = parse_expression();

        if (is_end_of_statement() && _currentToken.type != TokenType::CLOSED_BRACKET)
            move_next_non_wspace_pass_eols();

        if (_currentToken.type != TokenType::OPEN_BRACKET)
            Diagnostics::log_error("While statement did not have an open bracket.");

        w->get_struct<While_S>()->body = parse_block();

        return w;
    }
    // If
    else if (_currentToken.type == TokenType::IF)
    {
        return parse_if();
    }
    // INCREMENT
    else if (is_increment())
    {
        std::string vn = _currentToken.value;
        move_next_non_wspace();
        move_next_non_wspace();

        if (!is_end_of_statement())
            Diagnostics::log_error("Variable increment was not properly formatted.");

        return new Node(NodeType::VARIABLE_ASSIGNMENT, _currentToken.lineNumber, new VariableAssignment_S{
            vn,
            new Node(NodeType::EXPRESSION, _currentToken.lineNumber, new Expression_S{
                new Node(NodeType::ADD_OPERATOR, _currentToken.lineNumber, new BinaryOperator_S{
                    new Node(NodeType::VARIABLE, _currentToken.lineNumber, new std::string(vn)),
                    new Node(NodeType::INTEGER, _currentToken.lineNumber, new int(1))
                })
            })
        });
    }
    // DECREMENT
    else if (is_decrement())
    {
        std::string vn = _currentToken.value;
        move_next_non_wspace();
        move_next_non_wspace();

        if (!is_end_of_statement())
            Diagnostics::log_error("Variable decrement was not properly formatted.");

        return new Node(NodeType::VARIABLE_ASSIGNMENT, _currentToken.lineNumber, new VariableAssignment_S{
            vn,
            new Node(NodeType::EXPRESSION, _currentToken.lineNumber, new Expression_S{
                new Node(NodeType::SUB_OPERATOR, _currentToken.lineNumber, new BinaryOperator_S{
                    new Node(NodeType::VARIABLE, _currentToken.lineNumber, new std::string(vn)),
                    new Node(NodeType::INTEGER, _currentToken.lineNumber, new int(1))
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

/// @brief Parses a list of tokens and returns its Abstract Syntax Tree
/// @return Abstract Syntax Tree (AST*)
AST* Parser::parse_ast()
{
    _ast = new AST();
    move_next();
    _ast->root = parse_block(); 
    return _ast;
}