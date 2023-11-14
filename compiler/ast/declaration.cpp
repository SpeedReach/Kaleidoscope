//
// Created by gomil on 2023/11/14.
//
#include "include/declaration.h"

const std::string& VariableDeclaration::getIdentifier() const noexcept  {
    return identifier;
}

const std::string& CallableDeclaration::getIdentifier() const noexcept {
    return name;
}

const std::string& CallableImplementation::getIdentifier() const noexcept {
    return name;
}
