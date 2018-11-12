#include <vcrate/vasm/parser/ast/DereferencedConstant.hpp>

namespace vcrate::vasm::parser {

DereferencedConstant::DereferencedConstant(std::unique_ptr<Constant> constant): constant(std::move(constant)) {}

std::string DereferencedConstant::to_string() const {
    return "DEREFCONST{ " + constant->to_string() + " }";
}

}