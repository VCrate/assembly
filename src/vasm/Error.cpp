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
    auto const& locations = this->locations.locations;
    for(std::size_t i{0}; i < locations.size(); ++i) {
        auto line = locations[i].line;
        
        if(line >= 2)
            os << std::setw(8) << std::setfill(' ') << line - 2 << " | " << source[line - 2];
        if(line >= 1)
            os << std::setw(8) << std::setfill(' ') << line - 1 << " | " << source[line - 1];

        os << std::setw(8) << std::setfill(' ') << line << " | " << source[line];

        if(use_color) {
            os << "\033[91m";
        }

        for(; i < locations.size() && line == locations[i].line; ++i) {
            auto const& location = locations[i];

            if (location.lenght > 0) {
                os << std::string(11 + location.character, ' ') << std::string(location.lenght - 1, '~') << '^';
            }
        }

        os << '\n';

        auto info = get_type_info(type);
        os << to_string(info.category) << " error";
        if(use_color)
            os << "\033[0m";
        os << "(" << line << ":" << locations[i].character + 1 << ") " << to_string(info.type) << '\n';
    }

    return os;
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