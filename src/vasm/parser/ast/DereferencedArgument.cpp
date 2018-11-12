#include <vcrate/vasm/parser/ast/DereferencedArgument.hpp>

namespace vcrate::vasm::parser {

DereferencedArgument::DereferencedArgument(Register reg, std::unique_ptr<Constant> displacement) : reg(reg), displacement(std::move(displacement)) {}

std::string DereferencedArgument::to_string() const {
    if(displacement) {
        return "DEREFARG{ " + reg.to_string() + " + " + displacement->to_string() + " }";
    } else {
        return "DEREFARG{ " + reg.to_string() + " }";
    }
}

}