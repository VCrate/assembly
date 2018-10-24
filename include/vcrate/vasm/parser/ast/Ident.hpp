#pragma once

#include <string>

#include <vcrate/vasm/parser/ast/Term.hpp>

namespace vcrate::vasm::parser {

class Ident : public Term {
public:

    Ident(std::string const& name);
    std::string to_string() const override;

    std::string name;
    
};

}