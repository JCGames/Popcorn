#ifndef _FILE
#define _FILE

#include <vector>
#include <string>
#include <fstream>
#include <stdexcept>

namespace pop
{
    /**
     * This structure is used for saving the contents
     * of a file as lines in memory.
    */
    struct File
    {
    private:
        std::vector<std::string> lines;

    public:
        File(std::string fileName);

        /**
         * Gets a line from the file.
        */
        std::string get_line(const int& index);

        /**
         * Adds a line to the file.
        */
        void add_line(std::string line);

        /**
         * Gets the file length in lines.
        */
        int get_length() const;

        /**
         * Loads a file.
        */
        void load_file(std::string name);
    };
}

#endif