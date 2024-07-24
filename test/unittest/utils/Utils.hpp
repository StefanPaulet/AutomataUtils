//
// Created by stefan on 7/29/24.
//

#pragma once

namespace au::test {

namespace nfa {
struct Edge {
  unsigned int startIdx;
  unsigned int endIdx;
  std::optional<char> symbol;
};

inline auto testMachine(unsigned int totalStates, std::vector<Edge> const& edges) {
  std::vector<std::shared_ptr<NfaState>> container;
  container.reserve(totalStates);
  for (auto idx = 0; idx < totalStates; ++idx) {
    container.push_back(std::make_shared<NfaState>());
  }

  NfaAutomata nfa;
  nfa.start() = container[0];
  nfa.accepting() = container[totalStates - 1];

  for (auto const& edge : edges) {
    container[edge.startIdx]->addTransition(edge.symbol, container[edge.endIdx]);
  }

  return nfa;
}
} // namespace nfa
} // namespace au::test