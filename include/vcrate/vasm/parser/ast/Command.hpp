#pragma once

#include <vcrate/bytecode/Operations.hpp>

#include <vcrate/vasm/parser/ast/Argument.hpp>
#include <vcrate/vasm/parser/ast/Statement.hpp>

#include <vector>
#include <memory>

namespace vcrate::vasm::parser {

class Command : public Statement {
public:

    bytecode::Operations ope;
    std::vector<std::unique_ptr<Argument>> arguments; 

};

}