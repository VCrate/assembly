#pragma once

#include <iostream>
#include <vector>
#include <string>

#include <vcrate/vasm/lexer/Location.hpp>

namespace vcrate::vasm::lexer {

struct Error {
    enum class Type {
        UnknownCharacter,
        UnknownEscapeSequence
    };

    Type type;
    Location location;

    std::ostream& report_error(std::ostream& os, std::vector<std::string> const& source, bool use_color = true) const;

};

}