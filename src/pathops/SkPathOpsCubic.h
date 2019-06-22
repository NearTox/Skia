/*
 * Copyright 2012 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef SkPathOpsCubic_DEFINED
#define SkPathOpsCubic_DEFINED

#include "include/core/SkPath.h"
#include "src/core/SkArenaAlloc.h"
#include "src/pathops/SkPathOpsTCurve.h"

struct SkDCubicPair;

struct SkDCubic {
  static constexpr int kPointCount = 4;
  static constexpr int kPointLast = kPointCount - 1;
  static constexpr int kMaxIntersections = 9;

  enum SearchAxis { kXAxis, kYAxis };

  bool collapsed() const noexcept {
    return fPts[0].approximatelyEqual(fPts[1]) && fPts[0].approximatelyEqual(fPts[2]) &&
           fPts[0].approximatelyEqual(fPts[3]);
  }

  bool controlsInside() const noexcept {
    SkDVector v01 = fPts[0] - fPts[1];
    SkDVector v02 = fPts[0] - fPts[2];
    SkDVector v03 = fPts[0] - fPts[3];
    SkDVector v13 = fPts[1] - fPts[3];
    SkDVector v23 = fPts[2] - fPts[3];
    return v03.dot(v01) > 0 && v03.dot(v02) > 0 && v03.dot(v13) > 0 && v03.dot(v23) > 0;
  }

  static bool IsConic() noexcept { return false; }

  const SkDPoint& operator[](int n) const noexcept {
    SkASSERT(n >= 0 && n < kPointCount);
    return fPts[n];
  }
  SkDPoint& operator[](int n) noexcept {
    SkASSERT(n >= 0 && n < kPointCount);
    return fPts[n];
  }

  void align(int endIndex, int ctrlIndex, SkDPoint* dstPt) const noexcept;
  double binarySearch(double min, double max, double axisIntercept, SearchAxis xAxis) const
      noexcept;
  double calcPrecision() const noexcept;
  SkDCubicPair chopAt(double t) const noexcept;
  static void Coefficients(
      const double* cubic, double* A, double* B, double* C, double* D) noexcept;
  static int ComplexBreak(const SkPoint pts[4], SkScalar* t) noexcept;
  int convexHull(char order[kPointCount]) const noexcept;

  void debugInit() noexcept { sk_bzero(fPts, sizeof(fPts)); }

  void debugSet(const SkDPoint* pts);

  void dump() const;  // callable from the debugger when the implementation code is linked in
  void dumpID(int id) const;
  void dumpInner() const;
  SkDVector dxdyAtT(double t) const noexcept;
  bool endsAreExtremaInXOrY() const noexcept;
  static int FindExtrema(const double src[], double tValue[2]) noexcept;
  int findInflections(double tValues[2]) const noexcept;

  static int FindInflections(const SkPoint a[kPointCount], double tValues[2]) noexcept {
    SkDCubic cubic;
    return cubic.set(a).findInflections(tValues);
  }

  int findMaxCurvature(double tValues[]) const noexcept;

#ifdef SK_DEBUG
  SkOpGlobalState* globalState() const { return fDebugGlobalState; }
#endif

  bool hullIntersects(const SkDCubic& c2, bool* isLinear) const noexcept;
  bool hullIntersects(const SkDConic& c, bool* isLinear) const noexcept;
  bool hullIntersects(const SkDQuad& c2, bool* isLinear) const noexcept;
  bool hullIntersects(const SkDPoint* pts, int ptCount, bool* isLinear) const noexcept;
  bool isLinear(int startIndex, int endIndex) const noexcept;
  static int maxIntersections() noexcept { return kMaxIntersections; }
  bool monotonicInX() const noexcept;
  bool monotonicInY() const noexcept;
  void otherPts(int index, const SkDPoint* o1Pts[kPointCount - 1]) const noexcept;
  static int pointCount() noexcept { return kPointCount; }
  static int pointLast() noexcept { return kPointLast; }
  SkDPoint ptAtT(double t) const noexcept;
  static int RootsReal(double A, double B, double C, double D, double t[3]) noexcept;
  static int RootsValidT(
      const double A, const double B, const double C, double D, double s[3]) noexcept;

  int searchRoots(
      double extremes[6], int extrema, double axisIntercept, SearchAxis xAxis,
      double* validRoots) const;

  bool toFloatPoints(SkPoint*) const noexcept;
  /**
   *  Return the number of valid roots (0 < root < 1) for this cubic intersecting the
   *  specified horizontal line.
   */
  int horizontalIntersect(double yIntercept, double roots[3]) const;
  /**
   *  Return the number of valid roots (0 < root < 1) for this cubic intersecting the
   *  specified vertical line.
   */
  int verticalIntersect(double xIntercept, double roots[3]) const;

  // add debug only global pointer so asserts can be skipped by fuzzers
  const SkDCubic& set(
      const SkPoint pts[kPointCount] SkDEBUGPARAMS(SkOpGlobalState* state = nullptr)) noexcept {
    fPts[0] = pts[0];
    fPts[1] = pts[1];
    fPts[2] = pts[2];
    fPts[3] = pts[3];
    SkDEBUGCODE(fDebugGlobalState = state);
    return *this;
  }

  SkDCubic subDivide(double t1, double t2) const noexcept;
  void subDivide(double t1, double t2, SkDCubic* c) const noexcept { *c = this->subDivide(t1, t2); }

  static SkDCubic SubDivide(const SkPoint a[kPointCount], double t1, double t2) noexcept {
    SkDCubic cubic;
    return cubic.set(a).subDivide(t1, t2);
  }

  void subDivide(const SkDPoint& a, const SkDPoint& d, double t1, double t2, SkDPoint p[2]) const
      noexcept;

  static void SubDivide(
      const SkPoint pts[kPointCount], const SkDPoint& a, const SkDPoint& d, double t1, double t2,
      SkDPoint p[2]) noexcept {
    SkDCubic cubic;
    cubic.set(pts).subDivide(a, d, t1, t2, p);
  }

  double top(const SkDCubic& dCurve, double startT, double endT, SkDPoint* topPt) const noexcept;
  SkDQuad toQuad() const;

  static const int gPrecisionUnit;
  SkDPoint fPts[kPointCount];
  SkDEBUGCODE(SkOpGlobalState* fDebugGlobalState);
};

/* Given the set [0, 1, 2, 3], and two of the four members, compute an XOR mask
   that computes the other two. Note that:

   one ^ two == 3 for (0, 3), (1, 2)
   one ^ two <  3 for (0, 1), (0, 2), (1, 3), (2, 3)
   3 - (one ^ two) is either 0, 1, or 2
   1 >> (3 - (one ^ two)) is either 0 or 1
thus:
   returned == 2 for (0, 3), (1, 2)
   returned == 3 for (0, 1), (0, 2), (1, 3), (2, 3)
given that:
   (0, 3) ^ 2 -> (2, 1)  (1, 2) ^ 2 -> (3, 0)
   (0, 1) ^ 3 -> (3, 2)  (0, 2) ^ 3 -> (3, 1)  (1, 3) ^ 3 -> (2, 0)  (2, 3) ^ 3 -> (1, 0)
*/
constexpr inline int other_two(int one, int two) noexcept { return 1 >> (3 - (one ^ two)) ^ 3; }

struct SkDCubicPair {
  const SkDCubic first() const noexcept {
#ifdef SK_DEBUG
    SkDCubic result;
    result.debugSet(&pts[0]);
    return result;
#else
    return (const SkDCubic&)pts[0];
#endif
  }
  const SkDCubic second() const noexcept {
#ifdef SK_DEBUG
    SkDCubic result;
    result.debugSet(&pts[3]);
    return result;
#else
    return (const SkDCubic&)pts[3];
#endif
  }
  SkDPoint pts[7];
};

class SkTCubic : public SkTCurve {
 public:
  SkDCubic fCubic;

  SkTCubic() {}

  SkTCubic(const SkDCubic& c) noexcept : fCubic(c) {}

  ~SkTCubic() override {}

  const SkDPoint& operator[](int n) const noexcept override { return fCubic[n]; }
  SkDPoint& operator[](int n) noexcept override { return fCubic[n]; }

  bool collapsed() const noexcept override { return fCubic.collapsed(); }
  bool controlsInside() const noexcept override { return fCubic.controlsInside(); }
  void debugInit() noexcept override { return fCubic.debugInit(); }
#if DEBUG_T_SECT
  void dumpID(int id) const override { return fCubic.dumpID(id); }
#endif
  SkDVector dxdyAtT(double t) const noexcept override { return fCubic.dxdyAtT(t); }
#ifdef SK_DEBUG
  SkOpGlobalState* globalState() const override { return fCubic.globalState(); }
#endif
  bool hullIntersects(const SkDQuad& quad, bool* isLinear) const noexcept override;
  bool hullIntersects(const SkDConic& conic, bool* isLinear) const noexcept override;

  bool hullIntersects(const SkDCubic& cubic, bool* isLinear) const noexcept override {
    return cubic.hullIntersects(fCubic, isLinear);
  }

  bool hullIntersects(const SkTCurve& curve, bool* isLinear) const noexcept override {
    return curve.hullIntersects(fCubic, isLinear);
  }

  int intersectRay(SkIntersections* i, const SkDLine& line) const override;
  bool IsConic() const noexcept override { return false; }
  SkTCurve* make(SkArenaAlloc& heap) const override { return heap.make<SkTCubic>(); }

  int maxIntersections() const noexcept override { return SkDCubic::kMaxIntersections; }

  void otherPts(int oddMan, const SkDPoint* endPt[2]) const noexcept override {
    fCubic.otherPts(oddMan, endPt);
  }

  int pointCount() const noexcept override { return SkDCubic::kPointCount; }
  int pointLast() const noexcept override { return SkDCubic::kPointLast; }
  SkDPoint ptAtT(double t) const noexcept override { return fCubic.ptAtT(t); }
  void setBounds(SkDRect*) const noexcept override;

  void subDivide(double t1, double t2, SkTCurve* curve) const noexcept override {
    ((SkTCubic*)curve)->fCubic = fCubic.subDivide(t1, t2);
  }
};

#endif
