#include <iostream>

#include "latex/latex.hpp"

int main(int argc, char * argv[]) {

    using namespace mpa::latex;

    using ss_iterator = typename std::istream_iterator<mpa::String>;

    auto parser = LatexParser(ss_iterator(std::cin),
                              ss_iterator());
    ;

    while(true) {
        auto result = parser.parseExpression();

        result.operator Expression().getType().dump();

        std::cout << std::endl;

        // break;
    }

    return 0;
}