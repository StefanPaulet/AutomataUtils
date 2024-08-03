//
// Created by stefan on 8/3/24.
//

#pragma once

#include "State.hpp"
#include <algorithm>
#include <ranges>

namespace au {

class DfaState : public State<DfaState, std::shared_ptr<DfaState>> {
public:
  auto __addTransition(std::optional<char> sym, std::shared_ptr<DfaState> const& state) -> void {
    if (sym == std::nullopt) {
      throw exceptions::DfaEpsilonTransitionException {};
    }
    if (_transitions.contains(sym)) {
      throw exceptions::DfaConflictingTransitionException {sym};
    }
    _transitions.emplace(sym, state);
  }

  static inline std::shared_ptr<DfaState> deadState = std::make_shared<DfaState>();
};

template <> struct DotNodePrinter<DfaState> {
  auto label(DfaState const* n) -> std::string {
    if (auto const it = _ids.find(n); it != _ids.end()) {
      return std::to_string(it->second);
    }
    return std::to_string(_ids.emplace(n, _idInc++).first->second);
  }

  auto colour(DfaState const* n) const { return "black"; }

  unsigned int _idInc {};
  std::unordered_map<DfaState const*, unsigned> _ids {};
};

template <> struct DotEdgePrinter<DfaState> {
  auto operator()(DfaState const* n1, DfaState const* n2) const -> std::string {
    for (auto const& [sym, state] : n1->transitions()) {
      if (state.get() == n2) {
        return std::string(1, sym.value());
      }
    }
    return "";
  }
};

template <> struct GraphNodeChildren<DfaState> {
  auto operator()(DfaState const* n) const {
    std::vector<char> sortedSymbols {};
    for (auto const key : std::views::keys(n->transitions())) {
      sortedSymbols.push_back(key.value());
    }
    std::ranges::sort(sortedSymbols);

    std::vector<DfaState const*> nodes {};
    for (auto const& sym : sortedSymbols) {
      nodes.push_back(n->next(sym).get());
    }
    return nodes;
  }
};
} // namespace au