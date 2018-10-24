#include <vcrate/vasm/parser/ast/UnaryOperation.hpp>

namespace vcrate::vasm::parser {

std::string UnaryOperation::to_string() const {
    std::string op;
    switch(type) {
        case UnaryOperation::Type::Neg:         op = "- ";   break;
        case UnaryOperation::Type::Not:         op = "~ ";   break;
        case UnaryOperation::Type::LogicalNot:  op = "! ";   break;
        default:                                op = "??? "; break;
    }
    return "BINOP{ " + op + value->to_string() + " }";
}

}