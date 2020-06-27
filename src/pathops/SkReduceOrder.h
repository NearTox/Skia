/*
 * Copyright 2012 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */
#ifndef SkReduceOrder_DEFINED
#define SkReduceOrder_DEFINED

#include "src/pathops/SkPathOpsCubic.h"
#include "src/pathops/SkPathOpsLine.h"
#include "src/pathops/SkPathOpsQuad.h"

struct SkConic;

union SkReduceOrder {
  enum Quadratics { kNo_Quadratics, kAllow_Quadratics };

  int reduce(const SkDCubic& cubic, Quadratics) noexcept;
  int reduce(const SkDLine& line) noexcept;
  int reduce(const SkDQuad& quad) noexcept;

  static SkPath::Verb Conic(const SkConic& conic, SkPoint* reducePts) noexcept;
  static SkPath::Verb Cubic(const SkPoint pts[4], SkPoint* reducePts) noexcept;
  static SkPath::Verb Quad(const SkPoint pts[3], SkPoint* reducePts) noexcept;

  SkDLine fLine;
  SkDQuad fQuad;
  SkDCubic fCubic;
};

#endif
