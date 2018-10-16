#pragma once

#include <vcrate/vasm/parser/ast/Term.hpp>

#include <memory>

namespace vcrate::vasm::parser {

class DereferencedConstant : public Term {
public:

    std::unique_ptr<Constant> constant;

};

}