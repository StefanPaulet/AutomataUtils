//
// Created by stefan on 7/24/24.
//

#pragma once

#include <automata/Automata.hpp>
#include <automata/state/NfaState.hpp>
#include <cassert>
#include <type_traits>
#include <unordered_set>

namespace au {

class RegexToNfaParser;

class NfaAutomata : public Automata<NfaAutomata, NfaState> {
private:
  friend class RegexToNfaParser;
  friend class NfaToDfaParser;

  using Base = Automata<NfaAutomata, NfaState>;

  template <typename Range> auto next(Range&& states, std::optional<char> symbol) const {
    std::unordered_set<NfaState const*> nextStates {};
    for (auto&& state : std::forward<Range>(states)) {
      for (auto const& nextState : state->next(symbol)) {
        nextStates.insert(nextState.get());
      }
    }
    return nextStates;
  }

  auto closure(NfaState const* state) const { return closure(std::array {state}); }

  template <typename Range, std::enable_if_t<!std::is_same_v<std::remove_cvref_t<Range>, NfaState*>, int> = 0>
  auto closure(Range&& states) const -> std::unordered_set<NfaState const*> {
    std::unordered_set<NfaState const*> closureStates {};
    std::queue<NfaState const*> newStates {};
    for (auto const* state : std::forward<Range>(states)) {
      newStates.push(state);
      closureStates.insert(state);
    }
    while (!newStates.empty()) {
      auto const* currentState = newStates.front();
      newStates.pop();
      for (auto&& nextState : currentState->next(std::nullopt)) {
        if (!closureStates.contains(nextState.get())) {
          closureStates.insert(nextState.get());
          newStates.push(nextState.get());
        }
      }
    }
    return closureStates;
  }

public:
  using RegexParser = RegexToNfaParser;
  using Automata::Automata;

  auto __simulate(std::string_view const str) const -> std::tuple<bool, NfaState const*> {
    auto currentState = closure(start().get());
    for (auto const c : str) {
      currentState = closure(next(currentState, c));
    }
    return isAccepting(currentState);
  }
};

class RegexToNfaParser {
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
    for (auto const& [sym, states] : rhs._start->transitions()) {
      for (auto const& state : states) {
        res._accepting->addTransition(sym, state);
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

  static auto kleeneClosure(NfaAutomata const& automata) -> NfaAutomata {
    NfaAutomata res;
    res._start->addTransition(std::nullopt, automata._start);
    res._start->addTransition(std::nullopt, res._accepting);
    automata._accepting->addTransition(std::nullopt, automata._start);
    automata._accepting->addTransition(std::nullopt, res._accepting);
    return res;
  }

public:
  auto parse(Regex const& regex) {
    std::stack<NfaAutomata> automataStack;
    auto getAutomata = [&automataStack]() {
      auto automata = automataStack.top();
      automataStack.pop();
      return automata;
    };

    auto operatorSwitch = [&getAutomata](Operator const& op) {
      using namespace operators;
      if (op == starOp) {
        return kleeneClosure(getAutomata());
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
    for (auto&& tree = regex.parse(); auto const node : RegexSyntaxTreeTraversal {POSTORDER, tree.root().get()}) {
      if (auto [isOp, op] = regex.operators().isOperator(node->_character); isOp) {
        automataStack.push(operatorSwitch(op));
      } else {
        automataStack.push(character(node->_character));
      }
    }

    return automataStack.top();
  }
};

} // namespace au
