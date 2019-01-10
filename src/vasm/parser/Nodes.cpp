#include <vcrate/vasm/parser/Nodes.hpp>

namespace vcrate::vasm::parser {

precedence_t precedence_of(BinaryRelation::Type type) {
    switch(type) {
        case BinaryRelation::Exp:            return 22;

        case BinaryRelation::Multiplication: return 20;
        case BinaryRelation::Division:       return 20;
        case BinaryRelation::Modulo:         return 20;

        case BinaryRelation::Addition:       return 18;
        case BinaryRelation::Subtraction:    return 18;

        case BinaryRelation::ShiftLeft:      return 16;
        case BinaryRelation::ShiftRight:     return 16;
        case BinaryRelation::RotateLeft:     return 16;
        case BinaryRelation::RotateRight:    return 16;

        case BinaryRelation::Less:           return 14;
        case BinaryRelation::LessEquals:     return 14;
        case BinaryRelation::Greater:        return 14;
        case BinaryRelation::GreaterEquals:  return 14;

        case BinaryRelation::Equals:         return 12;
        case BinaryRelation::Unequals:       return 12;

        case BinaryRelation::And:            return 10;

        case BinaryRelation::Xor:            return 8;

        case BinaryRelation::Or:             return 6;

        case BinaryRelation::LogicalAnd:     return 4;

        case BinaryRelation::LogicalOr:      return 2;

        default:                                    return 0;
    }
}

}