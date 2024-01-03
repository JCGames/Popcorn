#include "runner.hpp"

using namespace run;
using namespace obj;

Runner::Runner() { }

void Runner::add_variable(_Variable variable, int initialIndex)
{
    for (size_t i = initialIndex; i < _variables.size(); ++i)
    {
        if (_variables[i].name == variable.name)
            throw std::runtime_error("Variable " + variable.name + " has already been declared!");
    }

    _variables.push_back(variable);
}

_Variable& Runner::get_variable(std::string name)
{
    for (int i = _variables.size() - 1; i >= 0; --i)
    {
        if (_variables[i].name == name)
            return _variables[i];
    }

    throw std::runtime_error("Variable " + name + " has lost scope or was never declared!");
}

bool Runner::has_variable(std::string name)
{
    for (int i = _variables.size() - 1; i >= 0; --i)
    {
        if (_variables[i].name == name)
            return true;
    }

    return false;
}

Object Runner::call_function(ast::FunctionCall* funcCall)
{
    /**
     * Built in functions
    */

    if (funcCall->functionName == "rdump")
    {
        dump_runner();
    }
    // PRINT FUNCTION
    else if (funcCall->functionName == "print")
    {
        if (funcCall->parameterList.size() != 1)
            throw std::runtime_error("Function [print] only takes one argument!");

        std::cout << interpret(funcCall->parameterList[0]).cast_to_string().get_str();
    }
    // PRINT LINE FUNCTION
    else if (funcCall->functionName == "printl")
    {
        if (funcCall->parameterList.size() > 1)
            throw std::runtime_error("Function [printl] only takes one argument!");
        else if (funcCall->parameterList.size() == 1)
            std::cout << interpret(funcCall->parameterList[0]).cast_to_string().get_str() << "\n";
        else
            std::cout << "\n";
    }
    // CAST TO INT FUNCTION
    else if (funcCall->functionName == "int")
    {
        if (funcCall->parameterList.size() != 1)
            throw std::runtime_error("Function [int] only takes one argument!");

        return interpret(funcCall->parameterList[0]).cast_to_int();
    }
    // CAST TO BOOL FUNCTION
    else if (funcCall->functionName == "bool")
    {
        if (funcCall->parameterList.size() != 1)
            throw std::runtime_error("Function [bool] only takes one argument!");
        
        return interpret(funcCall->parameterList[0]).cast_to_bool();
    }
    // CAST TO DOUBLE FUNCTION
    else if (funcCall->functionName == "double")
    {
        if (funcCall->parameterList.size() != 1)
            throw std::runtime_error("Function [double] only takes one argument!");
        
        return interpret(funcCall->parameterList[0]).cast_to_double();
    }
    // GET LINE FUNCTION
    else if (funcCall->functionName == "input")
    {
        if (funcCall->parameterList.size() != 0)
            throw std::runtime_error("Function [input] does not take any arguments!");
        
        std::string result;
        getline(std::cin, result);
        return Object(result);
    }
    else
    {
        for (size_t i = 0; i < _functionTable.size(); ++i)
        {
            if (_functionTable[i].name == funcCall->functionName)
            {
                if (_functionTable[i].funcRef.parameterNames.size() != funcCall->parameterList.size())
                    throw std::runtime_error("The function " + funcCall->functionName + " did not have the correct number of parameters past on line: " + std::to_string(funcCall->get_line_index()));
                
                int originalVariableVectorSize = _variables.size();

                for (size_t j = 0; j < _functionTable[i].funcRef.parameterNames.size(); ++j)
                    add_variable(_Variable(_functionTable[i].funcRef.parameterNames[j], interpret(funcCall->parameterList[j])), originalVariableVectorSize - 1);

                Object result;

                for (auto stat : _functionTable[i].funcRef.body->statements)
                {
                    if (stat->get_type() == ast::StatementType::RETURN)
                    {
                        if (ast::Return* r = static_cast<ast::Return*>(stat))
                        {
                            result = interpret(r->expression);
                        }
                        break;
                    }

                    interpret(stat);
                }
                
                for (size_t j = 0; j < _variables.size() - originalVariableVectorSize + 1; ++j)
                    _variables.pop_back();

                return result;
            }
        }

        throw std::runtime_error("The function " + funcCall->functionName + " did not exist!");
    }

    return Object();
}

Object Runner::interpret(ast::Statement* stat)
{
    switch (stat->get_type())
    {
        case ast::StatementType::DOUBLE:
            if (ast::Double* _double = static_cast<ast::Double*>(stat))
            {
                return Object(_double->value);
            }
            break;
        case ast::StatementType::INTEGER:
            if (ast::Integer* _int = static_cast<ast::Integer*>(stat))
            {
                return Object(_int->value);
            }
            break;
        case ast::StatementType::STRING:
            if (ast::String* str = static_cast<ast::String*>(stat))
            {
                return Object(str->value);
            }
            break;
        case ast::StatementType::BOOLEAN:
            if (ast::Boolean* str = static_cast<ast::Boolean*>(stat))
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
                    add_variable(_Variable(va->variableName, interpret(va->expression)));
                else
                    get_variable(va->variableName).object = interpret(va->expression);
            }
            break;
        case ast::StatementType::FUNCTION_CALL:
            if (ast::FunctionCall* funcCall = static_cast<ast::FunctionCall*>(stat))
            {
                return call_function(funcCall);
            }
            break;
        case ast::StatementType::ADD_OPERATOR:
            if (ast::AddOperator* addOp = static_cast<ast::AddOperator*>(stat))
            {
                Object left = interpret(addOp->left);
                Object right = interpret(addOp->right);

                return left.add_to(right);
            }
            break;
        case ast::StatementType::SUB_OPERATOR:
            if (ast::SubtractOperator* subtractOp = static_cast<ast::SubtractOperator*>(stat))
            {
                Object left = interpret(subtractOp->left);
                Object right = interpret(subtractOp->right);

                return left.subtract_from(right);
            }
            break;
        case ast::StatementType::MUL_OPERATOR:
            if (ast::MultiplyOperator* multiplyOp = static_cast<ast::MultiplyOperator*>(stat))
            {
                Object left = interpret(multiplyOp->left);
                Object right = interpret(multiplyOp->right);

                return left.multiplied_by(right);
            }
            break;
        case ast::StatementType::DIV_OPERATOR:
            if (ast::DivideOperator* divideOp = static_cast<ast::DivideOperator*>(stat))
            {
                Object left = interpret(divideOp->left);
                Object right = interpret(divideOp->right);

                return left.divided_by(right);
            }
            break;
        case ast::StatementType::NEGATE:
            if (ast::Negate* negate = static_cast<ast::Negate*>(stat))
            {
                return interpret(negate->value).negate();
            }
            break;
        case ast::StatementType::EQUALS_OPERATOR:
            if (ast::EqualsOperator* equalsOperator = static_cast<ast::EqualsOperator*>(stat))
            {
                Object left = interpret(equalsOperator->left);
                Object right = interpret(equalsOperator->right);

                return left.equals(right);
            }
            break;
        case ast::StatementType::NOT_EQUALS_OPERATOR:
            if (ast::NotEqualsOperator* notEqualsOperator = static_cast<ast::NotEqualsOperator*>(stat))
            {
                Object left = interpret(notEqualsOperator->left);
                Object right = interpret(notEqualsOperator->right);

                return left.not_equals(right);
            }
            break;
        case ast::StatementType::GREATER_THAN_OPERATOR:
            if (ast::GreaterThanOperator* greaterThanOperator = static_cast<ast::GreaterThanOperator*>(stat))
            {
                Object left = interpret(greaterThanOperator->left);
                Object right = interpret(greaterThanOperator->right);

                return left.greater_than(right);
            }
            break;
        case ast::StatementType::LESS_THAN_OPERATOR:
            if (ast::LessThanOperator* lessThanOperator = static_cast<ast::LessThanOperator*>(stat))
            {
                Object left = interpret(lessThanOperator->left);
                Object right = interpret(lessThanOperator->right);

                return left.less_than(right);
            }
            break;
        case ast::StatementType::GREATER_THAN_EQUALS_OPERATOR:
            if (ast::GreaterThanEqualsOperator* greaterThanEqualsOperator = static_cast<ast::GreaterThanEqualsOperator*>(stat))
            {
                Object left = interpret(greaterThanEqualsOperator->left);
                Object right = interpret(greaterThanEqualsOperator->right);

                return left.greater_than_equals(right);
            }
            break;
        case ast::StatementType::LESS_THAN_EQUALS_OPERATOR:
            if (ast::LessThanEqualsOperator* lessThanEqualsOperator = static_cast<ast::LessThanEqualsOperator*>(stat))
            {
                Object left = interpret(lessThanEqualsOperator->left);
                Object right = interpret(lessThanEqualsOperator->right);

                return left.less_than_equals(right);
            }
            break;
        case ast::StatementType::BLOCK:
            if (ast::Block* block = static_cast<ast::Block*>(stat))
            {
                for (auto& s : block->statements)
                    interpret(s);
            }
            break;
        case ast::StatementType::IF:
            if (ast::If* _if = static_cast<ast::If*>(stat))
            {
                Object result = interpret(_if->condition);

                if (result.get_type() == ObjectType::BOOLEAN && result.get_bool() == true)
                {
                    interpret(_if->body);
                }
                else if (_if->elseOrIf != nullptr)
                {
                    interpret(_if->elseOrIf);
                }
            }
            break;
        case ast::StatementType::ELSE:
            if (ast::Else* _else = static_cast<ast::Else*>(stat))
            {
                interpret(_else->body);
            }
            break;
        case ast::StatementType::AND_CONDITION:
            if (ast::AndCondition* andCondition = static_cast<ast::AndCondition*>(stat))
            {
                Object left = interpret(andCondition->left);
                Object right = interpret(andCondition->right);

                return Object((left.get_type() == ObjectType::BOOLEAN && left.get_bool()) &&
                            (left.get_type() == ObjectType::BOOLEAN && right.get_bool()));
            }
            break;
        case ast::StatementType::OR_CONDITION:
            if (ast::OrCondition* orCondition = static_cast<ast::OrCondition*>(stat))
            {
                Object left = interpret(orCondition->left);
                Object right = interpret(orCondition->right);

                return Object((left.get_type() == ObjectType::BOOLEAN && left.get_bool()) ||
                            (left.get_type() == ObjectType::BOOLEAN && right.get_bool()));
            }
            break;
        case ast::StatementType::WHILE:
            if (ast::While* _while = static_cast<ast::While*>(stat))
            {
                Object result = interpret(_while->condition);

                while (result.get_type() == ObjectType::BOOLEAN && result.get_bool())
                {
                    interpret(_while->body);
                    result = interpret(_while->condition);
                }
                
            }
            break;
        case ast::StatementType::MODULUS_OPERATOR:
            if (ast::ModulusOperator* modOperator = static_cast<ast::ModulusOperator*>(stat))
            {
                Object left = interpret(modOperator->left);
                Object right = interpret(modOperator->right);

                return left.modulus_by(right);
            }
            break;
    }

    return Object();
}

void Runner::create_function_table(ast::Statement* stat)
{
    if (stat->get_type() == ast::StatementType::FUNCTION)
    {
        if (ast::Function* func = static_cast<ast::Function*>(stat))
        {
            _functionTable.push_back({ func->functionName, *func });
        }
    }
    else if (stat->get_type() == ast::StatementType::BLOCK)
    {
        if (ast::Block* block = static_cast<ast::Block*>(stat))
        {
            for (ast::Statement* s : block->statements)
                create_function_table(s);
        }
    }
}

void Runner::run(ast::AST& ast)
{
    for (auto stat : ast.statements)
        create_function_table(stat);

    for (auto stat : ast.statements)
        interpret(stat);
}

void Runner::dump_runner()
{
    printf("========== RUNNER DUMP ==========\n");

    printf("\nFUNCTION TABLE:\n");
    for (auto& f : _functionTable)
    {
        printf("Function Name: %s\n", f.name.c_str());
    }

    printf("\nVARIABLES:\n");
    for (auto& v : _variables)
    {
        printf("Variable Name: |%s|, Value: ", v.name.c_str());
        std::cout << v.object.cast_to_string().get_str() << std::endl;
    }

    printf("\n=================================\n");
}