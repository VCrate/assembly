#include <vcrate/vasm/parser/ast/String.hpp>

namespace vcrate::vasm::parser {

String::String(std::string const& str) : str(str) {}

std::string String::to_string() const {
    return "STRING{ " + str + " }";
}

}