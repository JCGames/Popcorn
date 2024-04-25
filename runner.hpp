#ifndef RUNNER
#define RUNNER

#include <vector>
#include <string>
#include <memory>
#include <stdexcept>

#include "parser.hpp"
#include "object.hpp"

namespace pop
{
    /**
     * A structure basically made for stack variables.
    */
    struct StackAllocation
    {
        std::string variableName;
        Object value;
    };

    /**
     * A scope is exactly what it sounds like
     * ... a scope.
    */
    struct Scope
    {
    private:
        Scope* parent;
        std::vector<StackAllocation> stack;
        std::vector<Statement*> functions;

    public:
        bool returnFlag;
        bool breakFlag;
        bool continueFlag;

        Scope();

        bool has_variable(const std::string& variableName);
        Object get_variable(const std::string& variableName);
        void set_variable(const std::string& variableName, Object value);
        void set_parent(Scope* parent);
        void create_function_table(Statement& block);
        Statement* get_function_in_block(std::string functionName);
        std::vector<StackAllocation>& get_stack();
    };

    /**
     * Executes an abstract syntax tree.
    */
    class Runner 
    {
        Statement* root;
        Diagnostics* diagnostics;

        void run_block(Statement& root, Scope* parentScope, Object* result = nullptr);
        void run_statement(Statement& statement, Scope& scope, Object* result = nullptr);
        Object run_function_call(Statement& functionCall, Scope& scope);
        Object eval_expression(const Statement& statement, Scope& scope);

    public:
        void run(Statement* root, Diagnostics* diagnostics);
        void test1();
    };
}

#endif