/*
 * Copyright 2016 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef SKSL_POSITION
#define SKSL_POSITION

#include "src/sksl/SkSLUtil.h"

namespace SkSL {

/**
 * Represents a position in the source code. Both line and column are one-based. Column is currently
 * ignored.
 */
struct Position {
  constexpr Position() noexcept : fLine(-1), fColumn(-1) {}

  constexpr Position(int line, int column) noexcept : fLine(line), fColumn(column) {}

  String description() const { return to_string(fLine); }

  int fLine;
  int fColumn;
};

}  // namespace SkSL

#endif
