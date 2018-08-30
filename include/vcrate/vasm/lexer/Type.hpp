#pragma once

namespace vcrate::vasm::lexer {

enum class Type {

    Colon, Comma, NewLine,

    Register,
    Ident, 
    Hex, Dec, Oct, Bin,
    String,

    Directive,
    DB, DW, DD,

    Plus, Minus, Div, Mult, Mod, 
    ShiftL, ShiftR, RotateL, RotateR,
    Or, And, Xor, Neg, Not,

};

}