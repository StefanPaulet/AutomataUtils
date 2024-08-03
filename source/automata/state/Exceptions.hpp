//
// Created by stefan on 8/3/24.
//


#pragma once

#include "utils/Exceptions.hpp"
#include <format>
#include <optional>

namespace au::exceptions {

class DfaConflictingTransitionException : public Exception {
public:
  explicit DfaConflictingTransitionException(std::optional<char> const& sym) :
      Exception {std::format("Multiple transitions in dfa on symbol {}",
                             sym.has_value() ? std::string(1, sym.value()) : "eps")} {}
};

class DfaEpsilonTransitionException : public Exception {
public:
  explicit DfaEpsilonTransitionException() : Exception {"Cannot insert an epsilon transition on DFA"} {}
};
} // namespace au::exceptions
