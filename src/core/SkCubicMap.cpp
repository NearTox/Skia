/*
 * Copyright 2018 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "include/core/SkCubicMap.h"
#include "include/private/SkTPin.h"
#include "include/private/SkVx.h"
#include "src/core/SkOpts.h"

//#define CUBICMAP_TRACK_MAX_ERROR

#ifdef CUBICMAP_TRACK_MAX_ERROR
#  include "src/pathops/SkPathOpsCubic.h"
#endif

static inline bool nearly_zero(SkScalar x) {
  SkASSERT(x >= 0);
  return x <= 0.0000000001f;
}

#ifdef CUBICMAP_TRACK_MAX_ERROR
static int max_iters;
#endif

#ifdef CUBICMAP_TRACK_MAX_ERROR
static float compute_slow(float A, float B, float C, float x) {
  double roots[3];
  SkDEBUGCODE(int count =)
  SkDCubic::RootsValidT(A, B, C, -x, roots);
  SkASSERT(count == 1);
  return (float)roots[0];
}

static float max_err;
#endif

static float compute_t_from_x(float A, float B, float C, float x) {
#ifdef CUBICMAP_TRACK_MAX_ERROR
  float answer = compute_slow(A, B, C, x);
#endif
  float answer2 = SkOpts::cubic_solver(A, B, C, -x);

#ifdef CUBICMAP_TRACK_MAX_ERROR
  float err = sk_float_abs(answer - answer2);
  if (err > max_err) {
    max_err = err;
    SkDebugf("max error %g\n", max_err);
  }
#endif
  return answer2;
}

float SkCubicMap::computeYFromX(float x) const {
  x = SkTPin(x, 0.0f, 1.0f);

  if (nearly_zero(x) || nearly_zero(1 - x)) {
    return x;
  }
  if (fType == kLine_Type) {
    return x;
  }
  float t;
  if (fType == kCubeRoot_Type) {
    t = sk_float_pow(x / fCoeff[0].fX, 1.0f / 3);
  } else {
    t = compute_t_from_x(fCoeff[0].fX, fCoeff[1].fX, fCoeff[2].fX, x);
  }
  float a = fCoeff[0].fY;
  float b = fCoeff[1].fY;
  float c = fCoeff[2].fY;
  float y = ((a * t + b) * t + c) * t;

  return y;
}

static inline bool coeff_nearly_zero(float delta) { return sk_float_abs(delta) <= 0.0000001f; }

SkCubicMap::SkCubicMap(SkPoint p1, SkPoint p2) {
  // Clamp X values only (we allow Ys outside [0..1]).
  p1.fX = std::min(std::max(p1.fX, 0.0f), 1.0f);
  p2.fX = std::min(std::max(p2.fX, 0.0f), 1.0f);

  auto s1 = skvx::float2::Load(&p1) * 3;
  auto s2 = skvx::float2::Load(&p2) * 3;

  (1 + s1 - s2).store(&fCoeff[0]);
  (s2 - s1 - s1).store(&fCoeff[1]);
  s1.store(&fCoeff[2]);

  fType = kSolver_Type;
  if (SkScalarNearlyEqual(p1.fX, p1.fY) && SkScalarNearlyEqual(p2.fX, p2.fY)) {
    fType = kLine_Type;
  } else if (coeff_nearly_zero(fCoeff[1].fX) && coeff_nearly_zero(fCoeff[2].fX)) {
    fType = kCubeRoot_Type;
  }
}

SkPoint SkCubicMap::computeFromT(float t) const {
  auto a = skvx::float2::Load(&fCoeff[0]);
  auto b = skvx::float2::Load(&fCoeff[1]);
  auto c = skvx::float2::Load(&fCoeff[2]);

  SkPoint result;
  (((a * t + b) * t + c) * t).store(&result);
  return result;
}
