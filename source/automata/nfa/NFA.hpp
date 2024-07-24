//
// Created by stefan on 7/24/24.
//

#pragma once

#include <automata/Automata.hpp>
#include <automata/state/State.hpp>
#include <cassert>

namespace au {
class NfaAutomata : public Automata<NfaAutomata, NfaState> {
private:
  static auto character(char c) -> NfaAutomata {
    NfaAutomata res {};
    res._start->addTransition(c, res._accepting);
    return res;
  }

  static auto composition(NfaAutomata const& lhs, NfaAutomata const& rhs) -> NfaAutomata {
    NfaAutomata res;
    res._start = lhs._start;
    res._accepting = lhs._accepting;
    for (auto const& [chr, states] : rhs._start->transitions()) {
      for (auto const& state : states) {
        res._accepting->addTransition(chr, state);
      }
    }
    res._accepting = rhs._accepting;
    return res;
  }

  static auto disjunction(NfaAutomata const& lhs, NfaAutomata const& rhs) -> NfaAutomata {
    NfaAutomata res;
    res._start->addTransition(std::nullopt, lhs._start);
    res._start->addTransition(std::nullopt, rhs._start);
    lhs._accepting->addTransition(std::nullopt, res._accepting);
    rhs._accepting->addTransition(std::nullopt, res._accepting);
    return res;
  }

  static auto closure(NfaAutomata const& automata) -> NfaAutomata {
    NfaAutomata res;
    res._start->addTransition(std::nullopt, automata._start);
    res._start->addTransition(std::nullopt, res._accepting);
    automata._accepting->addTransition(std::nullopt, automata._start);
    automata._accepting->addTransition(std::nullopt, res._accepting);
    return res;
  }

public:
  using Automata::Automata;

  auto fromRegex(Regex const& regex) {
    std::stack<NfaAutomata> automataStack;
    auto getAutomata = [&automataStack]() {
      auto automata = automataStack.top();
      automataStack.pop();
      return automata;
    };

    auto operatorSwitch = [&getAutomata](Operator const& op) {
      using namespace operators;
      if (op == starOp) {
        return closure(getAutomata());
      }
      auto rhs = getAutomata();
      auto lhs = getAutomata();
      if (op == catOp) {
        return composition(lhs, rhs);
      }
      if (op == orOp) {
        return disjunction(lhs, rhs);
      }

      assert(false && "Unknown operator found");
    };

    using enum RegexSyntaxTreeTraversal::Order;
    auto&& tree = regex.parse();
    for (auto const node : RegexSyntaxTreeTraversal {POSTORDER, tree.root().get()}) {
      auto [isOp, op] = regex.operators().isOperator(node->_character);
      if (isOp) {
        automataStack.push(operatorSwitch(op));
      } else {
        automataStack.push(character(node->_character));
      }
    }

    *this = automataStack.top();
  }
};
} // namespace au
