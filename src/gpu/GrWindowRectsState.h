/*
 * Copyright 2016 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef GrWindowRectsState_DEFINED
#define GrWindowRectsState_DEFINED

#include "src/gpu/GrWindowRectangles.h"

class GrWindowRectsState {
 public:
  enum class Mode : bool { kExclusive, kInclusive };

  GrWindowRectsState() noexcept : fMode(Mode::kExclusive) {}
  GrWindowRectsState(const GrWindowRectangles& windows, Mode mode) noexcept
      : fMode(mode), fWindows(windows) {}

  bool enabled() const noexcept { return Mode::kInclusive == fMode || !fWindows.empty(); }
  Mode mode() const noexcept { return fMode; }
  const GrWindowRectangles& windows() const noexcept { return fWindows; }
  int numWindows() const noexcept { return fWindows.count(); }

  void setDisabled() noexcept {
    fMode = Mode::kExclusive;
    fWindows.reset();
  }

  void set(const GrWindowRectangles& windows, Mode mode) noexcept {
    fMode = mode;
    fWindows = windows;
  }

  bool operator==(const GrWindowRectsState& that) const noexcept {
    if (fMode != that.fMode) {
      return false;
    }
    return fWindows == that.fWindows;
  }
  bool operator!=(const GrWindowRectsState& that) const noexcept { return !(*this == that); }

 private:
  Mode fMode;
  GrWindowRectangles fWindows;
};

#endif
