#pragma once

#include <vcrate/vasm/Result.hpp>
#include <vcrate/vasm/parser/Nodes.hpp>

#include <vcrate/vcx/Executable.hpp>

namespace vcrate::vasm::generator {

using GeneratorResult = Result<vcx::Executable>;

GeneratorResult generate(std::vector<parser::Statement> const& statements);

}