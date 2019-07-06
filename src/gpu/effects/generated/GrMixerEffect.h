/*
 * Copyright 2019 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/**************************************************************************************************
 *** This file was autogenerated from GrMixerEffect.fp; do not modify.
 **************************************************************************************************/
#ifndef GrMixerEffect_DEFINED
#define GrMixerEffect_DEFINED
#include "include/core/SkTypes.h"

#include "src/gpu/GrCoordTransform.h"
#include "src/gpu/GrFragmentProcessor.h"
class GrMixerEffect : public GrFragmentProcessor {
 public:
  static OptimizationFlags OptFlags(
      const std::unique_ptr<GrFragmentProcessor>& fp0,
      const std::unique_ptr<GrFragmentProcessor>& fp1) {
    auto flags = ProcessorOptimizationFlags(fp0.get());
    if (fp1) {
      flags &= ProcessorOptimizationFlags(fp1.get());
    }
    return flags;
  }

  SkPMColor4f constantOutputForConstantInput(const SkPMColor4f& input) const override {
    const auto c0 = ConstantOutputForConstantInput(this->childProcessor(0), input),
               c1 = (this->numChildProcessors() > 1)
                        ? ConstantOutputForConstantInput(this->childProcessor(1), input)
                        : input;
    return {c0.fR + (c1.fR - c0.fR) * weight, c0.fG + (c1.fG - c0.fG) * weight,
            c0.fB + (c1.fB - c0.fB) * weight, c0.fA + (c1.fA - c0.fA) * weight};
  }
  static std::unique_ptr<GrFragmentProcessor> Make(
      std::unique_ptr<GrFragmentProcessor> fp0, std::unique_ptr<GrFragmentProcessor> fp1,
      float weight) {
    return std::unique_ptr<GrFragmentProcessor>(
        new GrMixerEffect(std::move(fp0), std::move(fp1), weight));
  }
  GrMixerEffect(const GrMixerEffect& src);
  std::unique_ptr<GrFragmentProcessor> clone() const override;
  const char* name() const override { return "MixerEffect"; }
  int fp0_index = -1;
  int fp1_index = -1;
  float weight;

 private:
  GrMixerEffect(
      std::unique_ptr<GrFragmentProcessor> fp0, std::unique_ptr<GrFragmentProcessor> fp1,
      float weight)
      : INHERITED(kGrMixerEffect_ClassID, (OptimizationFlags)OptFlags(fp0, fp1)), weight(weight) {
    SkASSERT(fp0);
    fp0_index = this->numChildProcessors();
    this->registerChildProcessor(std::move(fp0));
    if (fp1) {
      fp1_index = this->numChildProcessors();
      this->registerChildProcessor(std::move(fp1));
    }
  }
  GrGLSLFragmentProcessor* onCreateGLSLInstance() const override;
  void onGetGLSLProcessorKey(const GrShaderCaps&, GrProcessorKeyBuilder*) const override;
  bool onIsEqual(const GrFragmentProcessor&) const override;
  GR_DECLARE_FRAGMENT_PROCESSOR_TEST
  typedef GrFragmentProcessor INHERITED;
};
#endif
