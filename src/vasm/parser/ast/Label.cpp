#include <vcrate/vasm/parser/ast/Label.hpp>

namespace vcrate::vasm::parser {

std::string Label::to_string() const {
    return "LABEL{ " + name + " }";
}

}