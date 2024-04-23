#include "parser.hpp"

using namespace pop;

Statement::Statement() { }

Statement::Statement(StatementType type, std::string line, unsigned int lineColumn, unsigned int lineNumber)
{
    this->type = type;
    this->line = line;
    this->lineColumn = lineColumn;
    this->lineNumber = lineNumber;
}

bool Parser::eof() const
{
    return currentToken >= tokens->size() || tokens->at(currentToken).type == TokenType::_EOF;
}

void Parser::move_next()
{
    ++currentToken;
}

Token Parser::get()
{
    if (currentToken >= tokens->size())
        return Token("ERROR", TokenType::ERROR, "", 0, 0);
    return tokens->at(currentToken);
}

Token Parser::prev()
{
    if (currentToken - 1 < 0 || currentToken - 1 >= tokens->size())
        return Token("ERROR", TokenType::ERROR, "", 0, 0);
    return tokens->at(currentToken - 1);
}

Token Parser::next()
{
    if (currentToken + 1 >= tokens->size())
        return Token("ERROR", TokenType::ERROR, "", 0, 0);
    return tokens->at(currentToken + 1);
}

Statement Parser::parse_statement()
{
    Statement result(StatementType::ERROR, get().line, get().lineColumn, get().lineNumber);

    if (get().type == TokenType::WORD && next().type == TokenType::ASSIGNMENT)
    {
        std::shared_ptr<SI_String> siAssign = std::make_shared<SI_String>();
        siAssign->value = get().value;

        move_next();
        move_next(); // skip the = sign

        Statement expression = parse_expression();

        result.children.push_back(expression);

        result.info = siAssign;
        result.type = StatementType::ASSIGN;

        --currentToken;
    }
    else if (get().type == TokenType::IF)
    {
        result = parse_if();
    }
    else if (get().type == TokenType::WHILE)
    {
        Statement _while(StatementType::WHILE, get().line, get().lineColumn, get().lineNumber);

        move_next();

        _while.children.push_back(parse_expression());

        while (get().type == TokenType::EOL)
            move_next();

        _while.children.push_back(parse_block());

        result = _while;
    }
    // function
    else if (get().type == TokenType::FUNC)
    {
        Statement function(StatementType::FUNCTION, get().line, get().lineColumn, get().lineNumber);
        std::shared_ptr<SI_Function> siFunction = std::make_shared<SI_Function>();
        function.info = siFunction;

        move_next();

        if (get().type != TokenType::WORD)
            diagnostics->add_error("Function name is not specified!", get().line, get().lineColumn, get().lineNumber);

        siFunction->functionName = get().value;

        move_next();

        if (get().type == TokenType::OPEN_PARAN)
        {
            move_next();

            while (!eof() && get().type != TokenType::CLOSE_PARAN)
            {
                if (get().type == TokenType::COMMA)
                {
                    if (next().type != TokenType::WORD)
                    {
                        diagnostics->add_error("Only an idiot would add a comma here!", get().line, get().lineColumn, get().lineNumber);
                        break;
                    }
                    
                    move_next();
                }

                if (get().type != TokenType::WORD)
                {
                    diagnostics->add_error("That is not a valid parameter name!", get().line, get().lineColumn, get().lineNumber);
                    break;
                }

                siFunction->parameterNames.push_back(get().value);
                move_next();

                if (get().type != TokenType::COMMA && get().type != TokenType::CLOSE_PARAN)
                {
                    diagnostics->add_error("Yeah, you can't do this. Bad.", get().line, get().lineColumn, get().lineNumber);
                    break;
                }
            }

            move_next();
        }

        while (get().type == TokenType::EOL)
            move_next();

        function.children.push_back(parse_block());

        result = function;
    }
    // function call
    else if (get().type == TokenType::WORD && next().type == TokenType::OPEN_PARAN)
    {
        result = parse_function_call();
    }
    else if (get().type != TokenType::EOL && get().type != TokenType::_EOF)
    {
        diagnostics->add_error("Bad statement!", get().line, get().lineColumn, get().lineNumber);
    }

    return result;
}

Statement Parser::parse_function()
{

}

Statement Parser::parse_function_call()
{
    Statement functionCall(StatementType::FUNCTION_CALL, get().line, get().lineColumn, get().lineNumber);
    std::shared_ptr<SI_String> siFunctionCall = std::make_shared<SI_String>();
    functionCall.info = siFunctionCall;
    siFunctionCall->value = get().value;

    move_next();
    move_next();

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
    Statement _if(StatementType::IF, get().line, get().lineColumn, get().lineNumber);

    move_next();

    _if.children.push_back(parse_expression());

    while (get().type == TokenType::EOL)
        move_next();

    _if.children.push_back(parse_block());

    while (get().type == TokenType::EOL)
        move_next();

    if (get().type == TokenType::ELSE && next().type == TokenType::IF)
    {
        move_next();

        _if.children.push_back(parse_if());
    }
    else if (get().type == TokenType::ELSE)
    {
        _if.children.push_back(parse_else());
    }

    return _if;
}

Statement Parser::parse_else()
{
    Statement _else(StatementType::ELSE, get().line, get().lineColumn, get().lineNumber);

    move_next();

    while (get().type == TokenType::EOL)
        move_next();

    _else.children.push_back(parse_block());

    return _else;
}

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

Statement Parser::parse_block()
{
    if (get().type != TokenType::OPEN_CURL)
        diagnostics->add_error("Missing { for block!", get().line, get().lineColumn, get().lineNumber);

    Statement block(StatementType::BLOCK, get().line, get().lineColumn, get().lineNumber);
    move_next();

    while (!eof() && get().type != TokenType::CLOSE_CURL)
    {
        Statement statement = parse_statement();

        if (statement.type != StatementType::ERROR)
            block.children.push_back(statement);

        move_next();
    }

    if (get().type != TokenType::CLOSE_CURL)
        diagnostics->add_error("Missing } for block!", get().line, get().lineColumn, get().lineNumber);

    //move_next();
    return block;
}

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
    
    for (currentToken = 0; currentToken < tokens->size(); ++currentToken)
    {
        Statement statement = parse_statement();

        if (statement.type != StatementType::ERROR)
            root.children.push_back(statement);
    }
}

void Parser::print_ast()
{
    print_statement(root, "");
}