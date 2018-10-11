#include <vcrate/vasm/lexer/Error.hpp>

#include <iomanip>

namespace vcrate::vasm::lexer {

std::ostream& Error::report_error(std::ostream& os, std::vector<std::string> const& source, bool use_color) const {
    for(std::size_t i = 0; i < 3; ++i) {
        if (i == 0 && location.line == 0)
            continue;
        std::size_t line = location.line - 1 + i;
        if (line >= source.size())
            continue;
    }

    if(location.line >= 2) {
        auto str = std::string_view(source[location.line - 2]);
        str.remove_suffix(1); // last '\n'
        os << std::setw(9) << std::setfill(' ') << location.line - 2 << " | " << str << '\n';
    }
    if(location.line >= 1) {
        auto str = std::string_view(source[location.line - 1]);
        str.remove_suffix(1); // last '\n'
        os << std::setw(9) << std::setfill(' ') << location.line - 1 << " | " << str << '\n';
    }

    auto str = std::string_view(source[location.line]);
    str.remove_suffix(1); // last '\n'
    os << std::setw(9) << std::setfill(' ') << location.line  << " | " << str << '\n';

    if(use_color)
        os << "\033[31m";

    os << std::string(12 + location.character, ' ') << std::string(location.lenght, '^') << '\n';

    switch(type) {
        case Error::Type::UnknownCharacter:
            os << "Lexical error:"; 
            if(use_color)
                os << "\033[0m";
            return os << " Unknown character\n";
        case Error::Type::UnknownEscapeSequence:
            os << "Lexical error:"; 
            if(use_color)
                os << "\033[0m";
            return os << " Unknown escape sequence in string\n";
        default:
            os << "Error:"; 
            if(use_color)
                os << "\033[0m";
            return os << " Internal error...\n";
    }
}

}