//
// Created by stefan on 7/12/24.
//

#pragma once

#include "utils/Exceptions.hpp"

namespace au::exceptions {

class NonPositionalException : public Exception {
public:
  using Exception::Exception;
};

namespace concepts {
template <typename E> concept NPE = std::is_base_of_v<NonPositionalException, E>;
} // namespace concepts

template <concepts::NPE> class PositionalException : public Exception {
public:
  PositionalException(Exception const& base, std::size_t charPos) :
      Exception {std::format("{} on position {}", base.what(), charPos)} {}
};

class MissingOperandException : public NonPositionalException {
public:
  explicit MissingOperandException(Operator const& op) :
      NonPositionalException {std::format("Missing operand for {}", op.op)} {}
};

class UnrecognizedSymbolException : public NonPositionalException {
public:
  explicit UnrecognizedSymbolException(char symbol) :
      NonPositionalException {std::format("Unrecognized symbol {}", symbol)} {}
};

class UnmatchedParenthesesException : public NonPositionalException {
public:
  explicit UnmatchedParenthesesException() : NonPositionalException {"Unmatched parentheses"} {}
};

class HangingParenthesesException : public Exception {
public:
  HangingParenthesesException() : Exception {"Opened parentheses not closed"} {}
};
} // namespace au::exceptions
