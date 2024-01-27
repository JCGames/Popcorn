#ifndef NODE
#define NODE

#include <string>
#include <vector>
#include <memory>
#include <stdexcept>
#include "diagnostics.hpp"

using namespace popcorn::diagnostics;

namespace popcorn::parser
{
    enum class NodeType : char
    {
        NONE,
        UNARY_OPERATOR,
        BINARY_OPERATOR,
        ADD_OPERATOR,
        SUB_OPERATOR,
        DIV_OPERATOR,
        MUL_OPERATOR,
        DOUBLE,
        INTEGER,
        STRING,
        EXPRESSION,
        VARIABLE_ASSIGNMENT,
        BLOCK,
        VARIABLE,
        FUNCTION_CALL,
        NEGATE,
        BOOLEAN,
        EQUALS_OPERATOR,
        NOT_EQUALS_OPERATOR,
        GREATER_THAN_OPERATOR,
        LESS_THAN_OPERATOR,
        GREATER_THAN_EQUALS_OPERATOR,
        LESS_THAN_EQUALS_OPERATOR,
        IF,
        ELSE,
        WHILE,
        FOR,
        AND_CONDITION,
        OR_CONDITION,
        MODULUS_OPERATOR,
        FUNCTION,
        RETURN,
        POWER_OPERATOR,
        BREAK,
    };

    std::string get_node_type_name(NodeType type);

    class Node
    {
        NodeType _type;
        std::shared_ptr<void> _dataStructure;
        std::string _dataStructureType;
        int _lineIndex;

        public:
            Node(const Node& other);
            Node* operator=(const Node* other);
            Node(NodeType type, int lineIndex);

            template<typename T>
            Node(NodeType type, int lineIndex, T* dataStructure)
            {
                _dataStructureType = typeid(T).name();
                _type = type;
                _dataStructure = std::shared_ptr<void>(dataStructure);
                _lineIndex = lineIndex;
            }

            ~Node();

            template<typename T>
            T* get_struct()
            {
                if (is_struct_null())
                    throw std::runtime_error("Trying to get a node data structure that is a nullptr is invalid!");

                if (typeid(T).name() != _dataStructureType)
                    throw std::runtime_error("Trying to cast type of " + _dataStructureType + " to type of " + typeid(T).name() + " is invalid!");

                return static_cast<T*>(_dataStructure.get());
            }

            bool is_struct_null() { return _dataStructure.get() == nullptr; }

            NodeType get_type();
            int get_line_index();
    };

    struct Funciton_S
    {
        std::string name;
        std::vector<std::string> paramList;
        Node* body;

        ~Funciton_S()
        {
            if (body != nullptr)
                delete body;
        }
    };

    struct FunctionCall_S
    {
        std::string functionName;
        std::vector<Node*> paramValues;

        ~FunctionCall_S()
        {
            for (auto item : paramValues)
            {
                if (item != nullptr)
                    delete item;
            }
        }
    };

    struct VariableAssignment_S
    {
        std::string variableName;
        Node* expression;

        ~VariableAssignment_S()
        {
            if (expression != nullptr)
                delete expression;
        }
    };

    struct BinaryOperator_S
    {
        Node* left;
        Node* right;

        ~BinaryOperator_S()
        {
            if (left != nullptr)
                delete left;
            
            if (right != nullptr)
                delete right;
        }
    };

    struct UnaryOperator_S
    {
        Node* child;

        ~UnaryOperator_S()
        {
            if (child != nullptr)
                delete child;
        }
    };

    struct Expression_S
    {
        Node* root;

        ~Expression_S()
        {
            if (root != nullptr)
                delete root;
        }
    };

    struct Block_S
    {
        std::vector<Node*> statements;
        
        ~Block_S()
        {
            for (auto stmt : statements)
            {
                if (stmt != nullptr)
                    delete stmt;
            }
        }
    };


    struct If_S
    {
        Node* condition;
        Node* body;
        Node* branchingElseOrIf;

        ~If_S()
        {
            if (condition != nullptr)
                delete condition;

            if (body != nullptr)
                delete body;
            
            if (branchingElseOrIf != nullptr)
                delete branchingElseOrIf;
        }
    };

    struct Else_S
    {
        Node* body;

        ~Else_S()
        {
            if (body != nullptr)
                delete body;
        }
    };

    struct While_S
    {
        Node* condition;
        Node* body;

        ~While_S()
        {
            if (condition != nullptr)
                delete condition;

            if (body != nullptr)
                delete body;
        }
    };

    struct Return_S
    {
        Node* expression;

        ~Return_S()
        {
            if (expression != nullptr)
                delete expression;
        }
    };
}

#endif // NODE