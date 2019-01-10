#pragma once

#include <vcrate/vasm/parser/ast/Node.hpp>
#include <vcrate/vasm/parser/ast/DereferencedArgument.hpp>
#include <vcrate/vasm/parser/ast/Register.hpp>
#include <vcrate/vasm/parser/ast/ShortConstant.hpp>

#include <vcrate/vasm/generator/Argument.hpp>

#include <vcrate/vasm/Result.hpp>

#include <variant>
#include <memory>

namespace vcrate::vasm::parser {

struct Argument : public Node {

    explicit Argument(Register const& reg);
    explicit Argument(DereferencedArgument&& arg);
    explicit Argument(std::unique_ptr<ShortConstant>&& cst);

    Result<instruction::Argument> argument_value(generator::Generator const& gen) const;

    std::string to_string() const override;

    std::variant<Register, DereferencedArgument, std::unique_ptr<ShortConstant>> value;

};

}