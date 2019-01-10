#pragma once

#include <vcrate/instruction/Argument.hpp>
#include <vcrate/vasm/parser/ast/Argument.hpp>

#include <variant>
#include <ostream>
#include <unordered_set>
#include <string>
#include <optional>
#include <vcrate/vasm/generator/Generator.hpp>

namespace vcrate::vasm::generator {

template<typename T, typename R>
class LabelDependant {
    struct Deferred {
        R processor;
        std::unordered_set<std::string> dependancies;
    };

public:

    static LabelDependant constant(T const& arg) {
        return LabelDependant(arg);
    }

    static LabelDependant defer(R const& processor, std::unordered_set<std::string> const& dependancies) {
        return LabelDependant(Deferred{ value, dependancies });
    }

    bool resolve(Generator const& gen) {
        if (is_resolved()) {
            return true;
        }

        auto& d = std::get<Deferred>(arg);

        for(auto const& s : d.dependancies) {
            if (!gen.know_label(s)) {
                return false;
            }
        }

        *this = d.processor(gen);

        return is_resolved();
    }

    bool is_resolved() const {
        return std::holds_alternative<T>(arg);
    }

    std::optional<T> get_argument() const {
        if (is_resolved()) {
            return std::get<T>(arg);
        } else {
            return std::nullopt;
        }
    }

private:

    LabelDependant(T const& t) : arg(t) {}
    LabelDependant(Deferred const& d) : arg(d) {}

    std::variant<T, Deferred> arg;

};

}