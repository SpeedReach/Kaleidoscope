//
// Created by gomil on 2023/11/18.
//

#ifndef KALEIDOSCOPE_NONNULL_H
#define KALEIDOSCOPE_NONNULL_H

#include <type_traits>

template <typename T,
        typename std::enable_if_t<std::is_pointer_v<T>>* = nullptr>
using Nonnull = T _Nonnull;
#endif //KALEIDOSCOPE_NONNULL_H
