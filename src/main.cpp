#include <vcrate/bytecode/Operations.hpp>
#include <vcrate/instruction/Instruction.hpp>
#include <vcrate/instruction/Make.hpp>

#include <vcrate/vasm/lexer/Lexer.hpp>
#include <vcrate/vasm/parser/Parser.hpp>
#include <vcrate/vasm/Reader.hpp>

#include <vcrate/vasm/test/Test.hpp>

#include <iostream>

int main() {

    std::vector<std::string> source = vcrate::vasm::read_file("test/hello.vasm");
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
/*
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
    }*/

    vcrate::vasm::lexer::ScatteredLocation ls;

    std::cout << "[0] ";
    for(auto const& l : ls.locations) {
        std::cout << l.line << ":" << l.character << "@" << l.lenght << ", ";
    }
    std::cout << '\n';

    ls.extends({1, 5, 3});
    ls.extends({1, 9, 3});

    std::cout << "[1] ";
    for(auto const& l : ls.locations) {
        std::cout << l.line << ":" << l.character << "@" << l.lenght << ", ";
    }
    std::cout << '\n';

    ls.extends({1, 10, 5});

    std::cout << "[2] ";
    for(auto const& l : ls.locations) {
        std::cout << l.line << ":" << l.character << "@" << l.lenght << ", ";
    }
    std::cout << '\n';

    ls.extends({0, 1, 15});

    std::cout << "[3] ";
    for(auto const& l : ls.locations) {
        std::cout << l.line << ":" << l.character << "@" << l.lenght << ", ";
    }
    std::cout << '\n';

    ls.extends({1, 4, 6});

    std::cout << "[4] ";
    for(auto const& l : ls.locations) {
        std::cout << l.line << ":" << l.character << "@" << l.lenght << ", ";
    }
    std::cout << '\n';

    ls.extends({1, 6, 6});

    std::cout << "[5] ";
    for(auto const& l : ls.locations) {
        std::cout << l.line << ":" << l.character << "@" << l.lenght << ", ";
    }
    std::cout << '\n';

    ls.extends({2, 6, 6});

    std::cout << "[6] ";
    for(auto const& l : ls.locations) {
        std::cout << l.line << ":" << l.character << "@" << l.lenght << ", ";
    }
    std::cout << '\n';

    ls.extends({2, 6, 6});

    std::cout << "[7] ";
    for(auto const& l : ls.locations) {
        std::cout << l.line << ":" << l.character << "@" << l.lenght << ", ";
    }
    std::cout << '\n';

    ls.extends({2, 7, 6});

    std::cout << "[8] ";
    for(auto const& l : ls.locations) {
        std::cout << l.line << ":" << l.character << "@" << l.lenght << ", ";
    }
    std::cout << '\n';

    ls.extends({3, 0, 2});
    ls.extends({3, 3, 2});
    ls.extends({3, 6, 2});
    ls.extends({3, 9, 3});
    ls.extends({3, 2, 8});

    std::cout << "[9] ";
    for(auto const& l : ls.locations) {
        std::cout << l.line << ":" << l.character << "@" << l.lenght << ", ";
    }
    std::cout << '\n';
}