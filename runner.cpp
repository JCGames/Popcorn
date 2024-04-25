#include "runner.hpp"

using namespace pop;

bool Scope::obj_exists_in_scope_history(const std::string& variableName)
{   
    for (auto& sa : stack)
    {
        if (sa.variableName == variableName)
        {
            return true;
        }
    }

    if (parent == nullptr) return false;
    return parent->obj_exists_in_scope_history(variableName);
}

Object Scope::get_obj(const std::string& variableName)
{
    for (auto& sa : stack)
    {
        if (sa.variableName == variableName)
            return sa.value;
    }

    if (parent != nullptr)
    {
        return parent->get_obj(variableName);
    } 

    Object nil;
    nil.type == ObjectType::NIL;
    return nil;
}

void Scope::set_obj(const std::string& variableName, Object value)
{
    if (obj_exists_in_scope_history(variableName))
    {
        for (auto& sa : stack)
        {
            if (sa.variableName == variableName)
            {
                sa.value = value;
                return;
            }
        }

        parent->set_obj(variableName, value);
    }
    else
    {
        StackAllocation sa;
        sa.variableName = variableName;
        sa.value = value;
        stack.push_back(sa);
    }
}

void Scope::delete_obj()
{
    
}

void Scope::set_parent(Scope* parent)
{
    if (this == parent)
        throw std::runtime_error("Trying to set a scopes parent to itself!");

    this->parent = parent;
}

std::vector<StackAllocation>& Scope::get_stack()
{
    return stack;
}

void Runner::run_block(Statement& root, Scope* parentScope)
{
    Scope currentScope;
    currentScope.set_parent(parentScope);

    for (auto& statement : root.children)
    {
        run_statement(statement, currentScope);

        if (diagnostics->has_errors())
            break;
    }

    // std::cout << "STACK:" << std::endl;
    
    // for (auto& sa : currentScope.get_stack())
    // {
    //     std::cout << "Variable Name: " << sa.variableName << std::endl;
    //     sa.value.print();
    // }
}

void Runner::run_statement(Statement& statement, Scope& scope)
{
    if (statement.type == StatementType::ASSIGN)
    {
        if (SI_String* siAssign = static_cast<SI_String*>(statement.info.get()))
        {
            try 
            {
                scope.set_obj(siAssign->value, eval_expression(statement.children[0], scope));
            }
            catch (const std::exception& exp)
            {
                diagnostics->add_error(exp.what(), statement.line, statement.lineColumn, statement.lineNumber);
                return;
            }
        }
    }
    else if (statement.type == StatementType::IF)
    {
        Object condition = eval_expression(statement.children[0], scope);

        Scope ifScope;
        ifScope.set_parent(&scope);

        if (*static_cast<bool*>(condition.value.get()))
        {
            run_block(statement.children[1], &ifScope);
        }
        else if (statement.children.size() == 3)
        {
            run_statement(statement.children[2], scope);
        }
    }
    else if (statement.type == StatementType::ELSE)
    {
        Scope elseScope;
        elseScope.set_parent(&scope);

        run_block(statement.children[0], &elseScope);
    }
    else if (statement.type == StatementType::WHILE)
    {
        Object condition = eval_expression(statement.children[0], scope);

        Scope whileScope;
        whileScope.set_parent(&scope);

        while (*static_cast<bool*>(condition.value.get()))
        {
            run_block(statement.children[1], &whileScope);
            condition = eval_expression(statement.children[0], scope);
        }
    }
    else if (statement.type == StatementType::FUNCTION_CALL)
    {
        SI_String siFunctionCall = *static_cast<SI_String*>(statement.info.get());

        if (siFunctionCall.value == "print")
        {
            try
            {
                if (statement.children.size() == 1)
                    std::cout << *static_cast<std::string*>(eval_expression(statement.children[0], scope).to_string().value.get()) << std::endl;
            }
            catch (const std::exception& exp)
            {
                diagnostics->add_error("Could not print value!", statement.line, statement.lineColumn, statement.lineNumber);
            }
        }
    }
    else
    {
        diagnostics->add_error("Bad statement!", statement.line, statement.lineColumn, statement.lineNumber);
    }
}

Object Runner::eval_expression(const Statement& statement, Scope& scope)
{
    switch (statement.type)
    {
    case StatementType::FUNCTION_CALL:
        if (SI_String* siString = static_cast<SI_String*>(statement.info.get()))
        {
            try
            {
                if (siString->value == "int" && statement.children.size() == 1)
                {
                    return eval_expression(statement.children[0], scope).to_int32();
                }
                else if (siString->value == "float" && statement.children.size() == 1)
                {
                    return eval_expression(statement.children[0], scope).to_float32();
                }
                else if (siString->value == "bool" && statement.children.size() == 1)
                {
                    return eval_expression(statement.children[0], scope).to_bool();
                }
                else if (siString->value == "char" && statement.children.size() == 1)
                {
                    return eval_expression(statement.children[0], scope).to_char();
                }
                else if (siString->value == "str" && statement.children.size() == 1)
                {
                    return eval_expression(statement.children[0], scope).to_string();
                }
            }
            catch (const std::exception& exp)
            {
                diagnostics->add_error(exp.what(), statement.line, statement.lineColumn, statement.lineNumber);
            }
        }
        break;
    case StatementType::EXP:
        {
            return eval_expression(statement.children[0], scope);
        }
        break;
    case StatementType::STRING:
        if (SI_String* siString = static_cast<SI_String*>(statement.info.get()))
        {
            Object _string;
            _string.type = ObjectType::STRING;
            _string.value = std::make_shared<std::string>(siString->value);
            return _string;
        }
        break;
    case StatementType::BOOLEAN:
        if (SI_Boolean* siBoolean = static_cast<SI_Boolean*>(statement.info.get())) 
        {
            Object boolean;
            boolean.type = ObjectType::BOOL;
            boolean.value = std::make_shared<bool>(siBoolean->value);
            return boolean;
        }
        break;
    case StatementType::VARIABLE:
        if (SI_String* siVariable = static_cast<SI_String*>(statement.info.get()))
        {
            return scope.get_obj(siVariable->value);
        }
        break;
    case StatementType::NUMBER:
        if (SI_String* siNumber = static_cast<SI_String*>(statement.info.get()))
        {
            Object number;
            
            /**
             * If the number is a floating point number.
            */
            if (siNumber->value.find('.') != std::string::npos)
            {
                number.type = ObjectType::FLOAT32;
                number.value = std::make_shared<float>(std::stof(siNumber->value));
                return number;
            }
            /**
             * If the number is an integer.
            */
            else
            {
                number.type = ObjectType::INT32;
                number.value = std::make_shared<int>(std::stoi(siNumber->value));
                return number;
            } 
        }
        break;
    case StatementType::CHAR:
        if (SI_String* siChar = static_cast<SI_String*>(statement.info.get()))
        {
            Object _char;
            _char.type = ObjectType::CHAR;
            _char.value = std::make_shared<char>(siChar->value[0]);
            return _char;
        }
        break;
    case StatementType::ADD_OP:
        {
            Object left = eval_expression(statement.children[0], scope);
            Object right = eval_expression(statement.children[1], scope);
            return left + right;
        }
        break;
    case StatementType::SUB_OP:
        {
            Object left = eval_expression(statement.children[0], scope);
            Object right = eval_expression(statement.children[1], scope);
            return left - right;
        }
        break;
    case StatementType::MULT_OP:
        {
            Object left = eval_expression(statement.children[0], scope);
            Object right = eval_expression(statement.children[1], scope);
            return left * right;
        }
        break;
    case StatementType::DIV_OP:
        {
            Object left = eval_expression(statement.children[0], scope);
            Object right = eval_expression(statement.children[1], scope);
            return left / right;
        }
        break;
    case StatementType::MOD_OP:
        {
            Object left = eval_expression(statement.children[0], scope);
            Object right = eval_expression(statement.children[1], scope);
            return left % right;
        }
        break;
    case StatementType::EQUALS_OP:
        {
            Object left = eval_expression(statement.children[0], scope);
            Object right = eval_expression(statement.children[1], scope);
            return left == right;
        }
        break;
    case StatementType::NEQUALS_OP:
        {
            Object left = eval_expression(statement.children[0], scope);
            Object right = eval_expression(statement.children[1], scope);
            return left != right;
        }
        break;
    case StatementType::GTHANE_OP:
        {
            Object left = eval_expression(statement.children[0], scope);
            Object right = eval_expression(statement.children[1], scope);
            return left >= right;
        }
        break;
    case StatementType::LTHANE_OP:
        {
            Object left = eval_expression(statement.children[0], scope);
            Object right = eval_expression(statement.children[1], scope);
            return left <= right;
        }
        break;
    case StatementType::GTHAN_OP:
        {
            Object left = eval_expression(statement.children[0], scope);
            Object right = eval_expression(statement.children[1], scope);
            return left > right;
        }
        break;
    case StatementType::LTHAN_OP:
        {
            Object left = eval_expression(statement.children[0], scope);
            Object right = eval_expression(statement.children[1], scope);
            return left < right;
        }
        break;
    case StatementType::NEGATE_OP:
        {
            Object result = eval_expression(statement.children[0], scope);
            return -result;
        }
        break;
    }

    Object nil;
    nil.type = ObjectType::NIL;
    return nil;
}

void Runner::run(Statement* root, Diagnostics* diagnostics)
{
    this->root = root;
    this->diagnostics = diagnostics;
    run_block(*root, nullptr);
}

void Runner::test1()
{
    Statement add;
    add.type = StatementType::ADD_OP;
    
    Statement left;
    left.type = StatementType::NUMBER;
    std::shared_ptr<SI_String> leftIsNumber = std::make_shared<SI_String>();
    leftIsNumber->value = "10";
    left.info = leftIsNumber;

    Statement right;
    right.type = StatementType::NUMBER;
    std::shared_ptr<SI_String> rightIsNumber = std::make_shared<SI_String>();
    rightIsNumber->value = "10";
    right.info = rightIsNumber;

    add.children.push_back(left);
    add.children.push_back(right);

    Scope scope;
    Object result = eval_expression(add, scope);
    result.print();
}