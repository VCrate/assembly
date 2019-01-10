#pragma once

#include <vcrate/vasm/parser/ast/Register.hpp>
#include <vcrate/vasm/parser/ast/ShortConstant.hpp>

#include <memory>

namespace vcrate::vasm::parser {

class DereferencedArgument/* : public Argument*/ {
public:

    DereferencedArgument(Register reg, std::unique_ptr<ShortConstant> displacement);

    std::string to_string() const;

    Register reg;
    std::unique_ptr<ShortConstant> displacement;

};

}