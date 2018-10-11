#pragma once

#include <vcrate/Alias.hpp>

#include <vcrate/vasm/lexer/Location.hpp>
#include <vcrate/vasm/lexer/Type.hpp>

#include <string>

namespace vcrate::vasm::lexer {

struct Token {

    Location location;
    std::string content;
    Type type;

};

}