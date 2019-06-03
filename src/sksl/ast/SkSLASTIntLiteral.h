/*
 * Copyright 2016 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef SKSL_ASTINTLITERAL
#define SKSL_ASTINTLITERAL

#include "src/sksl/ast/SkSLASTExpression.h"

namespace SkSL {

/**
 * A literal integer. At the AST level, integer literals are always positive; a negative number will
 * appear as a unary minus being applied to an integer literal.
 */
struct ASTIntLiteral : public ASTExpression {
    ASTIntLiteral(int offset, uint64_t value) : INHERITED(offset, kInt_Kind), fValue(value) {}

    String description() const override { return to_string(fValue); }

    const uint64_t fValue;

    typedef ASTExpression INHERITED;
};

}  // namespace SkSL

#endif
