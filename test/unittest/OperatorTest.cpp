//
// Created by stefan on 7/14/24.
//


#include "operator/Operator.hpp"
#include "gtest/gtest.h"

namespace {
using namespace au;
using namespace operators;
} // namespace

TEST(OperatorTest, Construction) {
  Operator unaryOp {10, '*', true};
  Operator implicitlyBinaryOp {5, '|'};
  Operator explicitlyBinaryOp {8, '.', false};

  ASSERT_TRUE(unaryOp.unary);
  ASSERT_FALSE(implicitlyBinaryOp.unary);
  ASSERT_FALSE(explicitlyBinaryOp.unary);
}

TEST(OperatorTest, Comparison) {
  Operator const altCatOp {10, '.'};
  ASSERT_EQ(catOp, altCatOp);

  Operator const nonStarOp {starOp.priority, '0', starOp.unary};
  ASSERT_NE(starOp, nonStarOp);
}

TEST(OperatorTest, OperatorsConstruction) {
  Operator const unaryOp {10, '*', true};
  Operator const implicitlyBinaryOp {5, '|'};
  Operator const explicitlyBinaryOp {8, '.', false};

  Operators const ops {unaryOp, implicitlyBinaryOp, explicitlyBinaryOp};

  ASSERT_EQ(ops.ops[0], unaryOp);
  ASSERT_EQ(ops.ops[1], implicitlyBinaryOp);
}

TEST(OperatorTest, IsOperator) {
  Operator const unaryOp {10, '*', true};
  Operator const implicitlyBinaryOp {5, '|'};
  Operator const explicitlyBinaryOp {8, '.', false};

  Operators const ops {unaryOp, implicitlyBinaryOp, explicitlyBinaryOp};

  ASSERT_TRUE(ops.isOperator('*').first);
  ASSERT_TRUE(ops.isOperator('|').first);
  ASSERT_FALSE(ops.isOperator('?').first);
  ASSERT_FALSE(ops.isOperator('+').first);
}