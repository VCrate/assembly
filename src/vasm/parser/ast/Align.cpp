#include <vcrate/vasm/parser/ast/Align.hpp>

namespace vcrate::vasm::parser {

Align::Align(Number const& number) : number(number) {}

std::string Align::to_string() const {
    return "ALIGN{ " + number.to_string() + " }";
}

void Align::generate(generator::Generator& gen) const {
    //gen.set_align(number.value());
}

}