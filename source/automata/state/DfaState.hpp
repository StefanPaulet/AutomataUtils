//
// Created by stefan on 8/3/24.
//

#pragma once

#include "State.hpp"
#include <algorithm>
#include <ranges>
#include <unordered_set>

namespace au {

class DfaState : public State<DfaState, char, DfaState const*> {
private:
  using Base = State;
  friend class State;

public:
  using Base::addTransition;

  auto addTransition(std::optional<char>, DfaState*) -> void { throw exceptions::DfaEpsilonTransitionException {}; }

private:
  auto addTransitionImpl(char sym, DfaState* state) -> void {
    if (_transitions.contains(sym)) {
      throw exceptions::DfaConflictingTransitionException {sym};
    }
    _transitions.emplace(sym, state);
  }

  auto nextStatesImpl() const -> std::unordered_set<DfaState const*> {
    std::unordered_set<DfaState const*> result {};
    for (auto const& state : std::views::values(_transitions)) {
      result.insert(state);
    }
    return result;
  }
};

template <> struct DotNodePrinter<DfaState> {
  auto label(DfaState const* n) -> std::string {
    if (auto const it = _ids.find(n); it != _ids.end()) {
      return std::to_string(it->second);
    }
    return std::to_string(_ids.emplace(n, _idInc++).first->second);
  }

  auto colour([[maybe_unused]] DfaState const* _) const { return "black"; }

  unsigned int _idInc {};
  std::unordered_map<DfaState const*, unsigned> _ids {};
};

template <> struct DotEdgePrinter<DfaState> {
  auto operator()(DfaState const* n1, DfaState const* n2) const -> std::vector<std::string> {
    std::vector<std::string> result {};
    for (auto const& [sym, state] : n1->transitions()) {
      if (state == n2) {
        result.emplace_back(std::string(1, sym));
      }
    }
    return result;
  }
};

template <> struct GraphNodeChildren<DfaState> {
  auto operator()(DfaState const* n) const {
    std::vector<char> sortedSymbols {};
    for (auto const key : std::views::keys(n->transitions())) {
      sortedSymbols.push_back(key);
    }
    std::ranges::sort(sortedSymbols);

    std::unordered_set<DfaState const*> nodes {};
    std::vector<DfaState const*> result {};
    for (auto const& sym : sortedSymbols) {
      auto next {n->next(sym)};
      if (!nodes.contains(n->next(sym))) {
        nodes.emplace(next);
        result.emplace_back(next);
      }
    }
    return result;
  }
};
} // namespace au