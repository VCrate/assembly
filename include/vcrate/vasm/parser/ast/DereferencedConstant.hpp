#pragma once

#include <vcrate/vasm/parser/ast/Term.hpp>

#include <memory>

namespace vcrate::vasm::parser {

class DereferencedConstant : public Term {
public:

    std::string to_string() const override;

    std::unique_ptr<Constant> constant;

};

}