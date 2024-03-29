/*
 * Copyright 2016 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef SKSL_BREAKSTATEMENT
#define SKSL_BREAKSTATEMENT

#include "include/private/SkSLStatement.h"
#include "src/sksl/ir/SkSLExpression.h"

namespace SkSL {

/**
 * A 'break' statement.
 */
class BreakStatement final : public Statement {
 public:
  inline static constexpr Kind kStatementKind = Kind::kBreak;

  BreakStatement(Position pos) : INHERITED(pos, kStatementKind) {}

  static std::unique_ptr<Statement> Make(Position pos) {
    return std::make_unique<BreakStatement>(pos);
  }

  std::unique_ptr<Statement> clone() const override {
    return std::make_unique<BreakStatement>(fPosition);
  }

  std::string description() const override { return "break;"; }

 private:
  using INHERITED = Statement;
};

}  // namespace SkSL

#endif
