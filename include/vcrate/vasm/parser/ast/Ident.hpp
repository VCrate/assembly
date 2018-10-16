#pragma once

#include <string>

#include <vcrate/vasm/parser/ast/Term.hpp>

namespace vcrate::vasm::parser {

class Ident : public Term {
public:

    std::string name;
    
};

}