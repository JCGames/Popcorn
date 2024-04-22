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

    public:
        bool obj_exists_in_scope_history(const std::string& variableName);
        Object get_obj(const std::string& variableName);
        void set_obj(const std::string& variableName, Object value);
        void delete_obj();
        void set_parent(Scope* parent);
        std::vector<StackAllocation>& get_stack();
    };

    /**
     * Executes an abstract syntax tree.
    */
    class Runner 
    {
        Statement* root;
        Diagnostics* diagnostics;

        void run_block(Statement& root, Scope* parentScope);
        void run_statement(Statement& statement, Scope& scope);
        Object eval_expression(const Statement& statement, Scope& scope);

    public:
        void run(Statement* root, Diagnostics* diagnostics);
        void test1();
    };
}

#endif