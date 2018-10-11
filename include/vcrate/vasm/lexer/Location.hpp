#pragma once

#include <vcrate/Alias.hpp>
#include <vcrate/vasm/lexer/Position.hpp>

namespace vcrate::vasm::lexer {

struct Location : Position {

    ui32 lenght;

};

}