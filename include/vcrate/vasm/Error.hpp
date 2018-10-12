#pragma once

#include <iostream>
#include <vector>
#include <string>

#include <vcrate/vasm/lexer/Location.hpp>

namespace vcrate::vasm {

struct Error {
    enum class Type {
        LexUnknownCharacter,
        LexUnknownEscapeSequence
    };

    Type type;
    lexer::Location location;

    std::ostream& report_error(std::ostream& os, std::vector<std::string> const& source, bool use_color = true) const;

};

}