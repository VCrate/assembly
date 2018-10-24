#include <vcrate/vasm/parser/ast/Number.hpp>

namespace vcrate::vasm::parser {

Number::Number(std::string raw_value, ui8 base) : raw_value(raw_value), base(base) {}

std::string Number::to_string() const {
    return "NUMBER{ " + raw_value + " % " + std::to_string(base) + " }";
}

}