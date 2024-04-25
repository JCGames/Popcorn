#include "runner.hpp"

using namespace pop;

#pragma region Scope

Scope::Scope()
{
    returnFlag = false;
    breakFlag = false;
    continueFlag = false;
}

/**
 * Checks to see if a variable exists on the current or subsequent parent stacks.
*/
bool Scope::has_variable(const std::string& variableName)
{   
    for (auto& sa : stack)
    {
        if (sa.variableName == variableName)
        {
            return true;
        }
    }

    if (parent == nullptr) return false;
    return parent->has_variable(variableName);
}

/**
 * Gets the variable from the current or subsequent parent stacks.
*/
Object Scope::get_variable(const std::string& variableName)
{
    for (auto& sa : stack)
    {
        if (sa.variableName == variableName)
            return sa.value;
    }

    if (parent != nullptr)
    {
        return parent->get_variable(variableName);
    } 

    Object nil;
    nil.type == ObjectType::NIL;
    return nil;
}

/**
 * Sets the variable on the current or subsequent parent stacks.
*/
void Scope::set_variable(const std::string& variableName, Object value)
{
    if (has_variable(variableName))
    {
        for (auto& sa : stack)
        {
            if (sa.variableName == variableName)
            {
                sa.value = value;
                return;
            }
        }

        parent->set_variable(variableName, value);
    }
    else
    {
        StackAllocation sa;
        sa.variableName = variableName;
        sa.value = value;
        stack.push_back(sa);
    }
}

/**
 * Sets the parent scope.
*/
void Scope::set_parent(Scope* parent)
{
    if (this == parent)
        throw std::runtime_error("Trying to set a scopes parent to itself!");

    this->parent = parent;
}

/**
 * Finds all of the functions within a given block and saves them.
*/
void Scope::create_function_table(Statement& block)
{
    for (auto& stmt : block.children)
    {
        if (stmt.type == StatementType::FUNCTION)
        {
            functions.push_back(&stmt);
        }
    }
}

/**
 * Retrieves a function from the given scope.
*/
Statement* Scope::get_function_in_block(std::string functionName)
{
    for (auto& function : functions)
    {
        if (SI_String* siFunction = static_cast<SI_String*>(function->info.get()))
        {
            if (siFunction->value == functionName)
            {
                return function;
            }
        }
    }

    if (parent != nullptr)
        return parent->get_function_in_block(functionName);

    return nullptr;
}

/**
 * Gets this scopes stack.
*/
std::vector<StackAllocation>& Scope::get_stack()
{
    return stack;
}

#pragma endregion

#pragma region Private Methods

/**
 * Runs a single block of statements.
*/
void Runner::run_block(Statement& root, Scope* parentScope, Object* result)
{
    if (root.type != StatementType::BLOCK)
        throw std::runtime_error("Trying to run a statement that is not a block as a block.");

    Scope currentScope;
    currentScope.set_parent(parentScope);
    currentScope.create_function_table(root);

    for (auto& statement : root.children)
    {
        run_statement(statement, currentScope, result);

        if (currentScope.returnFlag)
        {
            parentScope->returnFlag = currentScope.returnFlag;
            break;
        }

        if (currentScope.breakFlag)
        {
            parentScope->breakFlag = currentScope.breakFlag;
            break;
        }

        if (currentScope.continueFlag)
        {
            parentScope->continueFlag = currentScope.continueFlag;
            break;
        }

        // if we are running through our statements
        // and one of our statements causes an error
        // stop running this block of statements
        if (diagnostics->has_errors())
            break;
    }
}

/**
 * Runs a single statement.
*/
void Runner::run_statement(Statement& statement, Scope& scope, Object* result)
{
    if (statement.type == StatementType::FUNCTION) return;
    if (scope.returnFlag || scope.breakFlag || scope.continueFlag) return;

    // ASSIGNMENT STATEMENT
    if (statement.type == StatementType::ASSIGN)
    {
        if (SI_String* siAssign = static_cast<SI_String*>(statement.info.get()))
        {
            try 
            {
                scope.set_variable(siAssign->value, eval_expression(statement.children[0], scope));
            }
            catch (const std::exception& exp)
            {
                diagnostics->add_error(exp.what(), statement.line, statement.lineColumn, statement.lineNumber);
                return;
            }
        }
    }
    // IF STATEMENT
    else if (statement.type == StatementType::IF)
    {
        Object condition = eval_expression(statement.children[0], scope);

        Scope ifScope;
        ifScope.set_parent(&scope);

        if (*static_cast<bool*>(condition.value.get()))
        {
            run_block(statement.children[1], &ifScope);

            if (ifScope.breakFlag)
            {
                diagnostics->add_error("Cannot break here.", statement.line, statement.lineColumn, statement.lineNumber);
            }

            if (ifScope.continueFlag)
            {
                diagnostics->add_error("Cannot continue here.", statement.line, statement.lineColumn, statement.lineNumber);
            }

            if (ifScope.returnFlag)
            {
                scope.returnFlag = true;
            }
        }
        else if (statement.children.size() == 3)
        {
            run_statement(statement.children[2], scope);
        }
    }
    // ELSE STATEMENT
    else if (statement.type == StatementType::ELSE)
    {
        Scope elseScope;
        elseScope.set_parent(&scope);

        run_block(statement.children[0], &elseScope);

        if (elseScope.breakFlag)
        {
            diagnostics->add_error("Cannot break here.", statement.line, statement.lineColumn, statement.lineNumber);
        }

        if (elseScope.continueFlag)
        {
            diagnostics->add_error("Cannot continue here.", statement.line, statement.lineColumn, statement.lineNumber);
        }

        if (elseScope.returnFlag)
        {
            scope.returnFlag = true;
        }
    }
    // WHILE STATEMENT
    else if (statement.type == StatementType::WHILE)
    {
        Object condition = eval_expression(statement.children[0], scope);

        Scope whileScope;
        whileScope.set_parent(&scope);

        while (*static_cast<bool*>(condition.value.get()))
        {
            run_block(statement.children[1], &whileScope);
            condition = eval_expression(statement.children[0], scope);
            
            if (whileScope.breakFlag)
                break;

            if (whileScope.continueFlag)
            {
                whileScope.continueFlag = false;
                continue;
            }

            if (whileScope.returnFlag)
            {
                scope.returnFlag = true;
                break;
            }
        }
    }
    // FUNCTION CALL STATEMENT
    else if (statement.type == StatementType::FUNCTION_CALL)
    {
        run_function_call(statement, scope);
    }
    else if (statement.type == StatementType::RETURN)
    {
        if (result != nullptr)
        {
            Object evaluatedObject = eval_expression(statement.children[0], scope);
            result->type == evaluatedObject.type;
            result->value = evaluatedObject.value;
        }
        else
        {
            diagnostics->add_error("Cannot return here.", statement.line, statement.lineColumn, statement.lineNumber);
        }
    }
    else
    {
        diagnostics->add_error("Bad statement!", statement.line, statement.lineColumn, statement.lineNumber);
    }
}

Object Runner::run_function_call(Statement& functionCall, Scope& scope)
{
    SI_String siFunctionCall = *static_cast<SI_String*>(functionCall.info.get());

    if (siFunctionCall.value == "print")
    {
        try
        {
            if (functionCall.children.size() == 1)
                std::cout << *static_cast<std::string*>(eval_expression(functionCall.children[0], scope).to_string().value.get()) << std::endl;
        }
        catch (const std::exception& exp)
        {
            diagnostics->add_error(exp.what(), functionCall.line, functionCall.lineColumn, functionCall.lineNumber);
        }
    }
    else
    {
        Statement* function = scope.get_function_in_block(siFunctionCall.value);

        if (function != nullptr)
        {
            SI_Function* siFunction = static_cast<SI_Function*>(function->info.get());
            
            Scope functionScope;
            functionScope.set_parent(&scope);
            functionScope.create_function_table(function->children[0]);

            if (siFunction->parameterNames.size() != functionCall.children.size()) 
            {
                diagnostics->add_error("Inccorect number of parameters!", functionCall.line, functionCall.lineColumn, functionCall.lineNumber);
                return Object(); 
            }

            for (int i = 0; i < siFunction->parameterNames.size(); i++)
            {
                functionScope.set_variable(siFunction->parameterNames[i], eval_expression(functionCall.children[i], scope));
            }

            Object result;
            run_block(function->children[0], &functionScope, &result);
            return result;
        }
        else
        {
            diagnostics->add_error("The function with the name " + siFunctionCall.value + " has not been defined!", functionCall.line, functionCall.lineColumn, functionCall.lineNumber);
        }
    }

    return Object();
}

/**
 * Evaluates an expression.
*/
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
            return scope.get_variable(siVariable->value);
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

#pragma endregion

#pragma region Public Methods

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

#pragma endregion