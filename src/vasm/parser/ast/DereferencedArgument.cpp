#include <vcrate/vasm/parser/ast/DereferencedArgument.hpp>

namespace vcrate::vasm::parser {

std::string DereferencedArgument::to_string() const {
    return "DEREFARG{ " + reg.to_string() + " + " + displacement->to_string() + " }";
}

}