#pragma once

#include <string>

#include <vcrate/Alias.hpp>
#include <vcrate/vasm/parser/ast/Constant.hpp>

namespace vcrate::vasm::parser {

class String : public Constant {
public:

    String(std::string const& str);
    std::string to_string() const override;

    std::string str;

};

}