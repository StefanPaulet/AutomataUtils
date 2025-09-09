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

template <typename TransitionType, typename NextStateContainerType>
class State {
public:
  [[nodiscard]] auto next(TransitionType c) const -> NextStateWrapper<NextStateContainerType const> {
    if (auto it = _transitions.find(c); it != _transitions.end()) {
      return {it->second};
    }
    return {};
  }

  template <typename D>
  auto addTransition(this D& self, TransitionType sym, D const* state) -> void { self.addTransitionImpl(sym, state); }

  [[nodiscard]] auto const& transitions() const { return _transitions; }

  template <typename D>
  [[nodiscard]] auto nextStates(this D const& self) { return self.nextStatesImpl(); }

protected:
  std::unordered_map<TransitionType, NextStateContainerType> _transitions {};
};

} // namespace au