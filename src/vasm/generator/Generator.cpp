#include <vcrate/vasm/generator/Generator.hpp>

#include <vcrate/instruction/Instruction.hpp>
#include <vcrate/vasm/generator/Argument.hpp>

#include <algorithm>

namespace vcrate::vasm::generator {

std::ostream& operator<< (std::ostream& os, Generator const& gen) {
    for(auto istr : gen.code) {
        os << istr << '\n';
    }
    return os;
}

void Generator::emit(bytecode::Operations ope, std::vector<Argument> const& args) {
    if (std::all_of(std::begin(args), std::end(args), [] (auto const& arg) {
        return !std::holds_alternative<Argument>(arg);
    })) {

    }
    instruction::Instruction istr(ope, )
}

void Generator::add_label(std::string const& name) {
    labels.emplace(name, code.size());
}


}