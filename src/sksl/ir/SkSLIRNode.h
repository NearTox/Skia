/*
 * Copyright 2016 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef SKSL_IRNODE
#define SKSL_IRNODE

#include "src/sksl/SkSLLexer.h"
#include "src/sksl/SkSLString.h"

namespace SkSL {

/**
 * Represents a node in the intermediate representation (IR) tree. The IR is a fully-resolved
 * version of the program (all types determined, everything validated), ready for code generation.
 */
struct IRNode {
  constexpr IRNode(int offset) noexcept : fOffset(offset) {}

  virtual ~IRNode() = default;

  virtual String description() const = 0;

  // character offset of this element within the program being compiled, for error reporting
  // purposes
  int fOffset;
};

}  // namespace SkSL

#endif
