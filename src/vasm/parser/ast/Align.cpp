#include <vcrate/vasm/parser/ast/Align.hpp>

namespace vcrate::vasm::parser {

std::string Align::to_string() const {
    return "ALIGN{ " + raw_value + " }";
}

}