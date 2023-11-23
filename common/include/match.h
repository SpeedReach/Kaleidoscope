//
// Created by gomil on 2023/11/21.
//

#ifndef KALEIDOSCOPE_MATCH_H
#define KALEIDOSCOPE_MATCH_H

#include <variant>

// https://en.cppreference.com/w/cpp/utility/variant/visit
template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
// explicit deduction guide (not needed as of C++20)
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

template <typename Val, typename... Ts>
auto match(Val val, Ts... ts) {
    return std::visit(overloaded{ts...}, val);
}

#endif //KALEIDOSCOPE_MATCH_H
