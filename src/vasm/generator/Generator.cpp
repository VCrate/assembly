#include <vcrate/vasm/generator/Generator.hpp>

#include <vcrate/instruction/Instruction.hpp>
#include <vcrate/bytecode/v1.hpp>

#include <vcrate/vasm/Visitor.hpp>

#include <algorithm>
#include <vector>
#include <ostream>
#include <unordered_map>
#include <string>
#include <memory>
#include <variant>
#include <cmath>

namespace vcrate::vasm::generator {

/*
    Executable generation happen in multiple passes
    The first pass collect informations about the size and the address of each instructions, datas and labels
    The second pass will generate each instruction and build the executable
*/

using TermValue = std::variant<parser::float_t, parser::integer_t>;

template<typename In, typename Out>
Out convert(In in) {
    union U {
        In in;
        Out out;
    };
    return U { .in = in }.out;
}

ui32 serialize_term(TermValue const& t) {
    return std::visit(
        [] (auto i) {
            return convert<decltype(i), ui32>(i);
        },
    t);
}


/*
    First pass
    Size and address of each instruction
    Collect label's positions
*/

struct Layout {
    using address_t = ui32;

    struct Instr {
        using content_t = std::variant<parser::Command const*, parser::Data const*>;

        address_t end_address; // if instructions starts at address `a` and has a maximum isze of `s` then end_address is `a + s`
        content_t content;
    };

    std::unordered_map<parser::label_t, address_t> labels;
    std::vector<Instr> instrs;

};

bool term_above(std::unordered_map<parser::label_t, Layout::address_t> const& known_labels, parser::Term::variant_t const& term, ui32 max_value) {
    return std::visit(Visitor{
        [&] (parser::float_t f) {
            return serialize_term(f < 0 ? -f : f) > max_value;
        },
        [&] (parser::integer_t i) {
            return i > max_value;
        },
        [&] (parser::Dereferenced const& d) {
            return term_above(known_labels, *d.term, max_value);
        }
        [&] (parser::UnaryRelation const& u) {
            return term_above(known_labels, *d.term, max_value);
        }
    }, term);
}

bool argument_need_extra(std::unordered_map<parser::label_t, Layout::address_t> const& known_labels, parser::Argument const& arg, bool on_12_bits) {
    return std::visit(Visitor{
        [] (parser::Argument::register_t const& reg) {
            return false;
        },
        [&] (parser::Argument::term_t const& term) {
            return term_need_extra(known_labels, term, on_12_bits);
        },
        [&] (parser::Pointer const& ptr) {
            return term_need_extra(known_labels, ptr.term.value, on_12_bits);
        },
        [&] (parser::Displacement const& disp) {
            return disp.term && term_need_extra(known_labels, disp.term->value, on_12_bits);
        },
    }, arg);
}

std::optional<Error> update_layout(Layout& layout, parser::Command const& command) {

}

std::optional<Error> update_layout(Layout& layout, parser::Directive const& directive) {

}

std::optional<Error> update_layout(Layout& layout, parser::Statement const& statement) {
    return std::visit(Visitor {
        [&] (parser::Directive const& directive) -> std::optional<Error> {
            return update_layout(layout, directive);
        },
        [&] (parser::Command const& command) -> std::optional<Error> {
            return update_layout(layout, command);
        },
        [&] (parser::label_t const& label) -> std::optional<Error> {
            if (layout.labels.count(label) >= 1) {
                return Error{ Error::Type::Internal };
            }

            Layout::address_t addr = layout.instrs.empty() ? 0 : layout.instrs.back().end_address;

            layout.labels[label] = addr;

            return std::nullopt;
        },
        [&] (parser::Data const& data) -> std::optional<Error> {
            ui32 size;
            switch(data.size) {
                case parser::Data::Size::Byte: size = 1;
                case parser::Data::Size::Word: size = 2;
                case parser::Data::Size::DoubleWord: size = 4;
            }

            ui32 cst{ 0 };
            for(auto const& c : data.constants) {
                if (auto* str = std::get_if<parser::string_t>(&c.value)) {
                    cst += str->size();
                } else {
                    ++cst;
                }
            }

            ui32 const data_size = cst * size;
            layout.instrs.push_back(Layout::Instr{
                (layout.instrs.empty() ? 0 : layout.instrs.back().end_address) + data_size,
                &data
            });

            return std::nullopt;
        },
    }, statement.value);
}

Result<Layout> find_layout(std::vector<parser::Statement> const& statements) {
    Layout layout;

    for(auto const& statement : statements) {
        auto err = update_layout(layout, statement);

        if (err) {
            return Result<Layout>::error(std::move(*err));
        }
    }   
    return Result<Layout>::success(std::move(layout)); 
}

Result<TermValue> generate_term(parser::Term::variant_t const& term, lexer::ScatteredLocation const& locations) {
    using result_t = Result<TermValue>;
    return std::visit(
        Visitor {
            [] (parser::integer_t i) -> result_t {
                return result_t::success(i);
            },
            [] (parser::float_t f) -> result_t {
                return result_t::success(f);
            },
            [&] (parser::ident_t i) -> result_t {
                return result_t::success(parser::integer_t{ 0xBAD'C0DE });
                //return result_t::error(Error::Type::Internal, locations);
            },
            [&] (parser::UnaryRelation const& rel) -> result_t {
                auto operand_res = generate_term(rel.operand->value, rel.operand->locations);

                if (auto* err = operand_res.get_if_error()) {
                    return result_t::error(std::move(*err));
                }

                auto& operand = operand_res.get_result();

                switch(rel.type) {
                    case parser::UnaryRelation::Negation:   // !
                        if (std::holds_alternative<parser::integer_t>(operand)) {
                            return result_t::success(std::visit([&] (auto v) -> TermValue {
                                return parser::integer_t{ !v ? 1u : 0u };
                            }, operand));
                        }
                        return result_t::error(Error::Type::Internal, locations);

                    case parser::UnaryRelation::Complement: // ~
                        return std::visit(Visitor {
                            [&] (parser::integer_t v) -> result_t {
                                return result_t::success(~v);
                            },
                            [&] (parser::float_t v) -> result_t {
                                return result_t::error(Error::Type::Internal, locations);
                            }
                        }, operand);
                    case parser::UnaryRelation::Opposite:   // -
                        return result_t::success(std::visit([&] (auto v) -> TermValue {
                            return -v;
                        }, operand));
                }
                std::cerr << __FILE__ << " : generate_term :: UnaryRelation::Type =>\nMissing a case for " << static_cast<int>(rel.type) << '\n';
                std::exit(1);
            },
            [&] (parser::BinaryRelation const& rel) -> result_t {
                auto lhs_res = generate_term(rel.lhs->value, rel.lhs->locations);
                auto rhs_res = generate_term(rel.rhs->value, rel.rhs->locations);

                if (auto* err = lhs_res.get_if_error()) {
                    return result_t::error(std::move(*err));
                }
                if (auto* err = rhs_res.get_if_error()) {
                    return result_t::error(std::move(*err));
                }

                auto& lhs = lhs_res.get_result();
                auto& rhs = rhs_res.get_result();

                auto otherwise_error = [&] (auto, auto) -> result_t {
                    return result_t::error(Error::Type::Internal, locations);
                };

                using parser::integer_t;
                using parser::float_t;

                switch(rel.type) {
                    case parser::BinaryRelation::Addition :       // +
                        return std::visit(
                            [&] (auto l, auto r) -> result_t {
                                return result_t::success(l + r);
                            }
                        , lhs, rhs);

                    case parser::BinaryRelation::Subtraction :    // -
                        return std::visit(
                            [&] (auto l, auto r) -> result_t {
                                return result_t::success(l - r);
                            }
                        , lhs, rhs);

                    case parser::BinaryRelation::Multiplication : // *
                        return std::visit(
                            [&] (auto l, auto r) -> result_t {
                                return result_t::success(l * r);
                            }
                        , lhs, rhs);

                    case parser::BinaryRelation::Division :       // /
                        return std::visit(
                            [&] (auto l, auto r) -> result_t {
                                return result_t::success(l / r);
                            }
                        , lhs, rhs);

                    case parser::BinaryRelation::Modulo :         // %
                        return std::visit(Visitor {
                            [&] (integer_t l, integer_t r) -> result_t {
                                return result_t::success(l % r);
                            },
                            [&] (float_t l, float_t r) -> result_t {
                                return result_t::success(std::fmod(l, r));
                            },
                            otherwise_error
                        }, lhs, rhs);

                    case parser::BinaryRelation::ShiftLeft :      // <<
                        return std::visit(Visitor {
                            [&] (integer_t l, integer_t r) -> result_t {
                                return result_t::success(l << r);
                            },
                            otherwise_error
                        }, lhs, rhs);

                    case parser::BinaryRelation::ShiftRight :     // >>
                        return std::visit(Visitor {
                            [&] (integer_t l, integer_t r) -> result_t {
                                return result_t::success(l >> r);
                            },
                            otherwise_error
                        }, lhs, rhs);

                    case parser::BinaryRelation::RotateLeft :     // <<<
                        return std::visit(Visitor {
                            [&] (integer_t l, integer_t r) -> result_t {
                                return result_t::success((l << r) | (l >> (-r & (sizeof(integer_t) - 1))));
                            },
                            otherwise_error
                        }, lhs, rhs);

                    case parser::BinaryRelation::RotateRight :    // >>>
                        return std::visit(Visitor {
                            [&] (integer_t l, integer_t r) -> result_t {
                                return result_t::success((l >> r) | (l << (-r & (sizeof(integer_t) - 1))));
                            },
                            otherwise_error
                        }, lhs, rhs);

                    case parser::BinaryRelation::Or :             // |
                        return std::visit(Visitor {
                            [&] (integer_t l, integer_t r) -> result_t {
                                return result_t::success(l | r);
                            },
                            otherwise_error
                        }, lhs, rhs);

                    case parser::BinaryRelation::LogicalOr :      // ||
                        return std::visit(Visitor {
                            [&] (integer_t l, integer_t r) -> result_t {
                                return result_t::success(l || r ? 1u : 0u);
                            },
                            otherwise_error
                        }, lhs, rhs);

                    case parser::BinaryRelation::And :            // &
                        return std::visit(Visitor {
                            [&] (integer_t l, integer_t r) -> result_t {
                                return result_t::success(l & r);
                            },
                            otherwise_error
                        }, lhs, rhs);

                    case parser::BinaryRelation::LogicalAnd :     // &&
                        return std::visit(Visitor {
                            [&] (integer_t l, integer_t r) -> result_t {
                                return result_t::success(l && r ? 1u : 0u);
                            },
                            otherwise_error
                        }, lhs, rhs);

                    case parser::BinaryRelation::Xor :            // ^
                        return std::visit(Visitor {
                            [&] (integer_t l, integer_t r) -> result_t {
                                return result_t::success(l ^ r);
                            },
                            otherwise_error
                        }, lhs, rhs);

                    case parser::BinaryRelation::Exp :            // **
                        return std::visit(Visitor {
                            [&] (integer_t l, integer_t r) -> result_t {
                                return result_t::success(static_cast<integer_t>(std::pow(l, r)));
                            },
                            [&] (auto l, auto r) -> result_t {
                                return result_t::success(static_cast<float_t>(std::pow(l, r)));
                            }
                        }, lhs, rhs);

                    case parser::BinaryRelation::Less :           // <
                        return std::visit(
                            [&] (auto l, auto r) -> result_t {
                                return result_t::success(l < r ? 1u : 0u);
                            }
                        , lhs, rhs);

                    case parser::BinaryRelation::LessEquals :     // <=
                        return std::visit(
                            [&] (auto l, auto r) -> result_t {
                                return result_t::success(l <= r ? 1u : 0u);
                            }
                        , lhs, rhs);

                    case parser::BinaryRelation::Greater :        // >
                        return std::visit(
                            [&] (auto l, auto r) -> result_t {
                                return result_t::success(l > r ? 1u : 0u);
                            }
                        , lhs, rhs);

                    case parser::BinaryRelation::GreaterEquals :  // >=
                        return std::visit(
                            [&] (auto l, auto r) -> result_t {
                                return result_t::success(l >= r ? 1u : 0u);
                            }
                        , lhs, rhs);

                    case parser::BinaryRelation::Equals :         // ==
                        return std::visit(
                            [&] (auto l, auto r) -> result_t {
                                return result_t::success(l == r ? 1u : 0u);
                            }
                        , lhs, rhs);

                    case parser::BinaryRelation::Unequals :       // !=
                        return std::visit(
                            [&] (auto l, auto r) -> result_t {
                                return result_t::success(l != r ? 1u : 0u);
                            }
                        , lhs, rhs);
                }
                std::cerr << __FILE__ << " : generate_term :: BinaryRelation::Type =>\nMissing a case for " << static_cast<int>(rel.type) << '\n';
                std::exit(1);
            },
            [&] (parser::Dereferenced const& ref) -> result_t {
                return result_t::success(parser::integer_t{ 0xBAD'C0DE });
                //return result_t::error(Error::Type::Internal, locations);
            }
        }
    , term);
}

Result<instruction::Argument> generate_argument(parser::Argument const& argument) {
    using result_t = Result<instruction::Argument>;
    return std::visit(
        Visitor {
            [&] (parser::Argument::term_t const& term) -> result_t {
                auto value_res = generate_term(term, argument.locations);
                if (auto* err = value_res.get_if_error()) {
                    return result_t::error(std::move(*err));
                }
                return result_t::success(instruction::Value{ serialize_term(value_res.get_result()) });
            },
            [] (parser::Argument::register_t const& reg) -> result_t {
                return result_t::success(instruction::Argument{ reg });
            },
            [] (parser::Displacement const& disp) -> result_t {
                if (!disp.term) {
                    return result_t::success(instruction::Displacement(disp.reg.reg, 0));
                }

                auto value_res = generate_term(disp.term->value, disp.term->locations);
                if (auto* err = value_res.get_if_error()) {
                    return result_t::error(std::move(*err));
                }
                return result_t::success(instruction::Displacement(disp.reg.reg, serialize_term(value_res.get_result())));
            },
            [] (parser::Pointer const& pointer) -> result_t {
                auto value_res = generate_term(pointer.term.value, pointer.term.locations);
                if (auto* err = value_res.get_if_error()) {
                    return result_t::error(std::move(*err));
                }
                return result_t::success(instruction::Address{ serialize_term(value_res.get_result()) });
            },
        },
        argument.value
    );
}

Result<instruction::Instruction> generate_instruction(parser::Command const& command, lexer::ScatteredLocation const& locations) {
    using result_t = Result<instruction::Instruction>;
    auto ope_def = bytecode::OpDefinition::get(command.ope);
    if (ope_def.arg_count() != command.args.size() || command.args.size() > 2) {
        return result_t::error(Error::Type::Internal, locations);
    }

    if (command.args.size() == 0) {
        return result_t::success(instruction::Instruction(command.ope));
    }

    auto arg0_res = generate_argument(command.args[0]);
    if (auto* err = arg0_res.get_if_error()) {
        return result_t::error(std::move(*err));
    }


    if (command.args.size() == 1) {
        return result_t::success(instruction::Instruction(command.ope, std::move(arg0_res.get_result())));
    }

    auto arg1_res = generate_argument(command.args[1]);
    if (auto* err = arg1_res.get_if_error()) {
        return result_t::error(std::move(*err));
    }

    return result_t::success(instruction::Instruction(command.ope, std::move(arg0_res.get_result()), std::move(arg1_res.get_result())));
}

GeneratorResult generate(std::vector<parser::Statement> const& statements) {
    using result_t = GeneratorResult;
    for(auto const& statement : statements) {
        if (auto* command = std::get_if<parser::Command>(&statement.value)) {
            auto res = generate_instruction(*command, statement.locations);
            if (auto* istr = res.get_if_result()) {
                std::cout << "> " << istr->to_string() << '\n';
            } else {
                std::cout << " > FAIL\n";
            }
        }
    }

    return result_t::error(Error{});
}

}