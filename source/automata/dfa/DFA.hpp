//
// Created by stefan on 8/3/24.
//

#pragma once

#include "automata/Automata.hpp"
#include "automata/nfa/NFA.hpp"
#include "automata/state/DfaState.hpp"
#include <ranges>
#include <set>
#include <unordered_set>

namespace au {

class RegexToDfaParser;

class DfaAutomata : public Automata<DfaAutomata, DfaState> {
private:
  friend class RegexToDfaParser;

public:
  using Automata::Automata;
  using RegexParser = RegexToDfaParser;

  auto __simulate(std::string_view const str) const -> std::tuple<bool, DfaState const*> {
    auto* currentState = start().get();
    for (auto chr : str) {
      currentState = currentState->next(chr).get();
    }
    if (isAccepting(currentState)) {
      return {true, currentState};
    }
    return {false, nullptr};
  }

  [[nodiscard]] auto size() const -> unsigned int {
    auto size = 0;
    std::queue<DfaState*> stateQueue;
    std::set<DfaState*> stateSet;
    stateQueue.push(_start.get());
    stateSet.emplace(_start.get());
    while (!stateQueue.empty()) {
      auto* currState = stateQueue.front();
      ++size;
      stateQueue.pop();
      for (auto const& nextState : std::views::values(currState->transitions())) {
        if (auto raw = nextState.get(); !stateSet.contains(raw)) {
          stateSet.emplace(raw);
          stateQueue.push(raw);
        }
      }
    }
    return size;
  }
};

class RegexToDfaParser {
public:
  auto parse(Regex const& regex) { assert(false && "Regex to dfa parsing not implemented yet"); }
};

class NfaToDfaParser {
private:
  using StateSet = std::unordered_set<NfaState const*>;
  using StateQueue = std::queue<StateSet>;

  struct StateSetHasher {
    auto operator()(StateSet const& set) const noexcept -> std::size_t {
      auto stateHash = std::hash<NfaState const*> {};
      std::size_t hash = set.size();
      for (auto const* value : set) {
        hash ^= stateHash(value);
      }
      return hash;
    }
  };

public:
  auto parse(NfaAutomata const& source) {
    DfaAutomata dfa {};

    auto startState = source.closure(source.start().get());

    StateQueue stateQueue;
    stateQueue.push(startState);

    std::unordered_map<StateSet, std::shared_ptr<DfaState>, StateSetHasher> visitedStates;
    visitedStates.emplace(startState, dfa.start());

    std::vector<std::shared_ptr<DfaState>> endStates;

    while (!stateQueue.empty()) {
      auto currentState = stateQueue.front();
      stateQueue.pop();

      std::set<char> nextSyms {};
      for (auto const& curr : currentState) {
        for (auto const& sym : std::views::keys(curr->transitions())) {
          if (sym.has_value()) {
            nextSyms.emplace(sym.value());
          }
        }
      }

      for (auto const& nextSym : nextSyms) {
        auto nextState = source.closure(source.next(currentState, nextSym));
        auto nextStIt = visitedStates.find(nextState);
        if (nextStIt == visitedStates.end()) {
          nextStIt = visitedStates.emplace(nextState, std::make_unique<DfaState>()).first;
          stateQueue.push(nextState);
          if (std::get<0>(source.isAccepting(nextState))) {
            endStates.push_back(nextStIt->second);
          }
        }
        visitedStates.at(currentState)->addTransition(nextSym, nextStIt->second);
      }
    }

    dfa.accepting() = endStates.front();
    return dfa;
  }
};

} // namespace au