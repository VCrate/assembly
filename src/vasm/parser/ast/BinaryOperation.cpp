#include <vcrate/vasm/parser/ast/BinaryOperation.hpp>

#include <cassert>
#include <cmath>

namespace vcrate::vasm::parser {

BinaryOperation::BinaryOperation(std::unique_ptr<ShortConstant> lhs, BinaryOperation::Type type, std::unique_ptr<ShortConstant> rhs) 
    : type(type), lhs(std::move(lhs)), rhs(std::move(rhs)) {}

std::string BinaryOperation::to_string() const {
    std::string op;
    switch(type) {
        case BinaryOperation::Type::Add:            op = " + ";   break;
        case BinaryOperation::Type::Sub:            op = " - ";   break;
        case BinaryOperation::Type::Mult:           op = " * ";   break;
        case BinaryOperation::Type::Div:            op = " / ";   break;
        case BinaryOperation::Type::Mod:            op = " % ";   break;
        case BinaryOperation::Type::ShiftL:         op = " << ";  break;
        case BinaryOperation::Type::ShiftR:         op = " >> ";  break;
        case BinaryOperation::Type::RotateL:        op = " <<< "; break;
        case BinaryOperation::Type::RotateR:        op = " >>> "; break;
        case BinaryOperation::Type::Or:             op = " | ";   break;
        case BinaryOperation::Type::LogicalOr:      op = " || ";  break;
        case BinaryOperation::Type::And:            op = " & ";   break;
        case BinaryOperation::Type::LogicalAnd:     op = " && ";  break;
        case BinaryOperation::Type::Xor:            op = " ^ ";   break;
        case BinaryOperation::Type::Exp:            op = " ** ";  break;
        case BinaryOperation::Type::Less:           op = " < ";   break;
        case BinaryOperation::Type::LessEquals:     op = " <= ";  break;
        case BinaryOperation::Type::Greater:        op = " > ";   break;
        case BinaryOperation::Type::GreaterEquals:  op = " >= ";  break;
        case BinaryOperation::Type::Equals:         op = " == ";  break;
        case BinaryOperation::Type::Unequals:       op = " != ";  break;
        default:                                    op = " ??? "; break;
    }
    return "BINOP{ " + lhs->to_string() + op + rhs->to_string() + " }";
}

generator::LabelDependant<ShortConstant::Type, ShortConstant::Processor> BinaryOperation::argument_value(generator::Generator const& gen) const {
    auto lres = lhs->argument_value(gen);
    auto rres = rhs->argument_value(gen);

    using result_t = generator::LabelDependant<ShortConstant::Type, ShortConstant::Processor>;

    if (!lres.is_resolved() && !rres.is_resolved()) {
        auto deps = lres.get_dependancies();
        auto rdeps = rres.get_dependancies();
        std::move(std::begin(rdeps), std::end(rdeps), std::inserter(deps));

        return result_t::defer(ShortConstant::Processor{ std::make_unique<ShortConstant>( *this ) }, std::move(deps));
    }

    if (!lres.is_resolved()) {
        return result_t::defer(ShortConstant::Processor{ std::make_unique<ShortConstant>( *this ) }, lres.get_dependancies());
    }

    if (!rres.is_resolved()) {
        return result_t::defer(ShortConstant::Processor{ std::make_unique<ShortConstant>( *this ) }, rres.get_dependancies());
    }

    switch(type) {
#define BEGIN \
            result_t::constant(\
                std::visit([res = std::move(rres)] (auto lv) {\
                    return std::visit([lv] (auto rv) { \
                        return ShortConstant::Type{ 
#define END \
                        };\
                    }, *res.get_argument()); \
                }, *lres.get_argument()))

        case BinaryOperation::Type::Add:
            return BEGIN
                ShortConstant::CommonType<decltype(lv), decltype(rv)>{ lv + rv }
            END;

        case BinaryOperation::Type::Sub:
            return BEGIN
                ShortConstant::CommonType<decltype(lv), decltype(rv)>{ lv - rv }
            END;

        case BinaryOperation::Type::Mult:
            return BEGIN
                ShortConstant::CommonType<decltype(lv), decltype(rv)>{ lv * rv }
            END;

        case BinaryOperation::Type::Div:
            return BEGIN
                ShortConstant::CommonType<decltype(lv), decltype(rv)>{ lv / rv }
            END;

        case BinaryOperation::Type::Mod:
            auto mod = [] (auto l, auto r) -> ShortConstant::CommonType<decltype(l), decltype(r)> {
                if constexpr (std::is_same_v<decltype(l), ui32> && std::is_same_v<decltype(r), ui32>) {
                    return ui32{ l % r };
                } else {
                    return std::fmodf(static_cast<float>(l), static_cast<float>(r));
                }
            };  
            return BEGIN
                mod(lv, rv)
            END;

        case BinaryOperation::Type::ShiftL:
            return BEGIN
                ShortConstant::CommonType<decltype(lv), decltype(rv)>{ lv + rv }
            END;

        case BinaryOperation::Type::ShiftR:
            return BEGIN
                ShortConstant::CommonType<decltype(lv), decltype(rv)>{ lv + rv }
            END;

        case BinaryOperation::Type::RotateL:
            return BEGIN
                ShortConstant::CommonType<decltype(lv), decltype(rv)>{ lv + rv }
            END;

        case BinaryOperation::Type::RotateR:
            return BEGIN
                ShortConstant::CommonType<decltype(lv), decltype(rv)>{ lv + rv }
            END;

        case BinaryOperation::Type::Or:
            return BEGIN
                ShortConstant::CommonType<decltype(lv), decltype(rv)>{ lv + rv }
            END;

        case BinaryOperation::Type::LogicalOr:
            return BEGIN
                ShortConstant::CommonType<decltype(lv), decltype(rv)>{ lv + rv }
            END;

        case BinaryOperation::Type::And:
            return BEGIN
                ShortConstant::CommonType<decltype(lv), decltype(rv)>{ lv + rv }
            END;

        case BinaryOperation::Type::LogicalAnd:
            return BEGIN
                ShortConstant::CommonType<decltype(lv), decltype(rv)>{ lv + rv }
            END;

        case BinaryOperation::Type::Xor:
            return BEGIN
                ShortConstant::CommonType<decltype(lv), decltype(rv)>{ lv + rv }
            END;

        case BinaryOperation::Type::Exp:
            return BEGIN
                ShortConstant::CommonType<decltype(lv), decltype(rv)>{ lv + rv }
            END;

        case BinaryOperation::Type::Less:
            return BEGIN
                ShortConstant::CommonType<decltype(lv), decltype(rv)>{ lv + rv }
            END;

        case BinaryOperation::Type::LessEquals:
            return BEGIN
                ShortConstant::CommonType<decltype(lv), decltype(rv)>{ lv + rv }
            END;

        case BinaryOperation::Type::Greater:
            return BEGIN
                ShortConstant::CommonType<decltype(lv), decltype(rv)>{ lv + rv }
            END;

        case BinaryOperation::Type::GreaterEquals:
            return BEGIN
                ShortConstant::CommonType<decltype(lv), decltype(rv)>{ lv + rv }
            END;

        case BinaryOperation::Type::Equals:
            return BEGIN
                ShortConstant::CommonType<decltype(lv), decltype(rv)>{ lv + rv }
            END;

        case BinaryOperation::Type::Unequals:
            return BEGIN
                ShortConstant::CommonType<decltype(lv), decltype(rv)>{ lv + rv }
            END;

    }
#undef BEGIN
#undef END

    assert(false);
}

BinaryOperation::precedence_t BinaryOperation::precedence_of(Type type) {
    switch(type) {
        case BinaryOperation::Type::Exp:            return 22;

        case BinaryOperation::Type::Mult:           return 20;
        case BinaryOperation::Type::Div:            return 20;
        case BinaryOperation::Type::Mod:            return 20;

        case BinaryOperation::Type::Add:            return 18;
        case BinaryOperation::Type::Sub:            return 18;

        case BinaryOperation::Type::ShiftL:         return 16;
        case BinaryOperation::Type::ShiftR:         return 16;
        case BinaryOperation::Type::RotateL:        return 16;
        case BinaryOperation::Type::RotateR:        return 16;

        case BinaryOperation::Type::Less:           return 14;
        case BinaryOperation::Type::LessEquals:     return 14;
        case BinaryOperation::Type::Greater:        return 14;
        case BinaryOperation::Type::GreaterEquals:  return 14;

        case BinaryOperation::Type::Equals:         return 12;
        case BinaryOperation::Type::Unequals:       return 12;

        case BinaryOperation::Type::And:            return 10;

        case BinaryOperation::Type::Xor:            return 8;

        case BinaryOperation::Type::Or:             return 6;

        case BinaryOperation::Type::LogicalAnd:     return 4;

        case BinaryOperation::Type::LogicalOr:      return 2;

        default:                                    return 0;
    }
}

}