//
// Created by stefan on 7/24/24.
//

#pragma once

#include <memory>
#include <regex/Regex.hpp>
#include <unordered_set>

namespace au {
template <typename StateType> struct StateAllocator {
  StateAllocator() = default;
  StateAllocator(StateAllocator const&) = delete;
  StateAllocator(StateAllocator&& other) noexcept { takeOwnership(std::move(other).container); }
  explicit StateAllocator(std::vector<std::unique_ptr<StateType>>&& source) { takeOwnership(std::move(source)); }

  auto takeOwnership(std::vector<std::unique_ptr<StateType>>&& other) {
    for (auto&& node : other) {
      container.emplace_back(std::move(node));
    }
  }
  auto takeOwnership(StateAllocator&& other) { takeOwnership(std::move(other).container); }

  auto operator()() {
    container.emplace_back(std::make_unique<StateType>());
    return container.back().get();
  }
  auto operator()(StateType const* ptr) {
    container.emplace_back(std::make_unique<StateType>(ptr));
    return container.back().get();
  }

private:
  std::vector<std::unique_ptr<StateType>> container {};
};

template <typename D, typename ST> class Automata : public StateAllocator<ST> {
public:
  using StateType = ST;
  using Allocator = StateAllocator<StateType>;
  using Allocator::Allocator;

  Automata() = default;
  Automata(Automata const&) = default;
  Automata(Automata&&) noexcept = default;
  Automata(StateType const* start, StateType const* accepting) :
      Allocator {}, _start {this->operator()(start)}, _accepting {this->operator()(accepting)} {}
  auto operator=(Automata const& other) -> Automata& = default;
  auto operator=(Automata&& other) noexcept -> Automata& = default;

  explicit Automata(Regex const& regex) : Automata {std::move(typename D::RegexParser {}.parse(regex))} {}

  auto allocate() { return static_cast<Allocator*>(this)->operator()(); }

  auto const& start() const { return _start; }

  auto const& accepting() const { return _accepting; }

  auto& start() { return _start; }

  auto& accepting() { return _accepting; }

  auto simulate(std::string_view string) const -> std::tuple<bool, StateType const*> {
    return static_cast<D const*>(this)->__simulate(string);
  }

  auto next(StateType const* state, std::optional<char> sym) const { return state->next(sym); }

  auto isAccepting(StateType const* state) const -> bool { return state == _accepting; }

  template <typename Range, std::enable_if_t<!std::is_pointer_v<std::remove_cvref_t<Range>>, int> = 0>
  auto isAccepting(Range&& states) const -> std::tuple<bool, StateType const*> {
    for (auto const* state : std::forward<Range>(states)) {
      if (isAccepting(state)) {
        return {true, state};
      }
    }
    return {false, nullptr};
  }

  auto states() const -> std::vector<StateType const*> {
    std::vector<StateType const*> stateSet;
    stateSet.push_back(_start);

    for (auto currIdx = 0ul, lastIdx = stateSet.size(); currIdx < lastIdx; ++currIdx) {
      auto const* currentState = stateSet[currIdx];
      for (auto const* next : currentState->nextStates()) {
        if (std::ranges::find(stateSet, next) == stateSet.end()) {
          stateSet.emplace_back(next);
          ++lastIdx;
        }
      }
    }
    return stateSet;
  }

  auto size() const { return states().size(); }

protected:
  StateType* _start {allocate()};
  StateType* _accepting {allocate()};
};


} // namespace au