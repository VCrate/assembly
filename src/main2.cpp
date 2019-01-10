#include <vcrate/bytecode/Operations.hpp>
#include <vcrate/instruction/Instruction.hpp>
#include <vcrate/instruction/Make.hpp>

#include <vcrate/vasm/lexer/Lexer.hpp>
#include <vcrate/vasm/generator/Generator.hpp>
#include <vcrate/vasm/parser/Parser.hpp>
#include <vcrate/vasm/Reader.hpp>

#include <vcrate/vasm/test/Test.hpp>

#include <iostream>
#include <iomanip>

int main() {

    using namespace vcrate::vasm;
    using namespace vcrate::instruction;
    using namespace vcrate::bytecode;

    generator::Generator g;

    g.emit(Operations::MOV, { generator::Argument{ generator::label_id_t{ 42 } }, generator::Argument{ Register::A } });
    g.emit(Operations::OUT, { generator::Argument{ generator::label_id_t{ 7 } } });
    g.emit(Operations::DBG, { generator::Argument{ Value{ 1337 } } });

    std::cout << g << '\n';

}
