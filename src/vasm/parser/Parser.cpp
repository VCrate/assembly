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
    using result_t = Result<lexer::Token>;
    if (pos < tokens.size()) {
        auto const& token = tokens[pos];
        if (token.type == type) {
            ++pos;
            return result_t::success(token);
        }
        return result_t::error( {Error::Type::Internal, token.location });
    }

    lexer::Location location;
    if (tokens.empty()) {
        location = {{0, 0}, 1};
    } else {
        auto const& last = tokens.back().location;
        location = {{last.line, last.character + last.lenght}, 1};
    }

    return result_t::error({ Error::Type::Internal, location });
}

Result<bytecode::Operations> parse_operation(std::vector<lexer::Token> const& tokens, std::size_t& pos) {
    using result_t = Result<bytecode::Operations>;

    auto token = eat(tokens, pos, lexer::Type::Ident);
    if (auto err = token.get_if_error()) return result_t::error(*err);
    
    if (token.get_result().content == "mov")
        return result_t::success(bytecode::Operations::MOV);

    return result_t::error({ Error::Type::Internal, token.get_result().location });
}

Result<Register> parse_register(std::vector<lexer::Token> const& tokens, std::size_t& pos) {
    using result_t = Result<Register>;

    auto reg = eat(tokens, pos, lexer::Type::Register);
    if (auto err = reg.get_if_error()) return result_t::error(*err);

    std::string_view name = reg.get_result().content;
    name.remove_prefix(1);

    if (name == "A") return result_t::success({ Register::Type::A });
    if (name == "B") return result_t::success({ Register::Type::B });
    if (name == "C") return result_t::success({ Register::Type::C });
    if (name == "D") return result_t::success({ Register::Type::D });
    if (name == "E") return result_t::success({ Register::Type::E });
    if (name == "F") return result_t::success({ Register::Type::F });
    if (name == "G") return result_t::success({ Register::Type::G });
    if (name == "H") return result_t::success({ Register::Type::H });
    if (name == "I") return result_t::success({ Register::Type::I });
    if (name == "J") return result_t::success({ Register::Type::J });
    if (name == "K") return result_t::success({ Register::Type::K });
    if (name == "L") return result_t::success({ Register::Type::L });
    if (name == "PC") return result_t::success({ Register::Type::PC });
    if (name == "FG") return result_t::success({ Register::Type::FG });
    if (name == "SP") return result_t::success({ Register::Type::SP });
    if (name == "BP") return result_t::success({ Register::Type::BP });

    return result_t::error({ Error::Type::Internal, reg.get_result().location });
}

Result<std::unique_ptr<Argument>> parse_argument(std::vector<lexer::Token> const& tokens, std::size_t& pos) {
    using result_t = Result<std::unique_ptr<Argument>>;

    auto reg = parse_register(tokens, pos);
    if (auto err = reg.get_if_error()) return result_t::error(*err);

    return result_t::success(std::make_unique<Register>(reg.get_result()));
}

Result<std::vector<std::unique_ptr<Argument>>> parse_arguments(std::vector<lexer::Token> const& tokens, std::size_t& pos) {
    using result_t = Result<std::vector<std::unique_ptr<Argument>>>;

    std::vector<std::unique_ptr<Argument>> args;    

    auto arg = parse_argument(tokens, pos);
    if (arg.is_error()) return result_t::success(std::move(args));
    args.emplace_back(std::move(arg.get_result()));

    while(eat(tokens, pos, lexer::Type::Comma).is_success()) {
        auto arg = parse_argument(tokens, pos);
        if (auto err = arg.get_if_error()) return result_t::error(*err);
        args.emplace_back(std::move(arg.get_result()));
    }
    
    return result_t::success(std::move(args));
}

Result<std::unique_ptr<Statement>> parse_command(std::vector<lexer::Token> const& tokens, std::size_t& pos) {
    using result_t = Result<std::unique_ptr<Statement>>;

    auto ope = parse_operation(tokens, pos);
    if (auto err = ope.get_if_error()) return result_t::error(*err);

    auto args = parse_arguments(tokens, pos);
    if (auto err = args.get_if_error()) return result_t::error(*err);

    return result_t::success(std::make_unique<Command>(ope.get_result(), std::move(args.get_result())));
}

Result<std::unique_ptr<Statement>> parse_statement(std::vector<lexer::Token> const& tokens, std::size_t& pos) {
    return std::move(parse_command(tokens, pos));
}

ParserResult parse(std::vector<lexer::Token> const& tokens) {
    using result_t = ParserResult;

    std::vector<std::unique_ptr<Statement>> statements;
    std::size_t pos{0};
    while(pos < tokens.size()) {
        auto statement = parse_statement(tokens, pos);
        if (statement.is_error())
            return result_t::error(statement.get_error());
        statements.emplace_back(std::move(statement.get_result()));

        if (pos >= tokens.size())
            break;

        auto nl = eat(tokens, pos, lexer::Type::NewLine);
        if (auto err = nl.get_if_error()) return result_t::error(*err);

        while(eat(tokens, pos, lexer::Type::NewLine).is_success());
    }
    return result_t::success(std::move(statements));
}

}