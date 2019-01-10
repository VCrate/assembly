#pragma once

#include <vcrate/vasm/Result.hpp>
#include <vcrate/vasm/lexer/Token.hpp>

#include <vcrate/vasm/parser/Nodes.hpp>

#include <vector>
#include <memory>

namespace vcrate::vasm::parser {

using ParserResult = Result<std::vector<Statement>>;

ParserResult parse(std::vector<lexer::Token> const& tokens);

}