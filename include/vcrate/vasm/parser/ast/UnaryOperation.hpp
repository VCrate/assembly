#pragma once

#include <memory>

#include <vcrate/vasm/parser/ast/ShortConstant.hpp>

namespace vcrate::vasm::parser {

class UnaryOperation : public ShortConstant {
public:
    enum class Type {
        Not,
        LogicalNot,
        Neg
    };

    UnaryOperation(Type type, std::unique_ptr<ShortConstant> value);
    std::string to_string() const override;

    generator::LabelDependant<ShortConstant::Type, ShortConstant::Processor> argument_value(generator::Generator const& gen) const override;

    Type type;
    std::unique_ptr<ShortConstant> value;

};

}