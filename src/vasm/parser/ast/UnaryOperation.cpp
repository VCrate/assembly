#include <vcrate/vasm/parser/ast/UnaryOperation.hpp>

#include <cassert>

namespace vcrate::vasm::parser {

UnaryOperation::UnaryOperation(Type type, std::unique_ptr<ShortConstant> value) : type(type), value(std::move(value)) {}

std::string UnaryOperation::to_string() const {
    std::string op;
    switch(type) {
        case UnaryOperation::Type::Neg:         op = "- ";   break;
        case UnaryOperation::Type::Not:         op = "~ ";   break;
        case UnaryOperation::Type::LogicalNot:  op = "! ";   break;
        default:                                op = "??? "; break;
    }
    return "UNOP{ " + op + value->to_string() + " }";
}

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

generator::LabelDependant<ShortConstant::Type, ShortConstant::Processor> UnaryOperation::argument_value(generator::Generator const& gen) const {
    auto res = value->argument_value(gen);

    if (!res.is_resolved()) {
        return res;
    }

    using result_t = generator::LabelDependant<ShortConstant::Type, ShortConstant::Processor>;

    switch(type) {
        case UnaryOperation::Type::Neg:
            return result_t::constant(std::visit([] (auto v) { return ShortConstant::Type{decltype(v){ -v }}; }, *res.get_argument()));

        case UnaryOperation::Type::Not:
            return result_t::constant(std::visit([] (auto v) { return ShortConstant::Type{decltype(v){ ~v }}; }, *res.get_argument()));

        case UnaryOperation::Type::LogicalNot:
            return result_t::constant(std::visit([] (auto v) { return ShortConstant::Type{decltype(v){ !static_cast<bool>(v) }}; }, *res.get_argument()));
    }

    assert(false);
}

}