/*
 * Copyright 2019 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/**************************************************************************************************
 *** This file was autogenerated from GrOverrideInputFragmentProcessor.fp; do not modify.
 **************************************************************************************************/
#ifndef GrOverrideInputFragmentProcessor_DEFINED
#define GrOverrideInputFragmentProcessor_DEFINED

#include "include/core/SkM44.h"
#include "include/core/SkTypes.h"

#include "src/gpu/GrFragmentProcessor.h"

class GrOverrideInputFragmentProcessor : public GrFragmentProcessor {
 public:
  static OptimizationFlags OptFlags(
      const std::unique_ptr<GrFragmentProcessor>& fp, const SkPMColor4f& color) {
    auto childFlags = ProcessorOptimizationFlags(fp.get());
    auto flags = kNone_OptimizationFlags;
    if (childFlags & kConstantOutputForConstantInput_OptimizationFlag) {
      flags |= kConstantOutputForConstantInput_OptimizationFlag;
    }
    if ((childFlags & kPreservesOpaqueInput_OptimizationFlag) && color.isOpaque()) {
      flags |= kPreservesOpaqueInput_OptimizationFlag;
    }
    return flags;
  }

  SkPMColor4f constantOutputForConstantInput(const SkPMColor4f& input) const override {
    return ConstantOutputForConstantInput(this->childProcessor(0), uniformColor);
  }

  static std::unique_ptr<GrFragmentProcessor> Make(
      std::unique_ptr<GrFragmentProcessor> fp, const SkPMColor4f& color, bool useUniform = true) {
    return std::unique_ptr<GrFragmentProcessor>(
        new GrOverrideInputFragmentProcessor(std::move(fp), useUniform, color, color));
  }
  GrOverrideInputFragmentProcessor(const GrOverrideInputFragmentProcessor& src);
  std::unique_ptr<GrFragmentProcessor> clone() const override;
  const char* name() const noexcept override { return "OverrideInputFragmentProcessor"; }
  bool useUniform;
  SkPMColor4f uniformColor;
  SkPMColor4f literalColor;

 private:
  GrOverrideInputFragmentProcessor(
      std::unique_ptr<GrFragmentProcessor> fp, bool useUniform, SkPMColor4f uniformColor,
      SkPMColor4f literalColor)
      : INHERITED(
            kGrOverrideInputFragmentProcessor_ClassID,
            (OptimizationFlags)OptFlags(fp, useUniform ? uniformColor : literalColor)),
        useUniform(useUniform),
        uniformColor(uniformColor),
        literalColor(literalColor) {
    SkASSERT(fp);
    this->registerChild(std::move(fp), SkSL::SampleUsage::PassThrough());
  }
  GrGLSLFragmentProcessor* onCreateGLSLInstance() const override;
  void onGetGLSLProcessorKey(const GrShaderCaps&, GrProcessorKeyBuilder*) const override;
  bool onIsEqual(const GrFragmentProcessor&) const noexcept override;
#if GR_TEST_UTILS
  SkString onDumpInfo() const override;
#endif
  GR_DECLARE_FRAGMENT_PROCESSOR_TEST
  typedef GrFragmentProcessor INHERITED;
};
#endif
