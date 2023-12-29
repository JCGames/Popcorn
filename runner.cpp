#include "runner.hpp"

Runner::Runner()
{

}

Object Runner::run_func(ast::FunctionCall* funcCall)
{
    if (funcCall->functionName == "print")
    {
        if (funcCall->parameterList.size() != 1)
            throw std::runtime_error("Function [print] only takes one argument!");

        std::cout << interpret(funcCall->parameterList[0]).cast_to_string().strValue << std::endl;
    }
    else if (funcCall->functionName == "int")
    {
        if (funcCall->parameterList.size() != 1)
            throw std::runtime_error("Function [int] only takes one argument!");

        return interpret(funcCall->parameterList[0]).cast_to_int();
    }
    else if (funcCall->functionName == "bool")
    {
        if (funcCall->parameterList.size() != 1)
            throw std::runtime_error("Function [bool] only takes one argument!");
        
        return interpret(funcCall->parameterList[0]).cast_to_bool();
    }

    return Object();
}

Object Runner::eval_binary_operator(ast::BinaryOperator* binaryOperator)
{
    switch (binaryOperator->get_type())
    {
        case ast::StatementType::ADD_OPERATOR:
            if (ast::AddOperator* addOp = static_cast<ast::AddOperator*>(binaryOperator))
            {
                Object left = interpret(addOp->left);
                Object right = interpret(addOp->right);

                return left.add_to(right);
            }
            break;
        case ast::StatementType::SUB_OPERATOR:
            if (ast::SubtractOperator* subtractOp = static_cast<ast::SubtractOperator*>(binaryOperator))
            {
                Object left = interpret(subtractOp->left);
                Object right = interpret(subtractOp->right);

                return left.subtract_from(right);
            }
            break;
        case ast::StatementType::MUL_OPERATOR:
            if (ast::MultiplyOperator* multiplyOp = static_cast<ast::MultiplyOperator*>(binaryOperator))
            {
                Object left = interpret(multiplyOp->left);
                Object right = interpret(multiplyOp->right);

                return left.multiplied_by(right);
            }
            break;
        case ast::StatementType::DIV_OPERATOR:
            if (ast::DivideOperator* divideOp = static_cast<ast::DivideOperator*>(binaryOperator))
            {
                Object left = interpret(divideOp->left);
                Object right = interpret(divideOp->right);

                return left.divided_by(right);
            }
            break;
    }

    return Object();
}

Object Runner::interpret(ast::Statement* stat)
{
    switch (stat->get_type())
    {
        case ast::StatementType::NUMBER:
            if (ast::Number* num = static_cast<ast::Number*>(stat))
            {
                return Object(num->value);
            }
            break;
        case ast::StatementType::STRING:
            if (ast::String* str = static_cast<ast::String*>(stat))
            {
                return Object(str->value);
            }
            break;
        case ast::StatementType::EXPRESSION:
            if (ast::Expression* expression = static_cast<ast::Expression*>(stat))
            {
                return interpret(expression->root);
            }
            break;
        case ast::StatementType::VARIABLE:
            if (ast::Variable* v = static_cast<ast::Variable*>(stat))
            {
                return get_variable(v->name).object;
            }
            break;
        case ast::StatementType::VARIABLE_ASSIGNMENT:
            if (ast::VariableAssignment* va = static_cast<ast::VariableAssignment*>(stat))
            {
                if (!has_variable(va->variableName))
                {
                    add_variable(_Variable(va->variableName, interpret(va->expression)));
                }
                else
                {
                    get_variable(va->variableName).object = interpret(va->expression);
                }
            }
            break;
        case ast::StatementType::FUNCTION_CALL:
            if (ast::FunctionCall* funcCall = static_cast<ast::FunctionCall*>(stat))
            {
                return run_func(funcCall);
            }
            break;
        case ast::StatementType::ADD_OPERATOR:
        case ast::StatementType::SUB_OPERATOR:
        case ast::StatementType::DIV_OPERATOR:
        case ast::StatementType::MUL_OPERATOR:
            if (ast::BinaryOperator* binaryOp = static_cast<ast::BinaryOperator*>(stat))
            {
                return eval_binary_operator(binaryOp);
            }
            break;
        case ast::StatementType::NEGATE:
            if (ast::Negate* negate = static_cast<ast::Negate*>(stat))
            {
                return interpret(negate->value).negate();
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

_Variable& Runner::get_variable(std::string name)
{
    for (auto& v : _variables)
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

void Runner::run(ast::AST& ast)
{
    for (auto stat : ast.statements)
        interpret(stat);
}