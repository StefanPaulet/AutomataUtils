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

} // namespace au