/*
 * Copyright 2016 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef SKSL_TERNARYEXPRESSION
#define SKSL_TERNARYEXPRESSION

#include "src/sksl/SkSLPosition.h"
#include "src/sksl/ir/SkSLExpression.h"

namespace SkSL {

/**
 * A ternary expression (test ? ifTrue : ifFalse).
 */
class TernaryExpression final : public Expression {
 public:
  static constexpr Kind kExpressionKind = Kind::kTernary;

  TernaryExpression(
      int offset, std::unique_ptr<Expression> test, std::unique_ptr<Expression> ifTrue,
      std::unique_ptr<Expression> ifFalse)
      : INHERITED(offset, kExpressionKind, &ifTrue->type()),
        fTest(std::move(test)),
        fIfTrue(std::move(ifTrue)),
        fIfFalse(std::move(ifFalse)) {
    SkASSERT(this->ifTrue()->type() == this->ifFalse()->type());
  }

  std::unique_ptr<Expression>& test() { return fTest; }

  const std::unique_ptr<Expression>& test() const { return fTest; }

  std::unique_ptr<Expression>& ifTrue() { return fIfTrue; }

  const std::unique_ptr<Expression>& ifTrue() const { return fIfTrue; }

  std::unique_ptr<Expression>& ifFalse() { return fIfFalse; }

  const std::unique_ptr<Expression>& ifFalse() const { return fIfFalse; }

  bool hasProperty(Property property) const override {
    return this->test()->hasProperty(property) || this->ifTrue()->hasProperty(property) ||
           this->ifFalse()->hasProperty(property);
  }

  bool isConstantOrUniform() const override {
    return this->test()->isConstantOrUniform() && this->ifTrue()->isConstantOrUniform() &&
           this->ifFalse()->isConstantOrUniform();
  }

  std::unique_ptr<Expression> clone() const override {
    return std::unique_ptr<Expression>(new TernaryExpression(
        fOffset, this->test()->clone(), this->ifTrue()->clone(), this->ifFalse()->clone()));
  }

  String description() const override {
    return "(" + this->test()->description() + " ? " + this->ifTrue()->description() + " : " +
           this->ifFalse()->description() + ")";
  }

 private:
  std::unique_ptr<Expression> fTest;
  std::unique_ptr<Expression> fIfTrue;
  std::unique_ptr<Expression> fIfFalse;

  using INHERITED = Expression;
};

}  // namespace SkSL

#endif
