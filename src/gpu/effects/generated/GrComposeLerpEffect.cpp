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

#include "src/core/SkUtils.h"
#include "src/gpu/GrTexture.h"
#include "src/gpu/glsl/GrGLSLFragmentProcessor.h"
#include "src/gpu/glsl/GrGLSLFragmentShaderBuilder.h"
#include "src/gpu/glsl/GrGLSLProgramBuilder.h"
#include "src/sksl/SkSLCPP.h"
#include "src/sksl/SkSLUtil.h"
class GrGLSLComposeLerpEffect : public GrGLSLFragmentProcessor {
 public:
  GrGLSLComposeLerpEffect() noexcept = default;
  void emitCode(EmitArgs& args) override {
    GrGLSLFPFragmentBuilder* fragBuilder = args.fFragBuilder;
    const GrComposeLerpEffect& _outer = args.fFp.cast<GrComposeLerpEffect>();
    (void)_outer;
    auto weight = _outer.weight;
    (void)weight;
    weightVar = args.fUniformHandler->addUniform(
        &_outer, kFragment_GrShaderFlag, kFloat_GrSLType, "weight");
    SkString _sample273 = this->invokeChild(0, args);
    SkString _sample289 = this->invokeChild(1, args);
    fragBuilder->codeAppendf(
        R"SkSL(%s = mix(%s, %s, half(%s));
)SkSL",
        args.fOutputColor, _sample273.c_str(), _sample289.c_str(),
        args.fUniformHandler->getUniformCStr(weightVar));
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
    const GrShaderCaps& caps, GrProcessorKeyBuilder* b) const {}
bool GrComposeLerpEffect::onIsEqual(const GrFragmentProcessor& other) const noexcept {
  const GrComposeLerpEffect& that = other.cast<GrComposeLerpEffect>();
  (void)that;
  if (weight != that.weight) return false;
  return true;
}
GrComposeLerpEffect::GrComposeLerpEffect(const GrComposeLerpEffect& src)
    : INHERITED(kGrComposeLerpEffect_ClassID, src.optimizationFlags()), weight(src.weight) {
  this->cloneAndRegisterAllChildProcessors(src);
}
std::unique_ptr<GrFragmentProcessor> GrComposeLerpEffect::clone() const {
  return std::make_unique<GrComposeLerpEffect>(*this);
}
#if GR_TEST_UTILS
SkString GrComposeLerpEffect::onDumpInfo() const { return SkStringPrintf("(weight=%f)", weight); }
#endif
