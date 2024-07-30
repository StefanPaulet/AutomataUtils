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
using namespace au::test::nfa;

auto compare(NfaAutomata const& n1, NfaAutomata const& n2) {
  std::unordered_map<std::shared_ptr<NfaState>, std::shared_ptr<NfaState>> stateEquivalence {};

  class EquivalenceMaintainer {
  private:
    using Map = std::add_lvalue_reference_t<decltype(stateEquivalence)>;
  public:
    EquivalenceMaintainer(
        Map map, std::shared_ptr<NfaState> const& fs, std::shared_ptr<NfaState> const& ss) :
        map {map}, firstState {fs}, secondState {ss} {
      map.try_emplace(firstState, secondState);
    }

    ~EquivalenceMaintainer() {
      map.erase(firstState);
    }

  private:
    Map map;
    std::shared_ptr<NfaState> const& firstState;
    std::shared_ptr<NfaState> const& secondState;
  };

  std::function<bool(std::shared_ptr<NfaState> const&, std::shared_ptr<NfaState> const&)> equivalent =
      [&stateEquivalence, &equivalent](std::shared_ptr<NfaState> const& s1, std::shared_ptr<NfaState> const& s2) {
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
  return equivalent(n1.start(), n2.start());
}
} // namespace

TEST(NfaTest, Loop) {
  auto test1 = testMachine(2, {
  Edge{0, 1, std::nullopt},
  Edge{1, 0, std::nullopt}});
  auto test2 = testMachine(2, {
  Edge{0, 1, std::nullopt},
  Edge{1, 0, std::nullopt}});
  ASSERT_TRUE(compare(test1, test2));
}

TEST(NfaTest, Comparison) {
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