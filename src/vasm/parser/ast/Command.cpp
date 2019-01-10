#include <vcrate/vasm/parser/ast/Command.hpp>

#include <vcrate/vasm/generator/Argument.hpp>

namespace vcrate::vasm::parser {

Command::Command(bytecode::Operations ope, std::vector<Argument>&& arguments) : ope(ope), arguments(std::move(arguments)) {}


std::string Command::to_string() const {
    std::string str =  "COMMAND{ " + bytecode::OpDefinition::get(ope).name;
    bool first = true;
    for(auto const& arg : arguments) {
        if (!first) str += ",";
        else        first = false;
        str += " " + arg.to_string();
    }
    return str += " }";
}

void Command::generate(generator::Generator& gen) const {
    std::vector<instruction::Argument> args;
    for(auto const& a : arguments) {
        auto res = a.argument_value(gen);
        if (auto* err = res.get_if_error()) {
            if (err->type == Error::Type::G001_Undefined_label) {

            }
        }
        args.emplace_back();
    }
    gen.emit(ope, args);
}

}