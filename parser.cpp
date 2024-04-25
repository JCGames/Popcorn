#include "parser.hpp"

using namespace pop;

#pragma region Statement

Statement::Statement() { }

Statement::Statement(StatementType type, std::string line, unsigned int lineColumn, unsigned int lineNumber)
{
    this->type = type;
    this->line = line;
    this->lineColumn = lineColumn;
    this->lineNumber = lineNumber;
}

#pragma endregion

#pragma region Private Methods

/**
 * Returns true if the current token is an end of file token or if the index equals the token count.
*/
bool Parser::eof() const
{
    return index >= tokens->size() || tokens->at(index).type == TokenType::_EOF;
}

/**
 * Moves to the next token.
*/
void Parser::move_next()
{
    ++index;
}

/**
 * Moves to the previous token.
*/
void Parser::move_back()
{
    --index;
}

/**
 * Looks at the current token.
*/
Token Parser::get()
{
    if (index >= tokens->size())
        return Token("ERROR", TokenType::ERROR, "", 0, 0);
    return tokens->at(index);
}

/**
 * Looks at the previous token.
*/
Token Parser::prev()
{
    if (index - 1 < 0 || index - 1 >= tokens->size())
        return Token("ERROR", TokenType::ERROR, "", 0, 0);
    return tokens->at(index - 1);
}

/**
 * Looks at the next token.
*/
Token Parser::next()
{
    if (index + 1 >= tokens->size())
        return Token("ERROR", TokenType::ERROR, "", 0, 0);
    return tokens->at(index + 1);
}

#pragma region Statements

Statement Parser::parse_next_statement()
{
    // VARIABLE ASSIGNMENT
    if (get().type == TokenType::WORD && next().type == TokenType::ASSIGNMENT)
    {
        Statement assignment(StatementType::ASSIGN, get().line, get().lineColumn, get().lineNumber);
        std::shared_ptr<SI_String> siAssign = std::make_shared<SI_String>();
        assignment.info = siAssign;
        
        // set the variable name
        siAssign->value = get().value; 

        move_next();
        move_next(); // skip the =

        Statement expression = parse_expression();

        assignment.children.push_back(expression);

        --index;

        return assignment;
    }
    // IF STATEMENT
    else if (get().type == TokenType::IF)
    {
        return parse_if();
    }
    // WHILE STATEMENT
    else if (get().type == TokenType::WHILE)
    {
        return parse_while();
    }
    // FUNCTION STATEMENT
    else if (get().type == TokenType::FUNC)
    {
        return parse_function();
    }
    // FUNCTION CALL STATEMENT
    else if (get().type == TokenType::WORD && next().type == TokenType::OPEN_PARAN)
    {
        return parse_function_call();
    }
    // RETURN STATEMENT
    else if (get().type == TokenType::RETURN)
    {
        Statement statement(StatementType::RETURN, get().line, get().lineColumn, get().lineNumber);
        move_next();
        statement.children.push_back(parse_expression());
        return statement;
    }
    // BREAK STATEMENT
    else if (get().type == TokenType::BREAK)
    {
        return Statement(StatementType::BREAK, get().line, get().lineColumn, get().lineNumber);
    }
    // CONTINUE STATEMENT
    else if (get().type == TokenType::CONTINUE)
    {
        return Statement(StatementType::CONTINUE, get().line, get().lineColumn, get().lineNumber);
    }
    else if (get().type != TokenType::EOL && get().type != TokenType::_EOF)
    {
        diagnostics->add_error("Bad statement!", get().line, get().lineColumn, get().lineNumber);
    }

    return Statement(StatementType::ERROR, get().line, get().lineColumn, get().lineNumber);
}

Statement Parser::parse_function()
{
    Statement function(StatementType::FUNCTION, get().line, get().lineColumn, get().lineNumber);
    std::shared_ptr<SI_Function> siFunction = std::make_shared<SI_Function>();
    function.info = siFunction;

    move_next(); // skip the func keyword

    if (get().type != TokenType::WORD)
        diagnostics->add_error("Function name is not specified!", get().line, get().lineColumn, get().lineNumber);

    // set the name of the function
    siFunction->functionName = get().value;

    move_next();

    // parse the functions parameters
    if (get().type == TokenType::OPEN_PARAN)
    {
        move_next();

        while (!eof() && get().type != TokenType::CLOSE_PARAN)
        {
            if (get().type == TokenType::COMMA)
            {
                if (next().type != TokenType::WORD)
                {
                    diagnostics->add_error("Commas should not go here.", get().line, get().lineColumn, get().lineNumber);
                    break;
                }
                
                move_next();
            }

            if (get().type != TokenType::WORD)
            {
                diagnostics->add_error("That is not a valid parameter name.", get().line, get().lineColumn, get().lineNumber);
                break;
            }

            siFunction->parameterNames.push_back(get().value);
            move_next();

            if (get().type != TokenType::COMMA && get().type != TokenType::CLOSE_PARAN)
            {
                diagnostics->add_error("Yeah, you can't do this.", get().line, get().lineColumn, get().lineNumber);
                break;
            }
        }

        move_next();
    }

    while (get().type == TokenType::EOL)
        move_next();

    function.children.push_back(parse_block());

    return function;
}

Statement Parser::parse_function_call()
{
    Statement functionCall(StatementType::FUNCTION_CALL, get().line, get().lineColumn, get().lineNumber);
    std::shared_ptr<SI_String> siFunctionCall = std::make_shared<SI_String>();
    functionCall.info = siFunctionCall;

    // set the name of the function to be called
    siFunctionCall->value = get().value;

    move_next();
    move_next(); // pass the open parenthesis

    // parse the function calls parameters
    while (!eof() && get().type != TokenType::CLOSE_PARAN)
    {
        if (get().type == TokenType::COMMA)
            move_next();

        functionCall.children.push_back(parse_expression());

        if (get().type != TokenType::COMMA && get().type != TokenType::CLOSE_PARAN)
        {
            diagnostics->add_error("Wow, that was fatal. Maybe you will be more careful next time?", get().line, get().lineColumn, get().lineNumber);
            break;
        }
    }

    return functionCall;
}

Statement Parser::parse_if()
{
    Statement ifStmt(StatementType::IF, get().line, get().lineColumn, get().lineNumber);

    move_next(); // move to the start of the if's expression

    ifStmt.children.push_back(parse_expression());

    // find the ifs block
    while (get().type == TokenType::EOL)
        move_next();

    ifStmt.children.push_back(parse_block());

    // move to the next token passing end of lines
    move_next();

    while (get().type == TokenType::EOL)
        move_next();

    // if the next token is an else if or an else
    if (get().type == TokenType::ELSE && next().type == TokenType::IF)
    {
        move_next(); // pass the else token
        ifStmt.children.push_back(parse_if());
    }
    else if (get().type == TokenType::ELSE)
    {
        ifStmt.children.push_back(parse_else());
    }

    return ifStmt;
}

Statement Parser::parse_else()
{
    Statement _else(StatementType::ELSE, get().line, get().lineColumn, get().lineNumber);

    // find the elses block
    move_next();

    while (get().type == TokenType::EOL)
        move_next();

    _else.children.push_back(parse_block());

    return _else;
}

Statement Parser::parse_while()
{
    Statement whileStmt(StatementType::WHILE, get().line, get().lineColumn, get().lineNumber);

    move_next(); // move to the start of the while's expression

    whileStmt.children.push_back(parse_expression());

    // find the whiles block
    while (get().type == TokenType::EOL)
        move_next();

    whileStmt.children.push_back(parse_block());

    return whileStmt;
}

Statement Parser::parse_block()
{
    if (get().type != TokenType::OPEN_CURL)
        diagnostics->add_error("Missing { for block!", get().line, get().lineColumn, get().lineNumber);

    Statement block(StatementType::BLOCK, get().line, get().lineColumn, get().lineNumber);
    move_next();

    while (!eof() && get().type != TokenType::CLOSE_CURL)
    {
        Statement statement = parse_next_statement();

        if (statement.type != StatementType::ERROR)
            block.children.push_back(statement);

        move_next();
    }

    if (get().type != TokenType::CLOSE_CURL)
        diagnostics->add_error("Missing } for block!", get().line, get().lineColumn, get().lineNumber);

    //move_next();
    return block;
}

#pragma endregion

#pragma region Expressions

Statement Parser::parse_expression()
{
    Statement expression(StatementType::EXP, get().line, get().lineColumn, get().lineNumber);
    expression.children.push_back(parse_boolean_operators());
    return expression;
}

Statement Parser::parse_boolean_operators()
{
    Statement left = parse_add_sub();

    while (get().type == TokenType::EQUALS || 
        get().type == TokenType::NEQUALS ||
        get().type == TokenType::GTHANE ||
        get().type == TokenType::LTHANE ||
        get().type == TokenType::GTHAN ||
        get().type == TokenType::LTHAN)
    {
        if (get().type == TokenType::EQUALS)
        {
            move_next();
            Statement newLeft(StatementType::EQUALS_OP, get().line, get().lineColumn, get().lineNumber);

            newLeft.children.push_back(left);
            newLeft.children.push_back(parse_add_sub());

            left = newLeft;
        }
        else if (get().type == TokenType::NEQUALS)
        {
            move_next();
            Statement newLeft(StatementType::NEQUALS_OP, get().line, get().lineColumn, get().lineNumber);

            newLeft.children.push_back(left);
            newLeft.children.push_back(parse_add_sub());

            left = newLeft;
        }
        else if (get().type == TokenType::GTHANE)
        {
            move_next();
            Statement newLeft(StatementType::GTHANE_OP, get().line, get().lineColumn, get().lineNumber);

            newLeft.children.push_back(left);
            newLeft.children.push_back(parse_add_sub());

            left = newLeft;
        }
        else if (get().type == TokenType::LTHANE)
        {
            move_next();
            Statement newLeft(StatementType::LTHANE_OP, get().line, get().lineColumn, get().lineNumber);

            newLeft.children.push_back(left);
            newLeft.children.push_back(parse_add_sub());

            left = newLeft;
        }
        else if (get().type == TokenType::GTHAN)
        {
            move_next();
            Statement newLeft(StatementType::GTHAN_OP, get().line, get().lineColumn, get().lineNumber);

            newLeft.children.push_back(left);
            newLeft.children.push_back(parse_add_sub());

            left = newLeft;
        }
        else if (get().type == TokenType::LTHAN)
        {
            move_next();
            Statement newLeft(StatementType::LTHAN_OP, get().line, get().lineColumn, get().lineNumber);

            newLeft.children.push_back(left);
            newLeft.children.push_back(parse_add_sub());

            left = newLeft;
        }
    }

    return left;
}

Statement Parser::parse_add_sub()
{
    Statement left = parse_mult_div_mod();

    while (get().type == TokenType::PLUS || get().type == TokenType::SUB)
    {
        if (get().type == TokenType::PLUS)
        {
            move_next();
            Statement newLeft(StatementType::ADD_OP, get().line, get().lineColumn, get().lineNumber);

            newLeft.children.push_back(left);
            newLeft.children.push_back(parse_mult_div_mod());

            left = newLeft;
        }
        else if (get().type == TokenType::SUB)
        {
            move_next();
            Statement newLeft(StatementType::SUB_OP, get().line, get().lineColumn, get().lineNumber);

            newLeft.children.push_back(left);
            newLeft.children.push_back(parse_mult_div_mod());

            left = newLeft;
        }
    }

    return left;
}

Statement Parser::parse_mult_div_mod()
{
    Statement left = parse_term();

    while (get().type == TokenType::MULT || get().type == TokenType::DIV || get().type == TokenType::MOD)
    {
        if (get().type == TokenType::MULT)
        {
            move_next();
            Statement newLeft(StatementType::MULT_OP, get().line, get().lineColumn, get().lineNumber);

            newLeft.children.push_back(left);
            newLeft.children.push_back(parse_term());

            left = newLeft;
        }
        else if (get().type == TokenType::DIV)
        {
            move_next();
            Statement newLeft(StatementType::DIV_OP, get().line, get().lineColumn, get().lineNumber);

            newLeft.children.push_back(left);
            newLeft.children.push_back(parse_term());

            left = newLeft;
        }
        else if (get().type == TokenType::MOD)
        {
            move_next();
            Statement newLeft(StatementType::MOD_OP, get().line, get().lineColumn, get().lineNumber);

            newLeft.children.push_back(left);
            newLeft.children.push_back(parse_term());

            left = newLeft;
        }
    }

    return left;
}

Statement Parser::parse_term()
{
    Statement result(StatementType::ERROR, get().line, get().lineColumn, get().lineNumber);

    if (get().type == TokenType::OPEN_PARAN)
    {
        move_next();
        Statement expression = parse_expression();

        if (get().type != TokenType::CLOSE_PARAN)
            diagnostics->add_error("Missing a )!", get().line, get().lineColumn, get().lineNumber);

        move_next();
        return expression;
    }
    else if (get().type == TokenType::NUMBER)
    {
        std::shared_ptr<SI_String> siString = std::make_shared<SI_String>();
        siString->value = get().value;
        result.info = siString;
        result.type = StatementType::NUMBER;
    }
    else if (get().type == TokenType::CHAR)
    {
        std::shared_ptr<SI_String> siString = std::make_shared<SI_String>();
        siString->value = get().value;
        result.info = siString;
        result.type = StatementType::CHAR;
    }
    else if (get().type == TokenType::WORD && next().type == TokenType::OPEN_PARAN)
    {
        result = parse_function_call();
    }
    else if (get().type == TokenType::WORD)
    {
        std::shared_ptr<SI_String> siString = std::make_shared<SI_String>();
        siString->value = get().value;
        result.info = siString;
        result.type = StatementType::VARIABLE;
    }
    else if (get().type == TokenType::TRUE)
    {
        std::shared_ptr<SI_Boolean> siBoolean = std::make_shared<SI_Boolean>();
        siBoolean->value = true;
        result.info = siBoolean;
        result.type = StatementType::BOOLEAN;
    }
    else if (get().type == TokenType::FALSE)
    {
        std::shared_ptr<SI_Boolean> siBoolean = std::make_shared<SI_Boolean>();
        siBoolean->value = false;
        result.info = siBoolean;
        result.type = StatementType::BOOLEAN;
    }
    else if (get().type == TokenType::STRING)
    {
        std::shared_ptr<SI_String> siString = std::make_shared<SI_String>();
        siString->value = get().value;
        result.info = siString;
        result.type = StatementType::STRING;
    }
    else if (get().type == TokenType::SUB)
    {
        move_next();
        result.type = StatementType::NEGATE_OP;
        result.children.push_back(parse_term());
        return result;
    }

    if (result.type == StatementType::ERROR)
        diagnostics->add_error("That is not a term!", get().line, get().lineColumn, get().lineNumber);

    move_next();

    return result;
}

#pragma endregion

/**
 * Prints a statement and its statements recursively.
*/
void Parser::print_statement(const Statement& statement, std::string padding)
{
    std::string typeName = statement_type_as_str(statement.type);

    std::cout << padding << typeName << std::endl;

    switch (statement.type)
    {
    case StatementType::ASSIGN:
        if (SI_String* siAssign = static_cast<SI_String*>(statement.info.get()))
        {
            std::cout << padding << "Variable Name: " << siAssign->value << std::endl;

            for (auto& child : statement.children)
                print_statement(child, padding + "\t");
        }
        break;
    case StatementType::NUMBER:
        if (SI_String* siNumber = static_cast<SI_String*>(statement.info.get()))
        {
            std::cout << padding << "Value: " << siNumber->value << std::endl;
        }
        break;
    case StatementType::CHAR:
        if (SI_String* siChar = static_cast<SI_String*>(statement.info.get()))
        {
            std::cout << padding << "Value: " << siChar->value << std::endl;
        }
        break;
    case StatementType::VARIABLE:
        if (SI_String* siVariable = static_cast<SI_String*>(statement.info.get()))
        {
            std::cout << padding << "Value: " << siVariable->value << std::endl;
        }
        break;
    case StatementType::BOOLEAN:
        if (SI_Boolean* siBoolean = static_cast<SI_Boolean*>(statement.info.get()))
        {
            std::cout << padding << "Value: " << (siBoolean->value ? "true" : "false") << std::endl;
        }
        break;
    case StatementType::BLOCK:
        for (auto& child : statement.children)
            print_statement(child, padding + "\t");
        break;
    case StatementType::FUNCTION:
        if (SI_Function* siFunction = static_cast<SI_Function*>(statement.info.get()))
        {
            std::cout << padding << "Function Name: " << siFunction->functionName << std::endl;
            std::cout << padding << "[" << std::endl;
            for (auto& paramName : siFunction->parameterNames)
                std::cout << padding << "\tParameter Name: " << paramName << "," << std::endl;
            std::cout << padding << "]" << std::endl;
            print_statement(statement.children[0], padding + '\t');
        }
        break;
    case StatementType::FUNCTION_CALL:
        if (SI_String* siFunctionCall = static_cast<SI_String*>(statement.info.get()))
        {
            std::cout << padding << "Function Name: " << siFunctionCall->value << std::endl;
            std::cout << padding << "[" << std::endl;
            for (auto& stmt : statement.children)
            {
                print_statement(stmt, padding + '\t');
                std::cout << padding << "\t," << std::endl;
            }
            std::cout << padding << "]" << std::endl;
        }
        break;
    default:
        for (auto& child : statement.children)
            print_statement(child, padding + "\t");
        break;
    }
}

#pragma endregion

#pragma region Public Methods

Parser::Parser() 
{
    tokens = nullptr;
}

Statement* Parser::get_root()
{
    return &root;
}

void Parser::parse_statements(std::vector<Token>* tokens, Diagnostics* diagnostics)
{
    this->tokens = tokens;
    this->diagnostics = diagnostics;
    root.type = StatementType::BLOCK;
    
    for (index = 0; index < tokens->size(); ++index)
    {
        Statement statement = parse_next_statement();

        if (statement.type != StatementType::ERROR)
            root.children.push_back(statement);
    }
}

void Parser::print_ast()
{
    print_statement(root, "");
}

#pragma endregion