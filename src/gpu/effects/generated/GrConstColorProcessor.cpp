/*
 * Copyright 2018 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/**************************************************************************************************
 *** This file was autogenerated from GrConstColorProcessor.fp; do not modify.
 **************************************************************************************************/
#include "GrConstColorProcessor.h"

#include "src/gpu/GrTexture.h"
#include "src/gpu/glsl/GrGLSLFragmentProcessor.h"
#include "src/gpu/glsl/GrGLSLFragmentShaderBuilder.h"
#include "src/gpu/glsl/GrGLSLProgramBuilder.h"
#include "src/sksl/SkSLCPP.h"
#include "src/sksl/SkSLUtil.h"
class GrGLSLConstColorProcessor : public GrGLSLFragmentProcessor {
 public:
  GrGLSLConstColorProcessor() {}
  void emitCode(EmitArgs& args) override {
    GrGLSLFPFragmentBuilder* fragBuilder = args.fFragBuilder;
    const GrConstColorProcessor& _outer = args.fFp.cast<GrConstColorProcessor>();
    (void)_outer;
    auto color = _outer.color;
    (void)color;
    auto mode = _outer.mode;
    (void)mode;
    colorVar =
        args.fUniformHandler->addUniform(&_outer, kFragment_GrShaderFlag, kHalf4_GrSLType, "color");
    fragBuilder->codeAppendf(
        "@switch (%d) {\n    case 0:\n        %s = %s;\n        break;\n    case 1:\n      "
        "  %s = %s * %s;\n        break;\n    case 2:\n        %s = %s.w * %s;\n        "
        "break;\n}\n",
        (int)_outer.mode, args.fOutputColor, args.fUniformHandler->getUniformCStr(colorVar),
        args.fOutputColor, args.fInputColor, args.fUniformHandler->getUniformCStr(colorVar),
        args.fOutputColor, args.fInputColor, args.fUniformHandler->getUniformCStr(colorVar));
  }

 private:
  void onSetData(const GrGLSLProgramDataManager& pdman, const GrFragmentProcessor& _proc) override {
    const GrConstColorProcessor& _outer = _proc.cast<GrConstColorProcessor>();
    {
      const SkPMColor4f& colorValue = _outer.color;
      if (colorPrev != colorValue) {
        colorPrev = colorValue;
        pdman.set4fv(colorVar, 1, colorValue.vec());
      }
    }
  }
  SkPMColor4f colorPrev = {SK_FloatNaN, SK_FloatNaN, SK_FloatNaN, SK_FloatNaN};
  UniformHandle colorVar;
};
GrGLSLFragmentProcessor* GrConstColorProcessor::onCreateGLSLInstance() const {
  return new GrGLSLConstColorProcessor();
}
void GrConstColorProcessor::onGetGLSLProcessorKey(
    const GrShaderCaps& caps, GrProcessorKeyBuilder* b) const {
  b->add32((int32_t)mode);
}
bool GrConstColorProcessor::onIsEqual(const GrFragmentProcessor& other) const noexcept {
  const GrConstColorProcessor& that = other.cast<GrConstColorProcessor>();
  (void)that;
  if (color != that.color) return false;
  if (mode != that.mode) return false;
  return true;
}
GrConstColorProcessor::GrConstColorProcessor(const GrConstColorProcessor& src) noexcept
    : INHERITED(kGrConstColorProcessor_ClassID, src.optimizationFlags()),
      color(src.color),
      mode(src.mode) {}
std::unique_ptr<GrFragmentProcessor> GrConstColorProcessor::clone() const {
  return std::unique_ptr<GrFragmentProcessor>(new GrConstColorProcessor(*this));
}
GR_DEFINE_FRAGMENT_PROCESSOR_TEST(GrConstColorProcessor);
#if GR_TEST_UTILS
std::unique_ptr<GrFragmentProcessor> GrConstColorProcessor::TestCreate(GrProcessorTestData* d) {
  SkPMColor4f color;
  int colorPicker = d->fRandom->nextULessThan(3);
  switch (colorPicker) {
    case 0: {
      uint32_t a = d->fRandom->nextULessThan(0x100);
      uint32_t r = d->fRandom->nextULessThan(a + 1);
      uint32_t g = d->fRandom->nextULessThan(a + 1);
      uint32_t b = d->fRandom->nextULessThan(a + 1);
      color = SkPMColor4f::FromBytes_RGBA(GrColorPackRGBA(r, g, b, a));
      break;
    }
    case 1: color = SK_PMColor4fTRANSPARENT; break;
    case 2:
      uint32_t c = d->fRandom->nextULessThan(0x100);
      color = SkPMColor4f::FromBytes_RGBA(c | (c << 8) | (c << 16) | (c << 24));
      break;
  }
  InputMode mode = static_cast<InputMode>(d->fRandom->nextULessThan(kInputModeCnt));
  return GrConstColorProcessor::Make(color, mode);
}
#endif
