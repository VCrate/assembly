#pragma once

#include <vcrate/vasm/parser/ast/Directive.hpp>

#include <string>

namespace vcrate::vasm::parser {

class Align : public Directive {
public:

    std::string to_string() const override;

    std::string raw_value;

};

}