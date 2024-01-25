#ifndef _AST
#define _AST

#include <vector>
#include <string>
#include <stdexcept>
#include "any.hpp"

namespace ast
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

    std::string get_statement_type_name(NodeType type);

    struct FunctionData
    {
        std::string functionName;
        std::vector<std::string> paramNames;
    };

    class Node
    {
        NodeType _type;
        std::vector<Node*> _children;
        any _value;
        int _lineIndex;

        public:
            Node(const Node& other);
            Node* operator=(const Node* other);
            Node(NodeType type, int lineIndex);
            Node(NodeType type, int lineIndex, std::vector<Node*> children);
            Node(NodeType type, any value, int lineIndex);
            Node(NodeType type, any value, int lineIndex, std::vector<Node*> children);
            ~Node();

            void clean_up();

            template<typename T>
            T get_value() { return _value.get_value<T>(); }

            template<typename T>
            T* get_value_as_pointer() { return _value.get_value_as_pointer<T>(); }

            std::vector<Node*>& get_children();
            Node* get_child(size_t index);
            void add_child(Node* child);

            NodeType get_type();
            int get_line_index();
    };
 
    class AST
    {
        public:
            Node* root;

            ~AST();
            void print();

        private:
            void print_statement(Node* stat, std::string indent);
    };
}

#endif // _AST