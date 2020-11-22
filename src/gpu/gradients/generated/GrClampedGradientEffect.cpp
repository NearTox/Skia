/*
 * Copyright 2018 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/**************************************************************************************************
 *** This file was autogenerated from GrClampedGradientEffect.fp; do not modify.
 **************************************************************************************************/
#include "GrClampedGradientEffect.h"

#include "src/core/SkUtils.h"
#include "src/gpu/GrTexture.h"
#include "src/gpu/glsl/GrGLSLFragmentProcessor.h"
#include "src/gpu/glsl/GrGLSLFragmentShaderBuilder.h"
#include "src/gpu/glsl/GrGLSLProgramBuilder.h"
#include "src/sksl/SkSLCPP.h"
#include "src/sksl/SkSLUtil.h"
class GrGLSLClampedGradientEffect : public GrGLSLFragmentProcessor {
 public:
  GrGLSLClampedGradientEffect() {}
  void emitCode(EmitArgs& args) override {
    GrGLSLFPFragmentBuilder* fragBuilder = args.fFragBuilder;
    const GrClampedGradientEffect& _outer = args.fFp.cast<GrClampedGradientEffect>();
    (void)_outer;
    auto leftBorderColor = _outer.leftBorderColor;
    (void)leftBorderColor;
    auto rightBorderColor = _outer.rightBorderColor;
    (void)rightBorderColor;
    auto makePremul = _outer.makePremul;
    (void)makePremul;
    auto colorsAreOpaque = _outer.colorsAreOpaque;
    (void)colorsAreOpaque;
    leftBorderColorVar = args.fUniformHandler->addUniform(
        &_outer, kFragment_GrShaderFlag, kHalf4_GrSLType, "leftBorderColor");
    rightBorderColorVar = args.fUniformHandler->addUniform(
        &_outer, kFragment_GrShaderFlag, kHalf4_GrSLType, "rightBorderColor");
    SkString _sample1102 = this->invokeChild(1, args);
    fragBuilder->codeAppendf(
        R"SkSL(half4 t = %s;
if (!%s && t.y < 0.0) {
    %s = half4(0.0);
} else if (t.x < 0.0) {
    %s = %s;
} else if (t.x > 1.0) {
    %s = %s;
} else {)SkSL",
        _sample1102.c_str(), (_outer.childProcessor(1)->preservesOpaqueInput() ? "true" : "false"),
        args.fOutputColor, args.fOutputColor,
        args.fUniformHandler->getUniformCStr(leftBorderColorVar), args.fOutputColor,
        args.fUniformHandler->getUniformCStr(rightBorderColorVar));
    SkString _coords1871("float2(half2(t.x, 0.0))");
    SkString _sample1871 = this->invokeChild(0, args, _coords1871.c_str());
    fragBuilder->codeAppendf(
        R"SkSL(
    %s = %s;
}
@if (%s) {
    %s.xyz *= %s.w;
}
)SkSL",
        args.fOutputColor, _sample1871.c_str(), (_outer.makePremul ? "true" : "false"),
        args.fOutputColor, args.fOutputColor);
  }

 private:
  void onSetData(const GrGLSLProgramDataManager& pdman, const GrFragmentProcessor& _proc) override {
    const GrClampedGradientEffect& _outer = _proc.cast<GrClampedGradientEffect>();
    {
      const SkPMColor4f& leftBorderColorValue = _outer.leftBorderColor;
      if (leftBorderColorPrev != leftBorderColorValue) {
        leftBorderColorPrev = leftBorderColorValue;
        pdman.set4fv(leftBorderColorVar, 1, leftBorderColorValue.vec());
      }
      const SkPMColor4f& rightBorderColorValue = _outer.rightBorderColor;
      if (rightBorderColorPrev != rightBorderColorValue) {
        rightBorderColorPrev = rightBorderColorValue;
        pdman.set4fv(rightBorderColorVar, 1, rightBorderColorValue.vec());
      }
    }
  }
  SkPMColor4f leftBorderColorPrev = {SK_FloatNaN, SK_FloatNaN, SK_FloatNaN, SK_FloatNaN};
  SkPMColor4f rightBorderColorPrev = {SK_FloatNaN, SK_FloatNaN, SK_FloatNaN, SK_FloatNaN};
  UniformHandle leftBorderColorVar;
  UniformHandle rightBorderColorVar;
};
GrGLSLFragmentProcessor* GrClampedGradientEffect::onCreateGLSLInstance() const {
  return new GrGLSLClampedGradientEffect();
}
void GrClampedGradientEffect::onGetGLSLProcessorKey(
    const GrShaderCaps& caps, GrProcessorKeyBuilder* b) const {
  b->add32((uint32_t)makePremul);
}
bool GrClampedGradientEffect::onIsEqual(const GrFragmentProcessor& other) const {
  const GrClampedGradientEffect& that = other.cast<GrClampedGradientEffect>();
  (void)that;
  if (leftBorderColor != that.leftBorderColor) return false;
  if (rightBorderColor != that.rightBorderColor) return false;
  if (makePremul != that.makePremul) return false;
  if (colorsAreOpaque != that.colorsAreOpaque) return false;
  return true;
}
bool GrClampedGradientEffect::usesExplicitReturn() const { return false; }
GrClampedGradientEffect::GrClampedGradientEffect(const GrClampedGradientEffect& src)
    : INHERITED(kGrClampedGradientEffect_ClassID, src.optimizationFlags()),
      leftBorderColor(src.leftBorderColor),
      rightBorderColor(src.rightBorderColor),
      makePremul(src.makePremul),
      colorsAreOpaque(src.colorsAreOpaque) {
  this->cloneAndRegisterAllChildProcessors(src);
}
std::unique_ptr<GrFragmentProcessor> GrClampedGradientEffect::clone() const {
  return std::make_unique<GrClampedGradientEffect>(*this);
}
#if GR_TEST_UTILS
SkString GrClampedGradientEffect::onDumpInfo() const {
  return SkStringPrintf(
      "(leftBorderColor=half4(%f, %f, %f, %f), rightBorderColor=half4(%f, %f, %f, %f), "
      "makePremul=%s, colorsAreOpaque=%s)",
      leftBorderColor.fR, leftBorderColor.fG, leftBorderColor.fB, leftBorderColor.fA,
      rightBorderColor.fR, rightBorderColor.fG, rightBorderColor.fB, rightBorderColor.fA,
      (makePremul ? "true" : "false"), (colorsAreOpaque ? "true" : "false"));
}
#endif
