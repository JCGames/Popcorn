
#include <iostream>
#include <string>
#include <vector>
#include <fstream>

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
        std::string warning = "WARNING (" + std::to_string(lineColumn) + ":" + std::to_string(lineNumber) + "): ";
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
        std::string error = "ERROR (" + std::to_string(lineColumn) + ":" + std::to_string(lineNumber) + "): ";
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
        this->lineColumn = lineColumn;
        this->lineNumber = lineNumber;
    }

    void print_token()
    {
        std::cout << "Value: |" << value << "| Type: |" << static_cast<int>(type) << "| Line Column: " << lineColumn << " Line Number: " << lineNumber << "\n"; 
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
};

int main(int argc, char** argv)
{
    File file;
    file.load_file("test");

    Diagnostics diagnostics;

    Tokenizer tokenizer;
    tokenizer.parse_file(&file, &diagnostics);

    tokenizer.print_tokens();

    std::cout << std::endl;

    diagnostics.dump();
}