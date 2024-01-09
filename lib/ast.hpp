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
        FUNCTION,
        RETURN,
        POWER_OPERATOR,
    };

    std::string get_statement_type_name(StatementType type);

    class Statement
    {
        int _lineIndex;

        public:
            Statement(int lineIndex);
            virtual ~Statement();

            virtual StatementType get_type();
            int get_line_index();
    };

    class UnaryOperator : public Statement
    {
        public:
            Statement* value;

            UnaryOperator(Statement* value, int lineIndex);
            ~UnaryOperator() override;

            StatementType get_type() override;
    };

    class BinaryOperator : public Statement
    {
        public:
            Statement* left;
            Statement* right;

            BinaryOperator(Statement* left, Statement* right, int lineIndex);
            ~BinaryOperator() override;

            StatementType get_type() override;
    };

    class AddOperator : public BinaryOperator
    {
        public:
            AddOperator(Statement* left, Statement* right, int lineIndex);

            StatementType get_type() override;
    };

    class SubtractOperator : public BinaryOperator
    {
        public:
            SubtractOperator(Statement* left, Statement* right, int lineIndex);

            StatementType get_type() override;
    };

    class DivideOperator : public BinaryOperator
    {
        public:
            DivideOperator(Statement* left, Statement* right, int lineIndex);

            StatementType get_type() override;
    };

    class ModulusOperator : public BinaryOperator
    {
        public:
            ModulusOperator(Statement* left, Statement* right, int lineIndex);

            StatementType get_type() override;
    };

    class MultiplyOperator : public BinaryOperator
    {
        public:
            MultiplyOperator(Statement* left, Statement* right, int lineIndex);

            StatementType get_type() override;
    };

    class EqualsOperator : public BinaryOperator
    {
        public:
            EqualsOperator(Statement* left, Statement* right, int lineIndex);

            StatementType get_type() override;
    };

    class PowerOperator : public BinaryOperator
    {
        public:
            PowerOperator(Statement* left, Statement* right, int lineIndex);

            StatementType get_type() override;
    };

    class NotEqualsOperator : public BinaryOperator
    {
        public:
            NotEqualsOperator(Statement* left, Statement* right, int lineIndex);

            StatementType get_type() override;
    };

    class GreaterThanOperator : public BinaryOperator
    {
        public:
            GreaterThanOperator(Statement* left, Statement* right, int lineIndex);

            StatementType get_type() override;
    };

    class LessThanOperator : public BinaryOperator
    {
        public:
            LessThanOperator(Statement* left, Statement* right, int lineIndex);

            StatementType get_type() override;
    };

    class GreaterThanEqualsOperator : public BinaryOperator
    {
        public:
            GreaterThanEqualsOperator(Statement* left, Statement* right, int lineIndex);

            StatementType get_type() override;
    };

    class LessThanEqualsOperator : public BinaryOperator
    {
        public:
            LessThanEqualsOperator(Statement* left, Statement* right, int lineIndex);

            StatementType get_type() override;
    };

    class AndCondition : public BinaryOperator
    {
        public:
            AndCondition(Statement* left, Statement* right, int lineIndex);

            StatementType get_type() override;
    };

    class OrCondition : public BinaryOperator
    {
        public:
            OrCondition(Statement* left, Statement* right, int lineIndex);

            StatementType get_type();
    };

    class Double : public Statement
    {
        public:
            double value;

            Double(double value, int lineIndex);

            StatementType get_type() override;
    };

    class Integer : public Statement
    {
        public:
            int value;

            Integer(int value, int lineIndex);

            StatementType get_type() override;
    };

    class String : public Statement
    {
        public:
            std::string value;

            String(std::string value, int lineIndex);

            StatementType get_type() override;
    };

    class Expression : public Statement
    {
        public:
            Statement* root;

            Expression(Statement* root, int lineIndex);
            ~Expression() override;

            StatementType get_type() override;
    };

    class VariableAssignment : public Statement
    {
        public:
            std::string variableName;
            Expression* expression;

            VariableAssignment(std::string variableName, Expression* expression, int lineIndex);
            ~VariableAssignment() override;

            StatementType get_type() override;
    };

    class Variable : public Statement
    {
        public:
            std::string name;

            Variable(std::string name, int lineIndex);
            
            StatementType get_type() override;
    };

    class Block : public Statement
    {
        public:
            std::vector<Statement*> statements;

            Block(int lineIndex);
            ~Block() override;

            StatementType get_type() override;
    };

    class FunctionCall : public Statement
    {
        public:
            std::string functionName;
            std::vector<Expression*> parameterList;

            FunctionCall(std::string functionName, int lineIndex);
            ~FunctionCall() override;

            StatementType get_type() override;
    };

    class Function : public Statement
    {
        public:
            std::string functionName;
            std::vector<std::string> parameterNames;
            Block* body;

            Function(std::string functionName, Block* body, int lineIndex);
            ~Function() override;

            StatementType get_type() override;
    };

    class Negate : public UnaryOperator
    {
        public:
            Negate(Statement* value, int lineIndex);

            StatementType get_type() override;
    };

    class Boolean : public Statement
    {
        public:
            bool value;

            Boolean(bool value, int lineIndex);

            StatementType get_type() override;
    };

    class If : public Statement
    {
        public:
            Expression* condition;
            Block* body;
            Statement* elseOrIf;

            If(Expression* condition, Block* body, int lineIndex);
            ~If() override;

            StatementType get_type() override;
    };

    class Else : public Statement
    {
        public:
            Block* body;

            Else(Block* body, int lineIndex);
            ~Else() override;

            StatementType get_type() override;
    };

    class While : public Statement
    {
        public:
            Expression* condition;
            Block* body;

            While(Expression* condition, Block* body, int lineIndex);
            ~While() override;

            StatementType get_type() override;
    };

    class Return : public Statement
    {
        public:
            Expression* expression;

            Return(Expression* expression, int lineIndex);
            ~Return() override;

            StatementType get_type() override;
    };
 
    class AST
    {
        public:
            Block* root;

            ~AST();
            void print();

        private:
            void print_statement(Statement* stat, std::string indent);
    };
}

#endif // _AST