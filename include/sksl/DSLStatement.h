/*
 * Copyright 2021 Google LLC.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef SKSL_DSL_STATEMENT
#define SKSL_DSL_STATEMENT

#include "include/core/SkTypes.h"
#include "include/private/SkSLStatement.h"
#include "include/sksl/SkSLPosition.h"

#include <memory>
#include <utility>

namespace SkSL {

class Expression;

namespace dsl {

class DSLBlock;
class DSLExpression;

class DSLStatement {
 public:
  DSLStatement() noexcept;

  DSLStatement(DSLExpression expr);

  DSLStatement(DSLBlock block);

  DSLStatement(DSLStatement&&) noexcept = default;

  DSLStatement(std::unique_ptr<SkSL::Expression> expr);

  DSLStatement(std::unique_ptr<SkSL::Statement> stmt, Position pos);

  DSLStatement(std::unique_ptr<SkSL::Statement> stmt) noexcept;

  ~DSLStatement();

  DSLStatement& operator=(DSLStatement&& other) noexcept = default;

  Position position() noexcept {
    SkASSERT(this->hasValue());
    return fStatement->fPosition;
  }

  void setPosition(Position pos) noexcept {
    SkASSERT(this->hasValue());
    fStatement->fPosition = pos;
  }

  bool hasValue() noexcept { return fStatement != nullptr; }

  std::unique_ptr<SkSL::Statement> release() noexcept {
    SkASSERT(this->hasValue());
    return std::move(fStatement);
  }

 private:
  std::unique_ptr<SkSL::Statement> releaseIfPossible() noexcept { return std::move(fStatement); }

  std::unique_ptr<SkSL::Statement> fStatement;

  friend class DSLBlock;
  friend class DSLCore;
  friend class DSLExpression;
  friend class DSLWriter;
  friend DSLStatement operator,(DSLStatement left, DSLStatement right);
};

DSLStatement operator,(DSLStatement left, DSLStatement right);

}  // namespace dsl

}  // namespace SkSL

#endif
