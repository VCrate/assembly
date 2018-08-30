#include <vcrate/vasm/Reader.hpp>

#include <fstream>

namespace vcrate::vasm {

std::vector<std::string> read_file(std::filesystem::path const& path) {
    std::vector<std::string> lines;

    std::ifstream file(path);

    for(std::string line; std::getline(file, line);)
        lines.emplace_back(line);

    return lines;
}

}