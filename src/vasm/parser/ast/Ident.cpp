#include <vcrate/vasm/parser/ast/Ident.hpp>

namespace vcrate::vasm::parser {

Ident::Ident(std::string const& name) : name(name) {}

std::string Ident::to_string() const {
    return "IDENT{ " + name + " }";
}

}