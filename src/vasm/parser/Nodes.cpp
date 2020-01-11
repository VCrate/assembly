#include <vcrate/vasm/parser/Nodes.hpp>

#include <vcrate/vasm/Visitor.hpp>

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

std::ostream& operator << (std::ostream& os, Term const& term) {
    return os << term.value;
}
std::ostream& operator << (std::ostream& os, Term::variant_t const& term) {
    return std::visit(
        Visitor {
            [&os] (integer_t i) -> std::ostream& {
                return os << i;
            },
            [&os] (float_t i) -> std::ostream& {
                return os << i;
            },
            [&os] (ident_t const& i) -> std::ostream& {
                return os << i;
            },
            [&os] (UnaryRelation const& rel) -> std::ostream& {
                switch(rel.type) {
                    case UnaryRelation::Complement: os << '~'; break;
                    case UnaryRelation::Negation: os << '!'; break;
                    case UnaryRelation::Opposite: os << '-'; break;
                }
                return os << *rel.operand;
            },
            [&os] (BinaryRelation const& rel) -> std::ostream& {
                os << '(' << *rel.lhs;
                switch(rel.type) {
                    case BinaryRelation::Exp: os << "**"; break;
                    case BinaryRelation::Multiplication: os << "*"; break;
                    case BinaryRelation::Division: os << "/"; break;
                    case BinaryRelation::Modulo: os << "%"; break;
                    case BinaryRelation::Addition: os << "+"; break;
                    case BinaryRelation::Subtraction: os << "-"; break;
                    case BinaryRelation::ShiftLeft: os << "<<"; break;
                    case BinaryRelation::ShiftRight: os << ">>"; break;
                    case BinaryRelation::RotateLeft: os << "<<<"; break;
                    case BinaryRelation::RotateRight: os << ">>>"; break;
                    case BinaryRelation::Less: os << "<"; break;
                    case BinaryRelation::LessEquals: os << "<="; break;
                    case BinaryRelation::Greater: os << ">"; break;
                    case BinaryRelation::GreaterEquals: os << ">="; break;
                    case BinaryRelation::Equals: os << "=="; break;
                    case BinaryRelation::Unequals: os << "!="; break;
                    case BinaryRelation::And: os << "&"; break;
                    case BinaryRelation::Xor: os << "^"; break;
                    case BinaryRelation::Or: os << "|"; break;
                    case BinaryRelation::LogicalAnd: os << "&&"; break;
                    case BinaryRelation::LogicalOr: os << "||"; break;
                }
                return os << *rel.rhs << ')';
            },
            [&os] (Dereferenced const& d) -> std::ostream& {
                return os << "#[" << *d.term << ']';
            },
        },
        term);
}

std::ostream& operator << (std::ostream& os, Constant const& constant) {
    return os << constant.value;
}

std::ostream& operator << (std::ostream& os, Constant::variant_t const& constant) {
    return std::visit(
        Visitor {
            [&os] (string_t const& str) -> std::ostream& {
                return os << '"' << str << '"';
            },
            [&os] (Constant::term_t const& t) -> std::ostream& {
                return os << t;
            },
        },
        constant);
}

std::ostream& operator << (std::ostream& os, Argument const& argument) {
    return os << argument.value;
}

std::ostream& operator << (std::ostream& os, Argument::variant_t const& argument) {
    return std::visit(
        Visitor {
            [&os] (Argument::register_t const& reg) -> std::ostream& {
                return os << reg.to_string();
            },
            [&os] (Argument::term_t const& t) -> std::ostream& {
                return os << t;
            },
            [&os] (Pointer const& p) -> std::ostream& {
                return os << '[' << p.term << ']';
            },
            [&os] (Displacement const& d) -> std::ostream& {
                os << '[' << d.reg.reg;
                if (d.term) {
                    os << " + " << *d.term;
                }
                return os << ']';
            },
        },
        argument);
}

std::ostream& operator << (std::ostream& os, Statement const& statement) {
    return os << statement.value;
}

std::ostream& operator << (std::ostream& os, Directive::variant_t const& dir) {
    return std::visit(
        Visitor {
            [&os] (Align const& align) -> std::ostream& {
                return os << "#align " << align.value;
            },
        },
        dir);
}

std::ostream& operator << (std::ostream& os, Directive const& dir) {
    return os << dir.value;
}

std::ostream& operator << (std::ostream& os, Statement::variant_t const& statement) {
    return std::visit(
        Visitor {
            [&os] (label_t const& label) -> std::ostream& {
                return os << label << ':';
            },
            [&os] (Directive const& dir) -> std::ostream& {
                return os << dir;
            },
            [&os] (Command const& cmd) -> std::ostream& {
                os << bytecode::OpDefinition::get(cmd.ope).name;
                bool first = true;
                for(auto const& arg : cmd.args) {
                    if (!first) {
                        os << ',';
                    }
                    first = false;
                    os << ' ' << arg;
                }
                return os;
            },
            [&os] (Data const& data) -> std::ostream& {
                switch(data.size) {
                    case Data::Byte: os << "db "; break;
                    case Data::Word: os << "dw "; break;
                    case Data::DoubleWord: os << "dd "; break;
                }
                bool first = true;
                for(auto const& cst : data.constants) {
                    if (!first) {
                        os << ',';
                    }
                    first = false;
                    os << ' ' << cst;
                }
                return os;
            },
        },
        statement);
}


}