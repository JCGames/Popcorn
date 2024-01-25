#include "runner.hpp"
#include "diagnostics.hpp"

using namespace run;
using namespace obj;

obj::Object& Scope::get_var(std::string name)
{
    for (auto& v : variables)
    {
        if (v.variableName == name)
            return v.object;
    }

    if (parent != nullptr)
        return parent->get_var(name);

    Diagnostics::log_error("Variable " + name + " was never declared.");
    throw std::runtime_error("Variable " + name + " was never declared.");
}

Runner::Runner()
{
    Diagnostics::info = DiagnosticInfo(DiagnosticState::_RUNNER);
}

void Runner::create_function_lookup_table(ast::Node* block, Scope& scope)
{
    for (auto stmt : block->get_children())
    {
        Diagnostics::info.lineNumber = stmt->get_line_index();

        if (stmt->get_type() == ast::NodeType::FUNCTION)
        {
            scope.add_func((stmt->get_value<ast::FunctionData>()).functionName, stmt);
        }
    }
}

Object Runner::call_function(ast::Node* funcCall, Scope& scope)
{
    std::string functionName = funcCall->get_value<std::string>();
    auto children = funcCall->get_children();

    /**
     * LANGUAGE DEFINED FUNCTIONS
    */
    if (functionName == "printl")
    {
        if (children.size() == 1)
            printf("%s\n", interpret(children[0]->get_child(0), scope).cast_to_string().get_str().c_str());
        else if (children.size() == 0)
            printf("\n");
        else
            Diagnostics::log_error(functionName + " only takes one argument. Or none.");
    }
    else if (functionName == "print")
    {
        if (children.size() == 1)
            printf("%s", interpret(children[0]->get_child(0), scope).cast_to_string().get_str().c_str());
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
            return interpret(children[0]->get_child(0), scope).cast_to_int();
        else
            Diagnostics::log_error(functionName + " takes one argument.");
    }
    else if (functionName == "double")
    {
        if (children.size() == 1)
            return interpret(children[0]->get_child(0), scope).cast_to_double();
        else
            Diagnostics::log_error(functionName + " takes one argument.");
    }
    else if (functionName == "bool")
    {
        if (children.size() == 1)
            return interpret(children[0]->get_child(0), scope).cast_to_bool();
        else
            Diagnostics::log_error(functionName + " takes one argument.");
    }
    else if (functionName == "string")
    {
        if (children.size() == 1)
            return interpret(children[0]->get_child(0), scope).cast_to_string();
        else
            Diagnostics::log_error(functionName + " takes one argument.");
    }
    /**
     * CUSTOM FUNCTIONS
    */
    else if (ast::Node* func = scope.get_func(functionName))
    {
        if (func->get_type() != ast::NodeType::FUNCTION)
            throw std::runtime_error("Not a function!");

        auto funcInfo = func->get_value<ast::FunctionData>();

        // declare a scope and set up the scope's parameters
        Scope functionScope(&scope);
        functionScope.canNestFunctions = true;

        // sets up function's local passed in variables
        if (funcInfo.paramNames.size() != children.size())
            Diagnostics::log_error("Function call " + functionName + " did not match the functions parameter list.");

        for (size_t i = 0; i < funcInfo.paramNames.size(); ++i)
            functionScope.add_var(funcInfo.paramNames[i], interpret(children[i]->get_child(0), scope));

        // allows for nesting functions within functions
        create_function_lookup_table(func->get_child(0), functionScope);

        // runs the function
        run_block(func->get_child(0), functionScope);
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

Object Runner::interpret(ast::Node* stmt, Scope& scope)
{
    Diagnostics::info.lineNumber = stmt->get_line_index();

    switch (stmt->get_type())
    {
        /**
         * Objects
        */
        case ast::NodeType::INTEGER:
            return Object(DataType::INTEGER, new int(stmt->get_value<int>()));
        case ast::NodeType::DOUBLE:
            return Object(DataType::DOUBLE, new double(stmt->get_value<double>()));
        case ast::NodeType::BOOLEAN:
            return Object(DataType::BOOLEAN, new bool(stmt->get_value<bool>()));
        case ast::NodeType::STRING:
            return Object(DataType::STRING, new std::string(stmt->get_value<std::string>()));
        case ast::NodeType::VARIABLE:
            return scope.get_var(stmt->get_value<std::string>());
        case ast::NodeType::EXPRESSION:
            return interpret(stmt->get_child(0), scope);

        /**
         * Operators
        */
       // Binary operators
        case ast::NodeType::ADD_OPERATOR:
            return interpret(stmt->get_child(0), scope).add_to(interpret(stmt->get_child(1), scope));
        case ast::NodeType::SUB_OPERATOR:
            return interpret(stmt->get_child(0), scope).subtract_from(interpret(stmt->get_child(1), scope));
        case ast::NodeType::MUL_OPERATOR:
            return interpret(stmt->get_child(0), scope).multiplied_by(interpret(stmt->get_child(1), scope));
        case ast::NodeType::DIV_OPERATOR:
            return interpret(stmt->get_child(0), scope).divided_by(interpret(stmt->get_child(1), scope));
        case ast::NodeType::MODULUS_OPERATOR:
            return interpret(stmt->get_child(0), scope).modulus_by(interpret(stmt->get_child(1), scope));
        case ast::NodeType::EQUALS_OPERATOR:
            return interpret(stmt->get_child(0), scope).equals(interpret(stmt->get_child(1), scope));
        case ast::NodeType::NOT_EQUALS_OPERATOR:
            return interpret(stmt->get_child(0), scope).not_equal_to(interpret(stmt->get_child(1), scope));
        case ast::NodeType::GREATER_THAN_OPERATOR:
            return interpret(stmt->get_child(0), scope).greater_than(interpret(stmt->get_child(1), scope));
        case ast::NodeType::LESS_THAN_OPERATOR:
            return interpret(stmt->get_child(0), scope).less_than(interpret(stmt->get_child(1), scope));
        case ast::NodeType::GREATER_THAN_EQUALS_OPERATOR:
            return interpret(stmt->get_child(0), scope).greater_than_or_equal_to(interpret(stmt->get_child(1), scope));
        case ast::NodeType::LESS_THAN_EQUALS_OPERATOR:
            return interpret(stmt->get_child(0), scope).less_than_or_equal_to(interpret(stmt->get_child(1), scope));
        case ast::NodeType::POWER_OPERATOR:
            return interpret(stmt->get_child(0), scope).power(interpret(stmt->get_child(1), scope));
        
        // Unary operators
        case ast::NodeType::NEGATE:
            return interpret(stmt->get_child(0), scope).negate();

        /**
         * Operations
        */
        case ast::NodeType::VARIABLE_ASSIGNMENT:
            {
                auto name = stmt->get_value<std::string>();

                if (!scope.has_var(name))
                    scope.add_var(name, interpret(stmt->get_child(0), scope));
                else
                    scope.get_var(name) = interpret(stmt->get_child(0), scope);
            }
            break;

        case ast::NodeType::FUNCTION_CALL:
            return call_function(stmt, scope);

        case ast::NodeType::WHILE:
            {
                auto condition = stmt->get_child(0)->get_child(0);
                Object conditionResult = interpret(condition, scope);

                while (true)
                {
                    if (conditionResult.get_type() != DataType::BOOLEAN)
                        Diagnostics::log_warning("While loop did not recieve a boolean value.");

                    // if the result of the last run was false do not continue
                    if (conditionResult.get_type() == DataType::BOOLEAN && !conditionResult.get_bool())
                        break;

                    Scope whileScope(&scope);
                    Object stmtValue = run_block(stmt->get_child(1), whileScope);

                    if (whileScope.returnFlag)
                    {
                        scope.returnFlag = true;
                        return stmtValue;
                    }
                    else if (whileScope.breakFlag)
                    {
                        return Object();
                    }

                    // checks to see if the while loop should continue
                    conditionResult = interpret(condition, scope);
                }
            }
            break;

        case ast::NodeType::IF:
            {
                Object conditionResult = interpret(stmt->get_child(0)->get_child(0), scope);

                if (conditionResult.get_type() != DataType::BOOLEAN)
                    Diagnostics::log_warning("If statement did not recieve a boolean value.");

                Scope ifScope(&scope);

                if (conditionResult.get_type() == DataType::BOOLEAN && conditionResult.get_bool()) 
                {
                    Object stmtValue = run_block(stmt->get_child(1), ifScope);
                    
                    if (ifScope.returnFlag)
                    {
                        scope.returnFlag = true;
                        return stmtValue;
                    }
                }
                else if (stmt->get_child(2) != nullptr)
                {
                    return interpret(stmt->get_child(2), scope);
                }
            }
            break;

        case ast::NodeType::ELSE:
            {
                Scope elseScope(&scope);
                Object stmtValue = run_block(stmt->get_child(0), elseScope);

                if (elseScope.returnFlag)
                {
                    scope.returnFlag = true;
                    return stmtValue;
                }
            }
            break;

        case ast::NodeType::RETURN:
            scope.returnFlag = true;
            return interpret(stmt->get_child(0)->get_child(0), scope);
            break;

        case ast::NodeType::BREAK:
            scope.breakFlag = true;
            break;

        case ast::NodeType::FUNCTION:
            break;
        
        default:
            Diagnostics::log_warning("Could not interpret statement " + ast::get_statement_type_name(stmt->get_type()) + ".");
            break;
    }

    return Object();
}

Object Runner::run_block(ast::Node* block, Scope& scope)
{
    for (const auto& s : block->get_children())
    {
        if (!scope.canNestFunctions && s->get_type() == ast::NodeType::FUNCTION)
            Diagnostics::log_error("Nested functions can only be nested in other functions.");

        Object stmtValue = interpret(s, scope);

        if (scope.returnFlag)
            return stmtValue;
        else if (scope.breakFlag)
            return Object();
    }

    return Object();
}

void Runner::run(ast::AST& ast)
{
    create_function_lookup_table(ast.root, _rootScope);

    for (auto stat : ast.root->get_children())
        interpret(stat, _rootScope);
}