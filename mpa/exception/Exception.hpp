#ifndef MPA_EXCEPTION_EXCEPTION_HPP
#define MPA_EXCEPTION_EXCEPTION_HPP

#include <ostream>
#include <memory>

namespace mpa {
namespace exception {

enum class Seriousness {
    // shutdown requested
    Shutdown,
    // some task has failed.
    Fatal,
    // something happened, but task is resumable.
    Error,
    // to inform by creating exception.
    Warning
};

class Exception {
public:
    virtual ~Exception() = default;

    virtual void logError(std::ostream & os) const noexcept = 0;

    virtual void setSeriousness(Seriousness) noexcept { return; };

    virtual Seriousness getSeriousness(void) const noexcept = 0;
};

template <typename E, typename... Args>
std::unique_ptr<Exception> make_exception(Args &&... args) {
    auto exc = new E(std::forward<Args>(args)...);
    return std::unique_ptr<Exception>(
        reinterpret_cast<Exception *>(exc));
}

} // ns exception
} // ns mpa

#endif