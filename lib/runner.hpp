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

namespace popcorn::runner
{
    class Runner
    {
        Scope _rootScope;

        void create_function_lookup_table(popcorn::parser::Node* block, Scope& scope);
        Object call_function(popcorn::parser::Node* funcCall, Scope& scope);
        Object run_block(popcorn::parser::Node* block, Scope& scope);
        Object interpret(popcorn::parser::Node* stat, Scope& scope);

        public:
            Runner();
            
            void run(popcorn::parser::AST& ast);
    };
}

#endif // RUNNER