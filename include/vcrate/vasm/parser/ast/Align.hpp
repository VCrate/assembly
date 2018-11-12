#pragma once

#include <vcrate/vasm/parser/ast/Directive.hpp>
#include <vcrate/vasm/parser/ast/Number.hpp>

#include <string>

namespace vcrate::vasm::parser {

class Align : public Directive {
public:

    Align(Number const& number);

    std::string to_string() const override;

    Number number;

};

}