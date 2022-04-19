#ifndef MPA_UTILS_TYPEBOX_HPP
#define MPA_UTILS_TYPEBOX_HPP

namespace mpa {
namespace utils {

template <typename T>
class TypeBox {
public:
    using type = T;

    TypeBox() = default;
};

template <typename T>
auto wrap_type(void) -> TypeBox<T> {
    return TypeBox<T>();
}

} // ns utils
} // ns mpa

#endif