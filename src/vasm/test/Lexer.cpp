#include <vcrate/vasm/test/Test.hpp>

namespace {

using namespace vcrate::vasm;
namespace lx = lexer;
using namespace test;

TEST(LEXER) {
    CASE("General");
    LEXER_TEST({}, lx::LexerResult::success({
        {{{0, 1}, 1}, "\n", lx::Type::NewLine}
    }));
    LEXER_TEST({" "}, lx::LexerResult::success({
        {{{0, 1}, 1}, "\n", lx::Type::NewLine}
    }));

    CASE("Token Plus");
    LEXER_TEST({"+"}, lexer::LexerResult::success({
        {{{0, 0}, 1}, "+", lexer::Type::Plus},
        {{{0, 1}, 1}, "\n", lx::Type::NewLine}
    }));
}

}

int main() {
    REPORT_TEST(LEXER);
}
