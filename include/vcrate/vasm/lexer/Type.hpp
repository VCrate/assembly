#pragma once

#include <string_view>

namespace vcrate::vasm::lexer {

enum class Type {

    Colon, Comma,

    Register,
    Ident, 
    Hex, Dec, Oct, Bin, Float,
    String, Char,

    Directive,
    DB, DW, DD,

    // Arithmetic
    Plus, Minus, Div, Mult, Mod, Exp,
    ShiftL, ShiftR, RotateL, RotateR,
    Or, And, Xor, Neg,

    //Logical
    Not, OrLogic, AndLogic, 
    Eq, Neq, Gt, Lt, Geqt, Leqt,

    CloseBracket, OpenBracket,
    CloseParen, OpenParen,

    Comment, NewLine
};

std::string_view to_string(Type type);

}