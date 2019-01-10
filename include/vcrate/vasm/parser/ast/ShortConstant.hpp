#pragma once

#include <vcrate/vasm/parser/ast/Constant.hpp>

namespace vcrate::vasm::parser {

class ShortConstant : public Constant {
public:

    using Type = std::variant<float, ui32>;

    template<typename A, typename B>
    using CommonType = std::conditional_t<std::is_same_v<A, float> || std::is_same_v<B, float>, float, ui32>;

    struct Processor {
        generator::LabelDependant<Type, Processor> operator()(generator::Generator const& gen) const {
            return  value->argument_value(gen);
        }

        std::unique_ptr<ShortConstant> value;
    };  

    virtual generator::LabelDependant<Type, Processor> argument_value(generator::Generator const& gen) const = 0;

private:

};

}