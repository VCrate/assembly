#include <vcrate/vasm/lexer/Type.hpp>

namespace vcrate::vasm::lexer {

std::string_view to_string(Type type) {
    switch(type) {
        case Type::Colon: return "Colon"; 
        case Type::Comma: return "Comma";
        case Type::Register: return "Register";
        case Type::Ident: return "Ident"; 
        case Type::Hex: return "Hex"; 
        case Type::Dec: return "Dec"; 
        case Type::Oct: return "Oct"; 
        case Type::Bin: return "Bin";
        case Type::String: return "String";
        case Type::Directive: return "Directive";
        case Type::DB: return "DB"; 
        case Type::DW: return "DW"; 
        case Type::DD: return "DD";
        case Type::Plus: return "Plus"; 
        case Type::Minus: return "Minus"; 
        case Type::Div: return "Div"; 
        case Type::Mult: return "Mult"; 
        case Type::Mod: return "Mod"; 
        case Type::Exp: return "Exp"; 
        case Type::ShiftL: return "ShiftL"; 
        case Type::ShiftR: return "ShiftR"; 
        case Type::RotateL: return "RotateL"; 
        case Type::RotateR: return "RotateR";
        case Type::Or: return "Or"; 
        case Type::And: return "And"; 
        case Type::Xor: return "Xor"; 
        case Type::Neg: return "Neg";
        case Type::Not: return "Not";
        case Type::AndLogic: return "AndLogic";
        case Type::OrLogic: return "OrLogic";
        case Type::Eq: return "Eq";
        case Type::Neq: return "Neq";
        case Type::Gt: return "Gt";
        case Type::Lt: return "Lt";
        case Type::Leqt: return "Leqt";
        case Type::Geqt: return "Geqt";
        case Type::CloseBracket: return "CloseBracket"; 
        case Type::OpenBracket: return "OpenBracket";
        case Type::Comment: return "Comment";
        case Type::NewLine: return "NewLine";
    }
    return "Undefined";
}

}