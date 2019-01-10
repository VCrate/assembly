#pragma once

#include <vcrate/vasm/parser/ast/ShortConstant.hpp>

#include <memory>

namespace vcrate::vasm::parser {

class BinaryOperation : public ShortConstant {
public:

    enum class Type {
        Add,
        Sub,
        Mult,
        Div,
        Mod,
        ShiftL,
        ShiftR,
        RotateL,
        RotateR,
        Or,
        LogicalOr,
        And,
        LogicalAnd,
        Xor,
        Exp,
        Less,
        LessEquals,
        Greater,
        GreaterEquals,
        Equals,
        Unequals
    };

    BinaryOperation(std::unique_ptr<ShortConstant> lhs, Type type, std::unique_ptr<ShortConstant> rhs);

    std::string to_string() const override;

    generator::LabelDependant<ShortConstant::Type, ShortConstant::Processor> argument_value(generator::Generator const& gen) const override;

    using precedence_t = unsigned;
    static precedence_t precedence_of(Type type);

    Type type;
    std::unique_ptr<ShortConstant> lhs, rhs;

};

}