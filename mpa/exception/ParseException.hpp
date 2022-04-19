#ifndef MPA_EXCEPTION_PARSEEXCEPTION_HPP
#define MPA_EXCEPTION_PARSEEXCEPTION_HPP

#include <type_traits>
#include <variant>

#include "Exception.hpp"

namespace mpa {
namespace exception {

class ParseException : public Exception {
public:
    using string_container
        = typename std::variant<std::string,
                                std::function<std::string()>>;

    const string_container message_callback_;

    const Seriousness ser_;
public:

    template <typename T,
        typename = std::enable_if_t<
            std::disjunction_v<
                std::is_nothrow_invocable_r<std::string, T>,
                // std::is_nothrow_constructible<std::string, T>,
                // std::is_nothrow_convertible<std::string, T>,
                // std::is_nothrow_convertible<const char *, T>
                std::is_constructible<std::string, T>,
                std::is_convertible<std::string, T>,
                std::is_convertible<const char *, T>
                >
        >>
    ParseException(T && str, Seriousness ser = Seriousness::Fatal)
        : message_callback_(std::forward<T>(str)), ser_(ser) {
        ;
    }

    virtual ~ParseException() = default;

    void logError(std::ostream & os) const noexcept override {
        os <<
            std::visit([](auto && rhs) -> std::string {
                if constexpr (std::is_invocable_v<decltype(rhs)>)
                    return static_cast<std::string>(rhs());
                
                if constexpr (std::is_same_v<std::string, decltype(rhs)>)
                    return static_cast<std::string>(rhs);
                
                return std::string("");
            }, message_callback_) << '\n';
    }

    Seriousness getSeriousness(void) const noexcept override {
        return ser_;
    }
};

} // ns exception
} // ns mpa

#endif