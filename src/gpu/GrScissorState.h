/*
 * Copyright 2016 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef GrScissorState_DEFINED
#define GrScissorState_DEFINED

#include "include/core/SkRect.h"

class GrScissorState {
 public:
  GrScissorState() noexcept : fEnabled(false) {}
  GrScissorState(const SkIRect& rect) noexcept : fEnabled(true), fRect(rect) {}
  void setDisabled() noexcept { fEnabled = false; }
  void set(const SkIRect& rect) noexcept {
    fRect = rect;
    fEnabled = true;
  }
  bool SK_WARN_UNUSED_RESULT intersect(const SkIRect& rect) noexcept {
    if (!fEnabled) {
      this->set(rect);
      return true;
    }
    return fRect.intersect(rect);
  }
  bool operator==(const GrScissorState& other) const noexcept {
    return fEnabled == other.fEnabled && (false == fEnabled || fRect == other.fRect);
  }
  bool operator!=(const GrScissorState& other) const noexcept { return !(*this == other); }

  bool enabled() const noexcept { return fEnabled; }
  const SkIRect& rect() const noexcept {
    SkASSERT(fEnabled);
    return fRect;
  }

 private:
  bool fEnabled;
  SkIRect fRect;
};

#endif
