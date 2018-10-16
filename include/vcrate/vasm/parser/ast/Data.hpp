#pragma once

#include <vector>
#include <memory>

#include <vcrate/vasm/parser/ast/Constant.hpp>

namespace vcrate::vasm::parser {

class Data {
public:

    std::size_t size;
    std::vector<std::unique_ptr<Constant>> constants;

};

}