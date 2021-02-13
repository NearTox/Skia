

/**************************************************************************************************
 *** This file was autogenerated from GrSectionMake.fp; do not modify.
 **************************************************************************************************/
#include "GrSectionMake.h"

#include "src/core/SkUtils.h"
#include "src/gpu/GrTexture.h"
#include "src/gpu/glsl/GrGLSLFragmentProcessor.h"
#include "src/gpu/glsl/GrGLSLFragmentShaderBuilder.h"
#include "src/gpu/glsl/GrGLSLProgramBuilder.h"
#include "src/sksl/SkSLCPP.h"
#include "src/sksl/SkSLUtil.h"
class GrGLSLSectionMake : public GrGLSLFragmentProcessor {
 public:
  GrGLSLSectionMake() {}
  void emitCode(EmitArgs& args) override {
    GrGLSLFPFragmentBuilder* fragBuilder = args.fFragBuilder;
    const GrSectionMake& _outer = args.fFp.cast<GrSectionMake>();
    (void)_outer;
    fragBuilder->codeAppendf(
        R"SkSL(return half4(1.0);
)SkSL");
  }

 private:
  void onSetData(const GrGLSLProgramDataManager& pdman, const GrFragmentProcessor& _proc) override {
  }
};
GrGLSLFragmentProcessor* GrSectionMake::onCreateGLSLInstance() const {
  return new GrGLSLSectionMake();
}
void GrSectionMake::onGetGLSLProcessorKey(
    const GrShaderCaps& caps, GrProcessorKeyBuilder* b) const {}
bool GrSectionMake::onIsEqual(const GrFragmentProcessor& other) const {
  const GrSectionMake& that = other.cast<GrSectionMake>();
  (void)that;
  return true;
}
GrSectionMake::GrSectionMake(const GrSectionMake& src)
    : INHERITED(kGrSectionMake_ClassID, src.optimizationFlags()) {
  this->cloneAndRegisterAllChildProcessors(src);
}
std::unique_ptr<GrFragmentProcessor> GrSectionMake::clone() const {
  return std::make_unique<GrSectionMake>(*this);
}
#if GR_TEST_UTILS
SkString GrSectionMake::onDumpInfo() const { return SkString(); }
#endif
