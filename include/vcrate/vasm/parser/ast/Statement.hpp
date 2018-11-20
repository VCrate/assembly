#pragma once

#include <string>
#include <vector>

#include <vcrate/vasm/lexer/Location.hpp>

namespace vcrate::vasm::parser {

class Statement {
public:

    virtual std::string to_string() const = 0;

};

}