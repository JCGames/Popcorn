#include "scope.hpp"

#include "diagnostics.hpp"

using namespace popcorn::diagnostics;
using namespace popcorn::parser;
using namespace popcorn::runner;

FunctionPointer::FunctionPointer(std::string functionName, Node* function)
{
    this->functionName = functionName;
    this->function = function;
}

VariablePointer::VariablePointer(std::string variableName, Object object)
{
    this->variableName = variableName;
    this->object = object;
}

Scope::Scope()
{
    this->parent = nullptr;
    this->canNestFunctions = false;
    this->returnFlag = false;
    this->breakFlag = false;
}

Scope::Scope(Scope* parent)
{
    this->parent = parent;
    this->canNestFunctions = false;
    this->returnFlag = false;
    this->breakFlag = false;
}

void Scope::add_func(std::string name, Node* function)
{
    for (auto func : functions)
    {
        if (name == func.functionName)
            Diagnostics::log_error("Function " + name + " has already been declared in this scope.");
    }

    functions.push_back(FunctionPointer(name, function));
}

Node* Scope::get_func(std::string name)
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

void Scope::add_var(std::string name, Object object)
{
    for (const auto& v : variables)
    {
        if (v.variableName == name)
            Diagnostics::log_error("Variable " + name + " was already declared.");
    }

    variables.push_back(VariablePointer(name, object));
}

bool Scope::has_var(std::string name)
{
    for (const auto& v : variables)
    {
        if (v.variableName == name)
            return true;
    }

    if (parent != nullptr)
        return parent->has_var(name);

    return false;
}