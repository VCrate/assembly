#pragma once

#include <iostream>
#include <vector>
#include <string>

#include <vcrate/vasm/lexer/Location.hpp>

namespace vcrate::vasm {

struct Error {
    enum class Type {
        Internal,

        L001_Unknown_character,
        L002_Unknown_escape_sequence,
        L003_Invalid_number_prefix,
        L004_Invalid_symbol_in_number,
        L005_Invalid_register,
        L006_Invalid_floating_number,
        L007_Undefined_number,
        L008_Expected_register,

        G001_Undefined_label,
    };

    struct TypeInfo {
        enum class Category {
            Lexical,
            Internal
        };
        Type type;
        Category category;
    };

    Type type;
    lexer::ScatteredLocation locations;

    std::ostream& report_error(std::ostream& os, std::vector<std::string> const& source, bool use_color = true) const;

};

std::string_view to_string(Error::TypeInfo const& tinfo);
std::string_view to_string(Error::TypeInfo::Category const& cat);
std::string_view to_string(Error::Type const& type);
Error::TypeInfo get_type_info(Error::Type type);

}