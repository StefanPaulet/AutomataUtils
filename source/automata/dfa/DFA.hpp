//
// Created by stefan on 8/3/24.
//

#pragma once

#include "automata/Automata.hpp"
#include "automata/nfa/NFA.hpp"
#include "automata/state/DfaState.hpp"
#include "utils/Hasher.hpp"
#include <ranges>
#include <set>
#include <unordered_set>

namespace au {

class RegexToDfaParser;
class DfaMinimizer;

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

  template <typename M = DfaMinimizer> [[nodiscard]] auto minimize() const -> DfaAutomata {
    return M {}.minimize(*this);
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

public:
  auto parse(NfaAutomata const& source) {
    DfaAutomata dfa {};

    auto startState = source.closure(source.start().get());

    StateQueue stateQueue;
    stateQueue.push(startState);

    std::unordered_map<StateSet, std::shared_ptr<DfaState>, SetHasher> visitedStates;
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

class DfaMinimizer {
private:
  using StateSet = std::vector<DfaState const*>;
  using Partition = std::vector<StateSet>;
  using StateQueue = std::queue<StateSet>;

  template <typename P> auto findGroup(P&& partition, DfaState const* state) const {
    for (auto& set : std::forward<P>(partition)) {
      if (std::ranges::find(set, state) != set.end()) {
        return set;
      }
    }
    assert(false && "State not part of any set found");
  }

  auto mergeParitions(Partition const& first, Partition const& second) const -> Partition {
    Partition result;
    for (auto const& fs : first) {
      for (auto const& ss : second) {
        auto intersection = StateSet {};
        for (auto const* el : fs) {
          if (std::ranges::find(ss, el) != ss.end()) {
            intersection.push_back(el);
          }
        }
        result.push_back(intersection);
      }
    }

    return result;
  }

  auto initialPartition(DfaAutomata const& automata) const -> Partition {
    Partition result {2};

    for (auto const* state : automata.states()) {
      if (automata.isAccepting(state)) {
        result[1].push_back(state);
      } else {
        result[0].push_back(state);
      }
    }

    return result;
  }

  auto makePartition(Partition const& currentPartition, StateSet const& set) const {
    std::unordered_set<char> symSet;
    Partition newSets {};
    newSets.emplace_back();

    for (auto const* state : set) {
      newSets.back().push_back(state);
      for (auto const sym : std::views::keys(state->transitions())) {
        symSet.emplace(sym.value());
      }
    }

    for (auto const sym : symSet) {
      std::unordered_map<StateSet, StateSet, SetHasher> transitionMap {};
      for (auto const* state : set) {
        auto nextState = state->next(sym);
        StateSet targetSet;
        if (nextState != DfaState::deadState) {
          targetSet = findGroup(currentPartition, nextState.get());
        } else {
          continue;
        }
        transitionMap[targetSet].push_back(state);
      }
      if (transitionMap.size() == 1) {
        continue;
      }

      Partition newPartition;
      for (auto const& [_, newSet] : transitionMap) {
        newPartition.emplace_back();
        for (auto const* el : newSet) {
          newPartition.back().push_back(el);
        }
      }

      newSets = mergeParitions(newSets, newPartition);
    }

    return newSets;
  }

public:
  auto minimize(DfaAutomata const& dfa) const -> DfaAutomata {
    auto part = initialPartition(dfa);

    for (auto modified = true; modified;) {
      modified = false;
      for (auto oldPart = part; auto const& group : oldPart) {
        if (auto newGroups = makePartition(oldPart, group); newGroups.size() > 1) {
          std::erase(part, group);
          for (auto const& newGroup : newGroups) {
            part.emplace_back(newGroup);
          }
          modified = true;
        }
      }
    }

    std::unordered_map<StateSet, std::shared_ptr<DfaState>, SetHasher> representatives {};
    for (auto const& group : part) {
      representatives.emplace(group, std::make_shared<DfaState>());
    }

    for (auto const& group : part) {
      for (auto const& [sym, link] : group.front()->transitions()) {
        auto targetGroup = findGroup(part, link.get());
        representatives.at(group)->addTransition(sym, representatives.at(targetGroup));
      }
    }

    return DfaAutomata {representatives.at(findGroup(part, dfa.start().get())),
                        representatives.at(findGroup(part, dfa.accepting().get()))};
  }
};

} // namespace au