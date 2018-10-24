#pragma once

#include <vcrate/vasm/parser/ast/Argument.hpp>
#include <vcrate/vasm/parser/ast/Register.hpp>
#include <vcrate/vasm/parser/ast/Constant.hpp>

#include <memory>

namespace vcrate::vasm::parser {

class DereferencedArgument : public Argument {
public:

    std::string to_string() const override;

    Register reg;
    std::unique_ptr<Constant> displacement;

};

}