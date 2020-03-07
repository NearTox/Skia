/*
 * Copyright 2018 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/**************************************************************************************************
 *** This file was autogenerated from GrTiledGradientEffect.fp; do not modify.
 **************************************************************************************************/
#include "GrTiledGradientEffect.h"

#include "include/gpu/GrTexture.h"
#include "src/gpu/glsl/GrGLSLFragmentProcessor.h"
#include "src/gpu/glsl/GrGLSLFragmentShaderBuilder.h"
#include "src/gpu/glsl/GrGLSLProgramBuilder.h"
#include "src/sksl/SkSLCPP.h"
#include "src/sksl/SkSLUtil.h"
class GrGLSLTiledGradientEffect : public GrGLSLFragmentProcessor {
 public:
  GrGLSLTiledGradientEffect() {}
  void emitCode(EmitArgs& args) override {
    GrGLSLFPFragmentBuilder* fragBuilder = args.fFragBuilder;
    const GrTiledGradientEffect& _outer = args.fFp.cast<GrTiledGradientEffect>();
    (void)_outer;
    auto mirror = _outer.mirror;
    (void)mirror;
    auto makePremul = _outer.makePremul;
    (void)makePremul;
    auto colorsAreOpaque = _outer.colorsAreOpaque;
    (void)colorsAreOpaque;
    SkString _sample453;
    _sample453 = this->invokeChild(_outer.gradLayout_index, args);
    fragBuilder->codeAppendf(
        "half4 t = %s;\nif (!%s && t.y < 0.0) {\n    %s = half4(0.0);\n} else {\n    @if "
        "(%s) {\n        half t_1 = t.x - 1.0;\n        half tiled_t = (t_1 - 2.0 * "
        "floor(t_1 * 0.5)) - 1.0;\n        if (sk_Caps.mustDoOpBetweenFloorAndAbs) {\n     "
        "       tiled_t = clamp(tiled_t, -1.0, 1.0);\n        }\n        t.x = "
        "abs(tiled_t);\n    } else {\n        t.x = fract(t.x);\n    }",
        _sample453.c_str(),
        (_outer.childProcessor(_outer.gradLayout_index).preservesOpaqueInput() ? "true" : "false"),
        args.fOutputColor, (_outer.mirror ? "true" : "false"));
    SkString _input1464("t");
    SkString _sample1464;
    _sample1464 = this->invokeChild(_outer.colorizer_index, _input1464.c_str(), args);
    fragBuilder->codeAppendf(
        "\n    %s = %s;\n}\n@if (%s) {\n    %s.xyz *= %s.w;\n}\n", args.fOutputColor,
        _sample1464.c_str(), (_outer.makePremul ? "true" : "false"), args.fOutputColor,
        args.fOutputColor);
  }

 private:
  void onSetData(const GrGLSLProgramDataManager& pdman, const GrFragmentProcessor& _proc) override {
  }
};
GrGLSLFragmentProcessor* GrTiledGradientEffect::onCreateGLSLInstance() const {
  return new GrGLSLTiledGradientEffect();
}
void GrTiledGradientEffect::onGetGLSLProcessorKey(
    const GrShaderCaps& caps, GrProcessorKeyBuilder* b) const {
  b->add32((int32_t)mirror);
  b->add32((int32_t)makePremul);
}
bool GrTiledGradientEffect::onIsEqual(const GrFragmentProcessor& other) const {
  const GrTiledGradientEffect& that = other.cast<GrTiledGradientEffect>();
  (void)that;
  if (mirror != that.mirror) return false;
  if (makePremul != that.makePremul) return false;
  if (colorsAreOpaque != that.colorsAreOpaque) return false;
  return true;
}
GrTiledGradientEffect::GrTiledGradientEffect(const GrTiledGradientEffect& src)
    : INHERITED(kGrTiledGradientEffect_ClassID, src.optimizationFlags()),
      colorizer_index(src.colorizer_index),
      gradLayout_index(src.gradLayout_index),
      mirror(src.mirror),
      makePremul(src.makePremul),
      colorsAreOpaque(src.colorsAreOpaque) {
  this->registerChildProcessor(src.childProcessor(colorizer_index).clone());
  this->registerChildProcessor(src.childProcessor(gradLayout_index).clone());
}
std::unique_ptr<GrFragmentProcessor> GrTiledGradientEffect::clone() const {
  return std::unique_ptr<GrFragmentProcessor>(new GrTiledGradientEffect(*this));
}
