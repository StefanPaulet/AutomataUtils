//
// Created by stefan on 8/3/24.
//

#include <gtest/gtest.h>
#include <automata/dfa/DFA.hpp>
#include "utils/Utils.hpp"
#include <fstream>

namespace {
using namespace au;
using namespace test::dfa;

auto compare(DfaAutomata const& d1, DfaAutomata const& d2) {
  using State = DfaState*;
  std::unordered_map<State, State> stateEquivalence {};

  std::function<bool(State const&, State const&)> equivalent =
    [&stateEquivalence, &equivalent](State const& s1, State const& s2) -> bool {
    if (stateEquivalence.contains(s1)) {
      return stateEquivalence.at(s1) == s2;
    }
    if (s1->transitions().size() != s2->transitions().size()) {
      return false;
    }

    stateEquivalence.try_emplace(s1, s2);
    for (auto const& [sym, next1] : s1->transitions()) {
      if (!equivalent(next1, s2->next(sym))) {
        stateEquivalence.erase(s1);
        return false;
      }
    }
    return true;
  };

  return d1.size() == d2.size() && equivalent(d1.start(), d2.start());
}
}

TEST(DfaTest, Loop) {
  auto test1 = testMachine(2, {
  Edge {0, 1, 'a'},
  Edge {1, 0, 'a'}});
  auto test2 = testMachine(2, {
  Edge {0, 1, 'a'},
  Edge {1, 0, 'a'}});
  ASSERT_TRUE(compare(test1, test2));
}

TEST(DfaTest, FromNfa) {
  auto nfa = NfaAutomata {Regex {"(a|b)*abb"}};
  auto dfa = NfaToDfaParser {}.parse(nfa);
  auto target = testMachine(5, {
    Edge {0, 1, 'a'},
    Edge {0, 2, 'b'},
    Edge {1, 1, 'a'},
    Edge {1, 3, 'b'},
    Edge {2, 1, 'a'},
    Edge {2, 2, 'b'},
    Edge {3, 1, 'a'},
    Edge {3, 4, 'b'},
    Edge {4, 1, 'a'},
    Edge {4, 2, 'b'}
  });
  ASSERT_TRUE(compare(dfa, target));
}

TEST(DfaTest, Simulation) {
  auto regex = Regex("(a|b)*abb");
  auto dfa = NfaToDfaParser {}.parse(NfaAutomata {regex});

  auto acceptedString = "aabbaabb";
  auto rejectedString = "abbcdc";
  auto minimallyAcceptedStirng = "abb";
  auto partiallyMatchedString = "abbb";
  ASSERT_TRUE(std::get<0>(dfa.simulate(acceptedString)));
  ASSERT_FALSE(std::get<0>(dfa.simulate(rejectedString)));
  ASSERT_TRUE(std::get<0>(dfa.simulate(minimallyAcceptedStirng)));
  ASSERT_FALSE(std::get<0>(dfa.simulate(partiallyMatchedString)));
}

TEST(DfaTest, Minimization) {
  auto regex = Regex("(a|b)*abb");
  auto dfa = NfaToDfaParser {}.parse(NfaAutomata {regex});

  auto minimalDfa = dfa.minimize();

  auto target = testMachine(4, {
    Edge {0, 0, 'b'},
    Edge {0, 1, 'a'},
    Edge {1, 1, 'a'},
    Edge {1, 2, 'b'},
    Edge {2, 1, 'a'},
    Edge {2, 3, 'b'},
    Edge {3, 0, 'b'},
    Edge {3, 1, 'a'}
  });

  ASSERT_TRUE(compare(minimalDfa, target));
}

TEST(DfaTest, Minimization2) {
  auto dfa = testMachine(6, {
    Edge {0, 3, '0'},
    Edge {0, 1, '1'},
    Edge {1, 2, '0'},
    Edge {1, 5, '1'},
    Edge {2, 2, '0'},
    Edge {2, 5, '1'},
    Edge {3, 0, '0'},
    Edge {3, 4, '1'},
    Edge {4, 2, '0'},
    Edge {4, 5, '1'},
    Edge {5, 5, '0'},
    Edge {5, 5, '1'}
  });

  auto minimalDfa = dfa.minimize();

  auto target = testMachine(3, {
    Edge {0, 0, '0'},
    Edge {0, 1, '1'},
    Edge {1, 1, '0'},
    Edge {1, 2, '1'},
    Edge {2, 2, '0'},
    Edge {2, 2, '1'}
  });

  ASSERT_TRUE(compare(minimalDfa, target));
}