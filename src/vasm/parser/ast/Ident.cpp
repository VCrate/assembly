#include <vcrate/vasm/parser/ast/Ident.hpp>

namespace vcrate::vasm::parser {

std::string Ident::to_string() const {
    return "IDENT{ " + name + " }";
}

}