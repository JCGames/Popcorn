#ifndef _AST
#define _AST

#include <vector>
#include <string>
#include <stdexcept>

namespace ast 
{
    enum class StatementType : char
    {
        NONE,
        UNARY_OPERATOR,
        BINARY_OPERATOR,
        ADD_OPERATOR,
        SUB_OPERATOR,
        DIV_OPERATOR,
        MUL_OPERATOR,
        DOUBLE,
        INTEGER,
        STRING,
        EXPRESSION,
        VARIABLE_ASSIGNMENT,
        BLOCK,
        VARIABLE,
        FUNCTION_CALL,
        NEGATE,
        BOOLEAN,
        EQUALS_OPERATOR,
        NOT_EQUALS_OPERATOR,
        GREATER_THAN_OPERATOR,
        LESS_THAN_OPERATOR,
        GREATER_THAN_EQUALS_OPERATOR,
        LESS_THAN_EQUALS_OPERATOR,
        IF,
        ELSE,
        WHILE,
        FOR,
        AND_CONDITION,
        OR_CONDITION,
        MODULUS_OPERATOR,
    };

    static std::string get_statement_type_name(StatementType type)
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
            default: return "UNDEFINED";
        }
    }

    class Statement
    {
        int _lineIndex;

        public:
            Statement(int lineIndex)
            {
                _lineIndex = lineIndex;
            }

            virtual ~Statement()
            {
                
            }

            virtual StatementType get_type()
            {
                throw std::runtime_error("Statement function get_type not implemented!");
            }

            int get_line_index()
            {
                return _lineIndex;
            }
    };

    class UnaryOperator : public Statement
    {
        public:
            Statement* value;

            UnaryOperator(Statement* value, int lineIndex) : Statement(lineIndex)
            {
                this->value = value;
            }

            ~UnaryOperator() override
            {
                if (value != NULL)
                    delete value;
            }

            StatementType get_type() 
            {
                return StatementType::UNARY_OPERATOR;
            }
    };

    class BinaryOperator : public Statement
    {
        public:
            Statement* left;
            Statement* right;

            BinaryOperator(Statement* left, Statement* right, int lineIndex) : Statement(lineIndex)
            {
                this->left = left;
                this->right = right;
            }

            ~BinaryOperator() override
            {
                if (left != NULL) 
                    delete left;
                
                if (right != NULL)
                    delete right;
            }

            StatementType get_type() 
            {
                return StatementType::BINARY_OPERATOR;
            }
    };

    class AddOperator : public BinaryOperator
    {
        public:
            AddOperator(Statement* left, Statement* right, int lineIndex) : BinaryOperator(left, right, lineIndex) { }

            StatementType get_type()
            {
                return StatementType::ADD_OPERATOR;
            }
    };

    class SubtractOperator : public BinaryOperator
    {
        public:
            SubtractOperator(Statement* left, Statement* right, int lineIndex) : BinaryOperator(left, right, lineIndex) { }

            StatementType get_type()
            {
                return StatementType::SUB_OPERATOR;
            }
    };

    class DivideOperator : public BinaryOperator
    {
        public:
            DivideOperator(Statement* left, Statement* right, int lineIndex) : BinaryOperator(left, right, lineIndex) { }

            StatementType get_type()
            {
                return StatementType::DIV_OPERATOR;
            }
    };

    class ModulusOperator : public BinaryOperator
    {
        public:
            ModulusOperator(Statement* left, Statement* right, int lineIndex) : BinaryOperator(left, right, lineIndex) { }

            StatementType get_type()
            {
                return StatementType::MODULUS_OPERATOR;
            }
    };

    class MultiplyOperator : public BinaryOperator
    {
        public:
            MultiplyOperator(Statement* left, Statement* right, int lineIndex) : BinaryOperator(left, right, lineIndex) { }

            StatementType get_type()
            {
                return StatementType::MUL_OPERATOR;
            }
    };

    class EqualsOperator : public BinaryOperator
    {
        public:
            EqualsOperator(Statement* left, Statement* right, int lineIndex) : BinaryOperator(left, right, lineIndex) { }

            StatementType get_type()
            {
                return StatementType::EQUALS_OPERATOR;
            }
    };

    class NotEqualsOperator : public BinaryOperator
    {
        public:
            NotEqualsOperator(Statement* left, Statement* right, int lineIndex) : BinaryOperator(left, right, lineIndex) { }

            StatementType get_type()
            {
                return StatementType::NOT_EQUALS_OPERATOR;
            }
    };

    class GreaterThanOperator : public BinaryOperator
    {
        public:
            GreaterThanOperator(Statement* left, Statement* right, int lineIndex) : BinaryOperator(left, right, lineIndex) { }

            StatementType get_type()
            {
                return StatementType::GREATER_THAN_OPERATOR;
            }
    };

    class LessThanOperator : public BinaryOperator
    {
        public:
            LessThanOperator(Statement* left, Statement* right, int lineIndex) : BinaryOperator(left, right, lineIndex) { }

            StatementType get_type()
            {
                return StatementType::LESS_THAN_OPERATOR;
            }
    };

    class GreaterThanEqualsOperator : public BinaryOperator
    {
        public:
            GreaterThanEqualsOperator(Statement* left, Statement* right, int lineIndex) : BinaryOperator(left, right, lineIndex) { }

            StatementType get_type()
            {
                return StatementType::GREATER_THAN_EQUALS_OPERATOR;
            }
    };

    class LessThanEqualsOperator : public BinaryOperator
    {
        public:
            LessThanEqualsOperator(Statement* left, Statement* right, int lineIndex) : BinaryOperator(left, right, lineIndex) { }

            StatementType get_type()
            {
                return StatementType::LESS_THAN_EQUALS_OPERATOR;
            }
    };

    class AndCondition : public BinaryOperator
    {
        public:
            AndCondition(Statement* left, Statement* right, int lineIndex) : BinaryOperator(left, right, lineIndex) { }

            StatementType get_type()
            {
                return StatementType::AND_CONDITION;
            }
    };

    class OrCondition : public BinaryOperator
    {
        public:
            OrCondition(Statement* left, Statement* right, int lineIndex) : BinaryOperator(left, right, lineIndex) { }

            StatementType get_type()
            {
                return StatementType::OR_CONDITION;
            }
    };

    class Double : public Statement
    {
        public:
            double value;

            Double(double value, int lineIndex) : Statement(lineIndex)
            {
                this->value = value;
            }

            StatementType get_type() 
            {
                return StatementType::DOUBLE;
            }
    };

    class Integer : public Statement
    {
        public:
            int value;

            Integer(int value, int lineIndex) : Statement(lineIndex)
            {
                this->value = value;
            }

            StatementType get_type() 
            {
                return StatementType::INTEGER;
            }
    };

    class String : public Statement
    {
        public:
            std::string value;

            String(std::string value, int lineIndex) : Statement(lineIndex)
            {
                this->value = value;
            }

            StatementType get_type() 
            {
                return StatementType::STRING;
            }
    };

    class Expression : public Statement
    {
        public:
            Statement* root;

            Expression(Statement* root, int lineIndex) : Statement(lineIndex)
            {
                this->root = root;
            }

            ~Expression() override
            {
                if (root != NULL)
                    delete root;
            }

            StatementType get_type()
            {
                return StatementType::EXPRESSION;
            }
    };

    class VariableAssignment : public Statement
    {
        public:
            std::string variableName;
            Expression* expression;

            VariableAssignment(std::string variableName, Expression* expression, int lineIndex) : Statement(lineIndex)
            {
                this->variableName = variableName;
                this->expression = expression;
            }

            ~VariableAssignment() override
            {
                if (expression != NULL)
                    delete expression;
            }

            StatementType get_type()
            {
                return StatementType::VARIABLE_ASSIGNMENT;
            }
    };

    class Variable : public Statement
    {
        public:
            std::string name;

            Variable(std::string name, int lineIndex) : Statement(lineIndex)
            {
                this->name = name;
            }

            StatementType get_type()
            {
                return StatementType::VARIABLE;
            }
    };

    class Block : public Statement
    {
        public:
            std::vector<Statement*> statements;

            Block(int lineIndex) : Statement(lineIndex)
            {

            }

            ~Block() override
            {
                for (int i = 0; i < statements.size(); ++i)
                {
                    if (statements[i] != NULL)
                        delete statements[i];
                }
            }

            StatementType get_type()
            {
                return StatementType::BLOCK;
            }
    };

    class FunctionCall : public Statement
    {
        public:
            std::string functionName;
            std::vector<Expression*> parameterList;

            FunctionCall(std::string functionName, int lineIndex) : Statement(lineIndex)
            {
                this->functionName = functionName;
            }

            ~FunctionCall() override
            {
                for (int i = 0; i < parameterList.size(); ++i)
                {
                    if (parameterList[i] != NULL)
                        delete parameterList[i];
                }
            }

            StatementType get_type()
            {
                return StatementType::FUNCTION_CALL;
            }
    };

    class Negate : public UnaryOperator
    {
        public:
            Negate(Statement* value, int lineIndex) : UnaryOperator(value, lineIndex) { }

            StatementType get_type()
            {
                return StatementType::NEGATE;
            }
    };

    class Boolean : public Statement
    {
        public:
            bool value;

            Boolean(bool value, int lineIndex) : Statement(lineIndex)
            {
                this->value = value;
            }

            StatementType get_type()
            {
                return StatementType::BOOLEAN;
            }
    };

    class If : public Statement
    {

        public:
            Expression* condition;
            Block* body;
            Statement* elseOrIf;

            If(Expression* condition, Block* body, int lineIndex) : Statement(lineIndex)
            {
                this->condition = condition;
                this->body = body;
                this->elseOrIf = nullptr;
            }

            ~If() override
            {
                if (condition != nullptr)
                    delete condition;

                if (body != nullptr)
                    delete body;
            }

            StatementType get_type()
            {
                return StatementType::IF;
            }
    };

    class While : public Statement
    {
        public:
            Expression* condition;
            Block* body;

            While(Expression* condition, Block* body, int lineIndex) : Statement(lineIndex)
            {
                this->condition = condition;
                this->body = body;
            }

            ~While()
            {
                if (condition != nullptr)
                    delete condition;

                if (body != nullptr)
                    delete body;
            }

            StatementType get_type()
            {
                return StatementType::WHILE;
            }
    };

    class Else : public Statement
    {
        public:
            Block* body;

            Else(Block* body, int lineIndex) : Statement(lineIndex)
            {
                this->body = body;
            }

            ~Else() override
            {
                if (body != nullptr)
                    delete body;
            }

            StatementType get_type()
            {
                return StatementType::ELSE;
            }
    };

    class AST
    {
        public:
            std::vector<Statement*> statements;

            ~AST()
            {
                for (auto stmt : statements)
                {
                    if (stmt != NULL)
                        delete stmt;
                }
            }

            void print()
            {
                for (auto stmt : statements)
                    print_statement(stmt, "");
            }

        private:
            void print_statement(Statement* stat, std::string indent)
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
                }
            }
    };
}

#endif // _AST