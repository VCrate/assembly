#pragma once

#include <vcrate/Alias.hpp>

#include <vcrate/vasm/lexer/Position.hpp>
#include <vcrate/vasm/lexer/Type.hpp>

namespace vcrate::vasm::lexer {

struct Token {

    Position position;

    Type type;

};

}