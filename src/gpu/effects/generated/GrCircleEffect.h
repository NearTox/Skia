/*
 * Copyright 2017 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/**************************************************************************************************
 *** This file was autogenerated from GrCircleEffect.fp; do not modify.
 **************************************************************************************************/
#ifndef GrCircleEffect_DEFINED
#define GrCircleEffect_DEFINED
#include "include/core/SkTypes.h"
#include "include/core/SkM44.h"

#include "src/gpu/GrCoordTransform.h"
#include "src/gpu/GrFragmentProcessor.h"
class GrCircleEffect : public GrFragmentProcessor {
 public:
  static std::unique_ptr<GrFragmentProcessor> Make(
      GrClipEdgeType edgeType, SkPoint center, float radius) {
    // A radius below half causes the implicit insetting done by this processor to become
    // inverted. We could handle this case by making the processor code more complicated.
    if (radius < .5f && GrProcessorEdgeTypeIsInverseFill(edgeType)) {
      return nullptr;
    }
    return std::unique_ptr<GrFragmentProcessor>(new GrCircleEffect(edgeType, center, radius));
  }
  GrCircleEffect(const GrCircleEffect& src);
  std::unique_ptr<GrFragmentProcessor> clone() const override;
  const char* name() const override { return "CircleEffect"; }
  GrClipEdgeType edgeType;
  SkPoint center;
  float radius;

 private:
  GrCircleEffect(GrClipEdgeType edgeType, SkPoint center, float radius)
      : INHERITED(
            kGrCircleEffect_ClassID,
            (OptimizationFlags)kCompatibleWithCoverageAsAlpha_OptimizationFlag),
        edgeType(edgeType),
        center(center),
        radius(radius) {}
  GrGLSLFragmentProcessor* onCreateGLSLInstance() const override;
  void onGetGLSLProcessorKey(const GrShaderCaps&, GrProcessorKeyBuilder*) const override;
  bool onIsEqual(const GrFragmentProcessor&) const override;
  GR_DECLARE_FRAGMENT_PROCESSOR_TEST
  typedef GrFragmentProcessor INHERITED;
};
#endif
