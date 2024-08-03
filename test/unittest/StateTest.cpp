//
// Created by stefan on 7/25/24.
//

#include <automata/state/DfaState.hpp>
#include <automata/state/State.hpp>
#include <automata/state/NfaState.hpp>
#include <gtest/gtest.h>

namespace {
using namespace au;
}

TEST(NfaStateTest, Transitions) {
  auto s1 = std::make_shared<NfaState>();
  auto s2 = std::make_shared<NfaState>();
  auto s3 = std::make_shared<NfaState>();
  auto s4 = std::make_shared<NfaState>();

  s1->addTransition('a', s2);
  s1->addTransition('a', s3);
  s2->addTransition(std::nullopt, s4);
  s3->addTransition(std::nullopt, s4);

  ASSERT_EQ(s1->next('a').front(), s2);
  {
    auto it = s1->next('a').begin();
    ++it;
    ASSERT_EQ(*it, s3);
  }
  ASSERT_EQ(s2->next(std::nullopt).front(), s4);
  ASSERT_EQ(s3->next(std::nullopt).front(), s4);
}

TEST(DfaStateTest, Transitions) {
  auto s1 = std::make_shared<DfaState>();
  auto s2 = std::make_shared<DfaState>();
  auto s3 = std::make_shared<DfaState>();

  s1->addTransition('a', s2);
  s1->addTransition('b', s3);
  s2->addTransition('c', s3);
  ASSERT_EQ(s1->next('a'), s2);
  ASSERT_EQ(s1->next('b'), s3);
  ASSERT_EQ(s2->next('c'), s1->next('b'));
}

TEST(DfaStateTest, MultipleTransitionsException) {
  auto s1 = std::make_shared<DfaState>();
  auto s2 = std::make_shared<DfaState>();
  auto s3 = std::make_shared<DfaState>();
  s1->addTransition('a', s2);
  try {
    s1->addTransition('a', s3);
  } catch (exceptions::DfaConflictingTransitionException const& e) {
    ASSERT_EQ(std::string(e.what()), "Multiple transitions in dfa on symbol a");
  }
}

TEST(DfaStateTest, EpsilonTransition) {
  auto s1 = std::make_shared<DfaState>();
  auto s2 = std::make_shared<DfaState>();
  try {
    s1->addTransition(std::nullopt, s2);
  } catch (exceptions::DfaEpsilonTransitionException const& e) {
    ASSERT_EQ(std::string(e.what()), "Cannot insert an epsilon transition on DFA");
  }
}