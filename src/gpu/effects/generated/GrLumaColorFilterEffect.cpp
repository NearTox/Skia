/*
 * Copyright 2018 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/**************************************************************************************************
 *** This file was autogenerated from GrLumaColorFilterEffect.fp; do not modify.
 **************************************************************************************************/
#include "GrLumaColorFilterEffect.h"

#include "src/gpu/GrTexture.h"
#include "src/gpu/glsl/GrGLSLFragmentProcessor.h"
#include "src/gpu/glsl/GrGLSLFragmentShaderBuilder.h"
#include "src/gpu/glsl/GrGLSLProgramBuilder.h"
#include "src/sksl/SkSLCPP.h"
#include "src/sksl/SkSLUtil.h"
class GrGLSLLumaColorFilterEffect : public GrGLSLFragmentProcessor {
 public:
  GrGLSLLumaColorFilterEffect() {}
  void emitCode(EmitArgs& args) override {
    GrGLSLFPFragmentBuilder* fragBuilder = args.fFragBuilder;
    const GrLumaColorFilterEffect& _outer = args.fFp.cast<GrLumaColorFilterEffect>();
    (void)_outer;
    fragBuilder->codeAppendf(
        "\nhalf luma = clamp(dot(half3(0.2125999927520752, 0.71520000696182251, "
        "0.072200000286102295), %s.xyz), 0.0, 1.0);\n%s = half4(0.0, 0.0, 0.0, luma);\n",
        args.fInputColor, args.fOutputColor);
  }

 private:
  void onSetData(
      const GrGLSLProgramDataManager& pdman, const GrFragmentProcessor& _proc) noexcept override {}
};
GrGLSLFragmentProcessor* GrLumaColorFilterEffect::onCreateGLSLInstance() const {
  return new GrGLSLLumaColorFilterEffect();
}
void GrLumaColorFilterEffect::onGetGLSLProcessorKey(
    const GrShaderCaps& caps, GrProcessorKeyBuilder* b) const {}
bool GrLumaColorFilterEffect::onIsEqual(const GrFragmentProcessor& other) const noexcept {
  const GrLumaColorFilterEffect& that = other.cast<GrLumaColorFilterEffect>();
  (void)that;
  return true;
}
GrLumaColorFilterEffect::GrLumaColorFilterEffect(const GrLumaColorFilterEffect& src) noexcept
    : INHERITED(kGrLumaColorFilterEffect_ClassID, src.optimizationFlags()) {}
std::unique_ptr<GrFragmentProcessor> GrLumaColorFilterEffect::clone() const {
  return std::unique_ptr<GrFragmentProcessor>(new GrLumaColorFilterEffect(*this));
}
