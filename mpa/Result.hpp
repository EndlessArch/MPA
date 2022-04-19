#ifndef MPA_RESULT_HPP
#define MPA_RESULT_HPP

#include <variant>

#include "exception/Exception.hpp"
#include "exception/GlobalExceptionQueue.hpp"
#include "exception/UnhandledException.hpp"

namespace mpa {

template <typename T>
struct Result {
    using ExceptionPtr_t = std::unique_ptr<mpa::exception::Exception>;

    const std::variant<T, ExceptionPtr_t> data_;

    Result(T && t)
        : data_(std::forward<T>(t)) {
        ;
    }

    template <typename E>
    Result(std::unique_ptr<E> && e)
        : data_(std::forward<std::enable_if_t<std::is_base_of_v<mpa::exception::Exception, E>, ExceptionPtr_t>>(e)) {
        ;
    }

    constexpr T open(void) const noexcept {
        return std::visit([](auto && arg) -> T {
            if constexpr (std::is_same_v<std::decay_t<decltype(arg)>, ExceptionPtr_t>)
                mpa::exception::GlobalExceptionQueue::push(std::move(arg));
            
            if constexpr (!std::is_same_v<std::decay_t<decltype(arg)>, std::decay_t<T>>)
                return T{};
            else
                return arg;
        }, data_);
    }

    constexpr operator T() const noexcept {
        return open();
    }
};

} // ns mpa

#endif