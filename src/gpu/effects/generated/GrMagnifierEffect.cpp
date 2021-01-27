/*
 * Copyright 2018 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/**************************************************************************************************
 *** This file was autogenerated from GrMagnifierEffect.fp; do not modify.
 **************************************************************************************************/
#include "GrMagnifierEffect.h"

#include "src/core/SkUtils.h"
#include "src/gpu/GrTexture.h"
#include "src/gpu/glsl/GrGLSLFragmentProcessor.h"
#include "src/gpu/glsl/GrGLSLFragmentShaderBuilder.h"
#include "src/gpu/glsl/GrGLSLProgramBuilder.h"
#include "src/sksl/SkSLCPP.h"
#include "src/sksl/SkSLUtil.h"
class GrGLSLMagnifierEffect : public GrGLSLFragmentProcessor {
 public:
  GrGLSLMagnifierEffect() {}
  void emitCode(EmitArgs& args) override {
    GrGLSLFPFragmentBuilder* fragBuilder = args.fFragBuilder;
    const GrMagnifierEffect& _outer = args.fFp.cast<GrMagnifierEffect>();
    (void)_outer;
    auto bounds = _outer.bounds;
    (void)bounds;
    auto srcRect = _outer.srcRect;
    (void)srcRect;
    auto xInvZoom = _outer.xInvZoom;
    (void)xInvZoom;
    auto yInvZoom = _outer.yInvZoom;
    (void)yInvZoom;
    auto xInvInset = _outer.xInvInset;
    (void)xInvInset;
    auto yInvInset = _outer.yInvInset;
    (void)yInvInset;
    boundsUniformVar = args.fUniformHandler->addUniform(
        &_outer, kFragment_GrShaderFlag, kFloat4_GrSLType, "boundsUniform");
    xInvZoomVar = args.fUniformHandler->addUniform(
        &_outer, kFragment_GrShaderFlag, kFloat_GrSLType, "xInvZoom");
    yInvZoomVar = args.fUniformHandler->addUniform(
        &_outer, kFragment_GrShaderFlag, kFloat_GrSLType, "yInvZoom");
    xInvInsetVar = args.fUniformHandler->addUniform(
        &_outer, kFragment_GrShaderFlag, kFloat_GrSLType, "xInvInset");
    yInvInsetVar = args.fUniformHandler->addUniform(
        &_outer, kFragment_GrShaderFlag, kFloat_GrSLType, "yInvInset");
    offsetVar = args.fUniformHandler->addUniform(
        &_outer, kFragment_GrShaderFlag, kHalf2_GrSLType, "offset");
    fragBuilder->codeAppendf(
        R"SkSL(float2 zoom_coord = float2(%s) + %s * float2(%s, %s);
float2 delta = (%s - %s.xy) * %s.zw;
delta = min(delta, float2(half2(1.0, 1.0)) - delta);
delta *= float2(%s, %s);
float weight = 0.0;
if (delta.x < 2.0 && delta.y < 2.0) {
    delta = float2(half2(2.0, 2.0)) - delta;
    float dist = length(delta);
    dist = max(2.0 - dist, 0.0);
    weight = min(dist * dist, 1.0);
} else {
    float2 delta_squared = delta * delta;
    weight = min(min(delta_squared.x, delta_squared.y), 1.0);
})SkSL",
        args.fUniformHandler->getUniformCStr(offsetVar), args.fSampleCoord,
        args.fUniformHandler->getUniformCStr(xInvZoomVar),
        args.fUniformHandler->getUniformCStr(yInvZoomVar), args.fSampleCoord,
        args.fUniformHandler->getUniformCStr(boundsUniformVar),
        args.fUniformHandler->getUniformCStr(boundsUniformVar),
        args.fUniformHandler->getUniformCStr(xInvInsetVar),
        args.fUniformHandler->getUniformCStr(yInvInsetVar));
    SkString _coords0 = SkStringPrintf("mix(%s, zoom_coord, weight)", args.fSampleCoord);
    SkString _sample0 = this->invokeChild(0, args, _coords0.c_str());
    fragBuilder->codeAppendf(
        R"SkSL(
return %s;
)SkSL",
        _sample0.c_str());
  }

 private:
  void onSetData(const GrGLSLProgramDataManager& pdman, const GrFragmentProcessor& _proc) override {
    const GrMagnifierEffect& _outer = _proc.cast<GrMagnifierEffect>();
    {
      pdman.set1f(xInvZoomVar, (_outer.xInvZoom));
      pdman.set1f(yInvZoomVar, (_outer.yInvZoom));
      pdman.set1f(xInvInsetVar, (_outer.xInvInset));
      pdman.set1f(yInvInsetVar, (_outer.yInvInset));
    }
    auto bounds = _outer.bounds;
    (void)bounds;
    UniformHandle& boundsUniform = boundsUniformVar;
    (void)boundsUniform;
    auto srcRect = _outer.srcRect;
    (void)srcRect;
    UniformHandle& xInvZoom = xInvZoomVar;
    (void)xInvZoom;
    UniformHandle& yInvZoom = yInvZoomVar;
    (void)yInvZoom;
    UniformHandle& xInvInset = xInvInsetVar;
    (void)xInvInset;
    UniformHandle& yInvInset = yInvInsetVar;
    (void)yInvInset;
    UniformHandle& offset = offsetVar;
    (void)offset;

    pdman.set2f(offset, srcRect.x(), srcRect.y());
    pdman.set4f(boundsUniform, bounds.x(), bounds.y(), 1.f / bounds.width(), 1.f / bounds.height());
  }
  UniformHandle boundsUniformVar;
  UniformHandle offsetVar;
  UniformHandle xInvZoomVar;
  UniformHandle yInvZoomVar;
  UniformHandle xInvInsetVar;
  UniformHandle yInvInsetVar;
};
GrGLSLFragmentProcessor* GrMagnifierEffect::onCreateGLSLInstance() const {
  return new GrGLSLMagnifierEffect();
}
void GrMagnifierEffect::onGetGLSLProcessorKey(
    const GrShaderCaps& caps, GrProcessorKeyBuilder* b) const {}
bool GrMagnifierEffect::onIsEqual(const GrFragmentProcessor& other) const {
  const GrMagnifierEffect& that = other.cast<GrMagnifierEffect>();
  (void)that;
  if (bounds != that.bounds) return false;
  if (srcRect != that.srcRect) return false;
  if (xInvZoom != that.xInvZoom) return false;
  if (yInvZoom != that.yInvZoom) return false;
  if (xInvInset != that.xInvInset) return false;
  if (yInvInset != that.yInvInset) return false;
  return true;
}
bool GrMagnifierEffect::usesExplicitReturn() const { return true; }
GrMagnifierEffect::GrMagnifierEffect(const GrMagnifierEffect& src)
    : INHERITED(kGrMagnifierEffect_ClassID, src.optimizationFlags()),
      bounds(src.bounds),
      srcRect(src.srcRect),
      xInvZoom(src.xInvZoom),
      yInvZoom(src.yInvZoom),
      xInvInset(src.xInvInset),
      yInvInset(src.yInvInset) {
  this->cloneAndRegisterAllChildProcessors(src);
  this->setUsesSampleCoordsDirectly();
}
std::unique_ptr<GrFragmentProcessor> GrMagnifierEffect::clone() const {
  return std::make_unique<GrMagnifierEffect>(*this);
}
#if GR_TEST_UTILS
SkString GrMagnifierEffect::onDumpInfo() const {
  return SkStringPrintf(
      "(bounds=int4(%d, %d, %d, %d), srcRect=float4(%f, %f, %f, %f), xInvZoom=%f, "
      "yInvZoom=%f, xInvInset=%f, yInvInset=%f)",
      bounds.left(), bounds.top(), bounds.right(), bounds.bottom(), srcRect.left(), srcRect.top(),
      srcRect.right(), srcRect.bottom(), xInvZoom, yInvZoom, xInvInset, yInvInset);
}
#endif
GR_DEFINE_FRAGMENT_PROCESSOR_TEST(GrMagnifierEffect);
#if GR_TEST_UTILS
std::unique_ptr<GrFragmentProcessor> GrMagnifierEffect::TestCreate(GrProcessorTestData* d) {
  const int kMaxWidth = 200;
  const int kMaxHeight = 200;
  const SkScalar kMaxInset = 20.0f;
  uint32_t width = d->fRandom->nextULessThan(kMaxWidth);
  uint32_t height = d->fRandom->nextULessThan(kMaxHeight);
  SkScalar inset = d->fRandom->nextRangeScalar(1.0f, kMaxInset);

  SkIRect bounds = SkIRect::MakeWH(SkIntToScalar(kMaxWidth), SkIntToScalar(kMaxHeight));
  SkRect srcRect = SkRect::MakeWH(SkIntToScalar(width), SkIntToScalar(height));

  auto src = GrProcessorUnitTest::MakeChildFP(d);
  auto effect = GrMagnifierEffect::Make(
      std::move(src), bounds, srcRect, srcRect.width() / bounds.width(),
      srcRect.height() / bounds.height(), bounds.width() / inset, bounds.height() / inset);
  SkASSERT(effect);
  return effect;
}
#endif
