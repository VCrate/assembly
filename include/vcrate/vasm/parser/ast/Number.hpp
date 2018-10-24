#pragma once

#include <string>

#include <vcrate/Alias.hpp>
#include <vcrate/vasm/parser/ast/Term.hpp>

namespace vcrate::vasm::parser {

class Number : public Term {
public:

    Number(std::string raw_value, ui8 base);
    std::string to_string() const override;

    std::string raw_value;
    ui8 base;

};

}