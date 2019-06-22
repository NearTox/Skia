/*
 * Copyright 2016 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef SKSL_FLOATLITERAL
#define SKSL_FLOATLITERAL

#include "src/sksl/SkSLContext.h"
#include "src/sksl/ir/SkSLExpression.h"

namespace SkSL {

/**
 * A literal floating point number.
 */
struct FloatLiteral : public Expression {
  FloatLiteral(const Context& context, int offset, double value)
      : INHERITED(offset, kFloatLiteral_Kind, *context.fFloatLiteral_Type), fValue(value) {}

  FloatLiteral(int offset, double value, const Type* type) noexcept
      : INHERITED(offset, kFloatLiteral_Kind, *type), fValue(value) {}

  String description() const override { return to_string(fValue); }

  bool hasSideEffects() const noexcept override { return false; }

  bool isConstant() const noexcept override { return true; }

  int coercionCost(const Type& target) const override {
    if (target.isFloat()) {
      return 0;
    }
    return INHERITED::coercionCost(target);
  }

  bool compareConstant(const Context& context, const Expression& other) const noexcept override {
    FloatLiteral& f = (FloatLiteral&)other;
    return fValue == f.fValue;
  }

  double getConstantFloat() const noexcept override { return fValue; }

  std::unique_ptr<Expression> clone() const override {
    return std::unique_ptr<Expression>(new FloatLiteral(fOffset, fValue, &fType));
  }

  const double fValue;

  typedef Expression INHERITED;
};

}  // namespace SkSL

#endif
