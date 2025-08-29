//
// Created by stefan on 7/25/24.
//

#include <automata/state/DfaState.hpp>
#include <automata/state/State.hpp>
#include <automata/state/NfaState.hpp>
#include "utils/Utils.hpp"

namespace {
using namespace au;
}

TEST(NfaStateTest, Transitions) {
  auto s1 = std::make_unique<NfaState>();
  auto s2 = std::make_unique<NfaState>();
  auto s3 = std::make_unique<NfaState>();
  auto s4 = std::make_unique<NfaState>();

  s1->addTransition('a', s2.get());
  s1->addTransition('a', s3.get());
  s2->addTransition(std::nullopt, s4.get());
  s3->addTransition(std::nullopt, s4.get());

  ASSERT_EQ(*s1->next('a').begin(), s2.get());
  {
    auto it = s1->next('a').begin();
    ASSERT_EQ(*(++it), s3.get());
  }
  ASSERT_EQ(*s2->next(std::nullopt).begin(), s4.get());
  ASSERT_EQ(*s3->next(std::nullopt).begin(), s4.get());
}

TEST(DfaStateTest, Transitions) {
  auto s1 = std::make_unique<DfaState>();
  auto s2 = std::make_unique<DfaState>();
  auto s3 = std::make_unique<DfaState>();

  s1->addTransition('a', s2.get());
  s1->addTransition('b', s3.get());
  s2->addTransition('c', s3.get());
  ASSERT_EQ(static_cast<DfaState const*>(s2.get()), s1->next('a'));
  ASSERT_EQ(static_cast<DfaState const*>(s3.get()), s1->next('b'));
  ASSERT_EQ(static_cast<DfaState const*>(s1->next('b')), s2->next('c'));
}

TEST(DfaStateTest, MultipleTransitionsException) {
  auto s1 = std::make_unique<DfaState>();
  auto s2 = std::make_unique<DfaState>();
  auto s3 = std::make_unique<DfaState>();
  s1->addTransition('a', s2.get());
  try {
    s1->addTransition('a', s3.get());
  } catch (exceptions::DfaConflictingTransitionException const& e) {
    ASSERT_EQ(std::string(e.what()), "Multiple transitions in dfa on symbol a");
  }
}

TEST(DfaStateTest, EpsilonTransition) {
  auto s1 = std::make_unique<DfaState>();
  auto s2 = std::make_unique<DfaState>();
  try {
    s1->addTransition(std::nullopt, s2.get());
  } catch (exceptions::DfaEpsilonTransitionException const& e) {
    ASSERT_EQ(std::string(e.what()), "Cannot insert an epsilon transition on DFA");
  }
}