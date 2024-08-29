//
// Created by stefan on 8/29/24.
//


#include <gtest/gtest.h>
#include <stringMatching/fallback/KmpFallbackMachine.hpp>

namespace {
using namespace au::sm;
}

TEST(FallbackMachine, KmpFallbackMachine1) {
  auto machine = KmpFallbackMachine("ababaa");
  ASSERT_EQ(machine.get(0), 0);
  ASSERT_EQ(machine.get(1), 0);
  ASSERT_EQ(machine.get(2), 1);
  ASSERT_EQ(machine.get(3), 2);
  ASSERT_EQ(machine.get(4), 3);
  ASSERT_EQ(machine.get(5), 1);
}

TEST(FallbackMachine, KmpFallbackMachine2) {
  auto machine = KmpFallbackMachine("AAAA");
  ASSERT_EQ(machine.get(0), 0);
  ASSERT_EQ(machine.get(1), 1);
  ASSERT_EQ(machine.get(2), 2);
  ASSERT_EQ(machine.get(3), 3);
}