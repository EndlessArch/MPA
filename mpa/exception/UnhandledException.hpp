#ifndef MPA_EXCEPTION_UNHANDLEDEXCEPTION_HPP
#define MPA_EXCEPTION_UNHANDLEDEXCEPTION_HPP

#include "Exception.hpp"

namespace mpa {
namespace exception {

class UnhandledException : public Exception {
    const std::string message_;
public:
    UnhandledException(std::string message)
        : message_(message) {
        ;
    };

    virtual void logError(std::ostream & os) const noexcept override {
        os << "Unhandled exception: " << message_ << std::endl;
    }

    virtual Seriousness getSeriousness() const noexcept override {
        return Seriousness::Fatal;
    }
};

} // ns exception
} // ns mpa

#endif