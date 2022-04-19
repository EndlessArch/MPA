#ifndef MPA_STRING_HPP
#define MPA_STRING_HPP

#include <cstring>
#include <string>
#include <string_view>
#include <variant>

namespace mpa {

std::string
operator+(const std::string & lhs, const std::string_view & rhs) noexcept {
    return lhs + std::string(rhs);
}

std::string
operator+(const std::string_view & lhs, const std::string & rhs) noexcept {
    return std::string(lhs) + rhs;
}

std::string
operator+(const std::string_view & lhs, const std::string_view & rhs) noexcept {
    return std::string(lhs) + std::string(rhs);
}

class StringIterator {
    std::variant<std::string, std::string_view> it_;
public:

    using iterator_category = std::input_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = char;
    using const_value_type = const value_type;
    using reference = value_type &;
    using const_reference = const_value_type &;
    using pointer = value_type *;
    using const_pointer = const_value_type *;

    constexpr StringIterator(void) noexcept
        : it_(std::string_view("", 0)) {}

    template <std::size_t N>
    constexpr StringIterator(const char (&str)[N]) noexcept
        : it_(std::string_view(str, N - 1)) {}

    template <typename T>
    constexpr StringIterator(T && begin, T && end) noexcept
        : it_(std::string(std::forward<T>(begin),
                          std::forward<T>(end))) {}

    constexpr StringIterator(std::string && rhs) noexcept
        : it_(std::move(rhs)) {}

    constexpr StringIterator(std::string_view && rhs) noexcept
        : it_(std::move(rhs)) {}

    constexpr value_type operator*(void) const noexcept {
        return std::visit([](auto && arg) -> value_type {
            return arg[0];
        }, it_);
    }

    StringIterator operator+(std::size_t n) const noexcept {
        // constexpr auto svp = [n](auto && arg) -> StringIterator {
        //     using ArgT = std::decay_t<decltype(arg)>;
        //     if constexpr (std::is_same_v<ArgT, std::string_view>)
        //         return StringIterator();
        //     return StringIterator(arg.substr(n));
        // };

        // return std::visit(svp, it_);

        return std::visit([n](auto && arg) -> StringIterator {
            return StringIterator(arg.substr(n));
        }, it_);
    }

    StringIterator operator+(const StringIterator & it) const noexcept {
        return std::visit([](auto && lhs, auto && rhs) -> StringIterator {
            // using l_t = typename std::remove_cvref_t<decltype(lhs)>;
            // using r_t = typename std::remove_cvref_t<decltype(rhs)>;

            // if constexpr (std::is_same_v<l_t, std::string>) {
            //     if constexpr (!std::is_same_v<r_t, std::string>)
            //         return lhs + std::string(rhs);
            //     else
            //         return lhs + rhs;
            // } else {
            //     if constexpr (std::is_same_v<r_t, std::string>)
            //         return std::string(lhs) + rhs;
            //     else
            //         return std::string(lhs) + std::string(rhs);
            // }


            // if constexpr (std::is_same_v<l_t, std::string>
            //             && std::is_same_v<r_t, std::string>)
                // return lhs + rhs;
            return lhs + rhs;
        }, it_, it.it_);
    }

    StringIterator operator++(void) noexcept {
        StringIterator p1 = operator+(1);
        it_ = p1.it_;
        return *this;
    }

    StringIterator end(void) const noexcept {
        return operator+(len());
    }

    bool operator==(const StringIterator & rhs) const noexcept {
        return std::visit([](auto && arg, auto && rhs) -> bool {
            if(auto len = arg.length() != rhs.length()) return false;
            else
                for(auto i = 0; i< len; ++i)
                    if(arg[i] != rhs[i]) return false;
            return true;
        }, it_, rhs.it_);
    }

    bool operator!=(const StringIterator & rhs) const noexcept {
        return !(*this == rhs);
    }

    operator const_pointer (void) const noexcept {
        struct Functor {
            const_pointer operator()(const std::string & str) const noexcept {
                return str.c_str();
            }

            constexpr const_pointer operator()(const std::string_view & str) const noexcept {
                return str.data();
            }
        };

        return std::visit(Functor(), it_);
        // return std::visit([](auto && arg) -> const_pointer {
        //     return std::is_same_v<std::decay_t<decltype(arg)>, std::string>
        //         ? std::enable_if_t<false, const_pointer>()
        //         : static_cast<std::string_view>(arg).data();
        // }, it_);
    }

    constexpr operator std::string_view (void) const noexcept {
        return std::get<std::string_view>(it_);
    }

    constexpr std::size_t len(void) const noexcept {
        return std::visit([](auto && arg) -> std::size_t {
            return arg.length();
        }, it_);
    }
};

class String {
    
    StringIterator str_;

    using decay_str_t = std::decay_t<const char * const>;

    template <typename T>
    static constexpr bool is_primitive_string_v = std::is_same_v<std::decay_t<T>, decay_str_t>;

public:

    using char_type = char;

    constexpr String(void) noexcept
        : str_("") {}

    template <std::size_t N>
    constexpr String(const char (&str)[N])
        : str_(str) {}
    
    String(const String & rhs) noexcept
        : str_(rhs.str_) {}
        
    String(const char * begin, const char * end)
        : str_(begin, end) {}

    String(StringIterator && it)
        : str_(it) {}

    constexpr bool startsWith(char ch) const noexcept {
        return *str_ == ch;
    }

    /*constexpr*/ bool endsWith(char ch) const noexcept {
        return *(str_.end() - 1) == ch;
    }

    /*constexpr*/ std::size_t count(char ch) const noexcept {
        return std::count(str_, str_.end(), ch);
    }

    constexpr char operator[](std::size_t n) const noexcept {
        return str_.operator std::string_view()[n];
    }

    StringIterator begin(void) const noexcept {
        return str_;
    }

    StringIterator end(void) const noexcept {
        return str_ + str_.len();
    }

    String operator+(const String & rhs) const noexcept {
        return str_ + rhs.str_;
    }

    String operator=(const String & rhs) noexcept {
        str_ = rhs.str_;
        return *this;
    }

    bool operator==(char c) const noexcept {
        return str_[0] == c;
    }

    bool operator==(const String & rhs) const noexcept {
        return str_ == rhs.str_;
    }

    bool operator<(const String & rhs) const noexcept {
        return std::lexicographical_compare(str_, str_ + str_.len(),
                                            rhs.str_, rhs.str_ + rhs.str_.len());
    }

    String operator+(String && rhs) const noexcept {
        return String(str_ + rhs.str_);
    }

    // for easy comparison with const char *
    operator const char *(void) const noexcept {
        return str_.operator StringIterator::const_pointer();
    }

    operator std::string(void) const noexcept {
        return std::string(str_);
    }

    friend
    inline std::ostream &
    operator<<(std::ostream &, const String &) noexcept;
};

inline bool operator==(const char * lhs, const String & rhs) noexcept {
    return !std::strcmp(lhs, rhs.operator const char *());
}

inline std::istream &
operator>>(std::istream & is, String & str) noexcept {
    static char buf[1024];
    char * p = buf;

    typename std::istream::sentry sen(is, true);

    if(sen) {
        memset(buf, 0, 1024);

        while(true) {
            auto * sb = is.rdbuf();            
            char c = sb->sgetc();

// #include <iostream>
//             std::cout << (int)c << ": \'" << c << "\'\n";

            if(c == EOF)
                break;

            if(std::isspace(c)) {
                // end parsing identifier
                if(p - buf) {
                    // sb->snextc();
                    break;
                } else
                    while(std::isspace(c = (char)sb->snextc()));
            }
            
            if(c == '\\')
                if(p - buf) break;
            ;

            *p = c;
            ++p;
            sb->snextc();
        }
    }

    str = String(static_cast<char *>(buf), p);

    return is;
}

inline std::ostream &
operator<<(std::ostream & os, const String & str) noexcept {
    return os << str.str_.operator const char *();
}

template <std::size_t N>
String
operator+(const char (&lhs)[N], const String & rhs) noexcept {
    return String(lhs) + rhs;
}

} // ns mpa

#endif