/*
 * Copyright 2018 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/**************************************************************************************************
 *** This file was autogenerated from GrAARectEffect.fp; do not modify.
 **************************************************************************************************/
#ifndef GrAARectEffect_DEFINED
#define GrAARectEffect_DEFINED

#include "include/core/SkM44.h"
#include "include/core/SkTypes.h"

#include "src/gpu/GrFragmentProcessor.h"

class GrAARectEffect : public GrFragmentProcessor {
 public:
  static std::unique_ptr<GrFragmentProcessor> Make(
      std::unique_ptr<GrFragmentProcessor> inputFP, GrClipEdgeType edgeType, SkRect rect) {
    return std::unique_ptr<GrFragmentProcessor>(
        new GrAARectEffect(std::move(inputFP), edgeType, rect));
  }
  GrAARectEffect(const GrAARectEffect& src);
  std::unique_ptr<GrFragmentProcessor> clone() const override;
  const char* name() const override { return "AARectEffect"; }
  GrClipEdgeType edgeType;
  SkRect rect;

 private:
  GrAARectEffect(std::unique_ptr<GrFragmentProcessor> inputFP, GrClipEdgeType edgeType, SkRect rect)
      : INHERITED(
            kGrAARectEffect_ClassID,
            (OptimizationFlags)(inputFP ? ProcessorOptimizationFlags(inputFP.get()) : kAll_OptimizationFlags) &
                kCompatibleWithCoverageAsAlpha_OptimizationFlag),
        edgeType(edgeType),
        rect(rect) {
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
