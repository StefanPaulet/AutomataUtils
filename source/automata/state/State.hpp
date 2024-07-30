//
// Created by stefan on 7/23/24.
//

#pragma once

#include <memory>
#include <unordered_map>
#include <utils/Printer.hpp>
#include <vector>

namespace au {

template <typename D, typename StateContainerType> class State {
public:
  [[nodiscard]] auto next(std::optional<char> c) const -> StateContainerType const& {
    try {
      return _transitions.at(c);
    } catch (std::out_of_range const&) {
      return D::deadState;
    }
  }
  auto addTransition(std::optional<char> sym, std::shared_ptr<D> const& state) -> void {
    static_cast<D*>(this)->__addTransition(sym, state);
  }
  [[nodiscard]] auto const& transitions() const { return _transitions; }

protected:
  std::unordered_map<std::optional<char>, StateContainerType> _transitions {};
};

class NfaState : public State<NfaState, std::vector<std::shared_ptr<NfaState>>> {
public:
  auto __addTransition(std::optional<char> sym, std::shared_ptr<NfaState> const& state) -> void {
    _transitions[sym].push_back(state);
  }

  static inline std::vector<std::shared_ptr<NfaState>> deadState;
};

template <> struct DotNodePrinter<NfaState> {
  auto label(NfaState const* n) -> std::string {
    if (auto const it = _ids.find(n); it != _ids.end()) {
      return std::to_string(it->second);
    }
    return std::to_string(_ids.emplace(n, _idInc++).first->second);
  }

  auto colour(NfaState const* n) const { return "black"; }

  unsigned int _idInc {};
  std::unordered_map<NfaState const*, unsigned> _ids {};
};

template <> struct DotEdgePrinter<NfaState> {
  auto operator()(NfaState const* n1, NfaState const* n2) const -> std::string {
    for (auto const& [sym, states] : n1->transitions()) {
      for (auto const& state : states) {
        if (state.get() == n2) {
          if (sym.has_value()) {
            return std::string(1, sym.value());
          }
          return "eps";
        }
      }
    }
    return "";
  }
};

template <> struct GraphNodeChildren<NfaState> {
  auto operator()(NfaState const* n) const {
    std::vector<NfaState const*> nodes;
    for (auto const& [_, states] : n->transitions()) {
      for (auto const& state : states) {
        nodes.push_back(state.get());
      }
    }
    return nodes;
  }
};


} // namespace au