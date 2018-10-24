#include <vcrate/vasm/parser/ast/Number.hpp>

namespace vcrate::vasm::parser {

std::string Number::to_string() const {
    return "NUMBER{ " + raw_value + " }";
}

}