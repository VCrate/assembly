#include <vcrate/vasm/parser/ast/Label.hpp>

namespace vcrate::vasm::parser {

Label::Label(std::string const& name) : name(name) {}

std::string Label::to_string() const {
    return "LABEL{ " + name + " }";
}

}