#pragma once

#include <vcrate/vasm/parser/ast/Argument.hpp>

#include <vcrate/instruction/Argument.hpp>

namespace vcrate::vasm::parser {

class Register : public Argument {
public:

    using Type = instruction::Register;

    Register(Type const& reg);

    std::string to_string() const override;

    Type reg;

};

}