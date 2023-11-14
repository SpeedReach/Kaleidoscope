//
// Created by gomil on 2023/11/12.
//

#ifndef KALEIDOSCOPE_KALE_ERROR_H
#define KALEIDOSCOPE_KALE_ERROR_H

#include <string>
#include <variant>

struct Success {};

template <typename T>
class [[nodiscard]] ErrorOr {
public:
    // Constructs with an error; the error must not be Error::Success().
    // Implicit for easy construction on returns.
    // NOLINTNEXTLINE(google-explicit-constructor)
    ErrorOr(std::string err) : val_(std::move(err)) {}

    // Constructs with a value.
    // Implicit for easy construction on returns.
    // NOLINTNEXTLINE(google-explicit-constructor)
    ErrorOr(T val) : val_(std::move(val)) {}

    // Returns true for success.
    [[nodiscard]] auto ok() const -> bool { return std::holds_alternative<T>(val_); }

    [[nodiscard]] auto error() const& -> const std::string& {
        return std::get<std::string>(val_);
    }
    auto error() && -> std::string {
        return std::get<std::string>(std::move(val_));
    }

    // Returns the contained value.
    // REQUIRES: `ok()` is true.
    auto operator*() -> T& {
        return std::get<T>(val_);
    }

    // Returns the contained value.
    // REQUIRES: `ok()` is true.
    auto operator*() const -> const T& {
        return std::get<T>(val_);
    }

    // Returns the contained value.
    // REQUIRES: `ok()` is true.
    auto operator->() -> T* {
        return &std::get<T>(val_);
    }

    // Returns the contained value.
    // REQUIRES: `ok()` is true.
    auto operator->() const -> const T* {
        return &std::get<T>(val_);
    }

private:
    // Either an error message or a value.
    std::variant<std::string , T> val_;
};

#endif //KALEIDOSCOPE_KALE_ERROR_H
