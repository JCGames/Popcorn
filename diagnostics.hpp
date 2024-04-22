#ifndef DIAGNOSTICS
#define DIAGNOSTICS

#include <vector>
#include <string>
#include <iostream>

namespace pop
{
    /**
     * Responsible for showing
     * diagnostic information
    */
    class Diagnostics
    {
        std::vector<std::string> warnings;
        std::vector<std::string> errors;

    public:
        /**
         * Adds a warning to the diagnostics.
        */
        void add_warning(std::string message, std::string line, unsigned int lineColumn, unsigned int lineNumber);

        /**
         * Adds a error to the diagnostics.
        */
        void add_error(std::string message, std::string line, unsigned int lineColumn, unsigned int lineNumber);

        /**
         * Does the diagnostics have warnings.
        */
        bool has_warnings() const;

        /**
         * Does the diagnostics have errors.
        */
        bool has_errors() const;

        void dump();
    };
}

#endif