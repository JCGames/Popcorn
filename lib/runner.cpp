#include "runner.hpp"
#include "diagnostics.hpp"

using namespace popcorn::runner;
using namespace popcorn::diagnostics;
using namespace popcorn::parser;

Runner::Runner()
{
    Diagnostics::info = DiagnosticInfo(DiagnosticState::_RUNNER);
}

void Runner::create_function_lookup_table(Node* block, Scope& scope)
{
    for (auto stmt : block->get_struct<Block_S>()->statements)
    {
        Diagnostics::info.lineNumber = stmt->get_line_index();

        if (stmt->get_type() == NodeType::FUNCTION)
            scope.add_func(stmt->get_struct<Funciton_S>()->name, stmt);
    }
}

Object Runner::call_function(Node* funcCall, Scope& scope)
{
    auto fc_struct = funcCall->get_struct<FunctionCall_S>();
    std::string functionName = fc_struct->functionName;
    auto children = fc_struct->paramValues;

    /**
     * USER DEFINED FUNCTIONS
    */
    if (Node* func = scope.get_func(functionName))
    {
        if (func->get_type() != NodeType::FUNCTION)
            throw std::runtime_error("Not a function!");

        auto func_struct = func->get_struct<Funciton_S>();

        // declare a scope and set up the scope's parameters
        Scope functionScope(&scope);
        functionScope.canNestFunctions = true;

        // sets up function's local passed in variables
        if (func_struct->paramList.size() != children.size())
            Diagnostics::log_error("Function call " + functionName + " did not match the functions parameter list.");

        for (size_t i = 0; i < func_struct->paramList.size(); ++i)
            functionScope.add_var(func_struct->paramList[i], interpret(children[i]->get_struct<Expression_S>()->root, scope));

        // allows for nesting functions within functions
        create_function_lookup_table(func_struct->body, functionScope);

        // runs the function
        return run_block(func_struct->body, functionScope);
    }
    /**
     * LANGUAGE DEFINED FUNCTIONS
    */
    else if (functionName == "printl")
    {
        if (children.size() == 1)
            printf("%s\n", interpret(children[0]->get_struct<Expression_S>()->root, scope).cast_to_string().get_str().c_str());
        else if (children.size() == 0)
            printf("\n");
        else
            Diagnostics::log_error(functionName + " only takes one argument. Or none.");
    }
    else if (functionName == "print")
    {
        if (children.size() == 1)
            printf("%s", interpret(children[0]->get_struct<Expression_S>()->root, scope).cast_to_string().get_str().c_str());
        else
            Diagnostics::log_error(functionName + " takes one argument.");
    }
    else if (functionName == "input")
    {
        if (children.size() == 0)
        {
            std::string line;
            getline(std::cin, line);
            return Object(DataType::STRING, new std::string(line));
        }
        else
        {
            Diagnostics::log_error(functionName + " takes no arguments.");
        }
    }
    else if (functionName == "int")
    {
        if (children.size() == 1)
            return interpret(children[0]->get_struct<Expression_S>()->root, scope).cast_to_int();
        else
            Diagnostics::log_error(functionName + " takes one argument.");
    }
    else if (functionName == "double")
    {
        if (children.size() == 1)
            return interpret(children[0]->get_struct<Expression_S>()->root, scope).cast_to_double();
        else
            Diagnostics::log_error(functionName + " takes one argument.");
    }
    else if (functionName == "bool")
    {
        if (children.size() == 1)
            return interpret(children[0]->get_struct<Expression_S>()->root, scope).cast_to_bool();
        else
            Diagnostics::log_error(functionName + " takes one argument.");
    }
    else if (functionName == "string")
    {
        if (children.size() == 1)
            return interpret(children[0]->get_struct<Expression_S>()->root, scope).cast_to_string();
        else
            Diagnostics::log_error(functionName + " takes one argument.");
    }
    /**
     * FUNCTION DOES NOT EXIST
    */
    else
    {
        Diagnostics::log_error("Function " + functionName + " has not been declared.");
    }

    return Object();
}

Object Runner::interpret(Node* node, Scope& scope)
{
    Diagnostics::info.lineNumber = node->get_line_index();

    switch (node->get_type())
    {
        /**
         * Objects
        */
        case NodeType::INTEGER:
            return Object(DataType::INTEGER, new int(*node->get_struct<int>()));
        case NodeType::DOUBLE:
            return Object(DataType::DOUBLE, new double(*node->get_struct<double>()));
        case NodeType::BOOLEAN:
            return Object(DataType::BOOLEAN, new bool(*node->get_struct<bool>()));
        case NodeType::STRING:
            return Object(DataType::STRING, new std::string(*node->get_struct<std::string>()));
        case NodeType::VARIABLE:
            return scope.get_var(*node->get_struct<std::string>());
        case NodeType::EXPRESSION:
            return interpret(node->get_struct<Expression_S>()->root, scope);

        /**
         * Operators
        */
        // Binary operators
        case NodeType::ADD_OPERATOR:
            return interpret(node->get_struct<BinaryOperator_S>()->left, scope)
                .add_to(interpret(node->get_struct<BinaryOperator_S>()->right, scope));
        case NodeType::SUB_OPERATOR:
            return interpret(node->get_struct<BinaryOperator_S>()->left, scope)
                .subtract_from(interpret(node->get_struct<BinaryOperator_S>()->right, scope));
        case NodeType::MUL_OPERATOR:
            return interpret(node->get_struct<BinaryOperator_S>()->left, scope)
                .multiplied_by(interpret(node->get_struct<BinaryOperator_S>()->right, scope));
        case NodeType::DIV_OPERATOR:
            return interpret(node->get_struct<BinaryOperator_S>()->left, scope)
                .divided_by(interpret(node->get_struct<BinaryOperator_S>()->right, scope));
        case NodeType::MODULUS_OPERATOR:
            return interpret(node->get_struct<BinaryOperator_S>()->left, scope)
                .modulus_by(interpret(node->get_struct<BinaryOperator_S>()->right, scope));
        case NodeType::EQUALS_OPERATOR:
            return interpret(node->get_struct<BinaryOperator_S>()->left, scope)
                .equals(interpret(node->get_struct<BinaryOperator_S>()->right, scope));
        case NodeType::NOT_EQUALS_OPERATOR:
            return interpret(node->get_struct<BinaryOperator_S>()->left, scope)
                .not_equal_to(interpret(node->get_struct<BinaryOperator_S>()->right, scope));
        case NodeType::GREATER_THAN_OPERATOR:
            return interpret(node->get_struct<BinaryOperator_S>()->left, scope)
                .greater_than(interpret(node->get_struct<BinaryOperator_S>()->right, scope));
        case NodeType::LESS_THAN_OPERATOR:
            return interpret(node->get_struct<BinaryOperator_S>()->left, scope)
                .less_than(interpret(node->get_struct<BinaryOperator_S>()->right, scope));
        case NodeType::GREATER_THAN_EQUALS_OPERATOR:
            return interpret(node->get_struct<BinaryOperator_S>()->left, scope)
                .greater_than_or_equal_to(interpret(node->get_struct<BinaryOperator_S>()->right, scope));
        case NodeType::LESS_THAN_EQUALS_OPERATOR:
            return interpret(node->get_struct<BinaryOperator_S>()->left, scope)
                .less_than_or_equal_to(interpret(node->get_struct<BinaryOperator_S>()->right, scope));
        case NodeType::POWER_OPERATOR:
            return interpret(node->get_struct<BinaryOperator_S>()->left, scope)
                .power(interpret(node->get_struct<BinaryOperator_S>()->right, scope));
        
        // Unary operators
        case NodeType::NEGATE:
            return interpret(node->get_struct<UnaryOperator_S>()->child, scope).negate();

        /**
         * Operations
        */
        case NodeType::VARIABLE_ASSIGNMENT:
            {
                auto* va_struct = node->get_struct<VariableAssignment_S>();
                
                if (!scope.has_var(*va_struct->variable->get_struct<std::string>()))
                    scope.add_var(*va_struct->variable->get_struct<std::string>(), interpret(va_struct->expression, scope));
                else
                    scope.get_var(*va_struct->variable->get_struct<std::string>()) = interpret(va_struct->expression, scope);
            }
            break;

        case NodeType::FUNCTION_CALL:
            return call_function(node, scope);

        case NodeType::WHILE:
            {
                auto while_struct = node->get_struct<While_S>();
                Object conditionResult = interpret(while_struct->condition, scope);

                while (true)
                {
                    if (conditionResult.get_type() != DataType::BOOLEAN)
                        Diagnostics::log_warning("While loop did not recieve a boolean value.");

                    // if the result of the last run was false do not continue
                    if (conditionResult.get_type() == DataType::BOOLEAN && !conditionResult.get_bool())
                        break;

                    Scope whileScope(&scope);
                    Object stmtValue = run_block(while_struct->body, whileScope);
                    
                    if (whileScope.returnFlag)
                    {
                        scope.returnFlag = true;
                        return stmtValue;
                    }
                    else if (whileScope.breakFlag)
                    {
                        whileScope.breakFlag = true;
                        return Object();
                    }

                    // checks to see if the while loop should continue
                    conditionResult = interpret(while_struct->condition, scope);
                }
            }
            break;

        case NodeType::IF:
            {
                auto if_struct = node->get_struct<If_S>();
                Object conditionResult = interpret(if_struct->condition, scope);

                if (conditionResult.get_type() != DataType::BOOLEAN)
                    Diagnostics::log_warning("If statement did not recieve a boolean value.");

                Scope ifScope(&scope);

                if (conditionResult.get_type() == DataType::BOOLEAN && conditionResult.get_bool()) 
                {
                    Object stmtValue = run_block(if_struct->body, ifScope);
                    
                    if (ifScope.returnFlag)
                    {
                        scope.returnFlag = true;
                        return stmtValue;
                    }
                    else if (ifScope.breakFlag)
                    {
                        scope.breakFlag = true;
                        return Object();
                    }
                }
                else if (if_struct->branchingElseOrIf != nullptr)
                {
                    return interpret(if_struct->branchingElseOrIf, scope);
                }
            }
            break;

        case NodeType::ELSE:
            {
                Scope elseScope(&scope);
                Object stmtValue = run_block(node->get_struct<Else_S>()->body, elseScope);

                if (elseScope.returnFlag)
                {
                    scope.returnFlag = true;
                    return stmtValue;
                }
                else if (elseScope.breakFlag)
                {
                    scope.breakFlag = true;
                    return Object();
                }
            }
            break;

        case NodeType::RETURN:
            scope.returnFlag = true;
            return interpret(node->get_struct<Return_S>()->expression, scope);

        case NodeType::BREAK:
            scope.breakFlag = true;
            break;

        case NodeType::FUNCTION:
            break;
        
        default:
            Diagnostics::log_warning("Could not interpret statement " + get_node_type_name(node->get_type()) + ".");
            break;
    }

    return Object();
}

Object Runner::run_block(Node* block, Scope& scope)
{
    for (const auto& s : block->get_struct<Block_S>()->statements)
    {
        if (!scope.canNestFunctions && s->get_type() == NodeType::FUNCTION)
            Diagnostics::log_error("Nested functions can only be nested in other functions.");

        Object stmtValue = interpret(s, scope);

        if (scope.returnFlag)
            return stmtValue;
        else if (scope.breakFlag)
            return Object();
    }

    return Object();
}

void Runner::run(AST& ast)
{
    create_function_lookup_table(ast.root, _rootScope);

    for (auto stat : ast.root->get_struct<Block_S>()->statements)
        interpret(stat, _rootScope);
}