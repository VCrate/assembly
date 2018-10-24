#pragma once

#include <vcrate/vasm/lexer/Lexer.hpp>

#include <string>
#include <vector>

namespace vcrate::vasm::test {

struct TestResult {
    std::string test;
    std::string expect;
    std::string but;
    std::string message;
};

class TestCase {
public:
    TestCase(std::string const& testcase);

    void lexer_test(std::vector<std::string> const& source, lexer::LexerResult const& res);

    bool done() const;

    std::string testcase;

    bool is_done{false};
    std::vector<TestResult> results;

};

struct Test {
    std::string name;
    std::vector<TestCase> cases;

    std::ostream& report(std::ostream& os, bool use_color = true);
};

#define TEST(__name) \
    void test_function_ ## __name(Test& __test); \
    static Test test_ ## __name { # __name, {} }; \
    void test_function_ ## __name(Test& __test = test_ ## __name)

#define CASE(__name) \
    __test.cases.emplace_back(__name)

#define LEXER_TEST \
    __test.cases.back().lexer_test

#define RUN_TEST(__name) \
    test_function_ ## __name(test_ ## __name)

#define REPORT_TEST(__name) \
    (RUN_TEST(__name), test_ ## __name .report(std::cerr))

}