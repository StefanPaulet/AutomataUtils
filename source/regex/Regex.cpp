//
// Created by stefan on 7/10/24.
//

#include "Regex.hpp"
#include "Exceptions.hpp"
#include <exception>
#include <stack>

namespace au {
Regex::Regex(std::string&& string) : _str {std::move(string)} { augment(); }

namespace {
auto treeFromOp(Operator const& op, std::stack<RegexSyntaxTree>& stack) noexcept(false) -> void {
  RegexSyntaxTree newTree {};
  if (!op.unary) {
    newTree.root()->_pRight = std::move(stack.top().root());
    stack.pop();
    if (stack.empty()) {
      throw exceptions::MissingOperandException {op};
    }
    newTree.root()->_pLeft = std::move(stack.top().root());
    stack.pop();
  } else {
    if (stack.empty()) {
      throw exceptions::MissingOperandException {op};
    }
    newTree.root()->_pLeft = std::move(stack.top().root());
    stack.pop();
  }
  newTree.root()->_character = op.op;
  stack.push(std::move(newTree));
}

auto leftoverParse(std::stack<RegexSyntaxTree>& symbolStack, std::stack<Operator>& operatorStack)
  noexcept(false) -> RegexSyntaxTree {
  while (!operatorStack.empty()) {
    auto top = operatorStack.top();
    if (top == operators::parenthesesOp) {
      throw exceptions::HangingParenthesesException {};
    }
    treeFromOp(top, symbolStack);
    operatorStack.pop();
  }

  return std::move(symbolStack.top());
}
} // namespace

auto Regex::parse() const noexcept(false) -> RegexSyntaxTree {
  std::stack<RegexSyntaxTree> symbolStack {};
  std::stack<Operator> operatorStack {};

  auto pos = 0uL;
  for (; pos < _str.size(); ++pos) {
    try {
      __parse(_str[pos], symbolStack, operatorStack);
    } catch (exceptions::NonPositionalException& e) {
      throw exceptions::PositionalException<std::remove_cvref_t<decltype(e)>> {e, pos};
    } catch (exceptions::Exception&) {
      throw;
    }
  }

  try {
    return leftoverParse(symbolStack, operatorStack);
  } catch (exceptions::NonPositionalException& e) {
    throw exceptions::PositionalException<std::remove_cvref_t<decltype(e)>> {e, pos};
  } catch (exceptions::Exception&) {
    throw;
  }
}

auto Regex::__parse(char currChar, std::stack<RegexSyntaxTree>& symbolStack, std::stack<Operator>& operatorStack) const
    noexcept(false) -> void {
  if (_alphabet.contains(currChar)) {
    symbolStack.emplace(currChar);
    return;
  }
  if (currChar == '(') {
    operatorStack.push(operators::parenthesesOp);
    return;
  }
  if (currChar == ')') {
    while (!operatorStack.empty() && operatorStack.top() != operators::parenthesesOp) {
      treeFromOp(operatorStack.top(), symbolStack);
      operatorStack.pop();
    }
    if (operatorStack.empty() || operatorStack.top() != operators::parenthesesOp) {
      throw exceptions::UnmatchedParenthesesException {};
    }
    operatorStack.pop();
    return;
  }
  if (auto [isOp, op] = _operators.isOperator(currChar); isOp) {
    while (!operatorStack.empty() && op.priority <= operatorStack.top().priority) {
      treeFromOp(operatorStack.top(), symbolStack);
      operatorStack.pop();
    }
    operatorStack.push(op);
    return;
  }

  throw exceptions::UnrecognizedSymbolException {currChar};
}

auto Regex::augment() -> void {
  std::string result {};
  auto needsEnhancement = [this](char prev, char current) {
    auto prevInAlphabet = _alphabet.contains(prev);

    auto [isPrevOp, prevOp] = _operators.isOperator(prev);
    if (_alphabet.contains(current)) {
      auto condition = prevInAlphabet;
      if (isPrevOp) {
        condition = condition || prevOp.unary;
      }
      return condition;
    }
    return (current == '(') && (prevInAlphabet || isPrevOp && prevOp.unary);
  };
  result.push_back(_str[0]);
  for (auto idx = 1; idx < _str.size(); ++idx) {
    if (needsEnhancement(_str[idx - 1], _str[idx])) {
      result.push_back(operators::catOp.op);
    }
    result.push_back(_str[idx]);
  }

  _str = result;
}

} // namespace au