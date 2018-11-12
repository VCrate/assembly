#pragma once

#include <vcrate/vasm/parser/ast/Constant.hpp>

#include <memory>

namespace vcrate::vasm::parser {

class BinaryOperation : public Constant {
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

    BinaryOperation(std::unique_ptr<Constant> lhs, Type type, std::unique_ptr<Constant> rhs);

    std::string to_string() const override;

    using precedence_t = unsigned;
    static precedence_t precedence_of(Type type);

    Type type;
    std::unique_ptr<Constant> lhs, rhs;

};

}