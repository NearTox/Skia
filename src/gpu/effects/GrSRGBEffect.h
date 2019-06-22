/*
 * Copyright 2016 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef GrSRGBEffect_DEFINED
#define GrSRGBEffect_DEFINED

#include "src/gpu/GrFragmentProcessor.h"

class GrSRGBEffect : public GrFragmentProcessor {
 public:
  enum class Mode {
    kLinearToSRGB,
    kSRGBToLinear,
  };

  enum class Alpha {
    kPremul,
    kOpaque,
  };

  /**
   * Creates an effect that applies the sRGB transfer function (or its inverse)
   */
  static std::unique_ptr<GrFragmentProcessor> Make(Mode mode, Alpha alpha) {
    return std::unique_ptr<GrFragmentProcessor>(new GrSRGBEffect(mode, alpha));
  }

  const char* name() const noexcept override { return "sRGB"; }

  Mode mode() const noexcept { return fMode; }
  Alpha alpha() const noexcept { return fAlpha; }

  std::unique_ptr<GrFragmentProcessor> clone() const override;

 private:
  GrSRGBEffect(Mode mode, Alpha) noexcept;

  GrGLSLFragmentProcessor* onCreateGLSLInstance() const override;
  void onGetGLSLProcessorKey(const GrShaderCaps&, GrProcessorKeyBuilder*) const noexcept override;
  bool onIsEqual(const GrFragmentProcessor&) const noexcept override;

  SkPMColor4f constantOutputForConstantInput(const SkPMColor4f& input) const noexcept override;

  Mode fMode;
  Alpha fAlpha;

  GR_DECLARE_FRAGMENT_PROCESSOR_TEST

  typedef GrFragmentProcessor INHERITED;
};

#endif
