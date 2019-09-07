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

#include "include/gpu/GrTexture.h"
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
        kFragment_GrShaderFlag, kHalf4_GrSLType, "leftBorderColor");
    rightBorderColorVar = args.fUniformHandler->addUniform(
        kFragment_GrShaderFlag, kHalf4_GrSLType, "rightBorderColor");
    SkString _sample1099("_sample1099");
    this->invokeChild(_outer.gradLayout_index, &_sample1099, args);
    fragBuilder->codeAppendf(
        "half4 t = %s;\nif (!%s && t.y < 0.0) {\n    %s = half4(0.0);\n} else if (t.x < "
        "0.0) {\n    %s = %s;\n} else if (t.x > 1.0) {\n    %s = %s;\n} else {",
        _sample1099.c_str(),
        (_outer.childProcessor(_outer.gradLayout_index).preservesOpaqueInput() ? "true" : "false"),
        args.fOutputColor, args.fOutputColor,
        args.fUniformHandler->getUniformCStr(leftBorderColorVar), args.fOutputColor,
        args.fUniformHandler->getUniformCStr(rightBorderColorVar));
    SkString _input1767("t");
    SkString _sample1767("_sample1767");
    this->invokeChild(_outer.colorizer_index, _input1767.c_str(), &_sample1767, args);
    fragBuilder->codeAppendf(
        "\n    %s = %s;\n}\n@if (%s) {\n    %s.xyz *= %s.w;\n}\n", args.fOutputColor,
        _sample1767.c_str(), (_outer.makePremul ? "true" : "false"), args.fOutputColor,
        args.fOutputColor);
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
  b->add32((int32_t)makePremul);
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
GrClampedGradientEffect::GrClampedGradientEffect(const GrClampedGradientEffect& src)
    : INHERITED(kGrClampedGradientEffect_ClassID, src.optimizationFlags()),
      colorizer_index(src.colorizer_index),
      gradLayout_index(src.gradLayout_index),
      leftBorderColor(src.leftBorderColor),
      rightBorderColor(src.rightBorderColor),
      makePremul(src.makePremul),
      colorsAreOpaque(src.colorsAreOpaque) {
  this->registerChildProcessor(src.childProcessor(colorizer_index).clone());
  this->registerChildProcessor(src.childProcessor(gradLayout_index).clone());
}
std::unique_ptr<GrFragmentProcessor> GrClampedGradientEffect::clone() const {
  return std::unique_ptr<GrFragmentProcessor>(new GrClampedGradientEffect(*this));
}
