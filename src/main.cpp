#include <vcrate/bytecode/Operations.hpp>
#include <vcrate/instruction/Instruction.hpp>
#include <vcrate/instruction/Make.hpp>

#include <vcrate/vasm/lexer/Lexer.hpp>

#include <iostream>

int main() {

    std::vector<std::string> source = {
        "mov %A, 42"
        "; this is a comment",
        "# align 4",
        "db \"some thing\"",
        "db \"' \\\" \\ttab\n\ttab\n\\ttab \\\\ \\n \n\"",
        "add label, 0xF12",
        "label:",
        "jmpe [label - 0d5]"
    };

    for(auto token : vcrate::vasm::lexer::tokenize(source)) {
        std::cout << token.location.line << "/" << token.location.character << " ~" << token.location.lenght << " > '" << token.content << "'\n";
    }

}