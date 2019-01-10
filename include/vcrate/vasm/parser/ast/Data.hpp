#pragma once

#include <vector>
#include <memory>

#include <vcrate/vasm/parser/ast/Constant.hpp>
#include <vcrate/vasm/parser/ast/Statement.hpp>

namespace vcrate::vasm::parser {

class Data : public Statement {
public:

    Data(std::size_t size, std::vector<std::unique_ptr<Constant>> constants);

    std::string to_string() const override;
    void generate(generator::Generator& gen) const override;

    std::size_t size;
    std::vector<std::unique_ptr<Constant>> constants;

};

}