#include <vcrate/vasm/parser/ast/Register.hpp>

namespace vcrate::vasm::parser {

Register::Register(Type const& reg) : reg(reg) {}

std::string Register::to_string() const {
    return "REG{ " + reg.to_string() + " }";
}

}