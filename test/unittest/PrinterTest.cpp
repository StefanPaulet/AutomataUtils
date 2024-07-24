//
// Created by stefan on 7/18/24.
//

#include "regex/Regex.hpp"
#include "regex/RegexSyntaxTree.hpp"
#include "gtest/gtest.h"
#include <fstream>
#include <automata/nfa/NFA.hpp>

namespace {
using namespace au;
using namespace au::exceptions;

template <typename Target, typename Printer> auto testPrinter(Printer&& printer, Target&& target, std::string const& expected) {
  std::stringstream s {};
  std::forward<Printer>(printer).dump(s, std::forward<Target>(target));
  ASSERT_EQ(s.str(), expected);
}
} // namespace


TEST(TreePrinterTest, TreeGraphPrinter) {
  Regex r {"a*b|(c)"};
  auto const treeString = R"(Operator |
| Operator .
| | Operator *
| | ` a
| ` b
` c
)";
  testPrinter(TreeGraphPrinter {}, r.parse().root().get(), treeString);
}

TEST(TreePrinterTest, DotGraphPrinter) {
  Regex r {"a*b|(c)"};
  auto const dotString = R"(graph mygraph {
  n0 [label="|", color="blue"];
  n0 -- n1;
  n0 -- n2;
  n1 [label=".", color="blue"];
  n1 -- n3;
  n1 -- n4;
  n2 [label="c", color="black"];
  n3 [label="*", color="blue"];
  n3 -- n5;
  n4 [label="b", color="black"];
  n5 [label="a", color="black"];
}
)";
  testPrinter(DotGraphPrinter {}, r.parse().root().get(), dotString);
}

TEST(NfaPrinterTest, DotGraphPrinter) {
  Regex r {"(a|b)*"};
  auto const dotString = R"(digraph nfaGraph {
  n0 [label="0", color="black"];
  n0 -> n1 [label="eps"];
  n0 -> n2 [label="eps"];
  n1 [label="1", color="black"];
  n1 -> n3 [label="eps"];
  n1 -> n4 [label="eps"];
  n2 [label="2", color="black"];
  n3 [label="3", color="black"];
  n3 -> n5 [label="a"];
  n4 [label="4", color="black"];
  n4 -> n6 [label="b"];
  n5 [label="5", color="black"];
  n5 -> n7 [label="eps"];
  n6 [label="6", color="black"];
  n6 -> n7 [label="eps"];
  n7 [label="7", color="black"];
  n7 -> n1 [label="eps"];
  n7 -> n2 [label="eps"];
}
)";
  testPrinter(DotGraphPrinter {true, "nfaGraph"}, NfaAutomata {r}.start().get(), dotString);
}