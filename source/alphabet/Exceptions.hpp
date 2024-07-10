//
// Created by stefan on 7/10/24.
//

#pragma once

#include <exception>

namespace au::exceptions {
class AlphabetRangeException : public std::exception {
public:
  [[nodiscard]] char const* what() const noexcept override {
    return "Attempting to create alphabet with range where begin beyond end";
  }
};
} // namespace au::exceptions