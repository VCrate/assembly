#include <vcrate/vasm/parser/ast/Register.hpp>

namespace vcrate::vasm::parser {

std::string Register::to_string() const {
    return "REG{ " + reg.to_string() + " }";
}

}