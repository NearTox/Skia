

/**************************************************************************************************
 *** This file was autogenerated from GrChildProcessorAndGlobal.fp; do not modify.
 **************************************************************************************************/
#include "GrChildProcessorAndGlobal.h"

#include "src/core/SkUtils.h"
#include "src/gpu/GrTexture.h"
#include "src/gpu/glsl/GrGLSLFragmentProcessor.h"
#include "src/gpu/glsl/GrGLSLFragmentShaderBuilder.h"
#include "src/gpu/glsl/GrGLSLProgramBuilder.h"
#include "src/sksl/SkSLCPP.h"
#include "src/sksl/SkSLUtil.h"
class GrGLSLChildProcessorAndGlobal : public GrGLSLFragmentProcessor {
 public:
  GrGLSLChildProcessorAndGlobal() {}
  void emitCode(EmitArgs& args) override {
    GrGLSLFPFragmentBuilder* fragBuilder = args.fFragBuilder;
    const GrChildProcessorAndGlobal& _outer = args.fFp.cast<GrChildProcessorAndGlobal>();
    (void)_outer;
    hasCap = sk_Caps.externalTextureSupport;
    fragBuilder->codeAppendf(
        R"SkSL(bool hasCap = %s;
if (hasCap) {)SkSL",
        (hasCap ? "true" : "false"));
    SkString _sample0 = this->invokeChild(0, args);
    fragBuilder->codeAppendf(
        R"SkSL(
    %s = %s;
} else {
    %s = half4(1.0);
}
)SkSL",
        args.fOutputColor, _sample0.c_str(), args.fOutputColor);
  }

 private:
  void onSetData(const GrGLSLProgramDataManager& pdman, const GrFragmentProcessor& _proc) override {
  }
  bool hasCap = false;
};
GrGLSLFragmentProcessor* GrChildProcessorAndGlobal::onCreateGLSLInstance() const {
  return new GrGLSLChildProcessorAndGlobal();
}
void GrChildProcessorAndGlobal::onGetGLSLProcessorKey(
    const GrShaderCaps& caps, GrProcessorKeyBuilder* b) const {}
bool GrChildProcessorAndGlobal::onIsEqual(const GrFragmentProcessor& other) const {
  const GrChildProcessorAndGlobal& that = other.cast<GrChildProcessorAndGlobal>();
  (void)that;
  return true;
}
bool GrChildProcessorAndGlobal::usesExplicitReturn() const { return false; }
GrChildProcessorAndGlobal::GrChildProcessorAndGlobal(const GrChildProcessorAndGlobal& src)
    : INHERITED(kGrChildProcessorAndGlobal_ClassID, src.optimizationFlags()) {
  this->cloneAndRegisterAllChildProcessors(src);
}
std::unique_ptr<GrFragmentProcessor> GrChildProcessorAndGlobal::clone() const {
  return std::make_unique<GrChildProcessorAndGlobal>(*this);
}
#if GR_TEST_UTILS
SkString GrChildProcessorAndGlobal::onDumpInfo() const { return SkString(); }
#endif
