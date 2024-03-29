/*
 * Copyright 2017 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef SkSGInvalidationController_DEFINED
#define SkSGInvalidationController_DEFINED

#include "include/core/SkMatrix.h"
#include "include/core/SkTypes.h"

#include <vector>

struct SkRect;

namespace sksg {

/**
 * Receiver for invalidation events.
 *
 * Tracks dirty regions for repaint.
 */
class InvalidationController {
 public:
  InvalidationController() noexcept;
  InvalidationController(const InvalidationController&) = delete;
  InvalidationController& operator=(const InvalidationController&) = delete;

  void inval(const SkRect&, const SkMatrix& ctm = SkMatrix::I());

  const SkRect& bounds() const noexcept { return fBounds; }

  auto begin() const noexcept { return fRects.cbegin(); }
  auto end() const noexcept { return fRects.cend(); }

  void reset() noexcept;

 private:
  std::vector<SkRect> fRects;
  SkRect fBounds;
};

}  // namespace sksg

#endif  // SkSGInvalidationController_DEFINED
