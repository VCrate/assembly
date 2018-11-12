#include <vcrate/vasm/parser/ast/Align.hpp>

namespace vcrate::vasm::parser {

Align::Align(Number const& number) : number(number) {}

std::string Align::to_string() const {
    return "ALIGN{ " + number.to_string() + " }";
}

}