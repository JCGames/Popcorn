#include "ast.hpp"

#include "diagnostics.hpp"

using namespace popcorn::parser;

AST::~AST()
{
    delete root;
}

void AST::print()
{
    if (root->get_type() != NodeType::BLOCK)
        throw std::runtime_error("Not a block!");

    for (auto stmt : root->get_struct<Block_S>()->statements)
        print_statement(stmt, "");
}

void AST::print_statement(Node* stmt, std::string indent)
{
    if (stmt == NULL)
    {
        printf("NULL\n");
        return;
    }

    printf("%s%s\n", indent.c_str(), get_node_type_name(stmt->get_type()).c_str());
    switch (stmt->get_type())
    {
        case NodeType::NONE:
            break;
        case NodeType::UNARY_OPERATOR:
        case NodeType::NEGATE:
            print_statement(stmt->get_struct<UnaryOperator_S>()->child, indent + '\t');
            break;
        case NodeType::BINARY_OPERATOR:
        case NodeType::ADD_OPERATOR:
        case NodeType::SUB_OPERATOR:
        case NodeType::DIV_OPERATOR:
        case NodeType::MUL_OPERATOR:
        case NodeType::EQUALS_OPERATOR:
        case NodeType::NOT_EQUALS_OPERATOR:
        case NodeType::GREATER_THAN_OPERATOR:
        case NodeType::LESS_THAN_OPERATOR:
        case NodeType::GREATER_THAN_EQUALS_OPERATOR:
        case NodeType::LESS_THAN_EQUALS_OPERATOR:
        case NodeType::AND_CONDITION:
        case NodeType::OR_CONDITION:
        case NodeType::MODULUS_OPERATOR:
        case NodeType::POWER_OPERATOR:
            printf("%sLEFT:\n", indent.c_str());
            print_statement(stmt->get_struct<BinaryOperator_S>()->left, indent + '\t');
            printf("%sRIGHT:\n", indent.c_str());
            print_statement(stmt->get_struct<BinaryOperator_S>()->right, indent + '\t');
            break;
        case NodeType::DOUBLE:
            printf("%s%f\n", indent.c_str(), *stmt->get_struct<double>());
            break;
        case NodeType::INTEGER:
            printf("%s%i\n", indent.c_str(), *stmt->get_struct<int>());
            break;
        case NodeType::STRING:
            printf("%s%s\n", indent.c_str(), (*stmt->get_struct<std::string>()).c_str());
            break;
        case NodeType::VARIABLE:
            printf("%sNAME: %s\n", indent.c_str(), (*stmt->get_struct<std::string>()).c_str());
            break;
        case NodeType::EXPRESSION:
            print_statement(stmt->get_struct<Expression_S>()->root, indent + '\t');
            break;
        case NodeType::VARIABLE_ASSIGNMENT:
            printf("%sNAME: |%s|\n", indent.c_str(), stmt->get_struct<VariableAssignment_S>()->variableName.c_str());
            print_statement(stmt->get_struct<VariableAssignment_S>()->expression, indent + '\t');
            break;
        case NodeType::BLOCK:
            for (auto statement : stmt->get_struct<Block_S>()->statements)
                print_statement(statement, indent + '\t');
            break;
        case NodeType::FUNCTION_CALL:
            printf("%sFunction Name: %s\n", indent.c_str(), stmt->get_struct<FunctionCall_S>()->functionName.c_str());
            printf("%s---PARAMETER LIST:\n", indent.c_str());
            for (auto e : stmt->get_struct<FunctionCall_S>()->paramValues)
                print_statement(e, indent + '\t');
            break;
        case NodeType::BOOLEAN:
            if (stmt->get_struct<bool>())
                printf("%sTRUE\n", indent.c_str());
            else
                printf("%sFALSE\n", indent.c_str());
            break;
        case NodeType::IF:
            printf("%sCONDITION:\n", indent.c_str());
            print_statement(stmt->get_struct<If_S>()->condition, indent + '\t');
            printf("%sBODY:\n", indent.c_str());
            print_statement(stmt->get_struct<If_S>()->body, indent + '\t');

            if (stmt->get_struct<If_S>()->branchingElseOrIf != nullptr)
            {
                printf("%sCHILD STATEMENT:\n", indent.c_str());
                print_statement(stmt->get_struct<If_S>()->branchingElseOrIf, indent + '\t');
            }
            break;
        case NodeType::ELSE:
            printf("%sBODY:\n", indent.c_str());
            print_statement(stmt->get_struct<Else_S>()->body, indent + '\t');
            break;
        case NodeType::WHILE:
            printf("%sCONDITION:\n", indent.c_str());
            print_statement(stmt->get_struct<While_S>()->condition, indent + '\t');
            printf("%sBODY:\n", indent.c_str());
            print_statement(stmt->get_struct<While_S>()->body, indent + '\t');
            break;
        case NodeType::FUNCTION:
            {
                auto func_struct = stmt->get_struct<Funciton_S>();

                printf("%sNAME: %s\n", indent.c_str(), func_struct->name.c_str());
                printf("%sPARAMETERS: [", indent.c_str());

                if (func_struct->paramList.size() == 0)
                {
                    printf("]\n");
                }
                else
                {
                    for (size_t i = 0; i < func_struct->paramList.size(); ++i)
                    {
                        if (i != func_struct->paramList.size() - 1)
                            printf("%s, ", func_struct->paramList[i].c_str());
                        else
                            printf("%s]\n", func_struct->paramList[i].c_str());
                    }
                }

                printf("%sBODY:\n", indent.c_str());
                print_statement(func_struct->body, indent + '\t');
            }
            break;
        case NodeType::RETURN:
            if (!stmt->is_struct_null())
                print_statement(stmt->get_struct<Return_S>()->expression, indent + '\t');
            break;
    }
}