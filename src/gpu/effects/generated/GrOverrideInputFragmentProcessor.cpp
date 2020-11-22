/*
 * Copyright 2019 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/**************************************************************************************************
 *** This file was autogenerated from GrOverrideInputFragmentProcessor.fp; do not modify.
 **************************************************************************************************/
#include "GrOverrideInputFragmentProcessor.h"

#include "src/core/SkUtils.h"
#include "src/gpu/GrTexture.h"
#include "src/gpu/glsl/GrGLSLFragmentProcessor.h"
#include "src/gpu/glsl/GrGLSLFragmentShaderBuilder.h"
#include "src/gpu/glsl/GrGLSLProgramBuilder.h"
#include "src/sksl/SkSLCPP.h"
#include "src/sksl/SkSLUtil.h"
class GrGLSLOverrideInputFragmentProcessor : public GrGLSLFragmentProcessor {
 public:
  GrGLSLOverrideInputFragmentProcessor() {}
  void emitCode(EmitArgs& args) override {
    GrGLSLFPFragmentBuilder* fragBuilder = args.fFragBuilder;
    const GrOverrideInputFragmentProcessor& _outer =
        args.fFp.cast<GrOverrideInputFragmentProcessor>();
    (void)_outer;
    auto useUniform = _outer.useUniform;
    (void)useUniform;
    auto uniformColor = _outer.uniformColor;
    (void)uniformColor;
    auto literalColor = _outer.literalColor;
    (void)literalColor;
    if (useUniform) {
      uniformColorVar = args.fUniformHandler->addUniform(
          &_outer, kFragment_GrShaderFlag, kHalf4_GrSLType, "uniformColor");
    }
    SkString _input1853 = SkStringPrintf(
        "%s ? %s : half4(%f, %f, %f, %f)", (_outer.useUniform ? "true" : "false"),
        uniformColorVar.isValid() ? args.fUniformHandler->getUniformCStr(uniformColorVar)
                                  : "half4(0)",
        _outer.literalColor.fR, _outer.literalColor.fG, _outer.literalColor.fB,
        _outer.literalColor.fA);
    SkString _sample1853 = this->invokeChild(0, _input1853.c_str(), args);
    fragBuilder->codeAppendf(
        R"SkSL(return %s;
)SkSL",
        _sample1853.c_str());
  }

 private:
  void onSetData(const GrGLSLProgramDataManager& pdman, const GrFragmentProcessor& _proc) override {
    const GrOverrideInputFragmentProcessor& _outer = _proc.cast<GrOverrideInputFragmentProcessor>();
    {
      if (uniformColorVar.isValid()) {
        pdman.set4fv(uniformColorVar, 1, (_outer.uniformColor).vec());
      }
    }
  }
  UniformHandle uniformColorVar;
};
GrGLSLFragmentProcessor* GrOverrideInputFragmentProcessor::onCreateGLSLInstance() const {
  return new GrGLSLOverrideInputFragmentProcessor();
}
void GrOverrideInputFragmentProcessor::onGetGLSLProcessorKey(
    const GrShaderCaps& caps, GrProcessorKeyBuilder* b) const {
  b->add32((uint32_t)useUniform);
  if (!useUniform) {
    uint16_t red = SkFloatToHalf(literalColor.fR);
    uint16_t green = SkFloatToHalf(literalColor.fG);
    uint16_t blue = SkFloatToHalf(literalColor.fB);
    uint16_t alpha = SkFloatToHalf(literalColor.fA);
    b->add32(((uint32_t)red << 16) | green);
    b->add32(((uint32_t)blue << 16) | alpha);
  }
}
bool GrOverrideInputFragmentProcessor::onIsEqual(const GrFragmentProcessor& other) const {
  const GrOverrideInputFragmentProcessor& that = other.cast<GrOverrideInputFragmentProcessor>();
  (void)that;
  if (useUniform != that.useUniform) return false;
  if (uniformColor != that.uniformColor) return false;
  if (literalColor != that.literalColor) return false;
  return true;
}
bool GrOverrideInputFragmentProcessor::usesExplicitReturn() const { return true; }
GrOverrideInputFragmentProcessor::GrOverrideInputFragmentProcessor(
    const GrOverrideInputFragmentProcessor& src)
    : INHERITED(kGrOverrideInputFragmentProcessor_ClassID, src.optimizationFlags()),
      useUniform(src.useUniform),
      uniformColor(src.uniformColor),
      literalColor(src.literalColor) {
  this->cloneAndRegisterAllChildProcessors(src);
}
std::unique_ptr<GrFragmentProcessor> GrOverrideInputFragmentProcessor::clone() const {
  return std::make_unique<GrOverrideInputFragmentProcessor>(*this);
}
#if GR_TEST_UTILS
SkString GrOverrideInputFragmentProcessor::onDumpInfo() const {
  return SkStringPrintf(
      "(useUniform=%s, uniformColor=half4(%f, %f, %f, %f), literalColor=half4(%f, %f, %f, "
      "%f))",
      (useUniform ? "true" : "false"), uniformColor.fR, uniformColor.fG, uniformColor.fB,
      uniformColor.fA, literalColor.fR, literalColor.fG, literalColor.fB, literalColor.fA);
}
#endif
