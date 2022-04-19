#ifndef MPA_EXCEPTION_UNREACHABLECODEEXCEPTION_HPP
#define MPA_EXCEPTION_UNREACHABLECODEEXCEPTION_HPP

#include "Exception.hpp"

namespace mpa {
namespace exception {

class UnreachableCodeException : public Exception {
    const char * code_;
    bool ignorable_ = false;
public:
    UnreachableCodeException(const char * cd_) noexcept
        : code_(cd_) {}
    
    UnreachableCodeException(const char * cd_, const char *) noexcept
        : code_(cd_), ignorable_(true) {}

    virtual void logError(std::ostream & os) const noexcept override {
        os << "Unreachable code has been reached.\n";
        os << code_ << ": reached unreachable code.\n";

        if(ignorable_) return;
    }
};

} // ns exception
} // ns mpa

#endif