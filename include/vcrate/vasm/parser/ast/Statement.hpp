#pragma once

#include <string>
#include <vector>

#include <vcrate/vasm/lexer/Location.hpp>
#include <vcrate/vasm/generator/Generator.hpp>
#include <vcrate/vasm/parser/ast/Node.hpp>

namespace vcrate::vasm::parser {

class Statement : public Node {
public:

    virtual void generate(generator::Generator& gen) const = 0;

};

}