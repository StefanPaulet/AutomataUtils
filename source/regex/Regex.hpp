//
// Created by stefan on 7/10/24.
//

#pragma once

#include "RegexSyntaxTree.hpp"
#include <stack>
#include <string>

namespace au {

class Regex {
public:
  Regex() = default;
  Regex(Regex const&) = default;
  Regex(Regex&&) noexcept = default;
  explicit Regex(std::string&& string);

  [[nodiscard]] auto parse() const noexcept(false) -> RegexSyntaxTree;
  [[nodiscard]] auto string() const -> std::string_view { return _str; }
  [[nodiscard]] auto const& operators() const { return _operators; }

private:
  auto __parse(char currChar, std::stack<RegexSyntaxTree>& symbolStack, std::stack<Operator>& operatorStack) const
      noexcept(false) -> void;
  auto augment() -> void;

  std::string _str;
  Alphabet const& _alphabet {alphabet::alphanumeric};
  Operators const _operators {operators::catOp,  operators::orOp,          operators::starOp,
                              operators::plusOp, operators::parenthesesOp, operators::closedParenthesesOp};
};

} // namespace au
