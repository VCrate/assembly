#pragma once

#include <vcrate/vasm/parser/ast/Constant.hpp>

#include <memory>

namespace vcrate::vasm::parser {

class BinaryOperation : public Constant {
public:

    std::string to_string() const override;

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

    std::unique_ptr<Constant> lhs, rhs;
    Type type;

};

}