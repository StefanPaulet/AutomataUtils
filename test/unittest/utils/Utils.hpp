//
// Created by stefan on 7/29/24.
//

#pragma once

#include <automata/dfa/DFA.hpp>
#include <automata/nfa/NFA.hpp>

namespace au::test {

template <typename TransitionType>
struct Edge {
  unsigned int startIdx;
  unsigned int endIdx;
  TransitionType symbol;
};

template <typename Automata, typename TransitionType>
auto testMachine(unsigned int totalStates, std::vector<unsigned int> const& acceptingStates,
                 std::vector<Edge<TransitionType>> const& edges) {

  Automata automata {};

  std::vector<typename Automata::StateType*> container;
  container.reserve(totalStates);

  container.emplace_back(automata.start());
  for (auto idx = 1; idx < totalStates; ++idx) {
    container.emplace_back(automata.allocate());

    if (std::ranges::find(acceptingStates, idx) != acceptingStates.end()) {
      automata.markAccepting(container.back());
    }
  }

  for (auto const& edge : edges) {
    container[edge.startIdx]->addTransition(edge.symbol, container[edge.endIdx]);
  }

  return automata;
}

namespace nfa {
using Edge = Edge<std::optional<char>>;
inline auto testMachine(unsigned int const totalStates, std::vector<unsigned int> const& acceptingStates,
                        std::vector<Edge> const& edges) {
  return testMachine<NfaAutomata>(totalStates, acceptingStates, edges);
}
inline auto testMachine(unsigned int const totalStates, std::vector<Edge> const& edges) {
  return testMachine<NfaAutomata>(totalStates, {totalStates - 1}, edges);
}
} // namespace nfa

namespace dfa {
using Edge = Edge<char>;
inline auto testMachine(unsigned int const totalStates, std::vector<unsigned int> const& acceptingStates,
                        std::vector<Edge> const& edges) {
  return testMachine<DfaAutomata>(totalStates, acceptingStates, edges);
}
inline auto testMachine(unsigned int const totalStates, std::vector<Edge> const& edges) {
  return testMachine<DfaAutomata>(totalStates, {totalStates - 1}, edges);
}
}
} // namespace au::test