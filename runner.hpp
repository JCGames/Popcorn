#ifndef RUNNER
#define RUNNER

#include <vector>
#include <string>
#include <stack>
#include <iostream>
#include <stdexcept>
#include "ast.hpp"
#include "object.hpp"

struct _Variable
{
    std::string name;
    run::Object object;

    _Variable(std::string name, run::Object object)
    {
        this->name = name;
        this->object = object;
    }
};

class Runner
{
    std::vector<_Variable> _variables;

    run::Object call_function(ast::FunctionCall* funcCall);

    run::Object interpret(ast::Statement* stat);

    void add_variable(_Variable variable);
    void remove_variable(_Variable variable);
    _Variable& get_variable(std::string name);
    bool has_variable(std::string name);

    public:
        Runner();

        void run(ast::AST& ast);
};

#endif // RUNNER