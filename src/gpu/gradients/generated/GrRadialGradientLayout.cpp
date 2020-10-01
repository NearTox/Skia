/*
 * Copyright 2018 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/**************************************************************************************************
 *** This file was autogenerated from GrRadialGradientLayout.fp; do not modify.
 **************************************************************************************************/
#include "GrRadialGradientLayout.h"

#include "src/core/SkUtils.h"
#include "src/gpu/GrTexture.h"
#include "src/gpu/glsl/GrGLSLFragmentProcessor.h"
#include "src/gpu/glsl/GrGLSLFragmentShaderBuilder.h"
#include "src/gpu/glsl/GrGLSLProgramBuilder.h"
#include "src/sksl/SkSLCPP.h"
#include "src/sksl/SkSLUtil.h"
class GrGLSLRadialGradientLayout : public GrGLSLFragmentProcessor {
 public:
  GrGLSLRadialGradientLayout() noexcept = default;
  void emitCode(EmitArgs& args) override {
    GrGLSLFPFragmentBuilder* fragBuilder = args.fFragBuilder;
    const GrRadialGradientLayout& _outer = args.fFp.cast<GrRadialGradientLayout>();
    (void)_outer;
    fragBuilder->codeAppendf(
        R"SkSL(half t = half(length(%s));
%s = half4(t, 1.0, 0.0, 0.0);
)SkSL",
        args.fSampleCoord, args.fOutputColor);
  }

 private:
  void onSetData(const GrGLSLProgramDataManager& pdman, const GrFragmentProcessor& _proc) override {
  }
};
GrGLSLFragmentProcessor* GrRadialGradientLayout::onCreateGLSLInstance() const {
  return new GrGLSLRadialGradientLayout();
}
void GrRadialGradientLayout::onGetGLSLProcessorKey(
    const GrShaderCaps& caps, GrProcessorKeyBuilder* b) const {}
bool GrRadialGradientLayout::onIsEqual(const GrFragmentProcessor& other) const noexcept {
  const GrRadialGradientLayout& that = other.cast<GrRadialGradientLayout>();
  (void)that;
  return true;
}
GrRadialGradientLayout::GrRadialGradientLayout(const GrRadialGradientLayout& src)
    : INHERITED(kGrRadialGradientLayout_ClassID, src.optimizationFlags()) {
  this->cloneAndRegisterAllChildProcessors(src);
  this->setUsesSampleCoordsDirectly();
}
std::unique_ptr<GrFragmentProcessor> GrRadialGradientLayout::clone() const {
  return std::make_unique<GrRadialGradientLayout>(*this);
}
#if GR_TEST_UTILS
SkString GrRadialGradientLayout::onDumpInfo() const { return SkString(); }
#endif
GR_DEFINE_FRAGMENT_PROCESSOR_TEST(GrRadialGradientLayout);
#if GR_TEST_UTILS
std::unique_ptr<GrFragmentProcessor> GrRadialGradientLayout::TestCreate(GrProcessorTestData* d) {
  SkScalar scale = GrGradientShader::RandomParams::kGradientScale;
  std::unique_ptr<GrFragmentProcessor> fp;
  GrTest::TestAsFPArgs asFPArgs(d);
  do {
    GrGradientShader::RandomParams params(d->fRandom);
    SkPoint center = {
        d->fRandom->nextRangeScalar(0.0f, scale), d->fRandom->nextRangeScalar(0.0f, scale)};
    SkScalar radius = d->fRandom->nextRangeScalar(0.0f, scale);
    sk_sp<SkShader> shader = params.fUseColors4f
                                 ? SkGradientShader::MakeRadial(
                                       center, radius, params.fColors4f, params.fColorSpace,
                                       params.fStops, params.fColorCount, params.fTileMode)
                                 : SkGradientShader::MakeRadial(
                                       center, radius, params.fColors, params.fStops,
                                       params.fColorCount, params.fTileMode);
    // Degenerate params can create an Empty (non-null) shader, where fp will be nullptr
    fp = shader ? as_SB(shader)->asFragmentProcessor(asFPArgs.args()) : nullptr;
  } while (!fp);
  return fp;
}
#endif

std::unique_ptr<GrFragmentProcessor> GrRadialGradientLayout::Make(
    const SkRadialGradient& grad, const GrFPArgs& args) {
  SkMatrix matrix;
  if (!grad.totalLocalMatrix(args.fPreLocalMatrix)->invert(&matrix)) {
    return nullptr;
  }
  matrix.postConcat(grad.getGradientMatrix());
  return GrMatrixEffect::Make(
      matrix, std::unique_ptr<GrFragmentProcessor>(new GrRadialGradientLayout()));
}
