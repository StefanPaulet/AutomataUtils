//
// Created by stefan on 7/29/24.
//

#pragma once

#include <automata/dfa/DFA.hpp>
#include <automata/nfa/NFA.hpp>

namespace au::test {

struct Edge {
  unsigned int startIdx;
  unsigned int endIdx;
  std::optional<char> symbol;
};

template <typename Automata>
auto testMachine(unsigned int totalStates, std::vector<Edge> const& edges) {
  std::vector<std::shared_ptr<typename Automata::StateType>> container;
  container.reserve(totalStates);
  for (auto idx = 0; idx < totalStates; ++idx) {
    container.push_back(std::make_shared<typename Automata::StateType>());
  }

  Automata automata;
  automata.start() = container[0];
  automata.accepting() = container[totalStates - 1];

  for (auto const& edge : edges) {
    container[edge.startIdx]->addTransition(edge.symbol, container[edge.endIdx]);
  }

  return automata;
}

namespace nfa {
inline auto testMachine(unsigned int totalStates, std::vector<Edge> const& edges) {
  return testMachine<NfaAutomata>(totalStates, edges);
}
} // namespace nfa

namespace dfa {
inline auto testMachine(unsigned int totalStates, std::vector<Edge> const& edges) {
  return testMachine<DfaAutomata>(totalStates, edges);
}
}
} // namespace au::test