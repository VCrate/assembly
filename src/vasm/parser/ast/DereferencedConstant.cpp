#include <vcrate/vasm/parser/ast/DereferencedConstant.hpp>

namespace vcrate::vasm::parser {

std::string DereferencedConstant::to_string() const {
    return "DEREFCONST{ " + constant->to_string() + " }";
}

}