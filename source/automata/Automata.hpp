//
// Created by stefan on 7/24/24.
//

#pragma once

#include <memory>
#include <regex/Regex.hpp>

namespace au {
template <typename D, typename StateType> class Automata {
public:
  Automata() = default;
  Automata(Automata const&) = default;
  Automata(Automata&&) noexcept = default;
  auto operator=(Automata const& other) -> Automata& = default;
  auto operator=(Automata&& other) noexcept -> Automata& = default;

  explicit Automata(Regex const& regex) { static_cast<D*>(this)->fromRegex(regex); }

  auto const& start() const { return _start; }
  auto const& accepting() const { return _accepting; }
  auto& start() { return _start; }
  auto& accepting() { return _accepting; }

protected:
  std::shared_ptr<StateType> _start {std::make_shared<StateType>()};
  std::shared_ptr<StateType> _accepting {std::make_shared<StateType>()};
};


} // namespace au