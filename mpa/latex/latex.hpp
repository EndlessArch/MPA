#ifndef MPA_LATEX_LATEX_HPP
#define MPA_LATEX_LATEX_HPP

#include <cctype>
#include <iostream>
#include <map>

#include "../Result.hpp"
#include "../String.hpp"
#include "../exception/ParseException.hpp"
#include "../utils/TypeBox.hpp"

namespace mpa {
namespace latex {
class ExpressionTy {
public:
    enum ExpTy {
        Constant,
        Variable,
        Expression
    };
private:
    ExpTy ty_;
public:
    constexpr ExpressionTy(ExpTy ty) noexcept : ty_(ty) {}

    constexpr bool is(ExpTy ty) const noexcept {
        return ty_ == ty;
    }

    void dump(std::ostream & os = std::cout) const noexcept {
        switch (ty_) {
        case Constant:
            os << "Constant";
            break;
        case Variable:
            os << "Variable";
            break;
        case Expression:
            os << "Expression";
            break;
        default:
            os << "Unknown";
        }
        std::cout << '\n';
        return;
    }
};

class Expression {
    ExpressionTy ty_;
public:
    constexpr
    Expression(ExpressionTy ty = ExpressionTy::Expression) noexcept
        : ty_(ty) {}

    constexpr ExpressionTy getType() const noexcept {
        return ty_;
    }
    
    constexpr bool operator==(const Expression & rhs) const noexcept {
        return true;
    }
};

template <typename ItTy>
class LatexParser {
public:

    ;

private:

    const ItTy input_begin_, input_end_;
    std::remove_cvref_t<ItTy> cursor_;

    // should cursor be updated or not
    bool cur_upd_ = false;

public:

    template <
    typename = std::enable_if_t<
        std::conjunction_v<
    // ItTy(ItTy &&) <- defined ?
        // [ nothrow flag appears to be DIFFERENT by the compiler ]
        // gcc: move_constructible<std::istream_iterator>
        // clang: nothrow_move_constructible<std::istream_iterator>
            // std::is_nothrow_move_constructible<ItTy>,
            std::conditional_t<
                std::is_rvalue_reference_v<ItTy &&>,
                std::is_move_constructible<ItTy>,
                std::is_copy_constructible<ItTy>
                >,
    // static_cast<mpa::String>(*(++it)) safe?
        // 1) ++it `HasTypeOf` ItTy
            std::is_same< ItTy,
                std::decay_t<decltype(++std::declval<ItTy>())>
                >,
        // 2) *it `isConvertibleTo` mpa::String
            std::is_nothrow_convertible<
                mpa::String, // to
                decltype(*std::declval<ItTy>()) // from
            >
        // done
            >
        >>
    LatexParser(ItTy && begin, ItTy && end)
        noexcept(
            std::conditional_t<std::is_rvalue_reference_v<ItTy &&>,
                std::is_nothrow_move_constructible<ItTy>,
                std::is_nothrow_constructible<ItTy>
                >::value
                )
        : input_begin_(std::move(begin)), input_end_(std::move(end)),
          cursor_(input_begin_) {}

    Result<Expression> parseExpression(void) noexcept {

        using namespace exception;
        using namespace utils;

        auto & cur = cursor_;

        // update cursor
        if(cur_upd_) {
            cur_upd_ = false;
            ++cur;
        }

        static auto request_cursor_update = [this](void) -> void {
            cur_upd_ = true;
        };

        static auto throw_exit = []<typename T>(TypeBox<T>, auto &&... args) -> Result<Expression> {
            request_cursor_update();
            return Result<Expression>(make_exception<T>(std::forward<decltype(args)>(args)...));
        };

        std::cout << "*cur: \"" << *cur << "\"" << std::endl;

        if (cur == input_end_)
            return Result<Expression>(Expression{});

        if (*cur == (char)0)
            return Result<Expression>(make_exception<ParseException>("Reached EOF", Seriousness::Shutdown));

        if(String idf = *cur; idf.startsWith('\\')) {
            // "\..."
            
            using parser_type = std::function<Result<Expression>(void)>;

            static std::map<String, parser_type> map = {
                {
                    "\\frac",
                    []() -> Result<Expression> {
                        std::cout << "\\frac lambda\n";
                        return Result<Expression>(Expression{});
                    }
                },
                {
                    "\\sqrt",
                    []() -> Result<Expression> {
                        std::cout << "\\sqrt lambda\n";
                        return Result<Expression>(Expression{});
                    }
                },
                {
                    "\\lim",
                    []() -> Result<Expression> {
                        std::cout << "\\lim lambda\n";
                        return Result<Expression>(Expression{});
                    }
                }
            };

            if(auto parser = map[idf]; parser) {
                request_cursor_update();
                return parser();
            }

            // return throw_exit<ParseException>("Unknown command: \"" + idf + "\"");
            // return throw_exit.template operator()<ParseException>("Unknown command: \"" + idf + "\"");
            return throw_exit(wrap_type<ParseException>(), "Unknown command: \"" + idf + "\"");
        } else {

            static auto isNumber = [](const String::char_type & ch) -> bool {
                return (int)'0' <= ch && ch <= (int)'9';
            };

            if(isNumber(*idf.begin())) {
                if(auto pointCnt = idf.count('.')) {
                    if(pointCnt != 1)
                        return throw_exit(
                            wrap_type<ParseException>(),
                            "Invalid constant input \'" + idf + "\': "
                            "Only one point is allowed");
                    std::cout << pointCnt << std::endl;
                    
                }
            }

            ///

            if(std::all_of(idf.begin(), idf.end(), isNumber)) {
                // constant number
                request_cursor_update();
                return Result<Expression>(Expression{ExpressionTy::Constant});
            } else {
                // ".decimal" or any decimal that doesn't end with '.'
                if((idf.startsWith('.') && std::all_of(idf.begin() + 1, idf.end(), isNumber))
                    || ((idf.count('.') == 1) && !idf.endsWith('.'))) {
                    // decimal number
                    request_cursor_update();
                    return Result<Expression>(Expression{ExpressionTy::Constant});
                }

                return throw_exit(wrap_type<ParseException>(),
                    "Invalid decimal format \'" + idf + "\'");
            }

            request_cursor_update();
            return Result<Expression>(Expression{ExpressionTy::Constant});
        }

        ;

        return Result<Expression>(Expression{});
    }
};

} // ns latex
} // ns mpa

#endif