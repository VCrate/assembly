#pragma once

#include <memory>

#include <vcrate/vasm/parser/ast/Constant.hpp>

namespace vcrate::vasm::parser {

class UnaryOperation : public Constant {
public:

    enum class Type {
        Not,
        LogicalNot,
        Neg
    };

    std::unique_ptr<Constant> value;
    Type type;

};

}