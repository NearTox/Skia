/*
 * Copyright 2015 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef SkPathBuilder_DEFINED
#define SkPathBuilder_DEFINED

#include "include/core/SkMatrix.h"
#include "include/core/SkPath.h"
#include "include/core/SkPathTypes.h"
#include "include/private/SkTDArray.h"

class SkPathBuilder {
 public:
  SkPathBuilder() noexcept;
  ~SkPathBuilder();

  SkPath snapshot();  // the builder is unchanged after returning this path
  SkPath detach();    // the builder is reset to empty after returning this path

  void setFillType(SkPathFillType ft) noexcept { fFillType = ft; }
  void setIsVolatile(bool isVolatile) noexcept { fIsVolatile = isVolatile; }

  SkPathBuilder& reset() noexcept;

  SkPathBuilder& moveTo(SkPoint pt) noexcept;
  SkPathBuilder& lineTo(SkPoint pt) noexcept;
  SkPathBuilder& quadTo(SkPoint pt1, SkPoint pt2) noexcept;
  SkPathBuilder& conicTo(SkPoint pt1, SkPoint pt2, SkScalar w) noexcept;
  SkPathBuilder& cubicTo(SkPoint pt1, SkPoint pt2, SkPoint pt3) noexcept;
  SkPathBuilder& close() noexcept;

  SkPathBuilder& moveTo(SkScalar x, SkScalar y) noexcept {
    return this->moveTo(SkPoint::Make(x, y));
  }
  SkPathBuilder& lineTo(SkScalar x, SkScalar y) noexcept {
    return this->lineTo(SkPoint::Make(x, y));
  }
  SkPathBuilder& quadTo(SkScalar x1, SkScalar y1, SkScalar x2, SkScalar y2) noexcept {
    return this->quadTo(SkPoint::Make(x1, y1), SkPoint::Make(x2, y2));
  }
  SkPathBuilder& quadTo(const SkPoint pts[2]) noexcept { return this->quadTo(pts[0], pts[1]); }
  SkPathBuilder& conicTo(SkScalar x1, SkScalar y1, SkScalar x2, SkScalar y2, SkScalar w) noexcept {
    return this->conicTo(SkPoint::Make(x1, y1), SkPoint::Make(x2, y2), w);
  }
  SkPathBuilder& conicTo(const SkPoint pts[2], SkScalar w) noexcept {
    return this->conicTo(pts[0], pts[1], w);
  }
  SkPathBuilder& cubicTo(
      SkScalar x1, SkScalar y1, SkScalar x2, SkScalar y2, SkScalar x3, SkScalar y3) noexcept {
    return this->cubicTo(SkPoint::Make(x1, y1), SkPoint::Make(x2, y2), SkPoint::Make(x3, y3));
  }
  SkPathBuilder& cubicTo(const SkPoint pts[3]) noexcept {
    return this->cubicTo(pts[0], pts[1], pts[2]);
  }

  SkPathBuilder& addRect(const SkRect&, SkPathDirection, unsigned startIndex) noexcept;
  SkPathBuilder& addOval(const SkRect&, SkPathDirection, unsigned startIndex) noexcept;
  SkPathBuilder& addRRect(const SkRRect&, SkPathDirection, unsigned startIndex) noexcept;

  SkPathBuilder& addRect(const SkRect& rect, SkPathDirection dir = SkPathDirection::kCW) noexcept {
    return this->addRect(rect, dir, 0);
  }
  SkPathBuilder& addOval(const SkRect& rect, SkPathDirection dir = SkPathDirection::kCW) noexcept {
    return this->addOval(rect, dir, 0);
  }
  SkPathBuilder& addRRect(
      const SkRRect& rect, SkPathDirection dir = SkPathDirection::kCW) noexcept {
    return this->addRRect(rect, dir, 0);
  }

  void incReserve(int extraPtCount, int extraVerbCount) noexcept;
  void incReserve(int extraPtCount) noexcept { this->incReserve(extraPtCount, extraPtCount); }

  static SkPath Make(
      const SkPoint[], int pointCount, const uint8_t[], int verbCount, const SkScalar[],
      int conicWeightCount, SkPathFillType, bool isVolatile = false);

 private:
  SkTDArray<SkPoint> fPts;
  SkTDArray<uint8_t> fVerbs;
  SkTDArray<SkScalar> fConicWeights;

  SkPathFillType fFillType;
  bool fIsVolatile;

  unsigned fSegmentMask;
  SkPoint fLastMovePoint;
  bool fNeedsMoveVerb;

  int countVerbs() const noexcept { return fVerbs.count(); }

  void ensureMove() noexcept {
    if (fNeedsMoveVerb) {
      this->moveTo(fLastMovePoint);
    }
  }
};

#endif
