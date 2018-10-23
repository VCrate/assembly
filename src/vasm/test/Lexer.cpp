#include <vcrate/vasm/test/Test.hpp>

#include <sstream>

namespace {

using namespace vcrate::vasm;
namespace lx = lexer;
using namespace test;

std::vector<std::string> as_source(std::string const& str) {
    std::stringstream ss(str);
    std::vector<std::string> source;
    for(std::string line; std::getline(ss, line);)
        source.emplace_back(line + '\n');
    return source;
}

TEST(LEXER) {
    CASE("General");
    LEXER_TEST({}, lx::LexerResult::success({
        {{{0, 1}, 1}, "\n", lx::Type::NewLine}
    }));
    LEXER_TEST(as_source(""), lx::LexerResult::success({
        {{{0, 1}, 1}, "\n", lx::Type::NewLine}
    }));
    LEXER_TEST(as_source(" "), lx::LexerResult::success({
        {{{0, 1}, 1}, "\n", lx::Type::NewLine}
    }));

    CASE("Token Plus");
    LEXER_TEST(as_source("+"), lexer::LexerResult::success({
        {{{0, 0}, 1}, "+", lexer::Type::Plus},
        {{{0, 1}, 1}, "\n", lx::Type::NewLine}
    }));

    CASE("Token Minus");
    LEXER_TEST(as_source("-"), lexer::LexerResult::success({
        {{{0, 0}, 1}, "-", lexer::Type::Minus},
        {{{0, 1}, 1}, "\n", lx::Type::NewLine}
    }));
}

}

int main() {
    REPORT_TEST(LEXER);
}
