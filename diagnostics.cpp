#include "diagnostics.hpp"

#define NC "\e[0m"
#define RED "\e[0;31m"
#define GRN "\e[0;32m"
#define CYN "\e[0;36m"
#define REDB "\e[41m"

using namespace pop;

void Diagnostics::add_warning(std::string message, std::string line, unsigned int lineColumn, unsigned int lineNumber)
{
    ++lineNumber;

    std::string warning = RED "WARNING (" + std::to_string(lineNumber) + ":" + std::to_string(lineColumn) + "): ";
    std::string space = "";

    for (int i = 0; i < lineColumn; ++i)
        space += ' ';

    warning += message + "\n\t" + line + "\n\t" + space + "^" NC;
    warnings.push_back(warning);
}

void Diagnostics::add_error(std::string message, std::string line, unsigned int lineColumn, unsigned int lineNumber)
{
    ++lineNumber;

    std::string error = RED "ERROR (" + std::to_string(lineNumber) + ":" + std::to_string(lineColumn) + "): ";
    std::string space = "";

    for (int i = 0; i < lineColumn; ++i)
        space += ' ';

    error += message + "\n\t" + line + "\n\t" + space + "^" NC;
    errors.push_back(error);
}

bool Diagnostics::has_warnings() const
{
    return warnings.size() > 0;
}

bool Diagnostics::has_errors() const
{
    return errors.size() > 0;
}

void Diagnostics::dump()
{
    for (auto& warning : warnings)
        std::cout << warning << std::endl;

    for (auto& error : errors)
        std::cout << error << std::endl;
}