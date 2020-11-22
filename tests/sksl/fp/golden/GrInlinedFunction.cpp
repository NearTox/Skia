

/**************************************************************************************************
 *** This file was autogenerated from GrInlinedFunction.fp; do not modify.
 **************************************************************************************************/
#include "GrInlinedFunction.h"

#include "src/core/SkUtils.h"
#include "src/gpu/GrTexture.h"
#include "src/gpu/glsl/GrGLSLFragmentProcessor.h"
#include "src/gpu/glsl/GrGLSLFragmentShaderBuilder.h"
#include "src/gpu/glsl/GrGLSLProgramBuilder.h"
#include "src/sksl/SkSLCPP.h"
#include "src/sksl/SkSLUtil.h"
class GrGLSLInlinedFunction : public GrGLSLFragmentProcessor {
 public:
  GrGLSLInlinedFunction() {}
  void emitCode(EmitArgs& args) override {
    GrGLSLFPFragmentBuilder* fragBuilder = args.fFragBuilder;
    const GrInlinedFunction& _outer = args.fFp.cast<GrInlinedFunction>();
    (void)_outer;
    colorVar =
        args.fUniformHandler->addUniform(&_outer, kFragment_GrShaderFlag, kHalf4_GrSLType, "color");
    fragBuilder->codeAppendf(
        R"SkSL(half4 _0_flip;
{
    _0_flip = %s.wzyx;
}

%s = _0_flip;

)SkSL",
        args.fUniformHandler->getUniformCStr(colorVar), args.fOutputColor);
  }

 private:
  void onSetData(const GrGLSLProgramDataManager& pdman, const GrFragmentProcessor& _proc) override {
  }
  UniformHandle colorVar;
};
GrGLSLFragmentProcessor* GrInlinedFunction::onCreateGLSLInstance() const {
  return new GrGLSLInlinedFunction();
}
void GrInlinedFunction::onGetGLSLProcessorKey(
    const GrShaderCaps& caps, GrProcessorKeyBuilder* b) const {}
bool GrInlinedFunction::onIsEqual(const GrFragmentProcessor& other) const {
  const GrInlinedFunction& that = other.cast<GrInlinedFunction>();
  (void)that;
  return true;
}
bool GrInlinedFunction::usesExplicitReturn() const { return false; }
GrInlinedFunction::GrInlinedFunction(const GrInlinedFunction& src)
    : INHERITED(kGrInlinedFunction_ClassID, src.optimizationFlags()) {
  this->cloneAndRegisterAllChildProcessors(src);
}
std::unique_ptr<GrFragmentProcessor> GrInlinedFunction::clone() const {
  return std::make_unique<GrInlinedFunction>(*this);
}
#if GR_TEST_UTILS
SkString GrInlinedFunction::onDumpInfo() const { return SkString(); }
#endif