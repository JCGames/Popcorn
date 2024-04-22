#include "diagnostics.hpp"

using namespace pop;

void Diagnostics::add_warning(std::string message, std::string line, unsigned int lineColumn, unsigned int lineNumber)
{
    std::string warning = "WARNING (" + std::to_string(lineNumber) + ":" + std::to_string(lineColumn) + "): ";
    std::string space = "";

    for (int i = 0; i < lineColumn; ++i)
        space += ' ';

    warning += message + "\n\t" + line + "\n\t" + space + "^";
    warnings.push_back(warning);
}

void Diagnostics::add_error(std::string message, std::string line, unsigned int lineColumn, unsigned int lineNumber)
{
    std::string error = "ERROR (" + std::to_string(lineNumber) + ":" + std::to_string(lineColumn) + "): ";
    std::string space = "";

    for (int i = 0; i < lineColumn; ++i)
        space += ' ';

    error += message + "\n\t" + line + "\n\t" + space + "^";
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