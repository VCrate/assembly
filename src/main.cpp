#include <vcrate/bytecode/Operations.hpp>
#include <vcrate/instruction/Instruction.hpp>
#include <vcrate/instruction/Make.hpp>

#include <vcrate/vasm/lexer/Lexer.hpp>
#include <vcrate/vasm/parser/Parser.hpp>
#include <vcrate/vasm/Reader.hpp>
#include <vcrate/vasm/generator/Generator.hpp>

#include <vcrate/vasm/test/Test.hpp>

#include <iostream>
#include <iomanip>

int main() {

    std::vector<std::string> source = vcrate::vasm::read_file("test/test0.vasm");
    /*std::vector<std::string> source {
        "mov",
        "mov %A",
        "mov %A, %PC",
        "mov %A, %F, %SP",
        "mov 0xFF + 22 >> abc, test, 5, ~0o2",
        "mov 0xFF + (22 >> abc), [[4 ** 5 % 5] ^ 8 && me]",
        "mov 1 + 2 * 3",
        "mov 1 + 2 + 3",
        "mov 1 + 2 | 3",
        "mov 1 ** 2 * 3",
        "mov 1 && 2 | 3",
        "mov [1]",
        "mov [1 + 1]",
        "inc [%PC + 1]",
        "mov [%A]",
        "label:",
        "db 1, [42], ok, [test ** here]",
        "dd l",
        "dw 0x55",
        "# align 0x4",
        "lea %A, [%B + (24 & 0b0101)]"
    };*/

    /*for(auto& s : source)
        s.push_back('\n');*/

    auto res = vcrate::vasm::lexer::tokenize(source);
    if(auto err = res.get_if_error(); err) {
        err->report_error(std::cerr, source) << '\n';
        return 1;
    } else {
        constexpr std::string_view color_red   = "\033[31m";
        constexpr std::string_view color_blue  = "\033[36m";
        constexpr std::string_view color_reset = "\033[0m";
        constexpr std::string_view color_bold  = "\033[1m";

        std::cout << color_bold << "  Line  | Column | Length |       Type       |           Content           \n" << color_reset;
        for(auto token : res.get_result()) {
            //std::cout << token.location.line << "/" << token.location.character << " ~" << token.location.lenght << " > " << vcrate::vasm::lexer::to_string(token.type) << " '" << token.content << "'\n";
            std::cout << color_blue << " " << std::setw(6) << token.location.line << " |";
            std::cout << color_blue << " " << std::setw(6) << token.location.character << " |";
            std::cout << color_blue << " " << std::setw(6) << token.location.lenght << " |";
            std::cout << color_red << " " << std::setw(16) << vcrate::vasm::lexer::to_string(token.type) << " | " << color_reset;
            std::string content;
            for(auto c : token.content) {
                switch(c) {
                    case '\n': content += "\\n"; break;
                    case '\t': content += "\\t"; break;
                    default: content.push_back(c); break;
                }
            }
            std::cout << content << '\n' << color_reset;
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
            std::cout << statement << '\n';
        }
    }
/*
    vcrate::vasm::generator::Generator gen;
    for(auto& statement : res_.get_result()) {
        statement->generate(gen);
    }

    std::cout << '\n';
    std::cout << gen << '\n';
*/
}