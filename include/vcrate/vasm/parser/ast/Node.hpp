#pragma once

#include <string>
#include <vector>

#include <vcrate/vasm/lexer/Location.hpp>
#include <vcrate/vasm/generator/Generator.hpp>

namespace vcrate::vasm::parser {

class Node {
public:

    virtual std::string to_string() const = 0;

    lexer::ScatteredLocation locations;

};

}