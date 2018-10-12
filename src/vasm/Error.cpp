#include <vcrate/vasm/Error.hpp>

#include <iomanip>

namespace vcrate::vasm {

std::ostream& print_substr(std::ostream& os, std::string const& str, std::size_t start, std::size_t len) {
    auto s = std::string_view{ str }.substr(start, len);
    return os << s;
}

std::string_view substr(std::string const& s, std::size_t start, int len) {
    if (len <= 0) len += s.size();
    return std::string_view{ s }.substr(start, len);
}

std::ostream& Error::report_error(std::ostream& os, std::vector<std::string> const& source, bool use_color) const {
    for(std::size_t i = 0; i < 3; ++i) {
        if (i == 0 && location.line == 0)
            continue;
        std::size_t line = location.line - 1 + i;
        if (line >= source.size())
            continue;
    }

    std::size_t pad_size = std::to_string(location.line).size();

    if(location.line >= 2)
        os << std::setw(pad_size) << std::setfill(' ') << location.line - 2 << " | " << source[location.line - 2];
    if(location.line >= 1)
        os << std::setw(pad_size) << std::setfill(' ') << location.line - 1 << " | " << source[location.line - 1];

    os << std::setw(pad_size) << std::setfill(' ') << location.line << " | " << source[location.line];

    if(use_color)
        os << "\033[91m";

    os << std::string(pad_size + 3 + location.character, ' ') << std::string(location.lenght, '~') << '\n';

    switch(type) {
        case Error::Type::LexUnknownCharacter:
            os << "Lexical error "; 
            if(use_color)
                os << "\033[0m";
            return os << "(" << location.line << ":" << location.character + 1 << ") Unknown character\n";
        case Error::Type::LexUnknownEscapeSequence:
            os << "Lexical error "; 
            if(use_color)
                os << "\033[0m";
            return os << "(" << location.line << ":" << location.character + 1 << ") Unknown escape sequence in string\n";
        default:
            os << "Error "; 
            if(use_color)
                os << "\033[0m";
            return os << "(" << location.line << ":" << location.character + 1 << ") Internal error...\n";
    }
}

}