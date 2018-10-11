#pragma once

#include <vcrate/vasm/lexer/Token.hpp>

#include <vector>
#include <string>

namespace vcrate::vasm::lexer {

std::vector<Token> tokenize(std::vector<std::string> const& source);

}