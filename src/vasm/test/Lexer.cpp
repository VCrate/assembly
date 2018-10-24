#include <vcrate/vasm/test/Test.hpp>

#include <sstream>

namespace {

using namespace vcrate::vasm;
namespace lx = lexer;
using namespace test;

std::vector<std::string> as_source(std::string const& str) {
    if (str.empty())
        return {"\n"};
    std::stringstream ss(str);
    std::vector<std::string> source;
    for(std::string line; std::getline(ss, line);)
        source.emplace_back(line + '\n');
    return source;
}

#define TOKEN(__line, __char, __len, __cont, __ty) ::vcrate::vasm::lexer::Token {{{__line, __char}, __len}, __cont, ::vcrate::vasm::lexer::Type::__ty}

TEST(LEXER) {
    CASE("General");
    LEXER_TEST({}, lx::LexerResult::success({}));
    LEXER_TEST(as_source(""), lx::LexerResult::success({
        TOKEN(0, 0, 1, "\n", NewLine)
    }));
    LEXER_TEST(as_source("\n\n"), lx::LexerResult::success({
        TOKEN(0, 0, 1, "\n", NewLine)
    }));
    LEXER_TEST(as_source("\n\n "), lx::LexerResult::success({
        TOKEN(0, 0, 1, "\n", NewLine)
    }));
    LEXER_TEST(as_source("\n \n "), lx::LexerResult::success({
        TOKEN(0, 0, 1, "\n", NewLine)
    }));
    LEXER_TEST(as_source(" \n \n "), lx::LexerResult::success({
        TOKEN(0, 1, 1, "\n", NewLine)
    }));
    LEXER_TEST(as_source(" "), lx::LexerResult::success({
        TOKEN(0, 1, 1, "\n", NewLine)
    }));

    CASE("Token Colon");
    LEXER_TEST(as_source(":"), lexer::LexerResult::success({
        TOKEN(0, 0, 1, ":", Colon),
        TOKEN(0, 1, 1, "\n", NewLine)
    }));

    CASE("Token Comma");
    LEXER_TEST(as_source(","), lexer::LexerResult::success({
        TOKEN(0, 0, 1, ",", Comma),
        TOKEN(0, 1, 1, "\n", NewLine)
    }));

    CASE("Token Register");
    LEXER_TEST(as_source("%B"), lexer::LexerResult::success({
        TOKEN(0, 0, 2, "%B", Register),
        TOKEN(0, 2, 1, "\n", NewLine)
    }));

    CASE("Token Ident");
    LEXER_TEST(as_source("test"), lexer::LexerResult::success({
        TOKEN(0, 0, 4, "test", Ident),
        TOKEN(0, 4, 1, "\n", NewLine)
    }));

    CASE("Token Hex");
    LEXER_TEST(as_source("0xA5"), lexer::LexerResult::success({
        TOKEN(0, 0, 4, "A5", Hex),
        TOKEN(0, 4, 1, "\n", NewLine)
    }));

    CASE("Token Oct");
    LEXER_TEST(as_source("0o70"), lexer::LexerResult::success({
        TOKEN(0, 0, 4, "70", Oct),
        TOKEN(0, 4, 1, "\n", NewLine)
    }));

    CASE("Token Dec");
    LEXER_TEST(as_source("0d92"), lexer::LexerResult::success({
        TOKEN(0, 0, 4, "92", Dec),
        TOKEN(0, 4, 1, "\n", NewLine)
    }));
    LEXER_TEST(as_source("92"), lexer::LexerResult::success({
        TOKEN(0, 0, 2, "92", Dec),
        TOKEN(0, 2, 1, "\n", NewLine)
    }));

    CASE("Token Bin");
    LEXER_TEST(as_source("0b10"), lexer::LexerResult::success({
        TOKEN(0, 0, 4, "10", Bin),
        TOKEN(0, 4, 1, "\n", NewLine)
    }));

    CASE("Token String");
    LEXER_TEST(as_source("\"string test\""), lexer::LexerResult::success({
        TOKEN(0, 0, 13, "string test", String),
        TOKEN(0, 13, 1, "\n", NewLine)
    }));

    CASE("Token Directive");
    LEXER_TEST(as_source("#"), lexer::LexerResult::success({
        TOKEN(0, 0, 1, "#", Directive),
        TOKEN(0, 1, 1, "\n", NewLine)
    }));

    CASE("Token DW");
    LEXER_TEST(as_source("dw"), lexer::LexerResult::success({
        TOKEN(0, 0, 2, "dw", DW),
        TOKEN(0, 2, 1, "\n", NewLine)
    }));

    CASE("Token DD");
    LEXER_TEST(as_source("DD"), lexer::LexerResult::success({
        TOKEN(0, 0, 2, "DD", DD),
        TOKEN(0, 2, 1, "\n", NewLine)
    }));

    CASE("Token DB");
    LEXER_TEST(as_source("Db"), lexer::LexerResult::success({
        TOKEN(0, 0, 2, "Db", DB),
        TOKEN(0, 2, 1, "\n", NewLine)
    }));

    CASE("Token Plus");
    LEXER_TEST(as_source("+"), lexer::LexerResult::success({
        TOKEN(0, 0, 1, "+", Plus),
        TOKEN(0, 1, 1, "\n", NewLine)
    }));

    CASE("Token Minus");
    LEXER_TEST(as_source("-"), lexer::LexerResult::success({
        TOKEN(0, 0, 1, "-", Minus),
        TOKEN(0, 1, 1, "\n", NewLine)
    }));

    CASE("Token Div");
    LEXER_TEST(as_source("/"), lexer::LexerResult::success({
        TOKEN(0, 0, 1, "/", Div),
        TOKEN(0, 1, 1, "\n", NewLine)
    }));

    CASE("Token Mult");
    LEXER_TEST(as_source("*"), lexer::LexerResult::success({
        TOKEN(0, 0, 1, "*", Mult),
        TOKEN(0, 1, 1, "\n", NewLine)
    }));

    CASE("Token Mod");
    LEXER_TEST(as_source("%"), lexer::LexerResult::success({
        TOKEN(0, 0, 1, "%", Mod),
        TOKEN(0, 1, 1, "\n", NewLine)
    }));

    CASE("Token Exp");
    LEXER_TEST(as_source("**"), lexer::LexerResult::success({
        TOKEN(0, 0, 2, "**", Exp),
        TOKEN(0, 2, 1, "\n", NewLine)
    }));

    CASE("Token ShiftL");
    LEXER_TEST(as_source("<<"), lexer::LexerResult::success({
        TOKEN(0, 0, 2, "<<", ShiftL),
        TOKEN(0, 2, 1, "\n", NewLine)
    }));

    CASE("Token ShiftR");
    LEXER_TEST(as_source(">>"), lexer::LexerResult::success({
        TOKEN(0, 0, 2, ">>", ShiftR),
        TOKEN(0, 2, 1, "\n", NewLine)
    }));

    CASE("Token RotateL");
    LEXER_TEST(as_source("<<<"), lexer::LexerResult::success({
        TOKEN(0, 0, 3, "<<<", RotateL),
        TOKEN(0, 3, 1, "\n", NewLine)
    }));

    CASE("Token RotateR");
    LEXER_TEST(as_source(">>>"), lexer::LexerResult::success({
        TOKEN(0, 0, 3, ">>>", RotateR),
        TOKEN(0, 3, 1, "\n", NewLine)
    }));

    CASE("Token Or");
    LEXER_TEST(as_source("|"), lexer::LexerResult::success({
        TOKEN(0, 0, 1, "|", Or),
        TOKEN(0, 1, 1, "\n", NewLine)
    }));

    CASE("Token And");
    LEXER_TEST(as_source("&"), lexer::LexerResult::success({
        TOKEN(0, 0, 1, "&", And),
        TOKEN(0, 1, 1, "\n", NewLine)
    }));

    CASE("Token Xor");
    LEXER_TEST(as_source("^"), lexer::LexerResult::success({
        TOKEN(0, 0, 1, "^", Xor),
        TOKEN(0, 1, 1, "\n", NewLine)
    }));

    CASE("Token Neg");
    LEXER_TEST(as_source("~"), lexer::LexerResult::success({
        TOKEN(0, 0, 1, "~", Neg),
        TOKEN(0, 1, 1, "\n", NewLine)
    }));

    CASE("Token Not");
    LEXER_TEST(as_source("!"), lexer::LexerResult::success({
        TOKEN(0, 0, 1, "!", Not),
        TOKEN(0, 1, 1, "\n", NewLine)
    }));

    CASE("Token OrLogic");
    LEXER_TEST(as_source("||"), lexer::LexerResult::success({
        TOKEN(0, 0, 2, "||", OrLogic),
        TOKEN(0, 2, 1, "\n", NewLine)
    }));

    CASE("Token AndLogic");
    LEXER_TEST(as_source("&&"), lexer::LexerResult::success({
        TOKEN(0, 0, 2, "&&", AndLogic),
        TOKEN(0, 2, 1, "\n", NewLine)
    }));

    CASE("Token Leqt");
    LEXER_TEST(as_source("<="), lexer::LexerResult::success({
        TOKEN(0, 0, 2, "<=", Leqt),
        TOKEN(0, 2, 1, "\n", NewLine)
    }));

    CASE("Token Geqt");
    LEXER_TEST(as_source(">="), lexer::LexerResult::success({
        TOKEN(0, 0, 2, ">=", Geqt),
        TOKEN(0, 2, 1, "\n", NewLine)
    }));

    CASE("Token Neq");
    LEXER_TEST(as_source("!="), lexer::LexerResult::success({
        TOKEN(0, 0, 2, "!=", Neq),
        TOKEN(0, 2, 1, "\n", NewLine)
    }));

    CASE("Token Eq");
    LEXER_TEST(as_source("=="), lexer::LexerResult::success({
        TOKEN(0, 0, 2, "==", Eq),
        TOKEN(0, 2, 1, "\n", NewLine)
    }));

    CASE("Token Lt");
    LEXER_TEST(as_source("<"), lexer::LexerResult::success({
        TOKEN(0, 0, 1, "<", Lt),
        TOKEN(0, 1, 1, "\n", NewLine)
    }));

    CASE("Token Gt");
    LEXER_TEST(as_source(">"), lexer::LexerResult::success({
        TOKEN(0, 0, 1, ">", Gt),
        TOKEN(0, 1, 1, "\n", NewLine)
    }));

    CASE("Token CloseBracket");
    LEXER_TEST(as_source(")"), lexer::LexerResult::success({
        TOKEN(0, 0, 1, ")", CloseBracket),
        TOKEN(0, 1, 1, "\n", NewLine)
    }));

    CASE("Token OpenBracket");
    LEXER_TEST(as_source("("), lexer::LexerResult::success({
        TOKEN(0, 0, 1, "(", OpenBracket),
        TOKEN(0, 1, 1, "\n", NewLine)
    }));

    CASE("Token Comment");
    LEXER_TEST(as_source("; comment"), lexer::LexerResult::success({
        TOKEN(0, 9, 1, "\n", NewLine)
    }));

}

}

int main() {
    REPORT_TEST(LEXER);
}
