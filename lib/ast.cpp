#include "ast.hpp"

using namespace ast;

static std::string ast::get_statement_type_name(StatementType type)
{
    switch (type)
    {
        case StatementType::NONE: return "NONE";
        case StatementType::UNARY_OPERATOR: return "UNARY OPERATOR";
        case StatementType::BINARY_OPERATOR: return "BINARY OPERATOR";
        case StatementType::ADD_OPERATOR: return "ADD OPERATOR";
        case StatementType::SUB_OPERATOR: return "SUBTRACT OPERATOR";
        case StatementType::DIV_OPERATOR: return "DIVIDE OPERATOR";
        case StatementType::MUL_OPERATOR: return "MULTIPLY OPERATOR";
        case StatementType::DOUBLE: return "DOUBLE";
        case StatementType::INTEGER: return "INTEGER";
        case StatementType::STRING: return "STRING";
        case StatementType::EXPRESSION: return "EXPRESSION";
        case StatementType::VARIABLE_ASSIGNMENT: return "VARIABLE ASSIGNMENT";
        case StatementType::BLOCK: return "BLOCK";
        case StatementType::VARIABLE: return "VARIABLE";
        case StatementType::FUNCTION_CALL: return "FUNCTION CALL";
        case StatementType::NEGATE: return "NEGATE";
        case StatementType::BOOLEAN: return "BOOLEAN";
        case StatementType::EQUALS_OPERATOR: return "EQUALS OPERATOR";
        case StatementType::NOT_EQUALS_OPERATOR: return "NOT EQUALS OPERATOR";
        case StatementType::GREATER_THAN_OPERATOR: return "GREATER THAN OPERATOR";
        case StatementType::LESS_THAN_OPERATOR: return "LESS THAN OPERATOR";
        case StatementType::GREATER_THAN_EQUALS_OPERATOR: return "GREATER THAN EQUALS OPERATOR";
        case StatementType::LESS_THAN_EQUALS_OPERATOR: return "LESS THAN EQUALS OPERATOR";
        case StatementType::IF: return "IF";
        case StatementType::ELSE: return "ELSE";
        case StatementType::WHILE: return "WHILE";
        case StatementType::FOR: return "FOR";
        case StatementType::AND_CONDITION: return "AND CONDITION";
        case StatementType::OR_CONDITION: return "OR CONDITION";
        case StatementType::MODULUS_OPERATOR: return "MODULUS OPERATOR";
        case StatementType::FUNCTION: return "FUNCTION";
        default: return "UNDEFINED";
    }
}

#pragma region Statement

Statement::Statement(int lineIndex)
{
    _lineIndex = lineIndex;
}

Statement::~Statement() { }

StatementType Statement::get_type()
{
    throw std::runtime_error("Statement function get_type not implemented!");
}

int Statement::get_line_index()
{
    return _lineIndex;
}

#pragma endregion

#pragma region UnaryOperator

UnaryOperator::UnaryOperator(Statement* value, int lineIndex) : Statement(lineIndex)
{
    this->value = value;
}

UnaryOperator::~UnaryOperator()
{
    if (value != NULL)
        delete value;
}

StatementType UnaryOperator::get_type()
{
    return StatementType::UNARY_OPERATOR;
}

#pragma endregion

#pragma region BinaryOperator

BinaryOperator::BinaryOperator(Statement* left, Statement* right, int lineIndex) : Statement(lineIndex)
{
    this->left = left;
    this->right = right;
}

BinaryOperator::~BinaryOperator()
{
    if (left != NULL) 
        delete left;
    
    if (right != NULL)
        delete right;
}

StatementType BinaryOperator::get_type() 
{
    return StatementType::BINARY_OPERATOR;
}

#pragma endregion

#pragma region AddOperator

AddOperator::AddOperator(Statement* left, Statement* right, int lineIndex) : BinaryOperator(left, right, lineIndex) { }

StatementType AddOperator::get_type()
{
    return StatementType::ADD_OPERATOR;
}

#pragma endregion

#pragma region SubtractOperator

SubtractOperator::SubtractOperator(Statement* left, Statement* right, int lineIndex) : BinaryOperator(left, right, lineIndex) { }

StatementType SubtractOperator::get_type()
{
    return StatementType::SUB_OPERATOR;
}

#pragma endregion

#pragma region DivideOperator

DivideOperator::DivideOperator(Statement* left, Statement* right, int lineIndex) : BinaryOperator(left, right, lineIndex) { }

StatementType DivideOperator::get_type()
{
    return StatementType::DIV_OPERATOR;
}

#pragma endregion

#pragma region ModulusOperator

ModulusOperator::ModulusOperator(Statement* left, Statement* right, int lineIndex) : BinaryOperator(left, right, lineIndex) { }

StatementType ModulusOperator::get_type()
{
    return StatementType::MODULUS_OPERATOR;
}

#pragma endregion

#pragma region MultiplyOperator

MultiplyOperator::MultiplyOperator(Statement* left, Statement* right, int lineIndex) : BinaryOperator(left, right, lineIndex) { }

StatementType MultiplyOperator::get_type()
{
    return StatementType::MUL_OPERATOR;
}

#pragma endregion

#pragma region EqualsOperator

EqualsOperator::EqualsOperator(Statement* left, Statement* right, int lineIndex) : BinaryOperator(left, right, lineIndex) { }

StatementType EqualsOperator::get_type()
{
    return StatementType::EQUALS_OPERATOR;
}

#pragma endregion

#pragma region NotEqualsOperator

NotEqualsOperator::NotEqualsOperator(Statement* left, Statement* right, int lineIndex) : BinaryOperator(left, right, lineIndex) { }

StatementType NotEqualsOperator::get_type()
{
    return StatementType::NOT_EQUALS_OPERATOR;
}

#pragma endregion

#pragma region GreaterThanOperator

GreaterThanOperator::GreaterThanOperator(Statement* left, Statement* right, int lineIndex) : BinaryOperator(left, right, lineIndex) { }

StatementType GreaterThanOperator::get_type()
{
    return StatementType::GREATER_THAN_OPERATOR;
}

#pragma endregion

#pragma region LessThanOperator

LessThanOperator::LessThanOperator(Statement* left, Statement* right, int lineIndex) : BinaryOperator(left, right, lineIndex) { }

StatementType LessThanOperator::get_type()
{
    return StatementType::LESS_THAN_OPERATOR;
}

#pragma endregion

#pragma region GreaterThanEqualsOperator

GreaterThanEqualsOperator::GreaterThanEqualsOperator(Statement* left, Statement* right, int lineIndex) : BinaryOperator(left, right, lineIndex) { }

StatementType GreaterThanEqualsOperator::get_type()
{
    return StatementType::GREATER_THAN_EQUALS_OPERATOR;
}

#pragma endregion

#pragma region LessThanEqualsOperator

LessThanEqualsOperator::LessThanEqualsOperator(Statement* left, Statement* right, int lineIndex) : BinaryOperator(left, right, lineIndex) { }

StatementType LessThanEqualsOperator::get_type()
{
    return StatementType::LESS_THAN_EQUALS_OPERATOR;
}

#pragma endregion

#pragma region AndCondition

AndCondition::AndCondition(Statement* left, Statement* right, int lineIndex) : BinaryOperator(left, right, lineIndex) { }

StatementType AndCondition::get_type()
{
    return StatementType::AND_CONDITION;
}

#pragma endregion

#pragma region OrCondition

OrCondition::OrCondition(Statement* left, Statement* right, int lineIndex) : BinaryOperator(left, right, lineIndex) { }

StatementType OrCondition::get_type()
{
    return StatementType::OR_CONDITION;
}

#pragma endregion

#pragma region Double

Double::Double(double value, int lineIndex) : Statement(lineIndex)
{
    this->value = value;
}

StatementType Double::get_type()
{
    return StatementType::DOUBLE;
}

#pragma endregion

#pragma region Integer

Integer::Integer(int value, int lineIndex) : Statement(lineIndex)
{
    this->value = value;
}

StatementType Integer::get_type()
{
    return StatementType::INTEGER;
}

#pragma endregion

#pragma region String

String::String(std::string value, int lineIndex) : Statement(lineIndex)
{
    this->value = value;
}

StatementType String::get_type() 
{
    return StatementType::STRING;
}

#pragma endregion

#pragma region Expression

Expression::Expression(Statement* root, int lineIndex) : Statement(lineIndex)
{
    this->root = root;
}

Expression::~Expression()
{
    if (root != NULL)
        delete root;
}

StatementType Expression::get_type()
{
    return StatementType::EXPRESSION;
}

#pragma endregion

#pragma region VariableAssignment

VariableAssignment::VariableAssignment(std::string variableName, Expression* expression, int lineIndex) : Statement(lineIndex)
{
    this->variableName = variableName;
    this->expression = expression;
}

VariableAssignment::~VariableAssignment()
{
    if (expression != NULL)
        delete expression;
}

StatementType VariableAssignment::get_type()
{
    return StatementType::VARIABLE_ASSIGNMENT;
}

#pragma endregion

#pragma region Variable

Variable::Variable(std::string name, int lineIndex) : Statement(lineIndex)
{
    this->name = name;
}

StatementType Variable::get_type()
{
    return StatementType::VARIABLE;
}

#pragma endregion

#pragma region Block

Block::Block(int lineIndex) : Statement(lineIndex) { }

Block::~Block()
{
    for (int i = 0; i < statements.size(); ++i)
    {
        if (statements[i] != NULL)
            delete statements[i];
    }
}

StatementType Block::get_type()
{
    return StatementType::BLOCK;
}

#pragma endregion

#pragma region FunctionCall

FunctionCall::FunctionCall(std::string functionName, int lineIndex) : Statement(lineIndex)
{
    this->functionName = functionName;
}

FunctionCall::~FunctionCall()
{
    for (int i = 0; i < parameterList.size(); ++i)
    {
        if (parameterList[i] != NULL)
            delete parameterList[i];
    }
}

StatementType FunctionCall::get_type()
{
    return StatementType::FUNCTION_CALL;
}

#pragma endregion

#pragma region Function

Function::Function(std::string functionName, Block* body, int lineIndex) : Statement(lineIndex)
{
    this->functionName = functionName;
    this->body = body;
}

Function::~Function()
{
    delete body;
}

StatementType Function::get_type() 
{
    return StatementType::FUNCTION;
}

#pragma endregion

#pragma region Negate

Negate::Negate(Statement* value, int lineIndex) : UnaryOperator(value, lineIndex) { }

StatementType Negate::get_type()
{
    return StatementType::NEGATE;
}

#pragma endregion

#pragma region Boolean

Boolean::Boolean(bool value, int lineIndex) : Statement(lineIndex)
{
    this->value = value;
}

StatementType Boolean::get_type()
{
    return StatementType::BOOLEAN;
}

#pragma endregion

#pragma region If

If::If(Expression* condition, Block* body, int lineIndex) : Statement(lineIndex)
{
    this->condition = condition;
    this->body = body;
    this->elseOrIf = nullptr;
}

If::~If()
{
    if (condition != nullptr)
        delete condition;

    if (body != nullptr)
        delete body;
}

StatementType If::get_type()
{
    return StatementType::IF;
}

#pragma endregion

#pragma region Else

Else::Else(Block* body, int lineIndex) : Statement(lineIndex)
{
    this->body = body;
}

Else::~Else()
{
    if (body != nullptr)
        delete body;
}

StatementType Else::get_type()
{
    return StatementType::ELSE;
}

#pragma endregion

#pragma region While

While::While(Expression* condition, Block* body, int lineIndex) : Statement(lineIndex)
{
    this->condition = condition;
    this->body = body;
}

While::~While()
{
    if (condition != nullptr)
        delete condition;

    if (body != nullptr)
        delete body;
}

StatementType While::get_type()
{
    return StatementType::WHILE;
}

#pragma endregion

#pragma region Return

Return::Return(Expression* expression, int lineIndex) : Statement(lineIndex)
{
    this->expression = expression;
}

Return::~Return()
{
    delete expression;
}

StatementType Return::get_type()
{
    return StatementType::RETURN;
}

#pragma endregion

#pragma region AST

AST::~AST()
{
    for (auto stmt : statements)
    {
        if (stmt != NULL)
            delete stmt;
    }
}

void AST::print()
{
    for (auto stmt : statements)
        print_statement(stmt, "");
}

void AST::print_statement(Statement* stat, std::string indent)
{
    if (stat == NULL)
    {
        printf("NULL\n");
        return;
    }

    printf("%s%s\n", indent.c_str(), get_statement_type_name(stat->get_type()).c_str());
    switch (stat->get_type())
    {
        case StatementType::NONE:
            break;
        case StatementType::UNARY_OPERATOR:
            if (UnaryOperator* value = static_cast<UnaryOperator*>(stat))
                print_statement(value->value, indent + '\t');
            break;
        case StatementType::BINARY_OPERATOR:
        case StatementType::ADD_OPERATOR:
        case StatementType::SUB_OPERATOR:
        case StatementType::DIV_OPERATOR:
        case StatementType::MUL_OPERATOR:
        case StatementType::EQUALS_OPERATOR:
        case StatementType::NOT_EQUALS_OPERATOR:
        case StatementType::GREATER_THAN_OPERATOR:
        case StatementType::LESS_THAN_OPERATOR:
        case StatementType::GREATER_THAN_EQUALS_OPERATOR:
        case StatementType::LESS_THAN_EQUALS_OPERATOR:
        case StatementType::AND_CONDITION:
        case StatementType::OR_CONDITION:
        case StatementType::MODULUS_OPERATOR:
            printf("%sLEFT:\n", indent.c_str());
            if (BinaryOperator* value = static_cast<BinaryOperator*>(stat))
                print_statement(value->left, indent + '\t');
            printf("%sRIGHT:\n", indent.c_str());
            if (BinaryOperator* value = static_cast<BinaryOperator*>(stat))
                print_statement(value->right, indent + '\t');
            break;
        case StatementType::DOUBLE:
            if (Double* _double = static_cast<Double*>(stat))
                printf("%s%f\n", indent.c_str(), _double->value);
            break;
        case StatementType::INTEGER:
            if (Integer* integer = static_cast<Integer*>(stat))
                printf("%s%i\n", indent.c_str(), integer->value);
            break;
        case StatementType::STRING:
            if (String* str = static_cast<String*>(stat))
                printf("%s%s\n", indent.c_str(), str->value.c_str());
            break;
        case StatementType::EXPRESSION:
            if (Expression* exp = static_cast<Expression*>(stat))
                print_statement(exp->root, indent + '\t');
            break;
        case StatementType::VARIABLE_ASSIGNMENT:
            if (VariableAssignment* varDec = static_cast<VariableAssignment*>(stat))
            {
                printf("%sNAME: |%s|\n", indent.c_str(), varDec->variableName.c_str());
                print_statement(varDec->expression, indent + '\t');
            }
            break;
        case StatementType::BLOCK:
            if (Block* block = static_cast<Block*>(stat))
            {
                for (auto statement : block->statements)
                    print_statement(statement, indent + '\t');
            }
            break;
        case StatementType::VARIABLE:
            if (Variable* variable = static_cast<Variable*>(stat))
                printf("%sNAME: %s\n", indent.c_str(), variable->name.c_str());
            break;
        case StatementType::FUNCTION_CALL:
            if (FunctionCall* funcCall = static_cast<FunctionCall*>(stat))
            {
                printf("%sFunction Name: %s\n", indent.c_str(), funcCall->functionName.c_str());
                printf("%s---PARAMETER LIST:\n", indent.c_str());
                for (auto e : funcCall->parameterList)
                    print_statement(e, indent + '\t');
            }
            break;
        case StatementType::NEGATE:
            if (Negate* negate = static_cast<Negate*>(stat))
            {
                print_statement(negate->value, indent + '\t');
            }
            break;
        case StatementType::BOOLEAN:
            if (Boolean* b = static_cast<Boolean*>(stat))
            {
                if (b->value)
                {
                    printf("%sTRUE\n", indent.c_str());
                }
                else
                {
                    printf("%sFALSE\n", indent.c_str());
                }
            }
            break;
        case StatementType::IF:
            if (If* _if = static_cast<If*>(stat))
            {
                printf("%sCONDITION:\n", indent.c_str());
                print_statement(_if->condition, indent + '\t');
                printf("%sBODY:\n", indent.c_str());
                print_statement(_if->body, indent + '\t');

                if (_if->elseOrIf != nullptr)
                {
                    printf("%sCHILD STATEMENT:\n", indent.c_str());
                    print_statement(_if->elseOrIf, indent + '\t');
                }
            }
            break;
        case StatementType::ELSE:
            if (Else* _else = static_cast<Else*>(stat))
            {
                printf("%sBODY:\n", indent.c_str());
                print_statement(_else->body, indent + '\t');
            }
            break;
        case StatementType::WHILE:
            if (While* _while = static_cast<While*>(stat))
            {
                printf("%sCONDITION:\n", indent.c_str());
                print_statement(_while->condition, indent + '\t');
                printf("%sBODY:\n", indent.c_str());
                print_statement(_while->body, indent + '\t');
            }
            break;
        case StatementType::FUNCTION:
            if (Function* func = static_cast<Function*>(stat))
            {
                printf("%sNAME: %s\n", indent.c_str(), func->functionName.c_str());
                printf("%sPARAMETERS: [", indent.c_str());

                if (func->parameterNames.size() == 0)
                {
                    printf("]\n");
                }
                else
                {
                    for (size_t i = 0; i < func->parameterNames.size(); ++i)
                    {
                        if (i != func->parameterNames.size() - 1)
                            printf("%s, ", func->parameterNames[i].c_str());
                        else
                            printf("%s]\n", func->parameterNames[i].c_str());
                    }
                }

                printf("%sBODY:\n", indent.c_str());
                print_statement(func->body, indent + '\t');
            }
            break;
        case StatementType::RETURN:
            if (Return* _return = static_cast<Return*>(stat))
            {
                if (_return->expression != nullptr)
                {
                    print_statement(_return->expression, indent + '\t');
                }
            }
            break;
    }
}

#pragma endregion