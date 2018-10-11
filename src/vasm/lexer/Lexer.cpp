#include <vcrate/vasm/lexer/Lexer.hpp>

#include <optional>
#include <iostream>
#include <utility>
#include <tuple>
#include <unordered_set>
#include <algorithm>

namespace vcrate::vasm::lexer {

using TokenPos = std::pair<Token, Position>;

Result<TokenPos> make_token(std::vector<std::string> const& source, Location const& location, Type type);

char at(std::vector<std::string> const& source, Position const& position);
std::string_view at(std::vector<std::string> const& source, Location const& location);

Position skip_whitespace(std::vector<std::string> const& source, Position position);
Position move_position(std::vector<std::string> const& source, Position position, ui32 offset = 1);
Position pass(std::vector<std::string> const& source, Location const& Location);

bool is_eof(std::vector<std::string> const& source, Position const& position);

Result<TokenPos> tokenize_base(std::vector<std::string> const& source, Position const& position);



Result<std::vector<Token>> tokenize(std::vector<std::string> const& source) {
    Position position = {
        .line = 0,
        .character = 0,
    };

    std::vector<Token> tokens;

    position = skip_whitespace(source, position);

    while(!is_eof(source, position)) {
        auto res = tokenize_base(source, position);
        if (res.is_error()) {
            return Result<std::vector<Token>>::error(res.get_error());
        }
        auto&&[token, pos] = res.get_result();
        if (token.type == Type::EndOfFile)
            break;
        position = skip_whitespace(source, pos);
        tokens.emplace_back(std::move(token));
    }

    return Result<std::vector<Token>>::success(tokens);
}

Result<TokenPos> make_token(std::vector<std::string> const& source, Location const& location, Type type) {
    return Result<TokenPos>::success({ 
        Token {location, std::string(at(source, location)), type}, 
        pass(source, location) 
    });
}

std::string_view at(std::vector<std::string> const& source, Location const& location) {
    return std::string_view(source.at(location.line)).substr(location.character, location.lenght);
}

char at(std::vector<std::string> const& source, Position const& position) {
    return source.at(position.line)[position.character];
}

bool is_eof(std::vector<std::string> const& source, Position const& position) {
    return 
        position.line >= source.size() ||
        (position.line == (source.size() - 1) && position.character >= source.back().size());
}

Position skip_whitespace(std::vector<std::string> const& source, Position position) {
    while(!is_eof(source, position)) {
        auto c = at(source, position);
        if (c != ' ' && c != '\t' && c != '\n' && c != '\r') 
            break;

        position = move_position(source, position);
    }
    return position;
}

Position move_position(std::vector<std::string> const& source, Position position, ui32 offset) {
    while(!is_eof(source, position) && offset > 0) {
        if (offset >= source[position.line].size() - position.character) {
            offset -= source[position.line].size() - position.character;
            position.line++;
            position.character = 0;
        } else {
            position.character += offset;
            return position;
        }
    }
    return position;
}

Position pass(std::vector<std::string> const& source, Location const& location) {
    return move_position(source, location, location.lenght);
}

Type ident_type(std::string content) {
    std::transform(content.begin(), content.end(), content.begin(), [](unsigned char c) { return ::tolower(c); });
    if (content == "db")
        return Type::DB;
    if (content == "dw")
        return Type::DW;
    if (content == "dd")
        return Type::DD;

    return Type::Ident;
}

Result<TokenPos> tokenize_ident(std::vector<std::string> const& source, Position position) {
    char c = at(source, position);
    if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_'))
        return Result<TokenPos>::error({Error::Type::UnknownCharacter, {position, 1}});

    Location location { position, 1 };
    position = move_position(source, position);
    c = at(source, position);
    while(!is_eof(source, position) && ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_')) {
        location.lenght++;
        position = move_position(source, position);
        c = at(source, position);
    }

    return make_token(source, location, ident_type(std::string { at(source, location) }));
}

Result<TokenPos> tokenize_string(std::vector<std::string> const& source, Position position) {
    char c = at(source, position);
    if (!(c == '"' || c == '\''))
        return Result<TokenPos>::error({Error::Type::UnknownCharacter, {position, 1}});

    char quote = c;

    Location location { position, 1 };
    bool escape = false;
    position = move_position(source, position);
    c = at(source, position);
    std::string content;

    while(escape || quote != c) {
        if (!escape && c == '\\') {
            escape = true;
            location.lenght++;
            position = move_position(source, position);
            c = at(source, position);
            continue;
        } else if (escape) {
            escape = false;
            switch(c) {
                case '\a':
                case '\b':
                case '\f':
                case '\n':
                case '\r':
                case '\t':
                case '\v':
                    break;
                case '\\':
                    content += '\t';
                    break;
                case 'a':
                    content += '\a';
                    break;
                case 'b':
                    content += '\b';
                    break;
                case 'f':
                    content += '\f';
                    break;
                case 'n':
                    content += '\n';
                    break;
                case 'r':
                    content += '\r';
                    break;
                case 't':
                    content += '\t';
                    break;
                case 'v':
                    content += '\v';
                    break;
                default:
                    if (c == quote)
                        content += c;
                    else {
                        Location location;
                        location.line = position.line;
                        location.character = position.character - 1;
                        location.lenght = 2;
                        return Result<TokenPos>::error({ Error::Type::UnknownEscapeSequence, location });
                    }
                    break;
            }
        } else {
            content += c;
        }

        location.lenght++;
        position = move_position(source, position);
        c = at(source, position);
    }

    location.lenght++;
    return Result<TokenPos>::success({
        Token {location, content, Type::String}, 
        pass(source, location) 
    });
}

Result<TokenPos> tokenize_register_or_mod(std::vector<std::string> const& source, Position position) {
    Location location { position, 1 };
    auto ident = tokenize_ident(source, move_position(source, position));
    if (ident.is_error())
        return make_token(source, location, Type::Mod);

    auto&& [token, pos] = ident.get_result();
    if (token.content == "A" ||
        token.content == "B" ||
        token.content == "C" ||
        token.content == "D" ||
        token.content == "E" ||
        token.content == "F" ||
        token.content == "G" ||
        token.content == "H" ||
        token.content == "I" ||
        token.content == "J" ||
        token.content == "K" ||
        token.content == "L" ||
        token.content == "SP" ||
        token.content == "BP" ||
        token.content == "PC" ||
        token.content == "FG"
    ) {
        location.lenght += token.location.lenght;
        return make_token(source, location, Type::Register);
    } 

    return make_token(source, location, Type::Mod);
}

Result<TokenPos> tokenize_number(std::vector<std::string> const& source, Position position) {
    char c = at(source, position);
    if (!(c >= '0' && c <= '9'))
        return Result<TokenPos>::error({Error::Type::UnknownCharacter, {position, 1}});

    Token token;
    token.type = Type::Dec;
    token.location.character = position.character;
    token.location.line = position.line;
    token.location.lenght = 0;
    token.content = "";

    std::unordered_set<char> symbols = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};

    if (c == '0') {
        position = move_position(source, position);
        c = at(source, position);
        token.location.lenght++;
        switch(c) {
            case 'x':
                token.type = Type::Hex;
                symbols = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'a', 'B', 'b', 'C', 'c', 'D', 'd', 'F', 'f'};
                position = move_position(source, position);
                c = at(source, position);
                token.location.lenght++;
                break;
            case 'b':
                token.type = Type::Bin;
                symbols = {'0', '1'};
                position = move_position(source, position);
                c = at(source, position);
                token.location.lenght++;
                break;
            case 'o':
                token.type = Type::Oct;
                symbols = {'0', '1', '2', '3', '4', '5', '6', '7'};
                position = move_position(source, position);
                c = at(source, position);
                token.location.lenght++;
                break;
            case 'd':
                position = move_position(source, position);
                c = at(source, position);
                token.location.lenght++;
                break;
            default:
                token.content += '0';
                break;
        }
    }
    
    while(!is_eof(source, position) && symbols.find(c) != symbols.end()) {
        token.content += c;
        position = move_position(source, position);
        c = at(source, position);
        token.location.lenght++;

        while(!is_eof(source, position) && (c == '\'' || c == '_')) {
            position = move_position(source, position);
            c = at(source, position);
            token.location.lenght++;
        }

    }

    return Result<TokenPos>::success({ token, position });
}

Result<TokenPos> tokenize_base(std::vector<std::string> const& source, Position const& position) {
    char c = at(source, position);
    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_')
        return tokenize_ident(source, position);

    if (c >= '0' && c <= '9')
            return tokenize_number(source, position);

    switch(c) {
        case '%':
            return tokenize_register_or_mod(source, position);

        case '"':
        case '\'':
            return tokenize_string(source, position);

        case '#':
            return make_token(source, { position, 1 }, Type::Directive);

        case ';': {
            Position new_position { position.line + 1, 0 };
            if (is_eof(source, new_position))
                return Result<TokenPos>::success({{{new_position, 0}, "", Type::EndOfFile}, new_position});
            return tokenize_base(source, new_position);
        }
        case ',':
            return make_token(source, { position, 1 }, Type::Comma);

        case ':':
            return make_token(source, { position, 1 }, Type::Colon);

        case '+':
            return make_token(source, { position, 1 }, Type::Plus);

        case '-':
            return make_token(source, { position, 1 }, Type::Minus);

        case '/':
            return make_token(source, { position, 1 }, Type::Div);

        case '*':
            return make_token(source, { position, 1 }, Type::Mult);

        case '|':
            return make_token(source, { position, 1 }, Type::Or);

        case '&':
            return make_token(source, { position, 1 }, Type::And);

        case '^':
            return make_token(source, { position, 1 }, Type::Xor);

        case '~':
            return make_token(source, { position, 1 }, Type::Not);

        case '[':
            return make_token(source, { position, 1 }, Type::OpenBracket);

        case ']':
            return make_token(source, { position, 1 }, Type::CloseBracket);

        default:
            break;
    }
    return Result<TokenPos>::error({Error::Type::UnknownCharacter, {position, 1} });
}

}