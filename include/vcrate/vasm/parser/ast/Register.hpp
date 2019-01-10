#pragma once

#include <vcrate/instruction/Argument.hpp>

namespace vcrate::vasm::parser {

class Register /*: public Argument*/ {
public:

    using Type = instruction::Register;

    Register(Type const& reg);

    std::string to_string() const;

    Type reg;

};

}