#pragma once

#include <string>

namespace vcrate::vasm::parser {

class Statement {
public:

    virtual std::string to_string() const = 0;

};

}