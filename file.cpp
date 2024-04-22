#include "file.hpp"

using namespace pop;

std::string File::get_line(const int& index)
{
    return lines[index];
}

void File::add_line(std::string line)
{
    lines.push_back(line);
}

int File::get_length() const
{
    return lines.size();
}

void File::load_file(std::string name)
{
    std::ifstream ifs(name);

    if (!ifs.is_open())
        throw std::runtime_error("Could not open file!");

    for (std::string line; std::getline(ifs, line); )
        add_line(line);

    ifs.close();
}