#include <vcrate/vasm/parser/ast/Command.hpp>

namespace vcrate::vasm::parser {

Command::Command(bytecode::Operations ope, std::vector<std::unique_ptr<Argument>>&& arguments) : ope(ope), arguments(std::move(arguments)) {}


std::string Command::to_string() const {
    std::string str =  "COMMAND{ " + bytecode::OpDefinition::get(ope).name + " ";
    bool first = true;
    for(auto const& arg : arguments) {
        if (!first) str += ", ";
        else        first = false;
        str += arg->to_string();
    }
    return str += " }";
}

}