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

#include "include/core/SkM44.h"
#include "include/core/SkTypes.h"

#include "src/gpu/GrFragmentProcessor.h"

class GrCircleEffect : public GrFragmentProcessor {
 public:
  static GrFPResult Make(
      std::unique_ptr<GrFragmentProcessor> inputFP, GrClipEdgeType edgeType, SkPoint center,
      float radius) {
    // A radius below half causes the implicit insetting done by this processor to become
    // inverted. We could handle this case by making the processor code more complicated.
    if (radius < .5f && GrProcessorEdgeTypeIsInverseFill(edgeType)) {
      return GrFPFailure(std::move(inputFP));
    }
    return GrFPSuccess(std::unique_ptr<GrFragmentProcessor>(
        new GrCircleEffect(std::move(inputFP), edgeType, center, radius)));
  }
  GrCircleEffect(const GrCircleEffect& src);
  std::unique_ptr<GrFragmentProcessor> clone() const override;
  const char* name() const override { return "CircleEffect"; }
  bool usesExplicitReturn() const override;
  GrClipEdgeType edgeType;
  SkPoint center;
  float radius;

 private:
  GrCircleEffect(
      std::unique_ptr<GrFragmentProcessor> inputFP, GrClipEdgeType edgeType, SkPoint center,
      float radius)
      : INHERITED(
            kGrCircleEffect_ClassID,
            (OptimizationFlags)(
                inputFP ? ProcessorOptimizationFlags(inputFP.get()) : kAll_OptimizationFlags) &
                kCompatibleWithCoverageAsAlpha_OptimizationFlag),
        edgeType(edgeType),
        center(center),
        radius(radius) {
    this->registerChild(std::move(inputFP), SkSL::SampleUsage::PassThrough());
  }
  GrGLSLFragmentProcessor* onCreateGLSLInstance() const override;
  void onGetGLSLProcessorKey(const GrShaderCaps&, GrProcessorKeyBuilder*) const override;
  bool onIsEqual(const GrFragmentProcessor&) const override;
#if GR_TEST_UTILS
  SkString onDumpInfo() const override;
#endif
  GR_DECLARE_FRAGMENT_PROCESSOR_TEST
  using INHERITED = GrFragmentProcessor;
};
#endif
