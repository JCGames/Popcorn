#ifndef SCOPE
#define SCOPE

#include <string>
#include "ast.hpp"
#include "object.hpp"

namespace popcorn::runner
{
    struct FunctionPointer
    {
        Function function;
        
        FunctionPointer(Function function);
    };

    struct VariablePointer
    {
        std::string variableName;
        Object object;

        VariablePointer(std::string variableName, Object object);
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

        void add_func(std::string name, popcorn::parser::Node* function);
        popcorn::parser::Node* get_func(std::string name);

        void add_var(std::string name, Object object);
        bool has_var(std::string name);
        Object& get_var(std::string name);
    };
}

#endif // SCOPE