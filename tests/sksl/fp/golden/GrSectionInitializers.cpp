

/**************************************************************************************************
 *** This file was autogenerated from GrSectionInitializers.fp; do not modify.
 **************************************************************************************************/
#include "GrSectionInitializers.h"

#include "src/core/SkUtils.h"
#include "src/gpu/GrTexture.h"
#include "src/gpu/glsl/GrGLSLFragmentProcessor.h"
#include "src/gpu/glsl/GrGLSLFragmentShaderBuilder.h"
#include "src/gpu/glsl/GrGLSLProgramBuilder.h"
#include "src/sksl/SkSLCPP.h"
#include "src/sksl/SkSLUtil.h"
class GrGLSLSectionInitializers : public GrGLSLFragmentProcessor {
 public:
  GrGLSLSectionInitializers() {}
  void emitCode(EmitArgs& args) override {
    GrGLSLFPFragmentBuilder* fragBuilder = args.fFragBuilder;
    const GrSectionInitializers& _outer = args.fFp.cast<GrSectionInitializers>();
    (void)_outer;
    fragBuilder->codeAppendf(
        R"SkSL(%s = half4(1.0);
)SkSL",
        args.fOutputColor);
  }

 private:
  void onSetData(const GrGLSLProgramDataManager& pdman, const GrFragmentProcessor& _proc) override {
  }
};
GrGLSLFragmentProcessor* GrSectionInitializers::onCreateGLSLInstance() const {
  return new GrGLSLSectionInitializers();
}
void GrSectionInitializers::onGetGLSLProcessorKey(
    const GrShaderCaps& caps, GrProcessorKeyBuilder* b) const {}
bool GrSectionInitializers::onIsEqual(const GrFragmentProcessor& other) const {
  const GrSectionInitializers& that = other.cast<GrSectionInitializers>();
  (void)that;
  return true;
}
bool GrSectionInitializers::usesExplicitReturn() const { return false; }
GrSectionInitializers::GrSectionInitializers(const GrSectionInitializers& src)
    : INHERITED(kGrSectionInitializers_ClassID, src.optimizationFlags()) {
  this->cloneAndRegisterAllChildProcessors(src);
}
std::unique_ptr<GrFragmentProcessor> GrSectionInitializers::clone() const {
  return std::make_unique<GrSectionInitializers>(*this);
}
#if GR_TEST_UTILS
SkString GrSectionInitializers::onDumpInfo() const { return SkString(); }
#endif
