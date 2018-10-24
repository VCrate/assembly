#include <vcrate/vasm/parser/ast/Data.hpp>

namespace vcrate::vasm::parser {

std::string Data::to_string() const {
    std::string str =  "DATA{ " + std::to_string(size) + "; ";
    bool first = true;
    for(auto const& constant : constants) {
        if (!first) str += ", ";
        else        first = false;
        str += constant->to_string();
    }
    return str += " }";
}

}