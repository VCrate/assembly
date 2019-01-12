#pragma once

#include <variant>
#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <optional>

#include <vcrate/bytecode/Operations.hpp>
#include <vcrate/instruction/Argument.hpp>
#include <vcrate/vasm/lexer/Location.hpp>

namespace vcrate::vasm::parser {

struct Located {
    lexer::ScatteredLocation locations;
};

/*

x   Statement:
x       Label
x       Directive
x       Command
x       Data

x   Label:
x       String

x   Directive:
x       Term (align)

x   Data:
x       Vector<Constant>, Size

x   Command:
x       Ope, Vector<Argument>

x   Constant:
x       Term
x       String

x   Term:
x       Integer
x       Float
x       Ident
x       UnaryRelation
x       BinaryRelation

x   UnaryRelation:
x       Term, UnaryType

x   BinaryRelation:
x       Term, Term, BinaryType

x   Argument:
x       Term (constant)
x       Term (derefenced)
x       Register, Optional<Term> (displacement)
x       Register

*/

// Forward declaration

struct Term;




// Term
using integer_t = ui32;
using float_t = float;
using ident_t = std::string;

struct UnaryRelation {
    enum Type {
        Negation,   // !
        Complement, // ~
        Opposite    // -
    };

    Type type;
    std::unique_ptr<Term> operand;
};

struct BinaryRelation {
    enum Type {
        Addition,       // +
        Subtraction,    // -
        Multiplication, // *
        Division,       // /
        Modulo,         // %
        ShiftLeft,      // <<
        ShiftRight,     // >>
        RotateLeft,     // <<<
        RotateRight,    // >>>
        Or,             // |
        LogicalOr,      // ||
        And,            // &
        LogicalAnd,     // &&
        Xor,            // ^
        Exp,            // **
        Less,           // <
        LessEquals,     // <=
        Greater,        // >
        GreaterEquals,  // >=
        Equals,         // ==
        Unequals        // !=
    };

    Type type;
    std::unique_ptr<Term> lhs;
    std::unique_ptr<Term> rhs;
};

struct Dereferenced {
    std::unique_ptr<Term> term;
};

struct Term : Located {
    using variant_t = std::variant<float_t, integer_t, ident_t, UnaryRelation, BinaryRelation, Dereferenced>;

    variant_t value;
};

std::ostream& operator << (std::ostream& os, Term const& term);
std::ostream& operator << (std::ostream& os, Term::variant_t const& term);

using precedence_t = ui32;
precedence_t precedence_of(BinaryRelation::Type type);




// Constant

using string_t = std::string;

struct Constant : Located {
    using term_t = Term::variant_t;
    using variant_t = std::variant<string_t, term_t>;

    variant_t value;
};

std::ostream& operator << (std::ostream& os, Constant const& constant);
std::ostream& operator << (std::ostream& os, Constant::variant_t const& constant);



// Argument

struct Register : Located {
    using register_t = instruction::Register;

    register_t reg;
};

struct Displacement {
    Register reg;
    std::optional<Term> term;
};

struct Argument : Located {
    using term_t = Term::variant_t;
    using register_t = Register::register_t;

    using variant_t = std::variant<term_t, register_t, Displacement>;

    variant_t value;
};

std::ostream& operator << (std::ostream& os, Argument const& argument);
std::ostream& operator << (std::ostream& os, Argument::variant_t const& argument);



// Data

struct Data {
    enum Size {
        Byte, 
        Word, 
        DoubleWord
    };

    Size size;
    std::vector<Constant> constants;
};



// Label

using label_t = std::string;



// Command

struct Command {
    using ope_t = bytecode::Operations;

    ope_t ope;
    std::vector<Argument> args;
};



// Directive

struct Align {
    Term value;
};

struct Directive {
    using variant_t = std::variant<Align>;

    variant_t value;
};



// Statement

struct Statement : Located {
    using variant_t = std::variant<Directive, label_t, Command, Data>;

    variant_t value;
};

std::ostream& operator << (std::ostream& os, Statement const& statement);
std::ostream& operator << (std::ostream& os, Statement::variant_t const& statement);

}