#include "file.hpp"

using namespace pop;

File::File(std::string fileName) 
{
    std::ifstream ifs(fileName);

    if (!ifs.is_open())
        throw std::runtime_error("Could not open file!");

    for (std::string line; std::getline(ifs, line); )
        add_line(line);

    ifs.close();
}

/**
 * Retrieves a line from the file.
*/
std::string File::get_line(const int& index)
{
    return lines[index];
}

/**
 * Adds a line to the file.
*/
void File::add_line(std::string line)
{
    lines.push_back(line);
}

/**
 * Gets the number of lines in the file.
*/
int File::get_length() const
{
    return lines.size();
}