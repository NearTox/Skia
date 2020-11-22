

/**************************************************************************************************
 *** This file was autogenerated from GrChildProcessors.fp; do not modify.
 **************************************************************************************************/
#include "GrChildProcessors.h"

#include "src/core/SkUtils.h"
#include "src/gpu/GrTexture.h"
#include "src/gpu/glsl/GrGLSLFragmentProcessor.h"
#include "src/gpu/glsl/GrGLSLFragmentShaderBuilder.h"
#include "src/gpu/glsl/GrGLSLProgramBuilder.h"
#include "src/sksl/SkSLCPP.h"
#include "src/sksl/SkSLUtil.h"
class GrGLSLChildProcessors : public GrGLSLFragmentProcessor {
 public:
  GrGLSLChildProcessors() {}
  void emitCode(EmitArgs& args) override {
    GrGLSLFPFragmentBuilder* fragBuilder = args.fFragBuilder;
    const GrChildProcessors& _outer = args.fFp.cast<GrChildProcessors>();
    (void)_outer;
    SkString _sample97 = this->invokeChild(0, args);
    SkString _sample114 = this->invokeChild(1, args);
    fragBuilder->codeAppendf(
        R"SkSL(%s = %s * %s;
)SkSL",
        args.fOutputColor, _sample97.c_str(), _sample114.c_str());
  }

 private:
  void onSetData(const GrGLSLProgramDataManager& pdman, const GrFragmentProcessor& _proc) override {
  }
};
GrGLSLFragmentProcessor* GrChildProcessors::onCreateGLSLInstance() const {
  return new GrGLSLChildProcessors();
}
void GrChildProcessors::onGetGLSLProcessorKey(
    const GrShaderCaps& caps, GrProcessorKeyBuilder* b) const {}
bool GrChildProcessors::onIsEqual(const GrFragmentProcessor& other) const {
  const GrChildProcessors& that = other.cast<GrChildProcessors>();
  (void)that;
  return true;
}
bool GrChildProcessors::usesExplicitReturn() const { return false; }
GrChildProcessors::GrChildProcessors(const GrChildProcessors& src)
    : INHERITED(kGrChildProcessors_ClassID, src.optimizationFlags()) {
  this->cloneAndRegisterAllChildProcessors(src);
}
std::unique_ptr<GrFragmentProcessor> GrChildProcessors::clone() const {
  return std::make_unique<GrChildProcessors>(*this);
}
#if GR_TEST_UTILS
SkString GrChildProcessors::onDumpInfo() const { return SkString(); }
#endif