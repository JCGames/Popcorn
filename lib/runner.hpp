#ifndef RUNNER
#define RUNNER

#include <vector>
#include <string>
#include <stack>
#include <iostream>
#include <stdexcept>
#include "ast.hpp"
#include "object.hpp"

namespace run 
{
    struct _Function
    {
        std::string name;
        ast::Function& funcRef;
    };

    struct _Variable
    {
        std::string name;
        obj::Object object;

        _Variable(std::string name, obj::Object object)
        {
            this->name = name;
            this->object = object;
        }
    };

    class Runner
    {
        std::vector<_Function> _functionTable;
        std::vector<_Variable> _variables;

        void add_variable(_Variable variable, int initialIndex = 0);
        _Variable& get_variable(std::string name);
        bool has_variable(std::string name);

        obj::Object call_function(ast::FunctionCall* funcCall);
        obj::Object interpret(ast::Statement* stat);
        void create_function_table(ast::Statement* stat);

        public:
            Runner();
            
            void dump_runner();
            void run(ast::AST& ast);
    };
}

#endif // RUNNER