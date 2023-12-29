#ifndef RUNNER
#define RUNNER

#include <vector>
#include <string>
#include <stack>
#include <iostream>
#include "ast.hpp"

enum class ObjectType
{
    BOOLEAN,
    INTEGER,
    DOUBLE,
    STRING,
    NULL_OBJ
};

class Object
{
    ObjectType type;

    public:
        bool boolValue;
        int intValue;
        double doubleValue;
        std::string strValue;

        Object()
        {
            type = ObjectType::NULL_OBJ;
        }
        Object(std::string strValue)
        {
            this->type = ObjectType::STRING;
            this->strValue = strValue;
        }
        Object(bool boolValue)
        {
            this->type = ObjectType::BOOLEAN;
            this->boolValue = boolValue;
        }
        Object(int intValue)
        {
            this->type = ObjectType::INTEGER;
            this->intValue = intValue;
        }
        Object(double doubleValue)
        {
            this->type = ObjectType::DOUBLE;
            this->doubleValue = doubleValue;
        }

        bool is_number()
        {
            return type == ObjectType::DOUBLE || type == ObjectType::INTEGER || type == ObjectType::BOOLEAN;
        }

        ObjectType get_type()
        {
            return type;
        }
};

struct _Variable
{
    std::string name;
    Object object;

    _Variable(std::string name, Object object)
    {
        this->name = name;
        this->object = object;
    }
};

class Runner
{
    std::vector<_Variable> _variables;

    Object run_func(ast::FunctionCall* funcCall);
    Object eval_binary_operator(ast::BinaryOperator* binaryOperator);
    Object cast_to_int(Object obj);
    std::string cast_to_string(Object obj);

    Object interpret(ast::Statement* stat);

    void add_variable(_Variable variable);
    void remove_variable(_Variable variable);
    _Variable get_variable(std::string name);
    bool has_variable(std::string name);

    public:
        Runner();

        void run(ast::AST& ast);
};

#endif // RUNNER