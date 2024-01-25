#include "diagnostics.hpp"

#include "ast.hpp"

using namespace ast;

std::string ast::get_statement_type_name(NodeType type)
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
    _value = other._value;
    _lineIndex = other._lineIndex;

    for (auto stmt : other._children)
    {
        if (stmt != nullptr)
        {
            _children.push_back(new Node(*stmt));
        }
    }
}

Node* Node::operator=(const Node* other)
{
    clean_up();

    _type = other->_type;
    _value = other->_value;
    _lineIndex = other->_lineIndex;

    for (auto stmt : other->_children)
    {
        if (stmt != nullptr)
        {
            _children.push_back(new Node(*stmt));
        }
    }
    
    return this;
}

Node::Node(NodeType type, int lineIndex)
{
    _type = type;
    _lineIndex = lineIndex;
}

Node::Node(NodeType type, int lineIndex, std::vector<Node*> children)
{
    _type = type;
    _lineIndex = lineIndex;
    _children = children;
}

Node::Node(NodeType type, any value, int lineIndex)
{
    _type = type;
    _value = value;
    _lineIndex = lineIndex;
}

Node::Node(NodeType type, any value, int lineIndex, std::vector<Node*> children)
{
    _type = type;
    _value = value;
    _lineIndex = lineIndex;
    _children = children;
}

Node::~Node()
{
    clean_up();
}

void Node::clean_up()
{
    for (auto stmt : _children)
    {
        if (stmt != nullptr)
            delete stmt;
    }
}

NodeType Node::get_type()
{
    return _type;
}

std::vector<Node*>& Node::get_children()
{
    return _children;
}

Node* Node::get_child(size_t index)
{
    if (index >= 0 && index < _children.size())
        return _children[index]; 

    return nullptr;
}

void Node::add_child(Node* child)
{
    _children.push_back(child);
}

int Node::get_line_index()
{
    return _lineIndex;
}

AST::~AST()
{
    delete root;
}

void AST::print()
{
    if (root->get_type() != NodeType::BLOCK)
        throw std::runtime_error("Not a block!");

    for (auto stmt : root->get_children())
        print_statement(stmt, "");
}

void AST::print_statement(Node* stmt, std::string indent)
{
    if (stmt == NULL)
    {
        printf("NULL\n");
        return;
    }

    printf("%s%s\n", indent.c_str(), get_statement_type_name(stmt->get_type()).c_str());
    switch (stmt->get_type())
    {
        case NodeType::NONE:
            break;
        case NodeType::UNARY_OPERATOR:
        case NodeType::NEGATE:
            print_statement(stmt->get_child(0), indent + '\t');
            break;
        case NodeType::BINARY_OPERATOR:
        case NodeType::ADD_OPERATOR:
        case NodeType::SUB_OPERATOR:
        case NodeType::DIV_OPERATOR:
        case NodeType::MUL_OPERATOR:
        case NodeType::EQUALS_OPERATOR:
        case NodeType::NOT_EQUALS_OPERATOR:
        case NodeType::GREATER_THAN_OPERATOR:
        case NodeType::LESS_THAN_OPERATOR:
        case NodeType::GREATER_THAN_EQUALS_OPERATOR:
        case NodeType::LESS_THAN_EQUALS_OPERATOR:
        case NodeType::AND_CONDITION:
        case NodeType::OR_CONDITION:
        case NodeType::MODULUS_OPERATOR:
        case NodeType::POWER_OPERATOR:
            printf("%sLEFT:\n", indent.c_str());
            print_statement(stmt->get_child(0), indent + '\t');
            printf("%sRIGHT:\n", indent.c_str());
            print_statement(stmt->get_child(1), indent + '\t');
            break;
        case NodeType::DOUBLE:
            printf("%s%f\n", indent.c_str(), stmt->get_value<double>());
            break;
        case NodeType::INTEGER:
            printf("%s%i\n", indent.c_str(), stmt->get_value<int>());
            break;
        case NodeType::STRING:
            printf("%s%s\n", indent.c_str(), (stmt->get_value<std::string>()).c_str());
            break;
        case NodeType::EXPRESSION:
            print_statement(stmt->get_child(0), indent + '\t');
            break;
        case NodeType::VARIABLE_ASSIGNMENT:
            printf("%sNAME: |%s|\n", indent.c_str(), (stmt->get_value<std::string>()).c_str());
            print_statement(stmt->get_child(0), indent + '\t');
            break;
        case NodeType::BLOCK:
            for (auto statement : stmt->get_children())
                print_statement(statement, indent + '\t');
            break;
        case NodeType::VARIABLE:
            printf("%sNAME: %s\n", indent.c_str(), (stmt->get_value<std::string>()).c_str());
            break;
        case NodeType::FUNCTION_CALL:
            printf("%sFunction Name: %s\n", indent.c_str(), (stmt->get_value<std::string>()).c_str());
            printf("%s---PARAMETER LIST:\n", indent.c_str());
            for (auto e : stmt->get_children())
                print_statement(e, indent + '\t');
            break;
        case NodeType::BOOLEAN:
            if (stmt->get_value<bool>())
            {
                printf("%sTRUE\n", indent.c_str());
            }
            else
            {
                printf("%sFALSE\n", indent.c_str());
            }
            break;
        case NodeType::IF:
            printf("%sCONDITION:\n", indent.c_str());
            print_statement(stmt->get_child(0), indent + '\t');
            printf("%sBODY:\n", indent.c_str());
            print_statement(stmt->get_child(1), indent + '\t');

            if (stmt->get_child(2) != nullptr)
            {
                printf("%sCHILD STATEMENT:\n", indent.c_str());
                print_statement(stmt->get_child(2), indent + '\t');
            }
            break;
        case NodeType::ELSE:
            printf("%sBODY:\n", indent.c_str());
            print_statement(stmt->get_child(0), indent + '\t');
            break;
        case NodeType::WHILE:
            printf("%sCONDITION:\n", indent.c_str());
            print_statement(stmt->get_child(0), indent + '\t');
            printf("%sBODY:\n", indent.c_str());
            print_statement(stmt->get_child(1), indent + '\t');
            break;
        case NodeType::FUNCTION:
            {
                FunctionData funcInfo = stmt->get_value<FunctionData>();

                printf("%sNAME: %s\n", indent.c_str(), funcInfo.functionName.c_str());
                printf("%sPARAMETERS: [", indent.c_str());

                if (funcInfo.paramNames.size() <= 1)
                {
                    printf("]\n");
                }
                else
                {
                    for (size_t i = 1; i < funcInfo.paramNames.size(); ++i)
                    {
                        if (i != funcInfo.paramNames.size() - 1)
                            printf("%s, ", funcInfo.paramNames[i].c_str());
                        else
                            printf("%s]\n", funcInfo.paramNames[i].c_str());
                    }
                }

                printf("%sBODY:\n", indent.c_str());
                print_statement(stmt->get_child(0), indent + '\t');
            }
            break;
        case NodeType::RETURN:
            if (stmt->get_child(0) != nullptr)
                print_statement(stmt->get_child(0), indent + '\t');
            break;
    }
}