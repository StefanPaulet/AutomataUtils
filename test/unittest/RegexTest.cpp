//
// Created by stefan on 7/14/24.
//


#include "regex/Regex.cpp"
#include "regex/Regex.hpp"
#include "gtest/gtest.h"
#include "operator/Operator.hpp"

namespace {
using namespace au;
using namespace au::exceptions;
using namespace operators;
using Node = RegexSyntaxTreeNode;
auto node(Operator const& op, std::unique_ptr<Node>&& left = nullptr, std::unique_ptr<Node>&& right = nullptr)
    -> std::unique_ptr<Node> {
  return std::make_unique<Node>(op.op, std::move(left), std::move(right));
}

auto node(Operator const& op, char left, std::unique_ptr<Node>&& right = nullptr) -> std::unique_ptr<Node> {
  return node(op, std::make_unique<Node>(left), std::move(right));
}

auto node(Operator const& op, std::unique_ptr<Node>&& left, char right) -> std::unique_ptr<Node> {
  return node(op, std::move(left), std::make_unique<Node>(right));
}

auto node(Operator const& op, char left, char right) -> std::unique_ptr<Node> {
  return node(op, std::make_unique<Node>(left), std::make_unique<Node>(right));
}

auto tree(std::unique_ptr<Node>&& root) { return RegexSyntaxTree {std::move(root)}; }

auto testOrFailureDump(Regex const& r, RegexSyntaxTree&& expected) {
  auto actual = std::move(r.parse());
  auto equals = (actual == expected);
  if (!equals) {
    TreeGraphPrinter t;
    std::cout << "Expected:\n";
    t.dump(std::cout, expected.root().get());
    std::cout << "Actual:\n";
    t.dump(std::cout, actual.root().release());
  }
  ASSERT_TRUE(equals);
}

auto testException(Regex const& r, std::string const& expectedMessage) {
  try {
    (void) r.parse();
  } catch (Exception& e) {
    auto equals = (std::string(e.what()) == expectedMessage);
    if (!equals) {
      std::cout << "Augmented string (for position reference):\n";
      std::cout << r.string() << '\n';
    }
    ASSERT_TRUE(equals);
    return;
  }
  ASSERT_TRUE(false);
}
} // namespace

TEST(RegexTest, Construction) {
  Regex const r1 {};
  ASSERT_EQ(r1.string(), "");

  Regex const r2 {"abc(d|e)"};
  ASSERT_EQ(r2.string(), "a.b.c.(d|e)");

  Regex r3 {r2};
  ASSERT_EQ(r3.string(), r2.string());

  Regex const r4 {std::move(r3)};
  ASSERT_EQ(r4.string(), "a.b.c.(d|e)");
}

TEST(RegexTest, Augmnentation) {
  Regex const r1 {"(01)*|(2|3)4"};
  ASSERT_EQ(r1.string(), "(0.1)*|(2|3).4");

  Regex const r2 {"12|3(45)"};
  ASSERT_EQ(r2.string(), "1.2|3.(4.5)");

  Regex const r3 {"0*1*((2))3**"};
  ASSERT_EQ(r3.string(), "0*.1*.((2)).3**");
}

TEST(RegexTest, ParseConcatenation) {
  Regex const r {"ab"};

  testOrFailureDump(r, tree(
    node(
      catOp,
      'a',
      'b'
    )
  ));
}

TEST(RegexTest, ParseDisjunction) {
  Regex const r {"a|b"};

  testOrFailureDump(r, tree(
    node(
      orOp,
      'a',
      'b'
    )
  ));
}

TEST(RegexTest, ParseClosure) {
  Regex const r {"a*"};

  testOrFailureDump(r, tree(
    node(
      starOp,
      'a'
    )
  ));
}

TEST(RegexTest, ParseExtra1) {
  Regex const r {"ae|f"};
  testOrFailureDump(r, tree(
    node(
      orOp,
      node(
        catOp,
        'a',
        'e'
      ),
      'f'
    )
  ));
}

TEST(RegexTest, ParseExtra2) {
  Regex const r {"01*|2"};

  testOrFailureDump(r, tree(
    node(
      orOp,
      node(
        catOp,
        '0',
        node(
          starOp,
          '1'
        )
      ),
      '2'
    )
  ));
}

TEST(RegexTest, ParseExtra3) {
  Regex const r {"(01)*|(2|3)4"};
  testOrFailureDump(r, tree(
    node(
      orOp,
      node(
        starOp,
        node(
            catOp,
            '0',
            '1'
        )
      ),
      node(
        catOp,
        node(
          orOp,
          '2',
          '3'
        ),
        '4'
      )
    )
  ));
}

TEST(RegexTest, ParseExtra4) {
  Regex const r {"0*1*((2))3**"};
  testOrFailureDump(r, tree(
    node(
      catOp,
      node(
        catOp,
        node(
          catOp,
          node(
            starOp,
            '0'
          ),
          node(
            starOp,
            '1'
          )
        ),
        '2'
      ),
      node(
        starOp,
        node(
          starOp,
          '3'
        )
      )
    )
  ));
}

TEST(RegexTest, ParseExtra5) {
  Regex const r {"(0|(1|2))*|((3)(4))"};
  testOrFailureDump(r, tree(
    node(
      orOp,
      node(
        starOp,
        node(
          orOp,
          '0',
          node(
            orOp,
            '1',
            '2'
          )
        )
      ),
      node(
        catOp,
        '3',
        '4'
      )
    )
  ));
}

TEST(RegexTest, Traversals) {
  Regex const r {"a|b"};
  using enum RegexSyntaxTreeTraversal::Order;
  auto compare = [](std::vector<RegexSyntaxTreeNode*> real, std::array<char, 3> expected) {
    auto expIt = expected.begin();
    for (auto const& node : real) {
      ASSERT_EQ(node->_character, *expIt++);
    }
  };

  RegexSyntaxTreeTraversal inOrder {INORDER, r.parse().root().get()};
  auto inOrderExpected = std::array<char, 3> {'a', '|', 'b'};

  RegexSyntaxTreeTraversal preOrder {PREORDER, r.parse().root().get()};
  auto preOrderExpected = std::array<char, 3> {'|', 'A', 'b'};

  RegexSyntaxTreeTraversal postOrder {POSTORDER, r.parse().root().get()};
  auto postOrderExpected = std::array<char, 3> {'a', 'b', '|'};
}

TEST(RegexTest, Exception1) {
  Regex const r {"0|"};
  testException(r, "Missing operand for | on position 2");
}

TEST(RegexTest, Exception2) {
  Regex const r {"****"};
  testException(r, "Missing operand for * on position 1");
}

TEST(RegexTest, Exception3) {
  Regex const r {"a|b*c)"};
  testException(r, "Unmatched parentheses on position 6");
}

TEST(RegexTest, Exception4) {
  Regex const r {"((a|c*)b|d"};
  testException(r, "Opened parentheses not closed");
}

TEST(RegexTest, Exception5) {
  Regex const r {"abf;gef"};
  testException(r, "Unrecognized symbol ; on position 5");
}