//
// Created by stefan on 7/25/24.
//


#include <automata/nfa/NFA.hpp>
#include <gtest/gtest.h>
#include <ranges>
#include <fstream>
#include "utils/Utils.hpp"

namespace {
using namespace au;
using namespace test::nfa;

auto compare(NfaAutomata const& n1, NfaAutomata const& n2) {
  using State = NfaState*;
  std::unordered_map<State, State> stateEquivalence {};

  class EquivalenceMaintainer {
  private:
    using Map = std::add_lvalue_reference_t<decltype(stateEquivalence)>;
  public:
    EquivalenceMaintainer(
        Map map, State const& fs, State const& ss) :
        map {map}, firstState {fs}, secondState {ss} {
      map.try_emplace(firstState, secondState);
    }

    ~EquivalenceMaintainer() {
      map.erase(firstState);
    }

  private:
    Map map;
    State const& firstState;
    State const& secondState;
  };

  std::function<bool(State const&, State const&)> equivalent =
      [&stateEquivalence, &equivalent](State const& s1, State const& s2) {
    if (stateEquivalence.contains(s1)) {
      return stateEquivalence.at(s1) == s2;
    }
    auto em = EquivalenceMaintainer {stateEquivalence, s1, s2};
    if (s1->transitions().size() != s2->transitions().size()) {
      return false;
    }
    for (auto const& [sym, states1] : s1->transitions()) {
      if (!s2->transitions().contains(sym) || s1->next(sym).size() != s2->next(sym).size()) {
        return false;
      }
      for (auto& next1 : states1) {
        bool foundEquivalent = false;
        for (auto& next2 : s2->next(sym)) {
          if (equivalent(next1, next2)) {
            foundEquivalent = true;
            break;
          }
        }
        if (!foundEquivalent) {
          return false;
        }
      }
    }
    return true;
  };
  return n1.size() == n2.size() && equivalent(n1.start(), n2.start());
}
} // namespace

TEST(NfaTest, LoopComparison) {
  auto test1 = testMachine(2, {
  Edge {0, 1, std::nullopt},
  Edge {1, 0, std::nullopt}});
  auto test2 = testMachine(2, {
  Edge {0, 1, std::nullopt},
  Edge {1, 0, std::nullopt}});
  ASSERT_TRUE(compare(test1, test2));
}

TEST(NfaTest, MultipleExits) {
  auto test = testMachine(4, {1, 2}, {
    Edge {0, 1, 'a'},
    Edge {0, 2, 'b'},
    Edge {0, 3, 'c'}
  });
  ASSERT_TRUE(std::get<0>(test.isAccepting(test.start()->next('a'))));
  ASSERT_TRUE(std::get<0>(test.isAccepting(test.start()->next('b'))));
  ASSERT_FALSE(std::get<0>(test.isAccepting(test.start()->next('c'))));
}

TEST(NfaTest, FromRegex) {
  auto regex = Regex("(a|b)*abb");
  auto nfa = NfaAutomata {regex};
  auto target = testMachine(11,{
    Edge {0, 1, std::nullopt},
    Edge {0, 7, std::nullopt},
    Edge {1, 2, std::nullopt},
    Edge {1, 4, std::nullopt},
    Edge {2, 3, 'b'},
    Edge {4, 5, 'a'},
    Edge {3, 6, std::nullopt},
    Edge {5, 6, std::nullopt},
    Edge {6, 1, std::nullopt},
    Edge {6, 7, std::nullopt},
    Edge {7, 8, 'a'},
    Edge {8, 9, 'b'},
    Edge {9, 10, 'b'}
  });

  ASSERT_TRUE(compare(nfa, target));
}

TEST(NfaTest, Simulation) {
  auto regex = Regex("(a|b)*abb");
  auto nfa = NfaAutomata {regex};

  auto acceptedString = "aabbaabb";
  auto rejectedString = "cdab";
  auto minimallyAcceptedStirng = "abb";
  auto partiallyMatchedString = "abbb";
  ASSERT_TRUE(std::get<0>(nfa.simulate(acceptedString)));
  ASSERT_FALSE(std::get<0>(nfa.simulate(rejectedString)));
  ASSERT_TRUE(std::get<0>(nfa.simulate(minimallyAcceptedStirng)));
  ASSERT_FALSE(std::get<0>(nfa.simulate(partiallyMatchedString)));
}