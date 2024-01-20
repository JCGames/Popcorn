#ifndef RUNNER
#define RUNNER

#include <vector>
#include <string>
#include <stack>
#include <iostream>
#include <stdexcept>
#include "ast.hpp"
#include "object.hpp"
#include "scope.hpp"

namespace run 
{
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