#pragma once

#include <vcrate/vasm/parser/ast/Statement.hpp>

#include <string>

namespace vcrate::vasm::parser {

class Label : public Statement {
public:

    std::string name;

};

}