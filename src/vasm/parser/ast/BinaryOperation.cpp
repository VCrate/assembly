#include <vcrate/vasm/parser/ast/BinaryOperation.hpp>

namespace vcrate::vasm::parser {

std::string BinaryOperation::to_string() const {
    std::string op;
    switch(type) {
        case BinaryOperation::Type::Add:            op = " + ";   break;
        case BinaryOperation::Type::Sub:            op = " - ";   break;
        case BinaryOperation::Type::Mult:           op = " * ";   break;
        case BinaryOperation::Type::Div:            op = " / ";   break;
        case BinaryOperation::Type::Mod:            op = " % ";   break;
        case BinaryOperation::Type::ShiftL:         op = " << ";  break;
        case BinaryOperation::Type::ShiftR:         op = " >> ";  break;
        case BinaryOperation::Type::RotateL:        op = " <<< "; break;
        case BinaryOperation::Type::RotateR:        op = " >>> "; break;
        case BinaryOperation::Type::Or:             op = " | ";   break;
        case BinaryOperation::Type::LogicalOr:      op = " || ";  break;
        case BinaryOperation::Type::And:            op = " & ";   break;
        case BinaryOperation::Type::LogicalAnd:     op = " && ";  break;
        case BinaryOperation::Type::Xor:            op = " ^ ";   break;
        case BinaryOperation::Type::Exp:            op = " ** ";  break;
        case BinaryOperation::Type::Less:           op = " < ";   break;
        case BinaryOperation::Type::LessEquals:     op = " <= ";  break;
        case BinaryOperation::Type::Greater:        op = " > ";   break;
        case BinaryOperation::Type::GreaterEquals:  op = " >= ";  break;
        case BinaryOperation::Type::Equals:         op = " == ";  break;
        case BinaryOperation::Type::Unequals:       op = " != ";  break;
        default:                                    op = " ??? "; break;
    }
    return "BINOP{ " + lhs->to_string() + op + rhs->to_string() + " }";
}

}