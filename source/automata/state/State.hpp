//
// Created by stefan on 7/23/24.
//

#pragma once

#include "Exceptions.hpp"
#include <memory>
#include <unordered_map>
#include <utils/Printer.hpp>
#include <vector>
namespace au {

template <typename D, typename TransitionType, typename StateContainerType> class State {
public:
  [[nodiscard]] auto next(TransitionType c) const -> StateContainerType const& {
    try {
      return _transitions.at(c);
    } catch (std::out_of_range const&) {
      return D::deadState();
    }
  }
  auto addTransition(TransitionType sym, D* state) -> void { static_cast<D*>(this)->__addTransition(sym, state); }
  [[nodiscard]] auto const& transitions() const { return _transitions; }
  [[nodiscard]] auto nextStates() const { return static_cast<D const*>(this)->__nextStates(); }

protected:
  std::unordered_map<TransitionType, StateContainerType> _transitions {};
};

} // namespace au