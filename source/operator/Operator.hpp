//
// Created by stefan on 7/12/24.
//

#pragma once

#include <vector>

namespace au {
struct Operator {
  char priority {};
  char op {};
  bool unary {false};

  auto operator==(Operator const& other) const -> bool { return op == other.op; }
};

namespace operators {
constexpr Operator catOp {9, '.'};
constexpr Operator orOp {8, '|'};
constexpr Operator starOp {11, '*', true};
constexpr Operator plusOp {11, '+', true};
constexpr Operator parenthesesOp {0, '('};
constexpr Operator closedParenthesesOp {0, ')', true};

} // namespace operators

struct Operators {
  Operators() = default;
  Operators(Operators const&) = default;
  Operators(Operators&&) noexcept = default;

  Operators(std::initializer_list<Operator> operators) {
    std::ranges::for_each(operators, [this](Operator const& op) { ops.push_back(op); });
  }

  [[nodiscard]] auto isOperator(char c) const -> std::pair<bool, Operator> {
    auto opIt = std::ranges::find(ops, c, [](Operator const& op) { return op.op; });
    auto isOperator = (opIt != ops.end());
    return {isOperator, isOperator ? *opIt : Operator {}};
  }

  std::vector<Operator> ops {};
};

} // namespace au