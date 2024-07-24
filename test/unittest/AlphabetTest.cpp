//
// Created by stefan on 7/10/24.
//

#include "alphabet/Alphabet.cpp"
#include "alphabet/Alphabet.hpp"
#include "gtest/gtest.h"

namespace {
using namespace au;
} // namespace

TEST(AlphabetTest, Construction) {
  Alphabet a1 {};

  Alphabet a2 {"bcda"};
  ASSERT_EQ(a2.symbols(), "abcd");

  Alphabet a3 {a2};
  ASSERT_EQ(a2.symbols(), a3.symbols());

  Alphabet a4 {'0', '9'};
  ASSERT_EQ(a4.symbols(), "0123456789");

  try {
    Alphabet a5 {'9', '4'};
    ASSERT_TRUE(false);
  } catch (...){
    ASSERT_TRUE(true);
  }
}

TEST(AlphabetTest, Extend) {
  Alphabet a1 {"abc"};

  Alphabet a2 = a1.extend("cde");
  ASSERT_EQ(a2.symbols(), "abcde");
}

TEST(AlphabetTest, Join) {
  Alphabet a1 {"xyz012"};
  Alphabet a2 {"abc123"};
  Alphabet a3 = a1.join(a2);

  ASSERT_EQ(a3.symbols(), "0123abcxyz");
}

TEST(AlphabetTest, Exception) {
  try {
    Alphabet a {'5', '1'};
  } catch (exceptions::AlphabetRangeException const& e) {
    ASSERT_EQ(e.what(), "Attempting to create alphabet with range where begin beyond end");
  }
}