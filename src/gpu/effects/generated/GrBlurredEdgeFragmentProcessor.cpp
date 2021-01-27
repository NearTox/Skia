/*
 * Copyright 2017 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/**************************************************************************************************
 *** This file was autogenerated from GrBlurredEdgeFragmentProcessor.fp; do not modify.
 **************************************************************************************************/
#include "GrBlurredEdgeFragmentProcessor.h"

#include "src/core/SkUtils.h"
#include "src/gpu/GrTexture.h"
#include "src/gpu/glsl/GrGLSLFragmentProcessor.h"
#include "src/gpu/glsl/GrGLSLFragmentShaderBuilder.h"
#include "src/gpu/glsl/GrGLSLProgramBuilder.h"
#include "src/sksl/SkSLCPP.h"
#include "src/sksl/SkSLUtil.h"
class GrGLSLBlurredEdgeFragmentProcessor : public GrGLSLFragmentProcessor {
 public:
  GrGLSLBlurredEdgeFragmentProcessor() {}
  void emitCode(EmitArgs& args) override {
    GrGLSLFPFragmentBuilder* fragBuilder = args.fFragBuilder;
    const GrBlurredEdgeFragmentProcessor& _outer = args.fFp.cast<GrBlurredEdgeFragmentProcessor>();
    (void)_outer;
    auto mode = _outer.mode;
    (void)mode;
    SkString _sample0 = this->invokeChild(0, args);
    fragBuilder->codeAppendf(
        R"SkSL(half inputAlpha = %s.w;
half factor = 1.0 - inputAlpha;
@switch (%d) {
    case 0:
        factor = exp((-factor * factor) * 4.0) - 0.017999999225139618;
        break;
    case 1:
        factor = smoothstep(1.0, 0.0, factor);
        break;
}
%s = half4(factor);
)SkSL",
        _sample0.c_str(), (int)_outer.mode, args.fOutputColor);
  }

 private:
  void onSetData(const GrGLSLProgramDataManager& pdman, const GrFragmentProcessor& _proc) override {
  }
};
GrGLSLFragmentProcessor* GrBlurredEdgeFragmentProcessor::onCreateGLSLInstance() const {
  return new GrGLSLBlurredEdgeFragmentProcessor();
}
void GrBlurredEdgeFragmentProcessor::onGetGLSLProcessorKey(
    const GrShaderCaps& caps, GrProcessorKeyBuilder* b) const {
  b->add32((uint32_t)mode);
}
bool GrBlurredEdgeFragmentProcessor::onIsEqual(const GrFragmentProcessor& other) const {
  const GrBlurredEdgeFragmentProcessor& that = other.cast<GrBlurredEdgeFragmentProcessor>();
  (void)that;
  if (mode != that.mode) return false;
  return true;
}
bool GrBlurredEdgeFragmentProcessor::usesExplicitReturn() const { return false; }
GrBlurredEdgeFragmentProcessor::GrBlurredEdgeFragmentProcessor(
    const GrBlurredEdgeFragmentProcessor& src)
    : INHERITED(kGrBlurredEdgeFragmentProcessor_ClassID, src.optimizationFlags()), mode(src.mode) {
  this->cloneAndRegisterAllChildProcessors(src);
}
std::unique_ptr<GrFragmentProcessor> GrBlurredEdgeFragmentProcessor::clone() const {
  return std::make_unique<GrBlurredEdgeFragmentProcessor>(*this);
}
#if GR_TEST_UTILS
SkString GrBlurredEdgeFragmentProcessor::onDumpInfo() const {
  return SkStringPrintf("(mode=%d)", (int)mode);
}
#endif
