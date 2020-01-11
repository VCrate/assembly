#include <vcrate/vasm/parser/Parser.hpp>

namespace vcrate::vasm::parser {

template<typename T>
struct WithLocation {
    lexer::ScatteredLocation locations;
    T data;
};

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

Result<Term> parse_sub_term(std::vector<lexer::Token> const& tokens, std::size_t& ref_pos);

Result<Term> parse_term(std::vector<lexer::Token> const& tokens, std::size_t& ref_pos, precedence_t pred = 0) {
    using result_t = Result<Term>;


    auto lhs_res = parse_sub_term(tokens, ref_pos);
    if (auto* err = lhs_res.get_if_error()) {
        return result_t::error(std::move(*err));
    }

    auto& lhs = lhs_res.get_result();

    while(true) {
        std::size_t pos = ref_pos;
        auto token_res = eat_one_of(tokens, pos, {
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
        if (auto* token = token_res.get_if_result()) {
            BinaryRelation::Type op;
            switch (token->type) {
                case lexer::Type::Plus:     op = BinaryRelation::Addition; break;
                case lexer::Type::Minus:    op = BinaryRelation::Subtraction; break;
                case lexer::Type::Mult:     op = BinaryRelation::Multiplication; break;
                case lexer::Type::Div:      op = BinaryRelation::Division; break;
                case lexer::Type::Mod:      op = BinaryRelation::Modulo; break;
                case lexer::Type::ShiftL:   op = BinaryRelation::ShiftLeft; break;
                case lexer::Type::ShiftR:   op = BinaryRelation::ShiftRight; break;
                case lexer::Type::RotateL:  op = BinaryRelation::RotateLeft; break;
                case lexer::Type::RotateR:  op = BinaryRelation::RotateRight; break;
                case lexer::Type::Or:       op = BinaryRelation::Or; break;
                case lexer::Type::OrLogic:  op = BinaryRelation::LogicalOr; break;
                case lexer::Type::And:      op = BinaryRelation::And; break;
                case lexer::Type::AndLogic: op = BinaryRelation::LogicalAnd; break;
                case lexer::Type::Xor:      op = BinaryRelation::Xor; break;
                case lexer::Type::Exp:      op = BinaryRelation::Exp; break;
                case lexer::Type::Leqt:     op = BinaryRelation::LessEquals; break;
                case lexer::Type::Lt:       op = BinaryRelation::Less; break;
                case lexer::Type::Gt:       op = BinaryRelation::Greater; break;
                case lexer::Type::Geqt:     op = BinaryRelation::GreaterEquals; break;
                case lexer::Type::Eq:       op = BinaryRelation::Equals; break;
                case lexer::Type::Neq:      op = BinaryRelation::Unequals; break;
                default: std::exit(666); break;
            }

            auto op_pred = precedence_of(op);
            if (op_pred < pred) {
                return std::move(lhs_res);

            } else if (op_pred > pred) {
                ref_pos = pos;
                auto rhs_res = parse_term(tokens, ref_pos, op_pred + 1);
                if (auto* err = rhs_res.get_if_error())
                    return result_t::error(std::move(*err));

                auto& rhs = rhs_res.get_result();
                
                lexer::ScatteredLocation locations = lhs.locations;
                locations.extends(rhs.locations);
                locations.extends(token->location);

                lhs = Term{
                    std::move(locations),
                    BinaryRelation{
                        op,
                        std::make_unique<Term>(std::move(lhs)),
                        std::make_unique<Term>(std::move(rhs))
                    }
                };

            } else {
                ref_pos = pos;
                auto rhs_res = parse_sub_term(tokens, ref_pos);
                if (auto* err = rhs_res.get_if_error())
                    return result_t::error(std::move(*err));

                auto& rhs = rhs_res.get_result();
                
                lexer::ScatteredLocation locations = lhs.locations;
                locations.extends(rhs.locations);
                locations.extends(token->location);

                lhs = Term{
                    std::move(locations),
                    BinaryRelation{
                        op,
                        std::make_unique<Term>(std::move(lhs)),
                        std::make_unique<Term>(std::move(rhs))
                    }
                };
            }
        } else {
            return std::move(lhs_res);
        }
    }
}

Result<Term> parse_sub_term(std::vector<lexer::Token> const& tokens, std::size_t& ref_pos) {
    using result_t = Result<Term>;
    std::size_t pos = ref_pos;
    {
        auto token_res = eat_one_of(tokens, pos, {lexer::Type::Neg, lexer::Type::Minus, lexer::Type::Not});
        if (auto* token = token_res.get_if_result()) {
            UnaryRelation::Type op;
            switch (token->type) {
                case lexer::Type::Neg:   op = UnaryRelation::Complement; break;
                case lexer::Type::Not:   op = UnaryRelation::Negation; break;
                case lexer::Type::Minus: op = UnaryRelation::Opposite; break;
                default: std::exit(666); break;
            }

            auto term_res = parse_sub_term(tokens, pos);
            if (auto err = term_res.get_if_error()) return result_t::error(*err);
            ref_pos = pos;

            auto& term = term_res.get_result();

            Term res{
                term.locations,
                UnaryRelation{
                    op,
                    std::make_unique<Term>(std::move(term))
                }
            };

            res.locations.extends(token->location);

            return result_t::success(std::move(res));
        }
    }
    {
        auto special_symbol_res = eat(tokens, pos, lexer::Type::Directive);
        if (auto* special_symbol = special_symbol_res.get_if_result()) {
            auto opening_bracket = eat(tokens, pos, lexer::Type::OpenBracket);
            if (auto err = opening_bracket.get_if_error()) return result_t::error(*err);

            auto term_res = parse_term(tokens, pos);
            if (auto err = term_res.get_if_error()) return result_t::error(*err);
            auto& term = term_res.get_result();

            auto closing_bracket = eat(tokens, pos, lexer::Type::CloseBracket);
            if (auto err = closing_bracket.get_if_error()) return result_t::error(*err);
            ref_pos = pos;

            Term res{
                term.locations,
                Dereferenced{
                    std::make_unique<Term>(std::move(term))
                }
            };

            res.locations.extends(special_symbol->location);
            res.locations.extends(opening_bracket.get_result().location);
            res.locations.extends(closing_bracket.get_result().location);

            return result_t::success(std::move(res));
        }
    }
    {
        auto opening_paren_res = eat(tokens, pos, lexer::Type::OpenParen);
        if (auto* opening_paren = opening_paren_res.get_if_result()) {
            auto term_res = parse_term(tokens, pos);
            if (auto err = term_res.get_if_error()) return result_t::error(*err);
            auto& term = term_res.get_result();

            auto closing_paren = eat(tokens, pos, lexer::Type::CloseParen);
            if (auto err = closing_paren.get_if_error()) return result_t::error(*err);
            ref_pos = pos;

            term.locations.extends(opening_paren->location);
            term.locations.extends(closing_paren.get_result().location);

            return result_t::success(std::move(term));
        }
    }
    {
        auto ident_res = eat(tokens, pos, lexer::Type::Ident);
        if (auto* ident = ident_res.get_if_result()) {
            ref_pos = pos;

            return result_t::success(Term{
                lexer::ScatteredLocation{{ std::move(ident->location) }},
                ident_t{ std::move(ident->content) }
            });
        }
    }
    {
        auto string_res = eat(tokens, pos, lexer::Type::String);
        if (auto* string = string_res.get_if_result(); string && string->content.size() == 1) {
            ref_pos = pos;

            return result_t::success(Term{
                lexer::ScatteredLocation{{ std::move(string->location) }},
                static_cast<integer_t>(string->content.front())
            });
        }
    }
    {
        auto num_res = eat_one_of(tokens, pos, { lexer::Type::Bin, lexer::Type::Oct, lexer::Type::Dec, lexer::Type::Hex, lexer::Type::Float });
        if (auto* num = num_res.get_if_result()) {
            ui8 base;
            switch (num->type) {
                case lexer::Type::Bin:   base = 2;  break;
                case lexer::Type::Oct:   base = 8;  break;
                case lexer::Type::Dec:   base = 10; break;
                case lexer::Type::Hex:   base = 16; break;
                case lexer::Type::Float: {
                    return result_t::success(Term{
                        lexer::ScatteredLocation{{ std::move(num->location) }},
                        float_t{ std::stof(num->content) }
                    });
                }
                default: std::exit(666); break;
            }
            ref_pos = pos;

            return result_t::success(Term{
                lexer::ScatteredLocation{{ std::move(num->location) }},
                static_cast<integer_t>(std::stoi(num->content, nullptr, base))
            });
        }
    }

    lexer::Location location;
    if (tokens.empty())
        location = {{0, 0}, 1};
    else
        location = tokens.back().location;

    return result_t::error(Error::Type::Internal, { location });
}

Result<WithLocation<bytecode::Operations>> parse_operation(std::vector<lexer::Token> const& tokens, std::size_t& pos) {
    using result_t = Result<WithLocation<bytecode::Operations>>;

    auto token_res = eat(tokens, pos, lexer::Type::Ident);
    if (auto err = token_res.get_if_error()) return result_t::error(*err);
    
    try {
        auto& token = token_res.get_result();
        auto ope = bytecode::OpDefinition::get(token.content).ope;
        return result_t::success({ lexer::ScatteredLocation{{ token.location }}, ope });
    } catch(std::out_of_range const&) {
        auto& token = token_res.get_result();
        return result_t::error(Error::Type::Internal, { token.location });
    }
}

Result<Register> parse_register(std::vector<lexer::Token> const& tokens, std::size_t& pos) {
    using result_t = Result<Register>;

    auto reg_res = eat(tokens, pos, lexer::Type::Register);
    if (auto err = reg_res.get_if_error()) return result_t::error(*err);

    auto& reg = reg_res.get_result();

    std::string_view name = reg.content;
    name.remove_prefix(1);

    if (name == "A" ) return result_t::success({ lexer::ScatteredLocation{{ reg.location }}, instruction::Register::A  });
    if (name == "B" ) return result_t::success({ lexer::ScatteredLocation{{ reg.location }}, instruction::Register::B  });
    if (name == "C" ) return result_t::success({ lexer::ScatteredLocation{{ reg.location }}, instruction::Register::C  });
    if (name == "D" ) return result_t::success({ lexer::ScatteredLocation{{ reg.location }}, instruction::Register::D  });
    if (name == "E" ) return result_t::success({ lexer::ScatteredLocation{{ reg.location }}, instruction::Register::E  });
    if (name == "F" ) return result_t::success({ lexer::ScatteredLocation{{ reg.location }}, instruction::Register::F  });
    if (name == "G" ) return result_t::success({ lexer::ScatteredLocation{{ reg.location }}, instruction::Register::G  });
    if (name == "H" ) return result_t::success({ lexer::ScatteredLocation{{ reg.location }}, instruction::Register::H  });
    if (name == "I" ) return result_t::success({ lexer::ScatteredLocation{{ reg.location }}, instruction::Register::I  });
    if (name == "J" ) return result_t::success({ lexer::ScatteredLocation{{ reg.location }}, instruction::Register::J  });
    if (name == "K" ) return result_t::success({ lexer::ScatteredLocation{{ reg.location }}, instruction::Register::K  });
    if (name == "L" ) return result_t::success({ lexer::ScatteredLocation{{ reg.location }}, instruction::Register::L  });
    if (name == "PC") return result_t::success({ lexer::ScatteredLocation{{ reg.location }}, instruction::Register::PC });
    if (name == "FG") return result_t::success({ lexer::ScatteredLocation{{ reg.location }}, instruction::Register::FG });
    if (name == "SP") return result_t::success({ lexer::ScatteredLocation{{ reg.location }}, instruction::Register::SP });
    if (name == "BP") return result_t::success({ lexer::ScatteredLocation{{ reg.location }}, instruction::Register::BP });

    return result_t::error(Error::Type::Internal, { reg.location });
}

Result<Argument> parse_argument(std::vector<lexer::Token> const& tokens, std::size_t& ref_pos) {
    using result_t = Result<Argument>;

    std::size_t pos = ref_pos;
    auto bracket_token_res = eat(tokens, pos, lexer::Type::OpenBracket);
    if (auto* bracket_token = bracket_token_res.get_if_result()) {
        Argument argument{ lexer::ScatteredLocation{{ bracket_token->location }}, Register::register_t::A };
        auto& locations = argument.locations;

        auto reg_res = parse_register(tokens, pos);
        if (auto* reg = reg_res.get_if_result()) {

            locations.extends(reg->locations);
            argument.value = Displacement{ std::move(*reg), std::nullopt };
            auto plus_token_res = eat(tokens, pos, lexer::Type::Plus);
            if (auto* plus_token = plus_token_res.get_if_result()) {
                auto term_res = parse_term(tokens, pos, precedence_of(BinaryRelation::Addition));
                if (auto err = term_res.get_if_error()) return result_t::error(*err);

                auto& term = term_res.get_result();
                locations.extends(term.locations);
                locations.extends(plus_token->location);

                auto& displacement = std::get<Displacement>(argument.value);
                displacement.term = std::move(term);
            } else {
                auto minus_token_res = eat(tokens, pos, lexer::Type::Plus);
                if (auto* minus_token = minus_token_res.get_if_result()) {
                    auto term_res = parse_term(tokens, pos, precedence_of(BinaryRelation::Addition));
                    if (auto err = term_res.get_if_error()) return result_t::error(*err);

                    auto& term = term_res.get_result();

                    Term exact_term {
                        term.locations
                    };

                    exact_term.value = UnaryRelation {
                        UnaryRelation::Opposite,
                        std::make_unique<Term>(std::move(term))
                    };

                    locations.extends(exact_term.locations);
                    locations.extends(minus_token->location);

                    auto& displacement = std::get<Displacement>(argument.value);
                    displacement.term = std::move(exact_term);
                }
            }

        } else {
            auto term_res = parse_term(tokens, pos, precedence_of(BinaryRelation::Addition));
            if (auto err = term_res.get_if_error()) return result_t::error(*err);

            auto& term = term_res.get_result();
            locations.extends(std::move(term.locations));

            argument.value = Pointer{ std::move(term) };
        }

        auto closing_bracket = eat(tokens, pos, lexer::Type::CloseBracket);
        if (auto err = closing_bracket.get_if_error()) return result_t::error(*err);

        ref_pos = pos;
        
        locations.extends(closing_bracket.get_result().location);

        return result_t::success(std::move(argument));
    }

    auto reg_res = parse_register(tokens, ref_pos);
    if (auto* reg = reg_res.get_if_result()) {
        return result_t::success(Argument{ std::move(reg->locations), std::move(reg->reg) });
    }

    auto term_res = parse_term(tokens, ref_pos);
    if(auto* term = term_res.get_if_result()) {
        return result_t::success(Argument{ std::move(term->locations), std::move(term->value) });
    }

    return result_t::error(term_res.get_error());
}

Result<WithLocation<std::vector<Argument>>> parse_arguments(std::vector<lexer::Token> const& tokens, std::size_t& ref_pos) {
    using result_t = Result<WithLocation<std::vector<Argument>>>;
    std::size_t pos = ref_pos;

    WithLocation<std::vector<Argument>> args_locations;
    auto& args = args_locations.data;
    auto& locations = args_locations.locations;

    {
        auto arg = parse_argument(tokens, pos);
        if (arg.is_error()) { 
            ref_pos = pos; 
            return result_t::success(std::move(args_locations)); 
        }

        locations.extends(
            args.emplace_back(std::move(arg.get_result())).locations
        );
    }

    auto comma = eat(tokens, pos, lexer::Type::Comma);
    while(comma.is_success()) {
        auto arg = parse_argument(tokens, pos);
        if (auto err = arg.get_if_error()) return result_t::error(*err);

        locations.extends(
            args.emplace_back(std::move(arg.get_result())).locations
        );
        locations.extends(comma.get_result().location);
        comma = eat(tokens, pos, lexer::Type::Comma);
    }
    ref_pos = pos;
    return result_t::success(std::move(args_locations)); 
}

Result<WithLocation<Command>> parse_command(std::vector<lexer::Token> const& tokens, std::size_t& pos) {
    using result_t = Result<WithLocation<Command>>;

    auto ope_res = parse_operation(tokens, pos);
    if (auto err = ope_res.get_if_error()) return result_t::error(*err);
    auto& ope = ope_res.get_result();

    auto args_res = parse_arguments(tokens, pos);
    if (auto err = args_res.get_if_error()) return result_t::error(*err);
    auto& args = args_res.get_result();

    WithLocation<Command> command_locations;

    auto& command = command_locations.data;
    command.ope = std::move(ope.data);
    command.args = std::move(args.data);

    auto& locations = command_locations.locations;
    locations.extends(std::move(args.locations));
    locations.extends(std::move(ope.locations));

    return result_t::success(std::move(command_locations));
}

Result<WithLocation<Data>> parse_data(std::vector<lexer::Token> const& tokens, std::size_t& ref_pos) {
    using result_t = Result<WithLocation<Data>>;
    std::size_t pos = ref_pos;

    auto token_res = eat_one_of(tokens, pos, { lexer::Type::DB, lexer::Type::DD, lexer::Type::DW });
    if (auto err = token_res.get_if_error()) { return result_t::error(*err); }
    auto& token = token_res.get_result();

    WithLocation<Data> data_locations;

    Data& data = data_locations.data;
    lexer::ScatteredLocation locations = data_locations.locations;
    std::vector<Constant>& constants = data.constants;

    switch (token.type) {
        case lexer::Type::DB:   data.size = Data::Byte; break;
        case lexer::Type::DW:   data.size = Data::Word; break;
        case lexer::Type::DD:   data.size = Data::DoubleWord; break;
        default: std::exit(666); break;
    }
    
    locations.extends(token.location);

    {
        if (auto term = parse_term(tokens, pos); term.is_success()) {
            auto& res = term.get_result();

            constants.emplace_back(Constant{
                std::move(res.locations),
                std::move(res.value)
            });

        } else {
            auto string = eat(tokens, pos, lexer::Type::String);
            if (auto err = string.get_if_error()) { return result_t::error(*err); }

            auto& res = string.get_result();

            constants.emplace_back(Constant{
                lexer::ScatteredLocation{{ std::move(res.location) }},
                string_t{ res.content }
            });
        }

        locations.extends(constants.back().locations);
    }

    auto comma = eat(tokens, pos, lexer::Type::Comma);
    while(comma.is_success()) {
        if (auto term = parse_term(tokens, pos); term.is_success()) {
            auto& res = term.get_result();

            constants.emplace_back(Constant{
                std::move(res.locations),
                std::move(res.value)
            });

        } else {
            auto string = eat(tokens, pos, lexer::Type::String);
            if (auto err = string.get_if_error()) { return result_t::error(*err); }

            auto& res = string.get_result();

            constants.emplace_back(Constant{
                lexer::ScatteredLocation{{ std::move(res.location) }},
                string_t{ res.content }
            });
        }

        locations.extends(constants.back().locations);
        locations.extends(comma.get_result().location);

        comma = eat(tokens, pos, lexer::Type::Comma);
    }
    ref_pos = pos;

    return result_t::success(std::move(data_locations));
}

Result<WithLocation<label_t>> parse_label(std::vector<lexer::Token> const& tokens, std::size_t& pos) {
    using result_t = Result<WithLocation<label_t>>;

    auto ident_res = eat(tokens, pos, lexer::Type::Ident);
    if (auto err = ident_res.get_if_error()) return result_t::error(*err);
    auto& ident = ident_res.get_result();

    auto colon = eat(tokens, pos, lexer::Type::Colon);
    if (auto err = colon.get_if_error()) return result_t::error(*err);

    WithLocation<label_t> label{ lexer::ScatteredLocation{}, ident.content };
    label.locations.extends(ident.location);
    label.locations.extends(colon.get_result().location);

    return result_t::success(std::move(label));
}

Result<WithLocation<Directive>> parse_directive(std::vector<lexer::Token> const& tokens, std::size_t& pos) {
    using result_t = Result<WithLocation<Directive>>;

    auto dir_token = eat(tokens, pos, lexer::Type::Directive);
    if (auto err = dir_token.get_if_error()) return result_t::error(*err);
   
    auto ident = eat(tokens, pos, lexer::Type::Ident);
    if (auto err = ident.get_if_error()) return result_t::error(*err);
    
    if (ident.get_result().content == "align") {
        auto term_res = parse_term(tokens, pos);
        if (auto err = term_res.get_if_error()) return result_t::error(*err);
        auto& term = term_res.get_result();

        WithLocation<Directive> directive{
            term.locations,
            Align{ std::move(term) }
        };

        directive.locations.extends(dir_token.get_result().location);
        directive.locations.extends(ident.get_result().location);

        return result_t::success(std::move(directive));
    }

    return result_t::error(Error::Type::Internal, { ident.get_result().location });
}

Result<Statement> parse_statement(std::vector<lexer::Token> const& tokens, std::size_t& ref_pos) {
    using result_t = Result<Statement>;

    {
        std::size_t pos = ref_pos;
        auto label_res = parse_label(tokens, pos);
        if(auto* label = label_res.get_if_result()) { 
            ref_pos = pos;
            return result_t::success(Statement{
                std::move(label->locations),
                std::move(label->data)
            });
        }
    }
    {
        std::size_t pos = ref_pos;
        auto command_res = parse_command(tokens, pos);
        if(auto* command = command_res.get_if_result()) { 
            ref_pos = pos;
            return result_t::success(Statement{
                std::move(command->locations),
                std::move(command->data)
            });
        }
    }
    {
        std::size_t pos = ref_pos;
        auto data_res = parse_data(tokens, pos);
        if(auto data = data_res.get_if_result()) { 
            ref_pos = pos;
            return result_t::success(Statement{
                std::move(data->locations),
                std::move(data->data)
            });
        }
    }
    {
        std::size_t pos = ref_pos;
        auto directive_res = parse_directive(tokens, pos);
        if(auto* directive = directive_res.get_if_result()) { 
            ref_pos = pos;
            return result_t::success(Statement{
                std::move(directive->locations),
                std::move(directive->data)
            });
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

    std::vector<Statement> statements;
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