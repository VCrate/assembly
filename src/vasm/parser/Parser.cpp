#include <vcrate/vasm/parser/Parser.hpp>

#include <vcrate/vasm/parser/ast/Align.hpp>
#include <vcrate/vasm/parser/ast/Argument.hpp>
#include <vcrate/vasm/parser/ast/BinaryOperation.hpp>
#include <vcrate/vasm/parser/ast/Command.hpp>
#include <vcrate/vasm/parser/ast/Constant.hpp>
#include <vcrate/vasm/parser/ast/Data.hpp>
#include <vcrate/vasm/parser/ast/DereferencedArgument.hpp>
#include <vcrate/vasm/parser/ast/DereferencedConstant.hpp>
#include <vcrate/vasm/parser/ast/Directive.hpp>
#include <vcrate/vasm/parser/ast/Ident.hpp>
#include <vcrate/vasm/parser/ast/Label.hpp>
#include <vcrate/vasm/parser/ast/Number.hpp>
#include <vcrate/vasm/parser/ast/Register.hpp>
#include <vcrate/vasm/parser/ast/Statement.hpp>
#include <vcrate/vasm/parser/ast/Term.hpp>
#include <vcrate/vasm/parser/ast/UnaryOperation.hpp>

namespace vcrate::vasm::parser {

Result<lexer::Token> eat(std::vector<lexer::Token> const& tokens, std::size_t& pos, lexer::Type type) {
    if (pos < tokens.size()) {
        auto const& token = tokens[pos];
        if (token.type == type) {
            ++pos;
            return Result<lexer::Token>::success(token);
        }
        return Result<lexer::Token>::error( {Error::Type::Internal, token.location });
    }

    lexer::Location location;
    if (tokens.empty())
        location = {{0, 0}, 1};
    else {
        auto const& last = tokens.back().location;
        location = {{last.line, last.character + last.lenght}, 1};
    }

    return Result<lexer::Token>::error({ Error::Type::Internal, location });
}

Result<bytecode::Operations> parse_operation(std::vector<lexer::Token> const& tokens, std::size_t& pos) {
    auto token = eat(tokens, pos, lexer::Type::Ident);
    if (auto err = token.get_if_error()) return Result<bytecode::Operations>::error(*err);
    
    if (token.get_result().content == "mov")
        return Result<bytecode::Operations>::success(bytecode::Operations::MOV);

    return Result<bytecode::Operations>::error({ Error::Type::Internal, token.get_result().location });
}

Result<std::unique_ptr<Statement>> parse_command(std::vector<lexer::Token> const& tokens, std::size_t& pos) {
    using Res = Result<std::unique_ptr<Statement>>;

    auto ope = parse_operation(tokens, pos);
    if (ope.is_error()) return Res::error(ope.get_error());
/*
    auto args = parse_arguments(tokens, pos);
    if (args.is_error()) return Res::error(ope.get_error());
*/
    std::vector<std::unique_ptr<Argument>> args = {};
    return Result<std::unique_ptr<Statement>>::success(std::make_unique<Command>(ope.get_result(), std::move(args/*.get_result()*/)));
}

Result<std::unique_ptr<Statement>> parse_statement(std::vector<lexer::Token> const& tokens, std::size_t& pos) {
    return std::move(parse_command(tokens, pos));
}

ParserResult parse(std::vector<lexer::Token> const& tokens) {
    std::vector<std::unique_ptr<Statement>> statements;
    std::size_t pos{0};
    while(pos < tokens.size()) {
        auto statement = parse_statement(tokens, pos);
        if (statement.is_error())
            return ParserResult::error(statement.get_error());
        statements.emplace_back(std::move(statement.get_result()));

        if (pos >= tokens.size())
            break;

        auto nl = eat(tokens, pos, lexer::Type::NewLine);
        if (auto err = nl.get_if_error()) return ParserResult::error(*err);

        while(eat(tokens, pos, lexer::Type::NewLine).is_success());
    }
    return ParserResult::success(std::move(statements));
}

}