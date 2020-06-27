/*
 * Copyright 2019 Google LLC
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/**************************************************************************************************
 *** This file was autogenerated from GrClampFragmentProcessor.fp; do not modify.
 **************************************************************************************************/
#ifndef GrClampFragmentProcessor_DEFINED
#define GrClampFragmentProcessor_DEFINED
#include "include/core/SkTypes.h"
#include "include/core/SkM44.h"

#include "src/gpu/GrCoordTransform.h"
#include "src/gpu/GrFragmentProcessor.h"
class GrClampFragmentProcessor : public GrFragmentProcessor {
 public:
  SkPMColor4f constantOutputForConstantInput(const SkPMColor4f& input) const noexcept override {
    float clampedAlpha = SkTPin(input.fA, 0.f, 1.f);
    float clampVal = clampToPremul ? clampedAlpha : 1.f;
    return {
        SkTPin(input.fR, 0.f, clampVal), SkTPin(input.fG, 0.f, clampVal),
        SkTPin(input.fB, 0.f, clampVal), clampedAlpha};
  }
  static std::unique_ptr<GrFragmentProcessor> Make(bool clampToPremul) {
    return std::unique_ptr<GrFragmentProcessor>(new GrClampFragmentProcessor(clampToPremul));
  }
  GrClampFragmentProcessor(const GrClampFragmentProcessor& src) noexcept;
  std::unique_ptr<GrFragmentProcessor> clone() const override;
  const char* name() const noexcept override { return "ClampFragmentProcessor"; }
  bool clampToPremul;

 private:
  GrClampFragmentProcessor(bool clampToPremul) noexcept
      : INHERITED(
            kGrClampFragmentProcessor_ClassID,
            (OptimizationFlags)kConstantOutputForConstantInput_OptimizationFlag |
                kPreservesOpaqueInput_OptimizationFlag),
        clampToPremul(clampToPremul) {}
  GrGLSLFragmentProcessor* onCreateGLSLInstance() const override;
  void onGetGLSLProcessorKey(const GrShaderCaps&, GrProcessorKeyBuilder*) const override;
  bool onIsEqual(const GrFragmentProcessor&) const noexcept override;
  GR_DECLARE_FRAGMENT_PROCESSOR_TEST
  typedef GrFragmentProcessor INHERITED;
};
#endif
