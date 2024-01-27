#include "node.hpp"

using namespace popcorn::parser;

std::string popcorn::parser::get_node_type_name(NodeType type)
{
    switch (type)
    {
        case NodeType::NONE: return "NONE";
        case NodeType::UNARY_OPERATOR: return "UNARY OPERATOR";
        case NodeType::BINARY_OPERATOR: return "BINARY OPERATOR";
        case NodeType::ADD_OPERATOR: return "ADD OPERATOR";
        case NodeType::SUB_OPERATOR: return "SUBTRACT OPERATOR";
        case NodeType::DIV_OPERATOR: return "DIVIDE OPERATOR";
        case NodeType::MUL_OPERATOR: return "MULTIPLY OPERATOR";
        case NodeType::DOUBLE: return "DOUBLE";
        case NodeType::INTEGER: return "INTEGER";
        case NodeType::STRING: return "STRING";
        case NodeType::EXPRESSION: return "EXPRESSION";
        case NodeType::VARIABLE_ASSIGNMENT: return "VARIABLE ASSIGNMENT";
        case NodeType::BLOCK: return "BLOCK";
        case NodeType::VARIABLE: return "VARIABLE";
        case NodeType::FUNCTION_CALL: return "FUNCTION CALL";
        case NodeType::NEGATE: return "NEGATE";
        case NodeType::BOOLEAN: return "BOOLEAN";
        case NodeType::EQUALS_OPERATOR: return "EQUALS OPERATOR";
        case NodeType::NOT_EQUALS_OPERATOR: return "NOT EQUALS OPERATOR";
        case NodeType::GREATER_THAN_OPERATOR: return "GREATER THAN OPERATOR";
        case NodeType::LESS_THAN_OPERATOR: return "LESS THAN OPERATOR";
        case NodeType::GREATER_THAN_EQUALS_OPERATOR: return "GREATER THAN EQUALS OPERATOR";
        case NodeType::LESS_THAN_EQUALS_OPERATOR: return "LESS THAN EQUALS OPERATOR";
        case NodeType::IF: return "IF";
        case NodeType::ELSE: return "ELSE";
        case NodeType::WHILE: return "WHILE";
        case NodeType::FOR: return "FOR";
        case NodeType::AND_CONDITION: return "AND CONDITION";
        case NodeType::OR_CONDITION: return "OR CONDITION";
        case NodeType::MODULUS_OPERATOR: return "MODULUS OPERATOR";
        case NodeType::FUNCTION: return "FUNCTION";
        case NodeType::RETURN: return "RETURN";
        case NodeType::POWER_OPERATOR: return "POWER OPERATOR";
        case NodeType::BREAK: return "BREAK";
        default: return "UNDEFINED";
    }
}

Node::Node(const Node& other)
{
    _type = other._type;
    _dataStructure = other._dataStructure;
    _lineIndex = other._lineIndex;
}

Node* Node::operator=(const Node* other)
{
    _type = other->_type;
    _dataStructure = other->_dataStructure;
    _lineIndex = other->_lineIndex;
    return this;
}

Node::~Node()
{
    
}

Node::Node(NodeType type, int lineIndex)
{
    _type = type;
    _lineIndex = lineIndex;
}

NodeType Node::get_type()
{
    return _type;
}

int Node::get_line_index()
{
    return _lineIndex;
}