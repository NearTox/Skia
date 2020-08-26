/*
 * Copyright 2019 Google LLC.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/**************************************************************************************************
 *** This file was autogenerated from GrComposeLerpEffect.fp; do not modify.
 **************************************************************************************************/
#include "GrComposeLerpEffect.h"

#include "src/gpu/GrTexture.h"
#include "src/gpu/glsl/GrGLSLFragmentProcessor.h"
#include "src/gpu/glsl/GrGLSLFragmentShaderBuilder.h"
#include "src/gpu/glsl/GrGLSLProgramBuilder.h"
#include "src/sksl/SkSLCPP.h"
#include "src/sksl/SkSLUtil.h"
class GrGLSLComposeLerpEffect : public GrGLSLFragmentProcessor {
 public:
  GrGLSLComposeLerpEffect() {}
  void emitCode(EmitArgs& args) override {
    GrGLSLFPFragmentBuilder* fragBuilder = args.fFragBuilder;
    const GrComposeLerpEffect& _outer = args.fFp.cast<GrComposeLerpEffect>();
    (void)_outer;
    auto weight = _outer.weight;
    (void)weight;
    weightVar = args.fUniformHandler->addUniform(
        &_outer, kFragment_GrShaderFlag, kFloat_GrSLType, "weight");
    SkString _sample290;
    if (_outer.child1_index >= 0) {
      _sample290 = this->invokeChild(_outer.child1_index, args);
    } else {
      _sample290 = "half4(1)";
    }
    SkString _sample358;
    if (_outer.child2_index >= 0) {
      _sample358 = this->invokeChild(_outer.child2_index, args);
    } else {
      _sample358 = "half4(1)";
    }
    fragBuilder->codeAppendf(
        R"SkSL(%s = mix(%s ? %s : %s, %s ? %s : %s, half(%s));
)SkSL",
        args.fOutputColor, _outer.child1_index >= 0 ? "true" : "false", _sample290.c_str(),
        args.fInputColor, _outer.child2_index >= 0 ? "true" : "false", _sample358.c_str(),
        args.fInputColor, args.fUniformHandler->getUniformCStr(weightVar));
  }

 private:
  void onSetData(const GrGLSLProgramDataManager& pdman, const GrFragmentProcessor& _proc) override {
    const GrComposeLerpEffect& _outer = _proc.cast<GrComposeLerpEffect>();
    { pdman.set1f(weightVar, (_outer.weight)); }
  }
  UniformHandle weightVar;
};
GrGLSLFragmentProcessor* GrComposeLerpEffect::onCreateGLSLInstance() const {
  return new GrGLSLComposeLerpEffect();
}
void GrComposeLerpEffect::onGetGLSLProcessorKey(
    const GrShaderCaps& caps, GrProcessorKeyBuilder* b) const noexcept {}
bool GrComposeLerpEffect::onIsEqual(const GrFragmentProcessor& other) const noexcept {
  const GrComposeLerpEffect& that = other.cast<GrComposeLerpEffect>();
  (void)that;
  if (weight != that.weight) return false;
  return true;
}
GrComposeLerpEffect::GrComposeLerpEffect(const GrComposeLerpEffect& src)
    : INHERITED(kGrComposeLerpEffect_ClassID, src.optimizationFlags()), weight(src.weight) {
  if (src.child1_index >= 0) {
    child1_index = this->cloneAndRegisterChildProcessor(src.childProcessor(src.child1_index));
  }
  if (src.child2_index >= 0) {
    child2_index = this->cloneAndRegisterChildProcessor(src.childProcessor(src.child2_index));
  }
}
std::unique_ptr<GrFragmentProcessor> GrComposeLerpEffect::clone() const {
  return std::unique_ptr<GrFragmentProcessor>(new GrComposeLerpEffect(*this));
}
