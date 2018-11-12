#pragma once

#include <vcrate/vasm/parser/ast/Statement.hpp>

#include <string>

namespace vcrate::vasm::parser {

class Label : public Statement {
public:

    Label(std::string const& name);

    std::string to_string() const override;

    std::string name;

};

}