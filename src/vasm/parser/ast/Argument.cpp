#include <vcrate/vasm/parser/ast/Argument.hpp>

namespace vcrate::vasm::parser {

Argument::Argument(Register const& reg) : value(reg) {}

Argument::Argument(DereferencedArgument&& arg) : value(std::move(arg)) {}

Argument::Argument(std::unique_ptr<ShortConstant>&& cst) : value(std::move(cst)) {}

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;
 
Result<instruction::Argument> Argument::argument_value(generator::Generator const& gen, bool abord_on_undefined_label) const {
    return std::visit(
        overloaded {
            [&gen, abord_on_undefined_label] (std::unique_ptr<ShortConstant> const& p) {
                auto res = p->argument_value(gen);
                if (auto* err = res.get_error_if()) {
                    if (err->type == Error::Type::G001_Undefined_label && !abord_on_undefined_label) {
                        return Result::sucess(instruction::Value{ std::numeric_limits<i32>::max() });
                    }
                }
                return std::move(res);
            },
            [&gen] (Register const& p) {
                return p.argument_value(gen);
            }, 
            [&gen, abord_on_undefined_label] (DereferencedArgument const& p) {
                auto res = p->argument_value(gen);
                if (auto* err = res.get_error_if()) {
                    if (err->type == Error::Type::G001_Undefined_label && !abord_on_undefined_label) {
                        return Result::sucess(instruction::Value{ std::numeric_limits<i32>::max() });
                    }
                }
                return std::move(res);
            },
        },
        value
    );
}

std::string Argument::to_string() const { 
    return std::visit(
        overloaded {
            [] (std::unique_ptr<ShortConstant> const& p) {
                return p->to_string();
            }, 
            [] (Register const& p) {
                return p.to_string();
            }, 
            [] (DereferencedArgument const& p) {
                return p.to_string();
            },             
        }, 
        value
    );
}


}