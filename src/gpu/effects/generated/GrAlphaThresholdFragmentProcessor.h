/*
 * Copyright 2018 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/**************************************************************************************************
 *** This file was autogenerated from GrAlphaThresholdFragmentProcessor.fp; do not modify.
 **************************************************************************************************/
#ifndef GrAlphaThresholdFragmentProcessor_DEFINED
#define GrAlphaThresholdFragmentProcessor_DEFINED

#include "include/core/SkM44.h"
#include "include/core/SkTypes.h"

#include "src/gpu/GrFragmentProcessor.h"

class GrAlphaThresholdFragmentProcessor : public GrFragmentProcessor {
 public:
  static std::unique_ptr<GrFragmentProcessor> Make(
      std::unique_ptr<GrFragmentProcessor> inputFP, std::unique_ptr<GrFragmentProcessor> maskFP,
      float innerThreshold, float outerThreshold) {
    return std::unique_ptr<GrFragmentProcessor>(new GrAlphaThresholdFragmentProcessor(
        std::move(inputFP), std::move(maskFP), innerThreshold, outerThreshold));
  }
  GrAlphaThresholdFragmentProcessor(const GrAlphaThresholdFragmentProcessor& src);
  std::unique_ptr<GrFragmentProcessor> clone() const override;
  const char* name() const override { return "AlphaThresholdFragmentProcessor"; }
  float innerThreshold;
  float outerThreshold;

 private:
  GrAlphaThresholdFragmentProcessor(
      std::unique_ptr<GrFragmentProcessor> inputFP, std::unique_ptr<GrFragmentProcessor> maskFP,
      float innerThreshold, float outerThreshold)
      : INHERITED(
            kGrAlphaThresholdFragmentProcessor_ClassID,
            (OptimizationFlags)(inputFP ? ProcessorOptimizationFlags(inputFP.get()) : kAll_OptimizationFlags) &
                ((outerThreshold >= 1.0) ? kPreservesOpaqueInput_OptimizationFlag
                                         : kNone_OptimizationFlags)),
        innerThreshold(innerThreshold),
        outerThreshold(outerThreshold) {
    this->registerChild(std::move(inputFP), SkSL::SampleUsage::PassThrough());
    this->registerChild(std::move(maskFP), SkSL::SampleUsage::PassThrough());
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
