//
// Created by stefan on 8/29/24.
//

#include <gtest/gtest.h>
#include <stringMatching/KMP.hpp>

namespace {
using namespace au::sm;
}

TEST(KmpTest, Matching) {
  auto kmpMachine = KMPMachine {"aba"};
  auto result = kmpMachine.match("cabbababaaaaaba");
  ASSERT_EQ(result.size(), 3);
  ASSERT_EQ(result[0], 4);
  ASSERT_EQ(result[1], 6);
  ASSERT_EQ(result[2], 12);
}