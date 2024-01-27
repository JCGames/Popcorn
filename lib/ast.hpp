#ifndef _AST
#define _AST

#include <vector>
#include <string>
#include <stdexcept>
#include "node.hpp"

namespace popcorn::parser
{
    class AST
    {
        public:
            Node* root;

            ~AST();
            void print();

        private:
            void print_statement(Node* stat, std::string indent);
    };
}

#endif // _AST