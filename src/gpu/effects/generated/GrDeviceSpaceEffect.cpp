/*
 * Copyright 2020 Google LLC
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/**************************************************************************************************
 *** This file was autogenerated from GrDeviceSpaceEffect.fp; do not modify.
 **************************************************************************************************/
#include "GrDeviceSpaceEffect.h"

#include "src/core/SkUtils.h"
#include "src/gpu/GrTexture.h"
#include "src/gpu/glsl/GrGLSLFragmentProcessor.h"
#include "src/gpu/glsl/GrGLSLFragmentShaderBuilder.h"
#include "src/gpu/glsl/GrGLSLProgramBuilder.h"
#include "src/sksl/SkSLCPP.h"
#include "src/sksl/SkSLUtil.h"
class GrGLSLDeviceSpaceEffect : public GrGLSLFragmentProcessor {
 public:
  GrGLSLDeviceSpaceEffect() {}
  void emitCode(EmitArgs& args) override {
    GrGLSLFPFragmentBuilder* fragBuilder = args.fFragBuilder;
    const GrDeviceSpaceEffect& _outer = args.fFp.cast<GrDeviceSpaceEffect>();
    (void)_outer;
    SkString _coords203("sk_FragCoord.xy");
    SkString _sample203 = this->invokeChild(0, args, _coords203.c_str());
    fragBuilder->codeAppendf(
        R"SkSL(%s = %s;
)SkSL",
        args.fOutputColor, _sample203.c_str());
  }

 private:
  void onSetData(const GrGLSLProgramDataManager& pdman, const GrFragmentProcessor& _proc) override {
  }
};
GrGLSLFragmentProcessor* GrDeviceSpaceEffect::onCreateGLSLInstance() const {
  return new GrGLSLDeviceSpaceEffect();
}
void GrDeviceSpaceEffect::onGetGLSLProcessorKey(
    const GrShaderCaps& caps, GrProcessorKeyBuilder* b) const {}
bool GrDeviceSpaceEffect::onIsEqual(const GrFragmentProcessor& other) const {
  const GrDeviceSpaceEffect& that = other.cast<GrDeviceSpaceEffect>();
  (void)that;
  return true;
}
bool GrDeviceSpaceEffect::usesExplicitReturn() const { return false; }
GrDeviceSpaceEffect::GrDeviceSpaceEffect(const GrDeviceSpaceEffect& src)
    : INHERITED(kGrDeviceSpaceEffect_ClassID, src.optimizationFlags()) {
  this->cloneAndRegisterAllChildProcessors(src);
}
std::unique_ptr<GrFragmentProcessor> GrDeviceSpaceEffect::clone() const {
  return std::make_unique<GrDeviceSpaceEffect>(*this);
}
#if GR_TEST_UTILS
SkString GrDeviceSpaceEffect::onDumpInfo() const { return SkString(); }
#endif
GR_DEFINE_FRAGMENT_PROCESSOR_TEST(GrDeviceSpaceEffect);
#if GR_TEST_UTILS
std::unique_ptr<GrFragmentProcessor> GrDeviceSpaceEffect::TestCreate(GrProcessorTestData* d) {
  return GrDeviceSpaceEffect::Make(GrProcessorUnitTest::MakeChildFP(d));
}
#endif
