/*
 * Copyright 2018 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/**************************************************************************************************
 *** This file was autogenerated from GrPremulInputFragmentProcessor.fp; do not modify.
 **************************************************************************************************/
#ifndef GrPremulInputFragmentProcessor_DEFINED
#define GrPremulInputFragmentProcessor_DEFINED
#include "include/core/SkTypes.h"
#include "include/private/SkM44.h"

#include "src/gpu/GrCoordTransform.h"
#include "src/gpu/GrFragmentProcessor.h"
class GrPremulInputFragmentProcessor : public GrFragmentProcessor {
 public:
  SkPMColor4f constantOutputForConstantInput(const SkPMColor4f& input) const override {
    return SkColor4f{input.fR, input.fG, input.fB, input.fA}.premul();
  }
  static std::unique_ptr<GrFragmentProcessor> Make() {
    return std::unique_ptr<GrFragmentProcessor>(new GrPremulInputFragmentProcessor());
  }
  GrPremulInputFragmentProcessor(const GrPremulInputFragmentProcessor& src);
  std::unique_ptr<GrFragmentProcessor> clone() const override;
  const char* name() const override { return "PremulInputFragmentProcessor"; }

 private:
  GrPremulInputFragmentProcessor()
      : INHERITED(
            kGrPremulInputFragmentProcessor_ClassID,
            (OptimizationFlags)kPreservesOpaqueInput_OptimizationFlag |
                kConstantOutputForConstantInput_OptimizationFlag) {}
  GrGLSLFragmentProcessor* onCreateGLSLInstance() const override;
  void onGetGLSLProcessorKey(const GrShaderCaps&, GrProcessorKeyBuilder*) const override;
  bool onIsEqual(const GrFragmentProcessor&) const override;
  GR_DECLARE_FRAGMENT_PROCESSOR_TEST
  typedef GrFragmentProcessor INHERITED;
};
#endif
