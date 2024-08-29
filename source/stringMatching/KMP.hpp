//
// Created by stefan on 8/29/24.
//

#pragma once

#include "automata/dfa/DFA.hpp"
#include "fallback/KmpFallbackMachine.hpp"


namespace au::sm {
class KMPMachine : public KmpFallbackMachine, public DfaAutomata {
public:
  explicit KMPMachine(std::string&& str) : KmpFallbackMachine {str}, DfaAutomata {Regex {std::move(str)}} {}

  auto match(std::string_view const text) const {
    std::vector<unsigned int> matches {};
    auto const& vec = DfaAutomata::objects();
    auto* state = DfaAutomata::start();

    for (auto idx = 0u, matchIdx = 0u; idx < text.size();) {
      state = state->next(text[idx]);
      if (state == DfaState::deadState()) {
        if (matchIdx != 0) {
          auto fallbackIdx = KmpFallbackMachine::get(matchIdx - 1);
          state = vec[fallbackIdx].get();
          matchIdx = fallbackIdx;
        } else {
          ++idx;
          state = DfaAutomata::start();
        }
      } else {
        if (DfaAutomata::isAccepting(state)) {
          matches.push_back(idx - matchIdx);
          auto fallbackIdx = KmpFallbackMachine::get(matchIdx);
          state = vec[fallbackIdx].get();
          matchIdx = fallbackIdx;
        } else {
          ++idx;
          ++matchIdx;
        }
      }
    }
    if (DfaAutomata::isAccepting(state)) {
      matches.push_back(text.size() - objects().size());
    }

    return matches;
  }
};

} // namespace au::sm