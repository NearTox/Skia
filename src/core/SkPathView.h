/*
 * Copyright 2020 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#pragma once

#include "include/core/SkPathTypes.h"
#include "include/core/SkPoint.h"
#include "include/core/SkRect.h"
#include "src/core/SkSpan.h"

struct SkPathView {
  SkPathView(
      SkSpan<const SkPoint> points, SkSpan<const uint8_t> verbs, SkSpan<const float> weights,
      SkPathFillType ft, SkPathConvexityType ct, const SkRect& bounds, unsigned segmentMask,
      bool isFinite) noexcept
      : fPoints(points),
        fVerbs(verbs),
        fWeights(weights),
        fBounds(bounds),
        fFillType(ft),
        fConvexity(ct),
        fSegmentMask(segmentMask),
        fIsFinite(isFinite) {
    this->validate();
  }

  bool isInverseFillType() const noexcept { return SkPathFillType_IsInverse(fFillType); }
  bool isConvex() const noexcept { return fConvexity == SkPathConvexityType::kConvex; }

  bool isEmpty() const noexcept { return fPoints.size() == 0; }

  bool isRect(SkRect*) const noexcept;
  bool isFinite() const noexcept { return fIsFinite; }

  SkSpan<const SkPoint> fPoints;
  SkSpan<const uint8_t> fVerbs;
  SkSpan<const float> fWeights;

  SkRect fBounds;

  SkPathFillType fFillType;
  SkPathConvexityType fConvexity;
  uint8_t fSegmentMask;
  bool fIsFinite;

#ifdef SK_DEBUG
  void validate() const;
#else
  void validate() const noexcept {}
#endif
};

static inline SkPathView SkPathView_triangle(const SkPoint pts[3], const SkRect& bounds) noexcept {
  static constexpr uint8_t verbs[] = {
      (uint8_t)SkPathVerb::kMove,
      (uint8_t)SkPathVerb::kLine,
      (uint8_t)SkPathVerb::kLine,
  };
  return SkPathView(
      {pts, 3}, SkMakeSpan(verbs), {}, SkPathFillType::kWinding, SkPathConvexityType::kConvex,
      bounds, kLine_SkPathSegmentMask, true);
}

static inline SkPathView SkPathView_quad(const SkPoint pts[4], const SkRect& bounds) noexcept {
  static constexpr uint8_t verbs[] = {
      (uint8_t)SkPathVerb::kMove,
      (uint8_t)SkPathVerb::kLine,
      (uint8_t)SkPathVerb::kLine,
      (uint8_t)SkPathVerb::kLine,
  };
  return SkPathView(
      {pts, 4}, SkMakeSpan(verbs), {}, SkPathFillType::kWinding, SkPathConvexityType::kConvex,
      bounds, kLine_SkPathSegmentMask, true);
};
