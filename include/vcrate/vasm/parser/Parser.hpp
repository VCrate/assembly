#pragma once

#include <vcrate/vasm/Result.hpp>
#include <vcrate/vasm/parser/ast/Statement.hpp>
#include <vcrate/vasm/lexer/Token.hpp>

#include <vector>
#include <memory>

namespace vcrate::vasm::parser {

using ParserResult = Result<std::vector<std::unique_ptr<Statement>>>;

ParserResult parse(std::vector<lexer::Token> const& tokens);

}