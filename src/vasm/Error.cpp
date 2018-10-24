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

    auto info = get_type_info(type);
    os << to_string(info.category) << " error";
    if(use_color)
        os << "\033[0m";
    return os << "(" << location.line << ":" << location.character + 1 << ") " << to_string(info.type);
}

std::string_view to_string(Error::TypeInfo const& tinfo) {
    return std::string { to_string(tinfo.category) } + " error: " + std::string{ to_string(tinfo.type) };
}

std::string_view to_string(Error::TypeInfo::Category const& cat) {
    switch(cat) {
        case Error::TypeInfo::Category::Lexical:
            return "Lexical";
        case Error::TypeInfo::Category::Internal:
        default:
            return "Internal";
    }
}

std::string_view to_string(Error::Type const& type) {
    switch(type) {
        case Error::Type::LexUnknownCharacter:
            return "Unknown character";
        case Error::Type::LexUnknownEscapeSequence:
            return "Invalid escape sequence in string";
        case Error::Type::LexInvalidNumber:
            return "Invalid number format";
        case Error::Type::Internal:
        default:
            return "Internal error";
    }
}

Error::TypeInfo get_type_info(Error::Type type) {
    switch(type) {
        case Error::Type::LexUnknownCharacter:
        case Error::Type::LexUnknownEscapeSequence:
        case Error::Type::LexInvalidNumber:
            return {type, Error::TypeInfo::Category::Lexical};

        case Error::Type::Internal:
        default:
            return {type, Error::TypeInfo::Category::Internal};
    }
}


}