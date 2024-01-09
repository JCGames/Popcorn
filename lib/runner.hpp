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
    struct FunctionPointer
    {
        std::string functionName;
        ast::Function* function;

        FunctionPointer(std::string functionName, ast::Function* function);
    };

    struct VariablePointer
    {
        std::string variableName;
        obj::Object object;

        VariablePointer(std::string variableName, obj::Object object);
    };

    struct Scope
    {
        Scope* parent;
        std::vector<FunctionPointer> functions;
        std::vector<VariablePointer> pointers;

        Scope();
        Scope(Scope* parent);

        ast::Function* get_func(std::string name);

        void add_var(std::string name, obj::Object object);
        bool has_var(std::string name);
        obj::Object& get_var(std::string name);
    };

    class Runner
    {
        Scope _rootScope;
        std::vector<FunctionPointer> _functionTable;

        void create_function_lookup_table(ast::Block* block, Scope& scope);
        obj::Object call_function(ast::FunctionCall* funcCall, Scope& scope);
        obj::Object interpret(ast::Statement* stat, Scope& scope);

        public:
            Runner();
            
            void run(ast::AST& ast);
    };
}

#endif // RUNNER