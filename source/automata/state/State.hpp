//
// Created by stefan on 7/23/24.
//

#pragma once

#include "Exceptions.hpp"
#include "NextState.hpp"
#include <memory>
#include <unordered_map>
#include <utils/Printer.hpp>
#include <vector>

namespace au {

template <typename Derived, typename TransitionType, typename NextStateContainerType>
class State {
public:
  [[nodiscard]] auto next(TransitionType c) const -> NextStateWrapper<NextStateContainerType const> {
    if (auto it = _transitions.find(c); it != _transitions.end()) {
      return {it->second};
    }
    return {};
  }

  auto addTransition(TransitionType sym, Derived* state) -> void { static_cast<Derived*>(this)->addTransitionImpl(sym, state); }
  [[nodiscard]] auto const& transitions() const { return _transitions; }
  [[nodiscard]] auto nextStates() const { return static_cast<Derived const*>(this)->nextStatesImpl(); }

protected:
  std::unordered_map<TransitionType, NextStateContainerType> _transitions {};
};

} // namespace au