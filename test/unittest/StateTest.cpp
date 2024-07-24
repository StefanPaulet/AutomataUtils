//
// Created by stefan on 7/25/24.
//

#include <automata/state/State.hpp>
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