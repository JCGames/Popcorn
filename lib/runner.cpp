#include "runner.hpp"
#include "diagnostics.hpp"

using namespace run;
using namespace obj;

FunctionPointer::FunctionPointer(std::string functionName, ast::Function* function)
{
    this->functionName = functionName;
    this->function = function;
}

VariablePointer::VariablePointer(std::string variableName, obj::Object object)
{
    this->variableName = variableName;
    this->object = object;
}

Scope::Scope()
{
    this->parent = nullptr;
}

Scope::Scope(Scope* parent)
{
    this->parent = parent;
}

void Scope::add_func(std::string name, ast::Function* function)
{
    for (auto func : functions)
    {
        if (name == func.functionName)
            Diagnostics::log_error("Function " + name + " has already been declared in this scope.");
    }

    functions.push_back(FunctionPointer(name, function));
}

ast::Function* Scope::get_func(std::string name)
{
    for (auto func : functions)
    {
        if (func.functionName == name)
        {
            return func.function;
        }
    }

    if (parent != nullptr)
        return parent->get_func(name);

    return nullptr;
}

void Scope::add_var(std::string name, obj::Object object)
{
    for (const auto& v : pointers)
    {
        if (v.variableName == name)
            Diagnostics::log_error("Variable " + name + " was already declared.");
    }

    pointers.push_back(VariablePointer(name, object));
}

bool Scope::has_var(std::string name)
{
    for (const auto& v : pointers)
    {
        if (v.variableName == name)
            return true;
    }

    if (parent != nullptr)
        return parent->has_var(name);

    return false;
}

obj::Object& Scope::get_var(std::string name)
{
    for (auto& v : pointers)
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

void Runner::create_function_lookup_table(ast::Block* block, Scope& scope)
{
    for (auto stmt : block->statements)
    {
        Diagnostics::info.lineNumber = stmt->get_line_index();

        if (stmt->get_type() == ast::StatementType::FUNCTION)
        {
            if (ast::Function* func = static_cast<ast::Function*>(stmt))
            {
                scope.add_func(func->functionName, func);
            }
        }
    }
}

Object Runner::call_function(ast::FunctionCall* funcCall, Scope& scope)
{
    if (funcCall->functionName == "printl")
    {
        if (funcCall->parameterList.size() == 1)
        {
            printf("%s\n", interpret(funcCall->parameterList[0], scope).cast_to_string().get_str().c_str());
        }
        else if (funcCall->parameterList.size() == 0)
        {
            printf("\n");
        }
        else
        {
            Diagnostics::log_error(funcCall->functionName + " only takes one argument. Or none.");
        }
    }
    else if (funcCall->functionName == "print")
    {
        if (funcCall->parameterList.size() == 1)
        {
            printf("%s", interpret(funcCall->parameterList[0], scope).cast_to_string().get_str().c_str());
        }
        else
        {
            Diagnostics::log_error(funcCall->functionName + " takes one argument.");
        }
    }
    else if (funcCall->functionName == "input")
    {
        if (funcCall->parameterList.size() == 0)
        {
            std::string line;
            getline(std::cin, line);
            return Object(DataType::STRING, new std::string(line));
        }
        else
        {
            Diagnostics::log_error(funcCall->functionName + " takes no arguments.");
        }
    }
    else if (funcCall->functionName == "int")
    {
        if (funcCall->parameterList.size() == 1)
        {
            return interpret(funcCall->parameterList[0]->root, scope).cast_to_int();
        }
        else
        {
            Diagnostics::log_error(funcCall->functionName + " takes one argument.");
        }
    }
    else if (funcCall->functionName == "double")
    {
        if (funcCall->parameterList.size() == 1)
        {
            return interpret(funcCall->parameterList[0]->root, scope).cast_to_double();
        }
        else
        {
            Diagnostics::log_error(funcCall->functionName + " takes one argument.");
        }
    }
    else if (funcCall->functionName == "bool")
    {
        if (funcCall->parameterList.size() == 1)
        {
            return interpret(funcCall->parameterList[0]->root, scope).cast_to_bool();
        }
        else
        {
            Diagnostics::log_error(funcCall->functionName + " takes one argument.");
        }
    }
    else if (funcCall->functionName == "string")
    {
        if (funcCall->parameterList.size() == 1)
        {
            return interpret(funcCall->parameterList[0]->root, scope).cast_to_string();
        }
        else
        {
            Diagnostics::log_error(funcCall->functionName + " takes one argument.");
        }
    }
    else if (ast::Function* func = scope.get_func(funcCall->functionName))
    {
        Scope functionScope(&scope);

        if (func->parameterNames.size() != funcCall->parameterList.size())
            Diagnostics::log_error("Function call " + func->functionName + " did not match the functions parameter list.");

        for (size_t i = 0; i < func->parameterNames.size(); ++i)
        {
            functionScope.add_var(func->parameterNames[i], interpret(funcCall->parameterList[i]->root, scope));
        }

        create_function_lookup_table(func->body, functionScope);

        for (const auto& stmt : func->body->statements)
        {
            if (stmt->get_type() == ast::StatementType::RETURN)
            {
                if (auto x = static_cast<ast::Return*>(stmt))
                {
                    return interpret(x->expression, functionScope);
                }
            }

            interpret(stmt, functionScope);
        }

        return Object();
    }
    else
    {
        Diagnostics::log_error("Function " + funcCall->functionName + " has not been declared.");
    }

    return Object();
}

Object Runner::interpret(ast::Statement* stmt, Scope& scope)
{
    Diagnostics::info.lineNumber = stmt->get_line_index();

    switch (stmt->get_type())
    {
        /**
         * Objects
        */
        case ast::StatementType::INTEGER:
            if (auto integer = static_cast<ast::Integer*>(stmt))
            {
                return Object(DataType::INTEGER, new int(integer->value));
            }
            break;
        case ast::StatementType::DOUBLE:
            if (auto _double = static_cast<ast::Double*>(stmt))
            {
                return Object(DataType::DOUBLE, new double(_double->value));
            }
            break;
        case ast::StatementType::BOOLEAN:
            if (auto boolean = static_cast<ast::Boolean*>(stmt))
            {
                return Object(DataType::BOOLEAN, new bool(boolean->value));
            }
            break;
        case ast::StatementType::STRING:
            if (auto _string = static_cast<ast::String*>(stmt))
            {
                return Object(DataType::STRING, new std::string(_string->value));
            }
            break;
        case ast::StatementType::VARIABLE:
            if (auto variable = static_cast<ast::Variable*>(stmt))
            {
                return scope.get_var(variable->name);
            }
            break;
        case ast::StatementType::EXPRESSION:
            if (auto expression = static_cast<ast::Expression*>(stmt))
            {
                return interpret(expression->root, scope);
            }
            break;

        /**
         * Operators
        */
        case ast::StatementType::ADD_OPERATOR:
            if (auto x = static_cast<ast::AddOperator*>(stmt))
            {
                return interpret(x->left, scope).add_to(interpret(x->right, scope));
            }
            break;
        case ast::StatementType::SUB_OPERATOR:
            if (auto x = static_cast<ast::SubtractOperator*>(stmt))
            {
                return interpret(x->left, scope).subtract_from(interpret(x->right, scope));
            }
            break;
        case ast::StatementType::MUL_OPERATOR:
            if (auto x = static_cast<ast::MultiplyOperator*>(stmt))
            {
                return interpret(x->left, scope).multiplied_by(interpret(x->right, scope));
            }
            break;
        case ast::StatementType::DIV_OPERATOR:
            if (auto x = static_cast<ast::DivideOperator*>(stmt))
            {
                return interpret(x->left, scope).divided_by(interpret(x->right, scope));
            }
            break;
        case ast::StatementType::MODULUS_OPERATOR:
            if (auto x = static_cast<ast::ModulusOperator*>(stmt))
            {
                return interpret(x->left, scope).modulus_by(interpret(x->right, scope));
            }
            break;
        case ast::StatementType::NEGATE:
            if (auto x = static_cast<ast::Negate*>(stmt))
            {
                return interpret(x->value, scope).negate();
            }
            break;
        case ast::StatementType::EQUALS_OPERATOR:
            if (auto x = static_cast<ast::EqualsOperator*>(stmt))
            {
                return interpret(x->left, scope).equals(interpret(x->right, scope));
            }
            break;
        case ast::StatementType::NOT_EQUALS_OPERATOR:
            if (auto x = static_cast<ast::NotEqualsOperator*>(stmt))
            {
                return interpret(x->left, scope).not_equal_to(interpret(x->right, scope));
            }
            break;
        case ast::StatementType::GREATER_THAN_OPERATOR:
            if (auto x = static_cast<ast::GreaterThanOperator*>(stmt))
            {
                return interpret(x->left, scope).greater_than(interpret(x->right, scope));
            }
            break;
        case ast::StatementType::LESS_THAN_OPERATOR:
            if (auto x = static_cast<ast::LessThanOperator*>(stmt))
            {
                return interpret(x->left, scope).less_than(interpret(x->right, scope));
            }
            break;
        case ast::StatementType::GREATER_THAN_EQUALS_OPERATOR:
            if (auto x = static_cast<ast::GreaterThanEqualsOperator*>(stmt))
            {
                return interpret(x->left, scope).greater_than_or_equal_to(interpret(x->right, scope));
            }
            break;
        case ast::StatementType::LESS_THAN_EQUALS_OPERATOR:
            if (auto x = static_cast<ast::LessThanEqualsOperator*>(stmt))
            {
                return interpret(x->left, scope).less_than_or_equal_to(interpret(x->right, scope));
            }
            break;
        case ast::StatementType::POWER_OPERATOR:
            if (auto x = static_cast<ast::PowerOperator*>(stmt))
            {
                return interpret(x->left, scope).power(interpret(x->right, scope));
            }
            break;

        /**
         * Operations
        */
        case ast::StatementType::VARIABLE_ASSIGNMENT:
            if (auto x = static_cast<ast::VariableAssignment*>(stmt))
            {
                if (!scope.has_var(x->variableName))
                    scope.add_var(x->variableName, interpret(x->expression, scope));
                else
                    scope.get_var(x->variableName) = interpret(x->expression, scope);
            }
            break;
        case ast::StatementType::FUNCTION_CALL:
            if (auto x = static_cast<ast::FunctionCall*>(stmt))
            {
                return call_function(x, scope);
            }
            break;
        case ast::StatementType::WHILE:
            if (auto x = static_cast<ast::While*>(stmt))
            {
                Object result = interpret(x->condition->root, scope);

                while (true)
                {
                    if (result.get_type() != DataType::BOOLEAN)
                        Diagnostics::log_warning("While loop did not recieve a boolean value.");

                    if (result.get_type() == DataType::BOOLEAN && result.get_bool() != true)
                        break;

                    Scope whileScope(&scope);

                    for (const auto& s : x->body->statements)
                    {
                        if (s->get_type() == ast::StatementType::FUNCTION)
                            Diagnostics::log_error("Functions cannot be declared within while loops.");

                        interpret(s, whileScope);
                    }

                    result = interpret(x->condition->root, scope);
                }
            }
            break;
        case ast::StatementType::IF:
            if (auto x = static_cast<ast::If*>(stmt))
            {
                Object result = interpret(x->condition->root, scope);

                Scope ifScope(&scope);

                if (result.get_type() != DataType::BOOLEAN)
                    Diagnostics::log_warning("If statement did not recieve a boolean value.");

                if (result.get_type() == DataType::BOOLEAN && result.get_bool() == true)
                {
                    for (const auto& s : x->body->statements) 
                    {
                        if (s->get_type() == ast::StatementType::FUNCTION)
                            Diagnostics::log_error("Functions cannot be declared within if statements.");

                        interpret(s, ifScope);
                    }
                }
                else if (x->elseOrIf != nullptr)
                {
                    interpret(x->elseOrIf, scope);
                }
            }
            break;
        case ast::StatementType::ELSE:
            if (auto x = static_cast<ast::Else*>(stmt))
            {
                Scope elseScope(&scope);

                for (const auto& s : x->body->statements) 
                {
                    if (s->get_type() == ast::StatementType::FUNCTION)
                        Diagnostics::log_error("Functions cannot be declared within else statements.");

                    interpret(s, elseScope);
                }
            }
            break;
        case ast::StatementType::FUNCTION:
            break;
        
        default:
            Diagnostics::log_warning("Could not interpret statement " + ast::get_statement_type_name(stmt->get_type()) + ".");
            break;
    }

    return Object();
}

void Runner::run(ast::AST& ast)
{
    create_function_lookup_table(ast.root, _rootScope);

    for (auto stat : ast.root->statements)
        interpret(stat, _rootScope);
}