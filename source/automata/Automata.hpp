//
// Created by stefan on 7/24/24.
//

#pragma once

#include <memory>
#include <regex/Regex.hpp>

namespace au {
template <typename D, typename ST> class Automata {
public:
  using StateType = ST;

  Automata() = default;
  Automata(Automata const&) = default;
  Automata(Automata&&) noexcept = default;
  Automata(std::shared_ptr<StateType> start, std::shared_ptr<StateType> accepting) :
      _start {start}, _accepting {_accepting} {}
  auto operator=(Automata const& other) -> Automata& = default;
  auto operator=(Automata&& other) noexcept -> Automata& = default;

  explicit Automata(Regex const& regex) { *this = typename D::RegexParser {}.parse(regex); }

  auto const& start() const { return _start; }

  auto const& accepting() const { return _accepting; }

  auto& start() { return _start; }

  auto& accepting() { return _accepting; }

  auto simulate(std::string_view string) const -> std::tuple<bool, StateType const*> {
    return static_cast<D const*>(this)->__simulate(string);
  }

  auto next(StateType const* state, std::optional<char> sym) const { return state->next(sym); }

  auto isAccepting(StateType const* state) const -> bool { return state == _accepting.get(); }

  template <typename Range, std::enable_if_t<!std::is_pointer_v<std::remove_cvref_t<Range>>, int> = 0>
  auto isAccepting(Range&& states) const -> std::tuple<bool, StateType const*> {
    for (auto const* state : std::forward<Range>(states)) {
      if (isAccepting(state)) {
        return {true, state};
      }
    }
    return {false, nullptr};
  }

protected:
  std::shared_ptr<StateType> _start {std::make_shared<StateType>()};
  std::shared_ptr<StateType> _accepting {std::make_shared<StateType>()};
};


} // namespace au