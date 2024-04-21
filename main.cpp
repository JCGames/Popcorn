
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <memory>
#include <cstring>

bool DEBUG_MODE = false;

#pragma region Boilerplate

/**
 * This structure is used for saving the contents
 * of a file as lines in memory.
*/
struct File
{
private:
    std::vector<std::string> lines;

public:
    /**
     * Gets a line from the file.
    */
    std::string get_line(const int& index)
    {
        return lines[index];
    }

    /**
     * Adds a line to the file.
    */
    void add_line(std::string line)
    {
        lines.push_back(line);
    }

    /**
     * Gets the file length in lines.
    */
    int get_length() const
    {
        return lines.size();
    }

    /**
     * Loads a file.
    */
    void load_file(std::string name)
    {
        std::ifstream ifs(name);

        if (!ifs.is_open())
            throw std::runtime_error("Could not open file!");

        for (std::string line; std::getline(ifs, line); )
            add_line(line);

        ifs.close();
    }
};

/**
 * Responsible for showing
 * diagnostic information
*/
struct Diagnostics
{
private:
    std::vector<std::string> warnings;
    std::vector<std::string> errors;

public:
    /**
     * Adds a warning to the diagnostics.
    */
    void add_warning(std::string message, std::string line, unsigned int lineColumn, unsigned int lineNumber)
    {
        std::string warning = "WARNING (" + std::to_string(lineNumber) + ":" + std::to_string(lineColumn) + "): ";
        std::string space = "";

        for (int i = 0; i < lineColumn; ++i)
            space += ' ';

        warning += message + "\n\t" + line + "\n\t" + space + "^";
        warnings.push_back(warning);
    }

    /**
     * Adds a error to the diagnostics.
    */
    void add_error(std::string message, std::string line, unsigned int lineColumn, unsigned int lineNumber)
    {
        std::string error = "ERROR (" + std::to_string(lineNumber) + ":" + std::to_string(lineColumn) + "): ";
        std::string space = "";

        for (int i = 0; i < lineColumn; ++i)
            space += ' ';

        error += message + "\n\t" + line + "\n\t" + space + "^";
        errors.push_back(error);
    }

    /**
     * Does the diagnostics have warnings.
    */
    bool has_warnings() const
    {
        return warnings.size() > 0;
    }

    /**
     * Does the diagnostics have errors.
    */
    bool has_errors() const
    {
        return errors.size() > 0;
    }

    void dump()
    {
        for (auto& warning : warnings)
            std::cout << warning << std::endl;

        for (auto& error : errors)
            std::cout << error << std::endl;
    }
};

#pragma endregion

#pragma region Tokenizer

/**
 * Determines the type of token.
*/
enum class TokenType : char
{
    _EOF,
    EOL,
    PLUS,
    SUB,
    MULT,
    DIV,
    MOD,
    WORD,
    COLON,
    ASSIGNMENT,
    NUMBER,
    CHAR,
    ERROR
};

/**
 * The data structure for holding a token.
*/
struct Token
{
    std::string value;
    TokenType type;
    std::string line;
    unsigned int lineColumn;
    unsigned int lineNumber;

    Token(std::string value,
        TokenType type,
        std::string line,
        unsigned int lineColumn,
        unsigned int lineNumber)
    {
        this->value = value;
        this->type = type;
        this->line = line;
        this->lineColumn = lineColumn;
        this->lineNumber = lineNumber;
    }

    void print_token()
    {
        std::cout << "Value: |" << value << "| Type: |" << static_cast<int>(type) << "| Line Column: " << lineColumn << " Line Number: " << lineNumber << " Line: " << line << "\n"; 
    }
};

/**
 * The data structure responsible for compiling a list of tokens.
*/
struct Tokenizer
{
private:
    std::vector<Token> tokens;
    File* file;
    Diagnostics* diagnostics;
    std::string currentLine;
    unsigned int lineColumn;
    unsigned int lineNumber;
    bool skipLine;

    bool eol() const
    {
        return lineColumn >= currentLine.size();
    }

    char get()
    {
        if (lineColumn >= currentLine.size())
            return '\0';
        return currentLine[lineColumn];
    }

    char next()
    {
        if (lineColumn + 1 >= currentLine.size())
            return '\0';
        return currentLine[lineColumn + 1];
    }

    void parse_lines()
    {
        for (lineNumber = 0; lineNumber < file->get_length(); ++lineNumber)
            parse_line();

        tokens.push_back(Token("EOF", TokenType::_EOF, currentLine, lineColumn, lineNumber));    
    }

    void parse_line()
    {
        currentLine = file->get_line(lineNumber);
        skipLine = false;
        
        for (lineColumn = 0; lineColumn < currentLine.length() && !skipLine; ++lineColumn)
            parse_token();

        tokens.push_back(Token("EOL", TokenType::EOL, currentLine, lineColumn, lineNumber));
    }

    void parse_token()
    {
        if (get() == '/' && next() == '/')
        {
            skipLine = true;
            return;
        }
        // if is word
        else if (isalpha(get()) || get() == '_')
        {
            std::string value = "";

            while (!eol() && (isalnum(get()) || get() == '_'))
            {
                value += get();
                ++lineColumn;
            }

            --lineColumn;

            tokens.push_back(Token(value, TokenType::WORD, currentLine, lineColumn, lineNumber));
        }
        else if (isdigit(get()) || (get() == '.' && isdigit(next())))
        {
            std::string value = "";
            bool hasDecimalPoint = false;

            while (!eol() && (isdigit(get()) || get() == '.'))
            {
                if (hasDecimalPoint && get() == '.')
                    diagnostics->add_error("To many decimal points in number!", currentLine, lineColumn, lineNumber);
                else if (get() == '.')
                    hasDecimalPoint = true;

                value += get();
                ++lineColumn;
            }

            --lineColumn;

            tokens.push_back(Token(value, TokenType::NUMBER, currentLine, lineColumn, lineNumber));
        }
        else if (get() == '\'')
        {
            ++lineColumn;
            char value = get();
            ++lineColumn;

            if (get() != '\'')
                diagnostics->add_error("Missing closing single quotes!", currentLine, lineColumn, lineNumber);
            
            tokens.push_back(Token(std::string(1, value), TokenType::CHAR, currentLine, lineColumn, lineNumber));
        }
        else if (get() == '+')
        {
            tokens.push_back(Token("+", TokenType::PLUS, currentLine, lineColumn, lineNumber));
        }
        else if (get() == '-')
        {
            tokens.push_back(Token("-", TokenType::SUB, currentLine, lineColumn, lineNumber));
        }
        else if (get() == '*')
        {
            tokens.push_back(Token("*", TokenType::MULT, currentLine, lineColumn, lineNumber));
        }
        else if (get() == '/')
        {
            tokens.push_back(Token("/", TokenType::DIV, currentLine, lineColumn, lineNumber));
        }
        else if (get() == '%')
        {
            tokens.push_back(Token("%", TokenType::MOD, currentLine, lineColumn, lineNumber));
        }
        else if (get() == ':')
        {
            tokens.push_back(Token(":", TokenType::COLON, currentLine, lineColumn, lineNumber));
        }
        else if (get() == '=')
        {
            tokens.push_back(Token("=", TokenType::ASSIGNMENT, currentLine, lineColumn, lineNumber));
        }
    }

public:
    void print_tokens()
    {
        for (auto& token : tokens)
            token.print_token();
    }

    void parse_file(File* file, Diagnostics* diagnostics)
    {
        this->file = file;
        this->diagnostics = diagnostics;
        parse_lines();
    }

    std::vector<Token>* get_tokens()
    {
        return &tokens;
    }
};

#pragma endregion

#pragma region Parser

enum class StatementType
{
    VARIABLE,
    NUMBER,
    CHAR,
    BLOCK,
    ASSIGN,
    ADD_OP,
    SUB_OP,
    MULT_OP,
    DIV_OP,
    MOD_OP,
    ERROR,
    EXP,
};

std::string statement_type_as_str(const StatementType& type)
{
    switch (type)
    {
    case StatementType::VARIABLE:
        return "VARIABLE";
    case StatementType::NUMBER:
        return "NUMBER";
    case StatementType::CHAR:
        return "CHARACTER";
    case StatementType::BLOCK:
        return "BLOCK";
    case StatementType::ASSIGN:
        return "ASSIGNMENT";
    case StatementType::ADD_OP:
        return "ADD OPERATOR";
    case StatementType::SUB_OP:
        return "SUBTRACT OPERATOR";
    case StatementType::MULT_OP:
        return "MULTIPLY OPERATOR";
    case StatementType::DIV_OP:
        return "DIVIDE OPERATOR";
    case StatementType::MOD_OP:
        return "MODULUS OPERATOR";
    case StatementType::ERROR:
        return "ERROR";
    case StatementType::EXP:
        return "EXPRESSION";
    }
}

#pragma region Data Structures for Statements

struct StatementInfo { };

struct SI_Assign : public StatementInfo
{
    std::string variableName;
};

struct SI_Number : public StatementInfo
{
    std::string number;
};

struct SI_Char : public StatementInfo
{
    std::string character;
};

struct SI_Variable : public StatementInfo
{
    std::string name;
};

#pragma endregion

struct Statement
{
    std::shared_ptr<StatementInfo> info;
    StatementType type;
    std::vector<Statement> children;

    Statement() { }

    Statement(StatementType type)
    {
        this->type = type;
    }
};

struct Parser
{
private:
    std::vector<Token>* tokens;
    Diagnostics* diagnostics;
    Statement root;
    unsigned int currentToken;

    /**
     * Checks to see if we are at the end of the token stream.
    */
    bool eof() const
    {
        return currentToken >= tokens->size() || tokens->at(currentToken).type == TokenType::_EOF;
    }

    /**
     * Increments the current token index.
    */
    void move_next()
    {
        ++currentToken;
    }

    /**
     * Gets the current token.
     * Returns an ERROR token if failure.
    */
    Token get()
    {
        if (currentToken >= tokens->size())
            return Token("ERROR", TokenType::ERROR, "", 0, 0);
        return tokens->at(currentToken);
    }

    /**
     * Gets the previous token.
     * Returns an ERROR token if failure.
    */
    Token prev()
    {
        if (currentToken - 1 < 0 || currentToken - 1 >= tokens->size())
            return Token("ERROR", TokenType::ERROR, "", 0, 0);
        return tokens->at(currentToken - 1);
    }

    /**
     * Gets the next token.
     * Returns an ERROR token if failure.
    */
    Token next()
    {
        if (currentToken + 1 >= tokens->size())
            return Token("ERROR", TokenType::ERROR, "", 0, 0);
        return tokens->at(currentToken + 1);
    }

    /**
     * Parses a single statement.
    */
    Statement parse_statement()
    {
        Statement result(StatementType::ERROR);

        if (get().type == TokenType::WORD && next().type == TokenType::ASSIGNMENT)
        {
            std::shared_ptr<SI_Assign> siAssign = std::make_shared<SI_Assign>();
            siAssign->variableName = get().value;

            move_next();
            move_next(); // skip the = sign

            Statement expression = parse_expression();

            result.children.push_back(expression);

            result.info = siAssign;
            result.type = StatementType::ASSIGN;
        }

        return result;
    }

    Statement parse_expression()
    {
        Statement expression(StatementType::EXP);
        expression.children.push_back(parse_add_sub());
        return expression;
    }

    Statement parse_add_sub()
    {
        Statement left = parse_mult_div_mod();

        while (get().type == TokenType::PLUS || get().type == TokenType::SUB)
        {
            if (get().type == TokenType::PLUS)
            {
                move_next();
                Statement newLeft;
                newLeft.type = StatementType::ADD_OP;

                newLeft.children.push_back(left);
                newLeft.children.push_back(parse_mult_div_mod());

                left = newLeft;
            }
            else if (get().type == TokenType::SUB)
            {
                move_next();
                Statement newLeft;
                newLeft.type = StatementType::SUB_OP;

                newLeft.children.push_back(left);
                newLeft.children.push_back(parse_mult_div_mod());

                left = newLeft;
            }
        }

        return left;
    }

    Statement parse_mult_div_mod()
    {
        Statement left = parse_term();

        while (get().type == TokenType::MULT || get().type == TokenType::DIV || get().type == TokenType::MOD)
        {
            if (get().type == TokenType::MULT)
            {
                move_next();
                Statement newLeft;
                newLeft.type = StatementType::MULT_OP;

                newLeft.children.push_back(left);
                newLeft.children.push_back(parse_term());

                left = newLeft;
            }
            else if (get().type == TokenType::DIV)
            {
                move_next();
                Statement newLeft;
                newLeft.type = StatementType::DIV_OP;

                newLeft.children.push_back(left);
                newLeft.children.push_back(parse_term());

                left = newLeft;
            }
            else if (get().type == TokenType::MOD)
            {
                move_next();
                Statement newLeft;
                newLeft.type = StatementType::MOD_OP;

                newLeft.children.push_back(left);
                newLeft.children.push_back(parse_term());

                left = newLeft;
            }
        }

        return left;
    }

    Statement parse_term()
    {
        Statement result(StatementType::ERROR);

        if (get().type == TokenType::NUMBER)
        {
            std::shared_ptr<SI_Number> siNumber = std::make_shared<SI_Number>();
            siNumber->number = get().value;
            result.info = siNumber;
            result.type = StatementType::NUMBER;
        }
        else if (get().type == TokenType::CHAR)
        {
            std::shared_ptr<SI_Char> siChar = std::make_shared<SI_Char>();
            siChar->character = get().value;
            result.info = siChar;
            result.type = StatementType::CHAR;
        }
        else if (get().type == TokenType::WORD)
        {
            std::shared_ptr<SI_Variable> siVariable = std::make_shared<SI_Variable>();
            siVariable->name = get().value;
            result.info = siVariable;
            result.type = StatementType::VARIABLE;
        }

        if (result.type == StatementType::ERROR)
            diagnostics->add_error("That is not a term!", get().line, get().lineColumn, get().lineNumber);

        move_next();

        return result;
    }

    /**
     * Prints a single statement.
    */
    void print_statement(const Statement& statement, std::string padding)
    {
        std::string typeName = statement_type_as_str(statement.type);

        std::cout << padding << typeName << std::endl;

        switch (statement.type)
        {
        case StatementType::ASSIGN:
            if (SI_Assign* siAssign = static_cast<SI_Assign*>(statement.info.get()))
            {
                std::cout << padding << "Variable Name: " << siAssign->variableName << std::endl;

                for (auto& child : statement.children)
                    print_statement(child, padding + "\t");
            }
            break;
        case StatementType::NUMBER:
            if (SI_Number* siNumber = static_cast<SI_Number*>(statement.info.get()))
            {
                std::cout << padding << "Value: " << siNumber->number << std::endl;
            }
            break;
        case StatementType::CHAR:
            if (SI_Char* siChar = static_cast<SI_Char*>(statement.info.get()))
            {
                std::cout << padding << "Value: " << siChar->character << std::endl;
            }
            break;
        case StatementType::VARIABLE:
            if (SI_Variable* siVariable = static_cast<SI_Variable*>(statement.info.get()))
            {
                std::cout << padding << "Value: " << siVariable->name << std::endl;
            }
            break;
        default:
            for (auto& child : statement.children)
                print_statement(child, padding + "\t");
            break;
        }
    }

public:
    Parser() 
    {
        tokens = nullptr;
    }

    Statement* get_root()
    {
        return &root;
    }

    /**
     * Parses all of the statements in a given list of tokens.
    */
    void parse_statements(std::vector<Token>* tokens, Diagnostics* diagnostics)
    {
        this->tokens = tokens;
        this->diagnostics = diagnostics;
        root.type = StatementType::BLOCK;
        
        for (currentToken = 0; currentToken < tokens->size(); ++currentToken)
        {
            Statement statement = parse_statement();

            if (statement.type != StatementType::ERROR)
                root.children.push_back(statement);
        }
    }

    /**
     * Prints the abstract syntax tree.
    */
    void print_ast()
    {
        print_statement(root, "");
    }
};

#pragma endregion

enum class ObjectType
{
    INT32,
    FLOAT32,
    CHAR,
    NIL
};

struct Object
{
    ObjectType type;
    std::shared_ptr<void> value;

    void print()
    {
        if (type == ObjectType::INT32)
        {
            std::cout << "Result: " << *static_cast<int*>(value.get()) << std::endl;
        }
        else if (type == ObjectType::FLOAT32)
        {
            std::cout << "Result: " << *static_cast<float*>(value.get()) << std::endl;
        }
        else if (type == ObjectType::CHAR)
        {
            std::cout << "Result: " << *static_cast<char*>(value.get()) << std::endl;
        }
    }
};

struct StackAllocation
{
    std::string variableName;
    Object value;
};

struct Scope
{
private:
    Scope* parent;
    std::vector<StackAllocation> stack;

public:
    Object get_obj(std::string variableName)
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

    void set_obj(std::string variableName, Object value)
    {
        for (auto& sa : stack)
        {
            if (sa.variableName == variableName)
            {
                sa.value = value;
                return;
            }
        }

        StackAllocation sa;
        sa.variableName = variableName;
        sa.value = value;
        stack.push_back(sa);
    }

    void delete_obj()
    {
        
    }

    void set_parent(Scope* parent)
    {
        this->parent = parent;
    }

    std::vector<StackAllocation>& get_stack()
    {
        return stack;
    }
};

struct Runner 
{
private:
    Statement* root;

    void run_block(Scope* parentScope)
    {
        Scope currentScope;
        currentScope.set_parent(parentScope);

        for (auto& statement : root->children)
        {
            if (statement.type == StatementType::ASSIGN)
            {
                if (SI_Assign* siAssign = static_cast<SI_Assign*>(statement.info.get()))
                {
                    currentScope.set_obj(siAssign->variableName, eval_expression(statement.children[0]));
                }
            }
        }

        std::cout << "STACK:" << std::endl;
        
        for (auto& sa : currentScope.get_stack())
        {
            std::cout << "Variable Name: " << sa.variableName << std::endl;
            sa.value.print();
        }
    }

    Object eval_expression(const Statement& statement)
    {
        switch (statement.type)
        {
        case StatementType::EXP:
            {
                return eval_expression(statement.children[0]);
            }
            break;
        case StatementType::NUMBER:
            if (SI_Number* siNumber = static_cast<SI_Number*>(statement.info.get()))
            {
                Object number;
                if (siNumber->number.find('.') > -1)
                {
                    // is float
                    number.type = ObjectType::FLOAT32;
                    number.value = std::make_shared<float>(std::stof(siNumber->number));
                    return number;
                }
                else
                {
                    // is int
                    number.type = ObjectType::INT32;
                    number.value = std::make_shared<int>(std::stoi(siNumber->number));
                    return number;
                } 
            }
            break;
        case StatementType::CHAR:
            if (SI_Char* siChar = static_cast<SI_Char*>(statement.info.get()))
            {
                Object _char;
                _char.type = ObjectType::CHAR;
                _char.value = std::make_shared<char>(siChar->character[0]);
                return _char;
            }
            break;
        case StatementType::ADD_OP:
            {
                Object left = eval_expression(statement.children[0]);
                Object right = eval_expression(statement.children[1]);

                if (left.type == ObjectType::INT32)
                {
                    if (right.type == ObjectType::INT32)
                    {
                        Object number;
                        number.type = ObjectType::INT32;
                        number.value = std::make_shared<int>(*static_cast<int*>(left.value.get()) + *static_cast<int*>(right.value.get()));
                        return number;
                    }
                    else if (right.type == ObjectType::FLOAT32)
                    {
                        Object number;
                        number.type = ObjectType::FLOAT32;
                        number.value = std::make_shared<float>(*static_cast<int*>(left.value.get()) + *static_cast<float*>(right.value.get()));
                        return number;
                    }
                    else if (right.type == ObjectType::CHAR)
                    {
                        Object number;
                        number.type = ObjectType::INT32;
                        number.value = std::make_shared<int>(*static_cast<int*>(left.value.get()) + *static_cast<char*>(right.value.get()));
                        return number;
                    }
                }
                else if (left.type == ObjectType::FLOAT32)
                {
                    if (right.type == ObjectType::INT32)
                    {
                        Object number;
                        number.type = ObjectType::INT32;
                        number.value = std::make_shared<float>(*static_cast<float*>(left.value.get()) + *static_cast<int*>(right.value.get()));
                        return number;
                    }
                    else if (right.type == ObjectType::FLOAT32)
                    {
                        Object number;
                        number.type = ObjectType::FLOAT32;
                        number.value = std::make_shared<float>(*static_cast<float*>(left.value.get()) + *static_cast<float*>(right.value.get()));
                        return number;
                    }
                    else if (right.type == ObjectType::CHAR)
                    {
                        Object number;
                        number.type = ObjectType::INT32;
                        number.value = std::make_shared<float>(*static_cast<float*>(left.value.get()) + *static_cast<char*>(right.value.get()));
                        return number;
                    }
                }
                else if (left.type == ObjectType::CHAR)
                {
                    if (right.type == ObjectType::INT32)
                    {
                        Object number;
                        number.type = ObjectType::INT32;
                        number.value = std::make_shared<int>(*static_cast<char*>(left.value.get()) + *static_cast<int*>(right.value.get()));
                        return number;
                    }
                    else if (right.type == ObjectType::FLOAT32)
                    {
                        Object number;
                        number.type = ObjectType::FLOAT32;
                        number.value = std::make_shared<float>(*static_cast<char*>(left.value.get()) + *static_cast<float*>(right.value.get()));
                        return number;
                    }
                    else if (right.type == ObjectType::CHAR)
                    {
                        Object number;
                        number.type = ObjectType::INT32;
                        number.value = std::make_shared<int>(*static_cast<char*>(left.value.get()) + *static_cast<char*>(right.value.get()));
                        return number;
                    }
                }
            }
            break;
        case StatementType::SUB_OP:
            {
                Object left = eval_expression(statement.children[0]);
                Object right = eval_expression(statement.children[1]);

                if (left.type == ObjectType::INT32)
                {
                    if (right.type == ObjectType::INT32)
                    {
                        Object number;
                        number.type = ObjectType::INT32;
                        number.value = std::make_shared<int>(*static_cast<int*>(left.value.get()) - *static_cast<int*>(right.value.get()));
                        return number;
                    }
                    else if (right.type == ObjectType::FLOAT32)
                    {
                        Object number;
                        number.type = ObjectType::FLOAT32;
                        number.value = std::make_shared<float>(*static_cast<int*>(left.value.get()) - *static_cast<float*>(right.value.get()));
                        return number;
                    }
                    else if (right.type == ObjectType::CHAR)
                    {
                        Object number;
                        number.type = ObjectType::INT32;
                        number.value = std::make_shared<int>(*static_cast<int*>(left.value.get()) - *static_cast<char*>(right.value.get()));
                        return number;
                    }
                }
                else if (left.type == ObjectType::FLOAT32)
                {
                    if (right.type == ObjectType::INT32)
                    {
                        Object number;
                        number.type = ObjectType::INT32;
                        number.value = std::make_shared<float>(*static_cast<float*>(left.value.get()) - *static_cast<int*>(right.value.get()));
                        return number;
                    }
                    else if (right.type == ObjectType::FLOAT32)
                    {
                        Object number;
                        number.type = ObjectType::FLOAT32;
                        number.value = std::make_shared<float>(*static_cast<float*>(left.value.get()) - *static_cast<float*>(right.value.get()));
                        return number;
                    }
                    else if (right.type == ObjectType::CHAR)
                    {
                        Object number;
                        number.type = ObjectType::INT32;
                        number.value = std::make_shared<float>(*static_cast<float*>(left.value.get()) - *static_cast<char*>(right.value.get()));
                        return number;
                    }
                }
                else if (left.type == ObjectType::CHAR)
                {
                    if (right.type == ObjectType::INT32)
                    {
                        Object number;
                        number.type = ObjectType::INT32;
                        number.value = std::make_shared<int>(*static_cast<char*>(left.value.get()) - *static_cast<int*>(right.value.get()));
                        return number;
                    }
                    else if (right.type == ObjectType::FLOAT32)
                    {
                        Object number;
                        number.type = ObjectType::FLOAT32;
                        number.value = std::make_shared<float>(*static_cast<char*>(left.value.get()) - *static_cast<float*>(right.value.get()));
                        return number;
                    }
                    else if (right.type == ObjectType::CHAR)
                    {
                        Object number;
                        number.type = ObjectType::INT32;
                        number.value = std::make_shared<int>(*static_cast<char*>(left.value.get()) - *static_cast<char*>(right.value.get()));
                        return number;
                    }
                }
            }
            break;
        case StatementType::MULT_OP:
            {
                Object left = eval_expression(statement.children[0]);
                Object right = eval_expression(statement.children[1]);

                if (left.type == ObjectType::INT32)
                {
                    if (right.type == ObjectType::INT32)
                    {
                        Object number;
                        number.type = ObjectType::INT32;
                        number.value = std::make_shared<int>(*static_cast<int*>(left.value.get()) * *static_cast<int*>(right.value.get()));
                        return number;
                    }
                    else if (right.type == ObjectType::FLOAT32)
                    {
                        Object number;
                        number.type = ObjectType::FLOAT32;
                        number.value = std::make_shared<float>(*static_cast<int*>(left.value.get()) * *static_cast<float*>(right.value.get()));
                        return number;
                    }
                    else if (right.type == ObjectType::CHAR)
                    {
                        Object number;
                        number.type = ObjectType::INT32;
                        number.value = std::make_shared<int>(*static_cast<int*>(left.value.get()) * *static_cast<char*>(right.value.get()));
                        return number;
                    }
                }
                else if (left.type == ObjectType::FLOAT32)
                {
                    if (right.type == ObjectType::INT32)
                    {
                        Object number;
                        number.type = ObjectType::INT32;
                        number.value = std::make_shared<float>(*static_cast<float*>(left.value.get()) * *static_cast<int*>(right.value.get()));
                        return number;
                    }
                    else if (right.type == ObjectType::FLOAT32)
                    {
                        Object number;
                        number.type = ObjectType::FLOAT32;
                        number.value = std::make_shared<float>(*static_cast<float*>(left.value.get()) * *static_cast<float*>(right.value.get()));
                        return number;
                    }
                    else if (right.type == ObjectType::CHAR)
                    {
                        Object number;
                        number.type = ObjectType::INT32;
                        number.value = std::make_shared<float>(*static_cast<float*>(left.value.get()) * *static_cast<char*>(right.value.get()));
                        return number;
                    }
                }
                else if (left.type == ObjectType::CHAR)
                {
                    if (right.type == ObjectType::INT32)
                    {
                        Object number;
                        number.type = ObjectType::INT32;
                        number.value = std::make_shared<int>(*static_cast<char*>(left.value.get()) * *static_cast<int*>(right.value.get()));
                        return number;
                    }
                    else if (right.type == ObjectType::FLOAT32)
                    {
                        Object number;
                        number.type = ObjectType::FLOAT32;
                        number.value = std::make_shared<float>(*static_cast<char*>(left.value.get()) * *static_cast<float*>(right.value.get()));
                        return number;
                    }
                    else if (right.type == ObjectType::CHAR)
                    {
                        Object number;
                        number.type = ObjectType::INT32;
                        number.value = std::make_shared<int>(*static_cast<char*>(left.value.get()) * *static_cast<char*>(right.value.get()));
                        return number;
                    }
                }
            }
            break;
        case StatementType::DIV_OP:
            {
                Object left = eval_expression(statement.children[0]);
                Object right = eval_expression(statement.children[1]);

                if (left.type == ObjectType::INT32)
                {
                    if (right.type == ObjectType::INT32)
                    {
                        Object number;
                        number.type = ObjectType::INT32;
                        number.value = std::make_shared<int>(*static_cast<int*>(left.value.get()) / *static_cast<int*>(right.value.get()));
                        return number;
                    }
                    else if (right.type == ObjectType::FLOAT32)
                    {
                        Object number;
                        number.type = ObjectType::FLOAT32;
                        number.value = std::make_shared<float>(*static_cast<int*>(left.value.get()) / *static_cast<float*>(right.value.get()));
                        return number;
                    }
                    else if (right.type == ObjectType::CHAR)
                    {
                        Object number;
                        number.type = ObjectType::INT32;
                        number.value = std::make_shared<int>(*static_cast<int*>(left.value.get()) / *static_cast<char*>(right.value.get()));
                        return number;
                    }
                }
                else if (left.type == ObjectType::FLOAT32)
                {
                    if (right.type == ObjectType::INT32)
                    {
                        Object number;
                        number.type = ObjectType::INT32;
                        number.value = std::make_shared<float>(*static_cast<float*>(left.value.get()) / *static_cast<int*>(right.value.get()));
                        return number;
                    }
                    else if (right.type == ObjectType::FLOAT32)
                    {
                        Object number;
                        number.type = ObjectType::FLOAT32;
                        number.value = std::make_shared<float>(*static_cast<float*>(left.value.get()) / *static_cast<float*>(right.value.get()));
                        return number;
                    }
                    else if (right.type == ObjectType::CHAR)
                    {
                        Object number;
                        number.type = ObjectType::INT32;
                        number.value = std::make_shared<float>(*static_cast<float*>(left.value.get()) / *static_cast<char*>(right.value.get()));
                        return number;
                    }
                }
                else if (left.type == ObjectType::CHAR)
                {
                    if (right.type == ObjectType::INT32)
                    {
                        Object number;
                        number.type = ObjectType::INT32;
                        number.value = std::make_shared<int>(*static_cast<char*>(left.value.get()) / *static_cast<int*>(right.value.get()));
                        return number;
                    }
                    else if (right.type == ObjectType::FLOAT32)
                    {
                        Object number;
                        number.type = ObjectType::FLOAT32;
                        number.value = std::make_shared<float>(*static_cast<char*>(left.value.get()) / *static_cast<float*>(right.value.get()));
                        return number;
                    }
                    else if (right.type == ObjectType::CHAR)
                    {
                        Object number;
                        number.type = ObjectType::INT32;
                        number.value = std::make_shared<int>(*static_cast<char*>(left.value.get()) / *static_cast<char*>(right.value.get()));
                        return number;
                    }
                }
            }
            break;
        case StatementType::MOD_OP:
            {
                Object left = eval_expression(statement.children[0]);
                Object right = eval_expression(statement.children[1]);

                if (left.type == ObjectType::INT32)
                {
                    if (right.type == ObjectType::INT32)
                    {
                        Object number;
                        number.type = ObjectType::INT32;
                        number.value = std::make_shared<int>(*static_cast<int*>(left.value.get()) % *static_cast<int*>(right.value.get()));
                        return number;
                    }
                    else if (right.type == ObjectType::FLOAT32)
                    {
                        Object number;
                        number.type = ObjectType::FLOAT32;
                        number.value = std::make_shared<float>(0);
                        return number;
                    }
                    else if (right.type == ObjectType::CHAR)
                    {
                        Object number;
                        number.type = ObjectType::INT32;
                        number.value = std::make_shared<int>(*static_cast<int*>(left.value.get()) % *static_cast<char*>(right.value.get()));
                        return number;
                    }
                }
                else if (left.type == ObjectType::FLOAT32)
                {
                    if (right.type == ObjectType::INT32)
                    {
                        Object number;
                        number.type = ObjectType::INT32;
                        number.value = std::make_shared<float>(0);
                        return number;
                    }
                    else if (right.type == ObjectType::FLOAT32)
                    {
                        Object number;
                        number.type = ObjectType::FLOAT32;
                        number.value = std::make_shared<float>(0);
                        return number;
                    }
                    else if (right.type == ObjectType::CHAR)
                    {
                        Object number;
                        number.type = ObjectType::INT32;
                        number.value = std::make_shared<float>(0);
                        return number;
                    }
                }
                else if (left.type == ObjectType::CHAR)
                {
                    if (right.type == ObjectType::INT32)
                    {
                        Object number;
                        number.type = ObjectType::INT32;
                        number.value = std::make_shared<int>(*static_cast<char*>(left.value.get()) % *static_cast<int*>(right.value.get()));
                        return number;
                    }
                    else if (right.type == ObjectType::FLOAT32)
                    {
                        Object number;
                        number.type = ObjectType::FLOAT32;
                        number.value = std::make_shared<float>(0);
                        return number;
                    }
                    else if (right.type == ObjectType::CHAR)
                    {
                        Object number;
                        number.type = ObjectType::INT32;
                        number.value = std::make_shared<int>(*static_cast<char*>(left.value.get()) % *static_cast<char*>(right.value.get()));
                        return number;
                    }
                }
            }
            break;
        }
    }

public:
    void run(Statement* root)
    {
        this->root = root;
        run_block(nullptr);
    }

    void test1()
    {
        Statement add;
        add.type = StatementType::ADD_OP;
        
        Statement left;
        left.type = StatementType::NUMBER;
        std::shared_ptr<SI_Number> leftIsNumber = std::make_shared<SI_Number>();
        leftIsNumber->number = "10";
        left.info = leftIsNumber;

        Statement right;
        right.type = StatementType::NUMBER;
        std::shared_ptr<SI_Number> rightIsNumber = std::make_shared<SI_Number>();
        rightIsNumber->number = "10";
        right.info = rightIsNumber;

        add.children.push_back(left);
        add.children.push_back(right);

        Object result = eval_expression(add);
        result.print();
    }
};

int main(int argc, char** argv)
{
    for (int i = 0; i < argc; ++i)
    {
        if (strcmp("-d", argv[i]) == 0)
        {
            DEBUG_MODE = true;
        }
    }

    File file;
    file.load_file("test.txt");

    Diagnostics diagnostics;

    Tokenizer tokenizer;
    tokenizer.parse_file(&file, &diagnostics);

    if (DEBUG_MODE)
        tokenizer.print_tokens();

    if (!diagnostics.has_errors())
    {
        Parser parser;
        parser.parse_statements(tokenizer.get_tokens(), &diagnostics);

        if (DEBUG_MODE)
            parser.print_ast();

        if (!diagnostics.has_errors())
        {
            Runner runner;
            runner.run(parser.get_root());
        }
    }

    // diagnostics dump
    std::cout << std::endl;
    diagnostics.dump();

    return 0;
}