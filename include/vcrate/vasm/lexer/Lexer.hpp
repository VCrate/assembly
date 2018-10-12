#pragma once

#include <vcrate/vasm/lexer/Token.hpp>
#include <vcrate/vasm/Error.hpp>
#include <vcrate/vasm/Result.hpp>

#include <vector>
#include <string>

namespace vcrate::vasm::lexer {

using LexerResult = Result<std::vector<Token>>;

LexerResult tokenize(std::vector<std::string> const& source);

}