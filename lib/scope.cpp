#include "scope.hpp"

#include "diagnostics.hpp"

using namespace popcorn::runner;
using namespace popcorn::diagnostics;
using namespace popcorn::parser;

FunctionPointer::FunctionPointer(Function function)
{
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
    for (auto funcPtr : functions)
    {
        if (name == funcPtr.function.name)
            Diagnostics::log_error("Function " + name + " has already been declared in this scope.");
    }

    functions.push_back(Function(name, function));
}

Node* Scope::get_func(std::string name)
{
    for (auto funcPtr : functions)
    {
        if (funcPtr.function.name == name)
        {
            return funcPtr.function.root;
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

Object& Scope::get_var(std::string name)
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