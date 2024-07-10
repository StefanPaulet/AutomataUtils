//
// Created by stefan on 7/18/24.
//

#include "regex/Regex.hpp"
#include "regex/RegexSyntaxTree.hpp"
#include "gtest/gtest.h"

namespace {
using namespace au;
using namespace au::exceptions;

template <typename Printer> auto testPrinter(Printer&& printer, Regex const& regex, std::string const& expected) {
  std::stringstream s {};
  std::forward<Printer>(printer).dump(s, regex.parse().root().get());
  ASSERT_EQ(s.str(), expected);
}
} // namespace


TEST(PrinterTest, TreeGraphPrinter) {
  Regex r {"a*b|(c)"};
  auto const treeString = R"(Operator |
| Operator .
| | Operator *
| | ` a
| ` b
` c
)";
  testPrinter(TreeGraphPrinter {}, r, treeString);
}

TEST(PrinterTest, DotGraphPrinter) {
  Regex r {"a*b|(c)"};
  auto const dotString = R"(graph regex_tree {
  n0 [label="|", color="blue"];
  n0 -- {n1, n2};
  n1 [label=".", color="blue"];
  n1 -- {n3, n4};
  n2 [label="c", color="black"];
  n3 [label="*", color="blue"];
  n3 -- {n5};
  n4 [label="b", color="black"];
  n5 [label="a", color="black"];
}
)";
  testPrinter(DotGraphPrinter {}, r, dotString);
}