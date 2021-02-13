/*
 * Copyright 2020 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/**************************************************************************************************
 *** This file was autogenerated from GrHighContrastFilterEffect.fp; do not modify.
 **************************************************************************************************/
#ifndef GrHighContrastFilterEffect_DEFINED
#define GrHighContrastFilterEffect_DEFINED

#include "include/core/SkM44.h"
#include "include/core/SkTypes.h"

#include "include/effects/SkHighContrastFilter.h"

#include "src/gpu/GrFragmentProcessor.h"

class GrHighContrastFilterEffect : public GrFragmentProcessor {
 public:
  static std::unique_ptr<GrFragmentProcessor> Make(
      std::unique_ptr<GrFragmentProcessor> inputFP, const SkHighContrastConfig& config,
      bool linearize) {
    float contrastMod = (1 + config.fContrast) / (1 - config.fContrast);

    return std::unique_ptr<GrFragmentProcessor>(new GrHighContrastFilterEffect(
        std::move(inputFP), contrastMod, contrastMod != 1.0, config.fGrayscale,
        config.fInvertStyle == SkHighContrastConfig::InvertStyle::kInvertBrightness,
        config.fInvertStyle == SkHighContrastConfig::InvertStyle::kInvertLightness, linearize));
  }
  GrHighContrastFilterEffect(const GrHighContrastFilterEffect& src);
  std::unique_ptr<GrFragmentProcessor> clone() const override;
  const char* name() const override { return "HighContrastFilterEffect"; }
  float contrastMod;
  bool hasContrast;
  bool grayscale;
  bool invertBrightness;
  bool invertLightness;
  bool linearize;

 private:
  GrHighContrastFilterEffect(
      std::unique_ptr<GrFragmentProcessor> inputFP, float contrastMod, bool hasContrast,
      bool grayscale, bool invertBrightness, bool invertLightness, bool linearize)
      : INHERITED(kGrHighContrastFilterEffect_ClassID, (OptimizationFlags)kNone_OptimizationFlags),
        contrastMod(contrastMod),
        hasContrast(hasContrast),
        grayscale(grayscale),
        invertBrightness(invertBrightness),
        invertLightness(invertLightness),
        linearize(linearize) {
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
