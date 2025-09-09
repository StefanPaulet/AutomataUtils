//
// Created by stefan on 8/3/24.
//

#pragma once

#include "State.hpp"
#include <ranges>

namespace au {


class NfaState : public State<NfaState, std::optional<char>, std::vector<NfaState const*>> {
private:
public:
  using StateType = NfaState*;

  auto addTransition(std::optional<char> sym, NfaState const* state) -> void { _transitions[sym].push_back(state); }

  auto nextStates() const -> std::unordered_set<NfaState const*> {
    std::unordered_set<NfaState const*> result {};
    for (auto const& next : std::views::values(_transitions)) {
      for (auto const& state : next) {
        result.insert(state);
      }
    }
    return result;
  }
};

template <> struct DotNodePrinter<NfaState> {
  auto label(NfaState const* n) -> std::string {
    if (auto const it = _ids.find(n); it != _ids.end()) {
      return std::to_string(it->second);
    }
    return std::to_string(_ids.emplace(n, _idInc++).first->second);
  }

  auto colour([[maybe_unused]] NfaState const*) const { return "black"; }

  unsigned int _idInc {};
  std::unordered_map<NfaState const*, unsigned> _ids {};
};

template <> struct DotEdgePrinter<NfaState> {
  auto operator()(NfaState const* n1, NfaState const* n2) const -> std::vector<std::string> {
    std::vector<std::string> result {};
    for (auto const& [sym, states] : n1->transitions()) {
      for (auto const& state : states) {
        if (state == n2) {
          if (sym.has_value()) {
            result.emplace_back(std::string(1, sym.value()));
          } else {
            result.emplace_back("eps");
          }
        }
      }
    }
    return result;
  }
};

template <> struct GraphNodeChildren<NfaState> {
  auto operator()(NfaState const* n) const {
    std::unordered_set<NfaState const*> nodes {};
    std::vector<NfaState const*> result {};
    for (auto const& [_, states] : n->transitions()) {
      for (auto const& state : states) {
        auto next = state;
        if (!nodes.contains(next)) {
          nodes.emplace(next);
          result.emplace_back(next);
        }
      }
    }
    return result;
  }
};
} // namespace au