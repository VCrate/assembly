#pragma once

#include <vcrate/vasm/lexer/Token.hpp>
#include <vcrate/vasm/lexer/Error.hpp>

#include <vector>
#include <variant>
#include <string>

namespace vcrate::vasm::lexer {

template<typename T>
class Result {
public:
    Error&       get_error()          { return std::get<Error>(result);     }
    Error const& get_error()    const { return std::get<Error>(result);     }
    Error*       get_if_error()       { return std::get_if<Error>(&result); }
    const Error* get_if_error() const { return std::get_if<Error>(&result); }

    T&       get_result()          { return std::get<T>(result);     }
    T const& get_result()    const { return std::get<T>(result);     }
    T*       get_if_result()       { return std::get_if<T>(&result); }
    const T* get_if_result() const { return std::get_if<T>(&result); }

    bool is_error() const {
        return std::holds_alternative<Error>(result);
    }

    bool is_success() const {
        return std::holds_alternative<T>(result);
    }

    static Result<T> success(T const& t) {
        return Result<T> { t };
    }

    static Result<T> error(Error const& e) {
        return Result<T> { e };
    }

private:

    Result<T>(Error const& e) : result(e) {};
    Result<T>(T const& t) : result(t) {};

    std::variant<T, Error> result;
};

Result<std::vector<Token>> tokenize(std::vector<std::string> const& source);

}