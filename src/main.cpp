#include <vcrate/bytecode/Operations.hpp>
#include <vcrate/instruction/Instruction.hpp>
#include <vcrate/instruction/Make.hpp>

#include <vcrate/vasm/lexer/Lexer.hpp>
#include <vcrate/vasm/parser/Parser.hpp>
#include <vcrate/vasm/Reader.hpp>

#include <vcrate/vasm/test/Test.hpp>

#include <iostream>

int main() {

    //std::vector<std::string> source = vcrate::vasm::read_file("test/test0.vasm");
    std::vector<std::string> source {
        "mov",
        "mov %A",
        "mov %A, %PC",
        "mov %A, %F, %SP",
    };

    for(auto& s : source)
        s.push_back('\n');

    auto res = vcrate::vasm::lexer::tokenize(source);
    if(auto err = res.get_if_error(); err) {
        err->report_error(std::cerr, source) << '\n';
        return 1;
    } else {
        for(auto token : res.get_result()) {
            std::cout << token.location.line << "/" << token.location.character << " ~" << token.location.lenght << " > " << vcrate::vasm::lexer::to_string(token.type) << " '" << token.content << "'\n";
        }
    }



    std::cout << "Parsing...\n";
    auto res_ = vcrate::vasm::parser::parse(res.get_result());
    std::cout << "Finished!\n";
    if(auto err = res_.get_if_error(); err) {
        err->report_error(std::cerr, source) << '\n';
        return 1;
    } else {
        for(auto& statement : res_.get_result()) {
            std::cout << statement->to_string() << '\n';
        }
    }
}