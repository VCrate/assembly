#include <vcrate/vasm/test/Test.hpp>

#include <algorithm>

namespace vcrate::vasm::test {

TestCase::TestCase(std::string const& testcase) : testcase(testcase) {}

bool TestCase::done() const { return is_done; }

std::string format_position(lexer::Position const& pos) {
    return std::to_string(pos.line) + ":" + std::to_string(pos.character);
}

std::string format_location(lexer::Location const& loc) {
    return format_position(loc) + "@" + std::to_string(loc.lenght);
}

std::string format_token(lexer::Token const& token) {
    if (token.type == lexer::Type::NewLine)
        return  "{" + format_location(token.location) + " {} " + std::string{ lexer::to_string(token.type) } + "}";
    return  "{" + format_location(token.location) + " {" + token.content + "} " + std::string{ lexer::to_string(token.type) } + "}";
}

std::string format_lexer_result(lexer::LexerResult const& res) {
    if(auto* error = res.get_if_error(); error) {
        return format_location(error->locations.locations[0]) + " " + std::string{ to_string(error->type) };
    }

    std::string str = "[";
    bool first = true;
    for(auto const& t : res.get_result()) {
        if (!first)
            str += " ";
        first = false;

        str += format_token(t);
    }
    return str + "]";
}

bool operator == (lexer::Position const& lhs, lexer::Position const& rhs) {
    return lhs.line == rhs.line && lhs.character == rhs.character;
}

bool operator == (lexer::Location const& lhs, lexer::Location const& rhs) {
    return static_cast<lexer::Position>(lhs) == static_cast<lexer::Position>(rhs) && lhs.lenght == rhs.lenght;
}

bool operator == (lexer::Token const& lhs, lexer::Token const& rhs) {
    return lhs.type == rhs.type && lhs.location == rhs.location && lhs.content == rhs.content;
}

bool operator == (lexer::LexerResult const& lhs, lexer::LexerResult const& rhs) {
    if (lhs.is_error() != rhs.is_error())
        return false;
    
    if (lhs.is_error()) {
        auto lerr = lhs.get_error();
        auto rerr = rhs.get_error();

        return lerr.locations.locations[0] == rerr.locations.locations[0] && lerr.type == rerr.type;
    }

    auto lres = lhs.get_result();
    auto rres = rhs.get_result();
    std::cerr << lres.size() << ", " << rres.size();

    bool b = lres.size() == rres.size() && std::equal(std::begin(lres), std::end(lres), std::begin(rres), [] (auto const& l, auto const& r) { return l == r; });
    std::cout << " => " << b << '\n';
    return b;
}

void TestCase::lexer_test(std::vector<std::string> const& source, lexer::LexerResult const& expected) {
    TestResult res;
    res.test = "[";
    bool first = true;
    for(auto const& s : source) {
        if (!first)
            res.test += " ";
        first = false;

        res.test += "{" + s.substr(0, s.size() - 1) + "}";
    }
    res.test += "]";
    res.expect = format_lexer_result(expected);

    auto tokens_res = lexer::tokenize(source);
    if (!(expected == tokens_res)) {
        std::cerr << "Failed on " << (source.empty() ? "..." : source[0].c_str()) << '\n';
        res.but = format_lexer_result(tokens_res);
    }

    results.push_back(res);
}

/*
    [LEXER]
    # Token Plus
      OK    {+} {; comment}
        ~   [{0:0@1 "+" Plus}]
      FAIL  {++}
        ~   [{0:0@1 "+" Plus} {0:1@1 "+" Plus}]
        BUT []
            This should not be the case...
    
    # Token Minus
      OK    {-}
        ~   [{0:0@1 "-" Minus}]
      FAIL  {--}
        ~   [{0:0@1 "-" Minus} {0:1@1 "-" Minus}]
        BUT Error.Unknown error at 0:0@0 "-"
            This should not be the case...

*/


std::ostream& Test::report(std::ostream& os, bool use_color) {
    constexpr std::string_view color_red   = "\033[31m";
    constexpr std::string_view color_green = "\033[32m";
    constexpr std::string_view color_blue  = "\033[34m";
    constexpr std::string_view color_reset = "\033[0m";
    constexpr std::string_view color_bold  = "\033[1m";

    os << color_bold << "[" << name << "]\n" << color_reset;
    for(auto const& testcase : cases) {
        os << color_blue << "# " << testcase.testcase << '\n' << color_reset;

        for(auto const& res : testcase.results) {
            if (!res.but.empty())
                os << color_red << color_bold << "\tFAIL  ";
            else
                os << color_green << color_bold << "\tOK    ";

            os << color_reset << res.test << '\n';
            os << "\t  ~   " << res.expect << '\n';
            if (!res.but.empty())
                os << "\t  BUT " << res.but << '\n';
            if (!res.message.empty())
                os << "\t      " << res.message << '\n';
        }
    }
    return os;
}

}