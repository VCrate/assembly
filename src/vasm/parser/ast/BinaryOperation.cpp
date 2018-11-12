#include <vcrate/vasm/parser/ast/BinaryOperation.hpp>

namespace vcrate::vasm::parser {

BinaryOperation::BinaryOperation(std::unique_ptr<Constant> lhs, BinaryOperation::Type type, std::unique_ptr<Constant> rhs) 
    : type(type), lhs(std::move(lhs)), rhs(std::move(rhs)) {}

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

BinaryOperation::precedence_t BinaryOperation::precedence_of(Type type) {
    switch(type) {
        case BinaryOperation::Type::Exp:            return 22;

        case BinaryOperation::Type::Mult:           return 20;
        case BinaryOperation::Type::Div:            return 20;
        case BinaryOperation::Type::Mod:            return 20;

        case BinaryOperation::Type::Add:            return 18;
        case BinaryOperation::Type::Sub:            return 18;

        case BinaryOperation::Type::ShiftL:         return 16;
        case BinaryOperation::Type::ShiftR:         return 16;
        case BinaryOperation::Type::RotateL:        return 16;
        case BinaryOperation::Type::RotateR:        return 16;

        case BinaryOperation::Type::Less:           return 14;
        case BinaryOperation::Type::LessEquals:     return 14;
        case BinaryOperation::Type::Greater:        return 14;
        case BinaryOperation::Type::GreaterEquals:  return 14;

        case BinaryOperation::Type::Equals:         return 12;
        case BinaryOperation::Type::Unequals:       return 12;

        case BinaryOperation::Type::And:            return 10;

        case BinaryOperation::Type::Xor:            return 8;

        case BinaryOperation::Type::Or:             return 6;

        case BinaryOperation::Type::LogicalAnd:     return 4;

        case BinaryOperation::Type::LogicalOr:      return 2;

        default:                                    return 0;
    }
}

}