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
        ast::Node* function;

        FunctionPointer(std::string functionName, ast::Node* function);
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
        std::vector<VariablePointer> variables;

        // primarily for functions
        bool canNestFunctions;
        bool returnFlag;

        // primarily for while loops
        bool breakFlag;

        Scope();
        Scope(Scope* parent);

        void add_func(std::string name, ast::Node* function);
        ast::Node* get_func(std::string name);

        void add_var(std::string name, obj::Object object);
        bool has_var(std::string name);
        obj::Object& get_var(std::string name);
    };

    class Runner
    {
        Scope _rootScope;

        void create_function_lookup_table(ast::Node* block, Scope& scope);
        obj::Object call_function(ast::Node* funcCall, Scope& scope);
        obj::Object run_block(ast::Node* block, Scope& scope);
        obj::Object interpret(ast::Node* stat, Scope& scope);

        public:
            Runner();
            
            void run(ast::AST& ast);
    };
}

#endif // RUNNER