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
#include <vcrate/vasm/parser/ast/String.hpp>
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
        return result_t::error(Error::Type::Internal, { token.location });
    }

    lexer::Location location;
    if (tokens.empty()) {
        location = {{0, 0}, 1};
    } else {
        auto const& last = tokens.back().location;
        location = {{last.line, last.character + last.lenght}, 1};
    }

    return result_t::error(Error::Type::Internal, { location });
}

Result<lexer::Token> eat_one_of(std::vector<lexer::Token> const& tokens, std::size_t& pos, std::initializer_list<lexer::Type> types) {
    using result_t = Result<lexer::Token>;

    for(auto t : types) {
        auto token = eat(tokens, pos, t);
        if (token.is_success())
            return std::move(token);
    }

    lexer::Location location;
    if (tokens.empty()) {
        location = {{0, 0}, 1};
    } else {
        auto const& last = tokens.back().location;
        location = {{last.line, last.character + last.lenght}, 1};
    }

    return result_t::error(Error::Type::Internal, { location });
}

Result<std::unique_ptr<Constant>> parse_constant_term(std::vector<lexer::Token> const& tokens, std::size_t& ref_pos);

Result<std::unique_ptr<Constant>> parse_constant(std::vector<lexer::Token> const& tokens, std::size_t& ref_pos, BinaryOperation::precedence_t pred = 0) {
    using result_t = Result<std::unique_ptr<Constant>>;

    auto lhs = parse_constant_term(tokens, ref_pos);
    if (lhs.is_error())
        return result_t::error(lhs.get_error());

    while(true) {
        std::size_t pos = ref_pos;
        auto token = eat_one_of(tokens, pos, {
            lexer::Type::Plus,
            lexer::Type::Minus,
            lexer::Type::Mult,
            lexer::Type::Div,
            lexer::Type::Mod,
            lexer::Type::ShiftL,
            lexer::Type::ShiftR,
            lexer::Type::RotateL,
            lexer::Type::RotateR,
            lexer::Type::Or,
            lexer::Type::OrLogic,
            lexer::Type::And,
            lexer::Type::AndLogic,
            lexer::Type::Xor,
            lexer::Type::Exp,
            lexer::Type::Leqt,
            lexer::Type::Lt,
            lexer::Type::Gt,
            lexer::Type::Geqt,
            lexer::Type::Eq,
            lexer::Type::Neq
        });
        if (token.is_success()) {
            BinaryOperation::Type op;
            switch (token.get_result().type) {
                case lexer::Type::Plus:     op = BinaryOperation::Type::Add; break;
                case lexer::Type::Minus:    op = BinaryOperation::Type::Sub; break;
                case lexer::Type::Mult:     op = BinaryOperation::Type::Mult; break;
                case lexer::Type::Div:      op = BinaryOperation::Type::Div; break;
                case lexer::Type::Mod:      op = BinaryOperation::Type::Mod; break;
                case lexer::Type::ShiftL:   op = BinaryOperation::Type::ShiftL; break;
                case lexer::Type::ShiftR:   op = BinaryOperation::Type::ShiftR; break;
                case lexer::Type::RotateL:  op = BinaryOperation::Type::RotateL; break;
                case lexer::Type::RotateR:  op = BinaryOperation::Type::RotateR; break;
                case lexer::Type::Or:       op = BinaryOperation::Type::Or; break;
                case lexer::Type::OrLogic:  op = BinaryOperation::Type::LogicalOr; break;
                case lexer::Type::And:      op = BinaryOperation::Type::And; break;
                case lexer::Type::AndLogic: op = BinaryOperation::Type::LogicalAnd; break;
                case lexer::Type::Xor:      op = BinaryOperation::Type::Xor; break;
                case lexer::Type::Exp:      op = BinaryOperation::Type::Exp; break;
                case lexer::Type::Leqt:     op = BinaryOperation::Type::LessEquals; break;
                case lexer::Type::Lt:       op = BinaryOperation::Type::Less; break;
                case lexer::Type::Gt:       op = BinaryOperation::Type::Greater; break;
                case lexer::Type::Geqt:     op = BinaryOperation::Type::GreaterEquals; break;
                case lexer::Type::Eq:       op = BinaryOperation::Type::Equals; break;
                case lexer::Type::Neq:      op = BinaryOperation::Type::Unequals; break;
                default: std::exit(666); break;
            }

            auto op_pred = BinaryOperation::precedence_of(op);
            if (op_pred < pred) {
                return std::move(lhs);

            } else if (op_pred > pred) {
                ref_pos = pos;
                auto rhs = parse_constant(tokens, ref_pos, op_pred + 1);
                if (rhs.is_error())
                    return result_t::error(rhs.get_error());
                
                auto bin = std::make_unique<BinaryOperation>(std::move(lhs.get_result()), op, std::move(rhs.get_result()));
                
                bin->locations.extends(bin->lhs->locations);
                bin->locations.extends(token.get_result().location);
                bin->locations.extends(bin->rhs->locations);

                lhs = result_t::success(std::move(bin));

            } else {
                ref_pos = pos;
                auto rhs = parse_constant_term(tokens, ref_pos);
                if (rhs.is_error())
                    return result_t::error(rhs.get_error());

                auto bin = std::make_unique<BinaryOperation>(std::move(lhs.get_result()), op, std::move(rhs.get_result()));
                
                bin->locations.extends(bin->lhs->locations);
                bin->locations.extends(token.get_result().location);
                bin->locations.extends(bin->rhs->locations);

                lhs = result_t::success(std::move(bin));
            }
        } else {
            return std::move(lhs);
        }
    }
}

Result<std::unique_ptr<Constant>> parse_constant_term(std::vector<lexer::Token> const& tokens, std::size_t& ref_pos) {
    using result_t = Result<std::unique_ptr<Constant>>;
    std::size_t pos = ref_pos;
    {
        auto token = eat_one_of(tokens, pos, {lexer::Type::Neg, lexer::Type::Minus, lexer::Type::Not});
        if (token.is_success()) {
            UnaryOperation::Type op;
            switch (token.get_result().type) {
                case lexer::Type::Neg:   op = UnaryOperation::Type::Not;        break;
                case lexer::Type::Not:   op = UnaryOperation::Type::LogicalNot; break;
                case lexer::Type::Minus: op = UnaryOperation::Type::Neg;        break;
                default: std::exit(666); break;
            }

            auto constant = parse_constant_term(tokens, pos);
            if (auto err = constant.get_if_error()) return result_t::error(*err);
            ref_pos = pos;

            auto res = std::make_unique<UnaryOperation>(op, std::move(constant.get_result()));
            res->locations.extends(res->value->locations);
            res->locations.extends(token.get_result().location);

            return result_t::success(std::move(res));
        }
    }
    {
        auto opening_bracket = eat(tokens, pos, lexer::Type::OpenBracket);
        if (opening_bracket.is_success()) {
            auto constant = parse_constant(tokens, pos);
            if (auto err = constant.get_if_error()) return result_t::error(*err);
            auto closing_bracket = eat(tokens, pos, lexer::Type::CloseBracket);
            if (auto err = closing_bracket.get_if_error()) return result_t::error(*err);
            ref_pos = pos;

            auto res = std::make_unique<DereferencedConstant>(std::move(constant.get_result()));
            res->locations.extends(opening_bracket.get_result().location);
            res->locations.extends(res->constant->locations);
            res->locations.extends(closing_bracket.get_result().location);

            return result_t::success(std::move(res));
        }
    }
    {
        auto opening_paren = eat(tokens, pos, lexer::Type::OpenParen);
        if (opening_paren.is_success()) {
            auto constant = parse_constant(tokens, pos);
            if (auto err = constant.get_if_error()) return result_t::error(*err);
            auto closing_paren = eat(tokens, pos, lexer::Type::CloseParen);
            if (auto err = closing_paren.get_if_error()) return result_t::error(*err);
            ref_pos = pos;

            constant.get_result()->locations.extends(opening_paren.get_result().location);
            constant.get_result()->locations.extends(closing_paren.get_result().location);

            return std::move(constant);
        }
    }
    {
        auto ident = eat(tokens, pos, lexer::Type::Ident);
        if (ident.is_success()) {
            ref_pos = pos;

            auto res = std::make_unique<Ident>(ident.get_result().content);
            res->locations.extends(ident.get_result().location);

            return result_t::success(std::move(res));
        }
    }
    {
        auto string = eat(tokens, pos, lexer::Type::String);
        if (string.is_success() && string.get_result().content.size() == 1) {
            ref_pos = pos;

            auto res = std::make_unique<String>(string.get_result().content);
            res->locations.extends(string.get_result().location);

            return result_t::success(std::move(res));
        }
    }
    {
        auto num = eat_one_of(tokens, pos, {lexer::Type::Bin, lexer::Type::Oct, lexer::Type::Dec, lexer::Type::Hex});
        if (num.is_success()) {
            ui8 base;
            switch (num.get_result().type) {
                case lexer::Type::Bin:   base = 2;  break;
                case lexer::Type::Oct:   base = 8;  break;
                case lexer::Type::Dec:   base = 10; break;
                case lexer::Type::Hex:   base = 16; break;
                default: std::exit(666); break;
            }
            ref_pos = pos;

            auto res = std::make_unique<Number>(num.get_result().content, base);
            res->locations.extends(num.get_result().location);

            return result_t::success(std::move(res));
        }
    }

    lexer::Location location;
    if (tokens.empty())
        location = {{0, 0}, 1};
    else
        location = tokens.back().location;

    return result_t::error(Error::Type::Internal, { location });
}

Result<std::pair<bytecode::Operations, lexer::Location>> parse_operation(std::vector<lexer::Token> const& tokens, std::size_t& pos) {
    using result_t = Result<std::pair<bytecode::Operations, lexer::Location>>;

    auto token = eat(tokens, pos, lexer::Type::Ident);
    if (auto err = token.get_if_error()) return result_t::error(*err);
    
    try {
        auto ope = bytecode::OpDefinition::get(token.get_result().content).ope;
        return result_t::success({ ope, token.get_result().location });
    } catch(std::out_of_range const&) {
        return result_t::error(Error::Type::Internal, { token.get_result().location });
    }
}

Result<std::pair<Register, lexer::Location>> parse_register(std::vector<lexer::Token> const& tokens, std::size_t& pos) {
    using result_t = Result<std::pair<Register, lexer::Location>>;

    auto reg = eat(tokens, pos, lexer::Type::Register);
    if (auto err = reg.get_if_error()) return result_t::error(*err);

    std::string_view name = reg.get_result().content;
    name.remove_prefix(1);

    if (name == "A") return result_t::success({ Register::Type::A, reg.get_result().location });
    if (name == "B") return result_t::success({ Register::Type::B, reg.get_result().location });
    if (name == "C") return result_t::success({ Register::Type::C, reg.get_result().location });
    if (name == "D") return result_t::success({ Register::Type::D, reg.get_result().location });
    if (name == "E") return result_t::success({ Register::Type::E, reg.get_result().location });
    if (name == "F") return result_t::success({ Register::Type::F, reg.get_result().location });
    if (name == "G") return result_t::success({ Register::Type::G, reg.get_result().location });
    if (name == "H") return result_t::success({ Register::Type::H, reg.get_result().location });
    if (name == "I") return result_t::success({ Register::Type::I, reg.get_result().location });
    if (name == "J") return result_t::success({ Register::Type::J, reg.get_result().location });
    if (name == "K") return result_t::success({ Register::Type::K, reg.get_result().location });
    if (name == "L") return result_t::success({ Register::Type::L, reg.get_result().location });
    if (name == "PC") return result_t::success({ Register::Type::PC, reg.get_result().location });
    if (name == "FG") return result_t::success({ Register::Type::FG, reg.get_result().location });
    if (name == "SP") return result_t::success({ Register::Type::SP, reg.get_result().location });
    if (name == "BP") return result_t::success({ Register::Type::BP, reg.get_result().location });

    return result_t::error(Error::Type::Internal, { reg.get_result().location });
}

Result<std::unique_ptr<Argument>> parse_argument(std::vector<lexer::Token> const& tokens, std::size_t& ref_pos) {
    using result_t = Result<std::unique_ptr<Argument>>;

    std::size_t pos = ref_pos;
    auto bracket_token = eat(tokens, pos, lexer::Type::OpenBracket);
    if (bracket_token.is_success()) {
        auto reg = parse_register(tokens, pos);
        if (reg.is_success()) {
            auto plus_token = eat(tokens, pos, lexer::Type::Plus);
            if (plus_token.is_error()) {

                auto closing_bracket = eat(tokens, pos, lexer::Type::CloseBracket);
                if (auto err = closing_bracket.get_if_error()) return result_t::error(*err);

                ref_pos = pos;
                
                auto res = std::make_unique<DereferencedArgument>(reg.get_result().first, nullptr);
                res->locations.extends(bracket_token.get_result().location);
                res->locations.extends(reg.get_result().second);
                res->locations.extends(closing_bracket.get_result().location);

                return result_t::success(std::move(res));

            } else {
                auto constant = parse_constant(tokens, pos, BinaryOperation::precedence_of(BinaryOperation::Type::Add));
                if (auto err = constant.get_if_error()) return result_t::error(*err);

                auto closing_bracket = eat(tokens, pos, lexer::Type::CloseBracket);
                if (auto err = closing_bracket.get_if_error()) return result_t::error(*err);

                ref_pos = pos;

                auto res = std::make_unique<DereferencedArgument>(reg.get_result().first, std::move(constant.get_result()));
                res->locations.extends(bracket_token.get_result().location);
                res->locations.extends(reg.get_result().second);
                res->locations.extends(plus_token.get_result().location);
                res->locations.extends(res->displacement->locations);
                res->locations.extends(closing_bracket.get_result().location);

                return result_t::success(std::move(res));
            }
        }
    }

    auto reg = parse_register(tokens, ref_pos);
    if (reg.is_success()) {
        auto res = std::make_unique<Register>(reg.get_result().first);
        res->locations.extends(reg.get_result().second);
        return result_t::success(std::move(res));
    }

    auto constant = parse_constant(tokens, ref_pos);
    if(constant.is_success()) {
        return result_t::success(std::move(constant.get_result()));
    }

    return result_t::error(constant.get_error());
}

Result<std::pair<std::vector<std::unique_ptr<Argument>>, lexer::ScatteredLocation>> parse_arguments(std::vector<lexer::Token> const& tokens, std::size_t& ref_pos) {
    using result_t = Result<std::pair<std::vector<std::unique_ptr<Argument>>, lexer::ScatteredLocation>>;
    std::size_t pos = ref_pos;

    std::vector<std::unique_ptr<Argument>> args;    
    lexer::ScatteredLocation locations;

    auto arg = parse_argument(tokens, pos);
    if (arg.is_error()) { ref_pos = pos; return result_t::success({ std::move(args), locations }); }
    args.emplace_back(std::move(arg.get_result()));
    locations.extends(args.back()->locations);

    auto comma = eat(tokens, pos, lexer::Type::Comma);
    while(comma.is_success()) {
        auto arg = parse_argument(tokens, pos);
        if (auto err = arg.get_if_error()) return result_t::error(*err);

        args.emplace_back(std::move(arg.get_result()));
        locations.extends(args.back()->locations);
        locations.extends(comma.get_result().location);
        comma = eat(tokens, pos, lexer::Type::Comma);
    }
    ref_pos = pos;
    return result_t::success({ std::move(args), locations });
}

Result<std::unique_ptr<Statement>> parse_command(std::vector<lexer::Token> const& tokens, std::size_t& pos) {
    using result_t = Result<std::unique_ptr<Statement>>;

    auto ope_res = parse_operation(tokens, pos);
    if (auto err = ope_res.get_if_error()) return result_t::error(*err);

    auto args = parse_arguments(tokens, pos);
    if (auto err = args.get_if_error()) return result_t::error(*err);

    auto res = std::make_unique<Command>(ope_res.get_result().first, std::move(args.get_result().first));
    res->locations.extends(args.get_result().second);
    res->locations.extends(ope_res.get_result().second);

    return result_t::success(std::move(res));
}

Result<std::unique_ptr<Statement>> parse_data(std::vector<lexer::Token> const& tokens, std::size_t& ref_pos) {
    using result_t = Result<std::unique_ptr<Statement>>;
    std::size_t pos = ref_pos;

    auto token = eat_one_of(tokens, pos, { lexer::Type::DB, lexer::Type::DD, lexer::Type::DW });
    if (auto err = token.get_if_error()) { return result_t::error(*err); }

    std::size_t size;
    switch (token.get_result().type) {
        case lexer::Type::DB:   size = 1; break;
        case lexer::Type::DW:   size = 2; break;
        case lexer::Type::DD:   size = 4; break;
        default: std::exit(666); break;
    }

    std::vector<std::unique_ptr<Constant>> constants;
    lexer::ScatteredLocation locations;
    locations.extends(token.get_result().location);

    {
        auto constant = parse_constant(tokens, pos);
        if (constant.is_success()) {
            constants.emplace_back(std::move(constant.get_result()));
        } else {
            auto string = eat(tokens, pos, lexer::Type::String);
            if (auto err = string.get_if_error()) { return result_t::error(*err); }
            constants.emplace_back(std::make_unique<String>(string.get_result().content))->locations.extends(string.get_result().location);
        }
        locations.extends(constants.back()->locations);
    }

    auto comma = eat(tokens, pos, lexer::Type::Comma);
    while(comma.is_success()) {
        auto constant = parse_constant(tokens, pos);
        if (constant.is_success()) {
            constants.emplace_back(std::move(constant.get_result()));
        } else {
            auto string = eat(tokens, pos, lexer::Type::String);
            if (auto err = string.get_if_error()) { return result_t::error(*err); }
            constants.emplace_back(std::make_unique<String>(string.get_result().content))->locations.extends(string.get_result().location);
        }
        locations.extends(constants.back()->locations);
        locations.extends(comma.get_result().location);

        comma = eat(tokens, pos, lexer::Type::Comma);
    }
    ref_pos = pos;

    auto res = std::make_unique<Data>(size, std::move(constants));
    res->locations = locations;

    return result_t::success(std::move(res));
}

Result<std::unique_ptr<Statement>> parse_label(std::vector<lexer::Token> const& tokens, std::size_t& pos) {
    using result_t = Result<std::unique_ptr<Statement>>;

    auto ident = eat(tokens, pos, lexer::Type::Ident);
    if (auto err = ident.get_if_error()) return result_t::error(*err);

    auto colon = eat(tokens, pos, lexer::Type::Colon);
    if (auto err = colon.get_if_error()) return result_t::error(*err);

    auto res = std::make_unique<Label>(ident.get_result().content);
    res->locations.extends(ident.get_result().location);
    res->locations.extends(colon.get_result().location);

    return result_t::success(std::move(res));
}

Result<std::unique_ptr<Statement>> parse_directive(std::vector<lexer::Token> const& tokens, std::size_t& pos) {
    using result_t = Result<std::unique_ptr<Statement>>;

    auto dir_token = eat(tokens, pos, lexer::Type::Directive);
    if (auto err = dir_token.get_if_error()) return result_t::error(*err);
   
    auto ident = eat(tokens, pos, lexer::Type::Ident);
    if (auto err = ident.get_if_error()) return result_t::error(*err);
    
    if (ident.get_result().content == "align") {
        auto num = eat_one_of(tokens, pos, {lexer::Type::Bin, lexer::Type::Oct, lexer::Type::Dec, lexer::Type::Hex});
        if (auto err = num.get_if_error()) return result_t::error(*err);

        ui8 base;
        switch (num.get_result().type) {
            case lexer::Type::Bin:   base = 2;  break;
            case lexer::Type::Oct:   base = 8;  break;
            case lexer::Type::Dec:   base = 10; break;
            case lexer::Type::Hex:   base = 16; break;
            default: std::exit(666); break;
        }
        auto res = std::make_unique<Align>(Number(num.get_result().content, base));

        res->number.locations.extends(num.get_result().location);

        res->locations.extends(dir_token.get_result().location);
        res->locations.extends(ident.get_result().location);
        res->locations.extends(res->number.locations);

        return result_t::success(std::move(res));
    }

    return result_t::error(Error::Type::Internal, { ident.get_result().location });
}

Result<std::unique_ptr<Statement>> parse_statement(std::vector<lexer::Token> const& tokens, std::size_t& ref_pos) {
    using result_t = Result<std::unique_ptr<Statement>>;

    {
        std::size_t pos = ref_pos;
        auto command = parse_command(tokens, pos);
        if(command.is_success()) { 
            ref_pos = pos;
            return std::move(command);
        }
    }
    {
        std::size_t pos = ref_pos;
        auto label = parse_label(tokens, pos);
        if(label.is_success()) { 
            ref_pos = pos;
            return std::move(label);
        }
    }
    {
        std::size_t pos = ref_pos;
        auto data = parse_data(tokens, pos);
        if(data.is_success()) { 
            ref_pos = pos;
            return std::move(data);
        }
    }
    {
        std::size_t pos = ref_pos;
        auto directive = parse_directive(tokens, pos);
        if(directive.is_success()) { 
            ref_pos = pos;
            return std::move(directive);
        }
    }

    lexer::Location location;
    if (ref_pos >= tokens.size()) {
        auto const& last = tokens.back().location;
        location = {{last.line, last.character + last.lenght}, 1};
    } else {
        location = tokens[ref_pos].location;
    }

    return result_t::error(Error::Type::Internal, { location });
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