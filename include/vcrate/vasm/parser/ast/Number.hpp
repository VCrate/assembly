#pragma once

#include <string>

#include <vcrate/vasm/parser/ast/Term.hpp>

namespace vcrate::vasm::parser {

class Number : public Term {
public:

    std::string to_string() const override;

    std::string raw_value;

};

}