#include "runner.hpp"

Runner::Runner()
{

}

Object Runner::run_func(FunctionCall* funcCall)
{
    if (funcCall->functionName == "print")
    {
        if (funcCall->parameterList.size() != 1)
            throw std::runtime_error("Function [print] only takes one argument!");

        std::cout << cast_to_string(interpret(funcCall->parameterList[0])) << std::endl;
    }
    else if (funcCall->functionName == "int")
    {
        if (funcCall->parameterList.size() != 1)
            throw std::runtime_error("Function [int] only takes one argument!");

        return cast_to_int(interpret(funcCall->parameterList[0]));
    }

    return Object();
}

Object Runner::eval_binary_operator(BinaryOperator* binaryOperator)
{
    switch (binaryOperator->get_type())
    {
        case StatementType::ADD_OPERATOR:
            if (AddOperator* addOp = static_cast<AddOperator*>(binaryOperator))
            {
                Object left = interpret(addOp->left);
                Object right = interpret(addOp->right);

                if (left.get_type() == ObjectType::STRING || right.get_type() == ObjectType::STRING)
                {

                }
                else if (left.doubleValue)
                {

                }
            }
            break;
        case StatementType::SUB_OPERATOR:
            if (SubtractOperator* subtractOp = static_cast<SubtractOperator*>(binaryOperator))
            {
                Object left = interpret(subtractOp->left);
                Object right = interpret(subtractOp->right);
            }
            break;
        case StatementType::MUL_OPERATOR:
            if (MultiplyOperator* multiplyOp = static_cast<MultiplyOperator*>(binaryOperator))
            {
                Object left = interpret(multiplyOp->left);
                Object right = interpret(multiplyOp->right);
            }
            break;
        case StatementType::DIV_OPERATOR:
            if (DivideOperator* divideOp = static_cast<DivideOperator*>(binaryOperator))
            {
                Object left = interpret(divideOp->left);
                Object right = interpret(divideOp->right);
            }
            break;
    }

    return Object();
}

Object Runner::cast_to_int(Object obj)
{
    if (obj.is_number())
    {
        switch (obj.get_type())
        {
            case ObjectType::BOOLEAN:
                return Object((int)obj.intValue);
            case ObjectType::INTEGER:
                return Object(obj.intValue);
            case ObjectType::DOUBLE:
                return Object((int)obj.doubleValue);
        }
    }
    else
    {
        throw std::runtime_error("Cannot cast a non-number to an integer!");
    }

    return Object(0);
}

std::string Runner::cast_to_string(Object obj)
{
    switch (obj.get_type())
    {
        case ObjectType::STRING:
            return obj.strValue;
        case ObjectType::BOOLEAN:
            return obj.boolValue ? "TRUE" : "FALSE";
        case ObjectType::INTEGER:
            return std::to_string(obj.intValue);
        case ObjectType::DOUBLE:
            return std::to_string(obj.doubleValue);
    }

    return "NULL";
}

Object Runner::interpret(Statement* stat)
{
    switch (stat->get_type())
    {
        case StatementType::NUMBER:
            if (Number* num = static_cast<Number*>(stat))
            {
                return Object(num->value);
            }
            break;
        case StatementType::STRING:
            if (String* str = static_cast<String*>(stat))
            {
                return Object(str->value);
            }
            break;
        case StatementType::EXPRESSION:
            if (Expression* expression = static_cast<Expression*>(stat))
            {
                return interpret(expression->root);
            }
            break;
        case StatementType::VARIABLE:
            if (Variable* v = static_cast<Variable*>(stat))
            {
                return get_variable(v->name).object;
            }
            break;
        case StatementType::VARIABLE_ASSIGNMENT:
            if (VariableAssignment* va = static_cast<VariableAssignment*>(stat))
            {
                add_variable(_Variable(va->variableName, interpret(va->expression)));
            }
            break;
        case StatementType::FUNCTION_CALL:
            if (FunctionCall* funcCall = static_cast<FunctionCall*>(stat))
            {
                return run_func(funcCall);
            }
            break;
        case StatementType::BINARY_OPERATOR:
            if (BinaryOperator* binaryOp = static_cast<BinaryOperator*>(stat))
            {
                return eval_binary_operator(binaryOp);
            }
            break;
    }

    return Object();
}

void Runner::add_variable(_Variable variable)
{
    for (const auto& v : _variables)
    {
        if (v.name == variable.name)
            throw std::runtime_error("Variable " + variable.name + " has already been declared!");
    }

    _variables.push_back(variable);
}

void Runner::remove_variable(_Variable variable)
{
    for (auto iter = _variables.begin(); iter != _variables.end(); ++iter)
    {
        if (iter->name == variable.name)
            _variables.erase(iter);
    }

    throw std::runtime_error("Variable " + variable.name + " has lost scope or was never declared!");
}

_Variable Runner::get_variable(std::string name)
{
    for (auto v : _variables)
    {
        if (v.name == name)
            return v;
    }

    throw std::runtime_error("Variable " + name + " has lost scope or was never declared!");
}

bool Runner::has_variable(std::string name)
{    
    for (const auto& v : _variables)
    {
        if (v.name == name)
            return true;
    }

    return false;
}

void Runner::run(AST& ast)
{
    for (auto stat : ast.statements)
        interpret(stat);
}