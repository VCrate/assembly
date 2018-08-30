#pragma once

#include <filesystem>
#include <vector>
#include <string>

namespace vcrate::vasm {

std::vector<std::string> read_file(std::filesystem::path const& path);

}