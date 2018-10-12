#pragma once

#include <string_view>

namespace vcrate::vasm::lexer {

enum class Type {

    Colon, Comma,

    Register,
    Ident, 
    Hex, Dec, Oct, Bin,
    String,

    Directive,
    DB, DW, DD,

    Plus, Minus, Div, Mult, Mod, 
    ShiftL, ShiftR, RotateL, RotateR,
    Or, And, Xor, Not,

    CloseBracket, OpenBracket,

    EndOfFile, Comment,
};

std::string_view to_string(Type type);

}