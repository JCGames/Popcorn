#include "ast.hpp"

using namespace ast;

static std::string get_statement_type_name(StatementType type)
{
    switch (type)
    {
        case StatementType::NONE: return "NONE";
        case StatementType::UNARY_OPERATOR: return "UNARY OPERATOR";
        case StatementType::BINARY_OPERATOR: return "BINARY OPERATOR";
        case StatementType::ADD_OPERATOR: return "ADD OPERATOR";
        case StatementType::SUB_OPERATOR: return "SUBTRACT OPERATOR";
        case StatementType::DIV_OPERATOR: return "DIVIDE OPERATOR";
        case StatementType::MUL_OPERATOR: return "MULTIPLY OPERATOR";
        case StatementType::DOUBLE: return "DOUBLE";
        case StatementType::INTEGER: return "INTEGER";
        case StatementType::STRING: return "STRING";
        case StatementType::EXPRESSION: return "EXPRESSION";
        case StatementType::VARIABLE_ASSIGNMENT: return "VARIABLE ASSIGNMENT";
        case StatementType::BLOCK: return "BLOCK";
        case StatementType::VARIABLE: return "VARIABLE";
        case StatementType::FUNCTION_CALL: return "FUNCTION CALL";
        case StatementType::NEGATE: return "NEGATE";
        case StatementType::BOOLEAN: return "BOOLEAN";
        case StatementType::EQUALS_OPERATOR: return "EQUALS OPERATOR";
        case StatementType::NOT_EQUALS_OPERATOR: return "NOT EQUALS OPERATOR";
        case StatementType::GREATER_THAN_OPERATOR: return "GREATER THAN OPERATOR";
        case StatementType::LESS_THAN_OPERATOR: return "LESS THAN OPERATOR";
        case StatementType::GREATER_THAN_EQUALS_OPERATOR: return "GREATER THAN EQUALS OPERATOR";
        case StatementType::LESS_THAN_EQUALS_OPERATOR: return "LESS THAN EQUALS OPERATOR";
        case StatementType::IF: return "IF";
        case StatementType::ELSE: return "ELSE";
        case StatementType::WHILE: return "WHILE";
        case StatementType::FOR: return "FOR";
        case StatementType::AND_CONDITION: return "AND CONDITION";
        case StatementType::OR_CONDITION: return "OR CONDITION";
        case StatementType::MODULUS_OPERATOR: return "MODULUS OPERATOR";
        default: return "UNDEFINED";
    }
}

#pragma region Statement

Statement::Statement(int lineIndex)
{
    _lineIndex = lineIndex;
}

StatementType Statement::get_type()
{
    throw std::runtime_error("Statement function get_type not implemented!");
}

int Statement::get_line_index()
{
    return _lineIndex;
}

#pragma endregion

#pragma region UnaryOperator

UnaryOperator::UnaryOperator(Statement* value, int lineIndex) : Statement(lineIndex)
{
    this->value = value;
}

UnaryOperator::~UnaryOperator()
{
    if (value != NULL)
        delete value;
}

StatementType UnaryOperator::get_type()
{
    return StatementType::UNARY_OPERATOR;
}

#pragma endregion

#pragma region BinaryOperator

BinaryOperator::BinaryOperator(Statement* left, Statement* right, int lineIndex) : Statement(lineIndex)
{
    this->left = left;
    this->right = right;
}

BinaryOperator::~BinaryOperator()
{
    if (left != NULL) 
        delete left;
    
    if (right != NULL)
        delete right;
}

StatementType BinaryOperator::get_type() 
{
    return StatementType::BINARY_OPERATOR;
}

#pragma endregion