

/**************************************************************************************************
 *** This file was autogenerated from GrSectionConstructor.fp; do not modify.
 **************************************************************************************************/
#include "GrSectionConstructor.h"

#include "src/core/SkUtils.h"
#include "src/gpu/GrTexture.h"
#include "src/gpu/glsl/GrGLSLFragmentProcessor.h"
#include "src/gpu/glsl/GrGLSLFragmentShaderBuilder.h"
#include "src/gpu/glsl/GrGLSLProgramBuilder.h"
#include "src/sksl/SkSLCPP.h"
#include "src/sksl/SkSLUtil.h"
class GrGLSLSectionConstructor : public GrGLSLFragmentProcessor {
 public:
  GrGLSLSectionConstructor() {}
  void emitCode(EmitArgs& args) override {
    GrGLSLFPFragmentBuilder* fragBuilder = args.fFragBuilder;
    const GrSectionConstructor& _outer = args.fFp.cast<GrSectionConstructor>();
    (void)_outer;
    fragBuilder->codeAppendf(
        R"SkSL(return half4(1.0);
)SkSL");
  }

 private:
  void onSetData(const GrGLSLProgramDataManager& pdman, const GrFragmentProcessor& _proc) override {
  }
};
GrGLSLFragmentProcessor* GrSectionConstructor::onCreateGLSLInstance() const {
  return new GrGLSLSectionConstructor();
}
void GrSectionConstructor::onGetGLSLProcessorKey(
    const GrShaderCaps& caps, GrProcessorKeyBuilder* b) const {}
bool GrSectionConstructor::onIsEqual(const GrFragmentProcessor& other) const {
  const GrSectionConstructor& that = other.cast<GrSectionConstructor>();
  (void)that;
  return true;
}
GrSectionConstructor::GrSectionConstructor(const GrSectionConstructor& src)
    : INHERITED(kGrSectionConstructor_ClassID, src.optimizationFlags()) {
  this->cloneAndRegisterAllChildProcessors(src);
}
std::unique_ptr<GrFragmentProcessor> GrSectionConstructor::clone() const {
  return std::make_unique<GrSectionConstructor>(*this);
}
#if GR_TEST_UTILS
SkString GrSectionConstructor::onDumpInfo() const { return SkString(); }
#endif
