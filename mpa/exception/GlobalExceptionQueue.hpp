#ifndef MPA_EXCEPTION_GLOBALEXCEPTIONQUEUE_HPP
#define MPA_EXCEPTION_GLOBALEXCEPTIONQUEUE_HPP

// #include <queue>
#include <iostream>
#include <memory>
#include <mutex>
#include <sstream>

#include "Exception.hpp"

namespace mpa {
namespace exception {

class GlobalExceptionQueue {
public:
    using ElementType = std::unique_ptr<Exception>;

private:
    static std::mutex mtx_;

public:
    
    template <typename T>
    static void push(const std::unique_ptr<T> && e) {
        std::lock_guard lg(mtx_);
        std::stringstream ss;
        e->logError(ss);

        std::string buf;
        ss.str((std::string &)buf);

        std::cerr << buf;

        auto ser = e->getSeriousness();
        switch(ser) {
        case Seriousness::Shutdown:
            ; // any message?
            std::terminate();
        default:
            ;
        }

        return;
    }

};

std::mutex GlobalExceptionQueue::mtx_;

} // ns exception
} // ns mpa

#endif