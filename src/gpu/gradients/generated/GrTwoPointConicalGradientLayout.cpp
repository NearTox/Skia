/*
 * Copyright 2018 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/**************************************************************************************************
 *** This file was autogenerated from GrTwoPointConicalGradientLayout.fp; do not modify.
 **************************************************************************************************/
#include "GrTwoPointConicalGradientLayout.h"

#include "src/core/SkUtils.h"
#include "src/gpu/GrTexture.h"
#include "src/gpu/glsl/GrGLSLFragmentProcessor.h"
#include "src/gpu/glsl/GrGLSLFragmentShaderBuilder.h"
#include "src/gpu/glsl/GrGLSLProgramBuilder.h"
#include "src/sksl/SkSLCPP.h"
#include "src/sksl/SkSLUtil.h"
class GrGLSLTwoPointConicalGradientLayout : public GrGLSLFragmentProcessor {
 public:
  GrGLSLTwoPointConicalGradientLayout() {}
  void emitCode(EmitArgs& args) override {
    GrGLSLFPFragmentBuilder* fragBuilder = args.fFragBuilder;
    const GrTwoPointConicalGradientLayout& _outer =
        args.fFp.cast<GrTwoPointConicalGradientLayout>();
    (void)_outer;
    auto type = _outer.type;
    (void)type;
    auto isRadiusIncreasing = _outer.isRadiusIncreasing;
    (void)isRadiusIncreasing;
    auto isFocalOnCircle = _outer.isFocalOnCircle;
    (void)isFocalOnCircle;
    auto isWellBehaved = _outer.isWellBehaved;
    (void)isWellBehaved;
    auto isSwapped = _outer.isSwapped;
    (void)isSwapped;
    auto isNativelyFocal = _outer.isNativelyFocal;
    (void)isNativelyFocal;
    auto focalParams = _outer.focalParams;
    (void)focalParams;
    focalParamsVar = args.fUniformHandler->addUniform(
        &_outer, kFragment_GrShaderFlag, kHalf2_GrSLType, "focalParams");
    fragBuilder->codeAppendf(
        R"SkSL(float t = -1.0;
half v = 1.0;
@switch (%d) {
    case 1:
        {
            half r0_2 = %s.y;
            t = float(r0_2) - %s.y * %s.y;
            if (t >= 0.0) {
                t = %s.x + sqrt(t);
            } else {
                v = -1.0;
            }
        }
        break;
    case 0:
        {
            half r0 = %s.x;
            @if (%s) {
                t = length(%s) - float(r0);
            } else {
                t = -length(%s) - float(r0);
            }
        }
        break;
    case 2:
        {
            half invR1 = %s.x;
            half fx = %s.y;
            float x_t = -1.0;
            @if (%s) {
                x_t = dot(%s, %s) / %s.x;
            } else if (%s) {
                x_t = length(%s) - %s.x * float(invR1);
            } else {
                float temp = %s.x * %s.x - %s.y * %s.y;
                if (temp >= 0.0) {
                    @if (%s || !%s) {
                        x_t = -sqrt(temp) - %s.x * float(invR1);
                    } else {
                        x_t = sqrt(temp) - %s.x * float(invR1);
                    }
                }
            }
            @if (!%s) {
                if (x_t <= 0.0) {
                    v = -1.0;
                }
            }
            @if (%s) {
                @if (%s) {
                    t = x_t;
                } else {
                    t = x_t + float(fx);
                }
            } else {
                @if (%s) {
                    t = -x_t;
                } else {
                    t = -x_t + float(fx);
                }
            }
            @if (%s) {
                t = 1.0 - t;
            }
        }
        break;
}
%s = half4(half(t), v, 0.0, 0.0);
)SkSL",
        (int)_outer.type, args.fUniformHandler->getUniformCStr(focalParamsVar), args.fSampleCoord,
        args.fSampleCoord, args.fSampleCoord, args.fUniformHandler->getUniformCStr(focalParamsVar),
        (_outer.isRadiusIncreasing ? "true" : "false"), args.fSampleCoord, args.fSampleCoord,
        args.fUniformHandler->getUniformCStr(focalParamsVar),
        args.fUniformHandler->getUniformCStr(focalParamsVar),
        (_outer.isFocalOnCircle ? "true" : "false"), args.fSampleCoord, args.fSampleCoord,
        args.fSampleCoord, (_outer.isWellBehaved ? "true" : "false"), args.fSampleCoord,
        args.fSampleCoord, args.fSampleCoord, args.fSampleCoord, args.fSampleCoord,
        args.fSampleCoord, (_outer.isSwapped ? "true" : "false"),
        (_outer.isRadiusIncreasing ? "true" : "false"), args.fSampleCoord, args.fSampleCoord,
        (_outer.isWellBehaved ? "true" : "false"), (_outer.isRadiusIncreasing ? "true" : "false"),
        (_outer.isNativelyFocal ? "true" : "false"), (_outer.isNativelyFocal ? "true" : "false"),
        (_outer.isSwapped ? "true" : "false"), args.fOutputColor);
  }

 private:
  void onSetData(const GrGLSLProgramDataManager& pdman, const GrFragmentProcessor& _proc) override {
    const GrTwoPointConicalGradientLayout& _outer = _proc.cast<GrTwoPointConicalGradientLayout>();
    {
      const SkPoint& focalParamsValue = _outer.focalParams;
      if (focalParamsPrev != focalParamsValue) {
        focalParamsPrev = focalParamsValue;
        pdman.set2f(focalParamsVar, focalParamsValue.fX, focalParamsValue.fY);
      }
    }
  }
  SkPoint focalParamsPrev = SkPoint::Make(SK_FloatNaN, SK_FloatNaN);
  UniformHandle focalParamsVar;
};
GrGLSLFragmentProcessor* GrTwoPointConicalGradientLayout::onCreateGLSLInstance() const {
  return new GrGLSLTwoPointConicalGradientLayout();
}
void GrTwoPointConicalGradientLayout::onGetGLSLProcessorKey(
    const GrShaderCaps& caps, GrProcessorKeyBuilder* b) const {
  b->add32((uint32_t)type);
  b->add32((uint32_t)isRadiusIncreasing);
  b->add32((uint32_t)isFocalOnCircle);
  b->add32((uint32_t)isWellBehaved);
  b->add32((uint32_t)isSwapped);
  b->add32((uint32_t)isNativelyFocal);
}
bool GrTwoPointConicalGradientLayout::onIsEqual(const GrFragmentProcessor& other) const {
  const GrTwoPointConicalGradientLayout& that = other.cast<GrTwoPointConicalGradientLayout>();
  (void)that;
  if (type != that.type) return false;
  if (isRadiusIncreasing != that.isRadiusIncreasing) return false;
  if (isFocalOnCircle != that.isFocalOnCircle) return false;
  if (isWellBehaved != that.isWellBehaved) return false;
  if (isSwapped != that.isSwapped) return false;
  if (isNativelyFocal != that.isNativelyFocal) return false;
  if (focalParams != that.focalParams) return false;
  return true;
}
bool GrTwoPointConicalGradientLayout::usesExplicitReturn() const { return false; }
GrTwoPointConicalGradientLayout::GrTwoPointConicalGradientLayout(
    const GrTwoPointConicalGradientLayout& src)
    : INHERITED(kGrTwoPointConicalGradientLayout_ClassID, src.optimizationFlags()),
      type(src.type),
      isRadiusIncreasing(src.isRadiusIncreasing),
      isFocalOnCircle(src.isFocalOnCircle),
      isWellBehaved(src.isWellBehaved),
      isSwapped(src.isSwapped),
      isNativelyFocal(src.isNativelyFocal),
      focalParams(src.focalParams) {
  this->cloneAndRegisterAllChildProcessors(src);
  this->setUsesSampleCoordsDirectly();
}
std::unique_ptr<GrFragmentProcessor> GrTwoPointConicalGradientLayout::clone() const {
  return std::make_unique<GrTwoPointConicalGradientLayout>(*this);
}
#if GR_TEST_UTILS
SkString GrTwoPointConicalGradientLayout::onDumpInfo() const {
  return SkStringPrintf(
      "(type=%d, isRadiusIncreasing=%s, isFocalOnCircle=%s, isWellBehaved=%s, isSwapped=%s, "
      "isNativelyFocal=%s, focalParams=half2(%f, %f))",
      (int)type, (isRadiusIncreasing ? "true" : "false"), (isFocalOnCircle ? "true" : "false"),
      (isWellBehaved ? "true" : "false"), (isSwapped ? "true" : "false"),
      (isNativelyFocal ? "true" : "false"), focalParams.fX, focalParams.fY);
}
#endif
GR_DEFINE_FRAGMENT_PROCESSOR_TEST(GrTwoPointConicalGradientLayout);
#if GR_TEST_UTILS
std::unique_ptr<GrFragmentProcessor> GrTwoPointConicalGradientLayout::TestCreate(
    GrProcessorTestData* d) {
  SkScalar scale = GrGradientShader::RandomParams::kGradientScale;
  SkScalar offset = scale / 32.0f;

  SkPoint center1 = {
      d->fRandom->nextRangeScalar(0.0f, scale), d->fRandom->nextRangeScalar(0.0f, scale)};
  SkPoint center2 = {
      d->fRandom->nextRangeScalar(0.0f, scale), d->fRandom->nextRangeScalar(0.0f, scale)};
  SkScalar radius1 = d->fRandom->nextRangeScalar(0.0f, scale);
  SkScalar radius2 = d->fRandom->nextRangeScalar(0.0f, scale);

  constexpr int kTestTypeMask = (1 << 2) - 1, kTestNativelyFocalBit = (1 << 2),
                kTestFocalOnCircleBit = (1 << 3), kTestSwappedBit = (1 << 4);
  // We won't treat isWellDefined and isRadiusIncreasing specially because they
  // should have high probability to be turned on and off as we're getting random
  // radii and centers.

  int mask = d->fRandom->nextU();
  int type = mask & kTestTypeMask;
  if (type == static_cast<int>(Type::kRadial)) {
    center2 = center1;
    // Make sure that the radii are different
    if (SkScalarNearlyZero(radius1 - radius2)) {
      radius2 += offset;
    }
  } else if (type == static_cast<int>(Type::kStrip)) {
    radius1 = std::max(radius1, .1f);  // Make sure that the radius is non-zero
    radius2 = radius1;
    // Make sure that the centers are different
    if (SkScalarNearlyZero(SkPoint::Distance(center1, center2))) {
      center2.fX += offset;
    }
  } else {  // kFocal_Type
    // Make sure that the centers are different
    if (SkScalarNearlyZero(SkPoint::Distance(center1, center2))) {
      center2.fX += offset;
    }

    if (kTestNativelyFocalBit & mask) {
      radius1 = 0;
    }
    if (kTestFocalOnCircleBit & mask) {
      radius2 = radius1 + SkPoint::Distance(center1, center2);
    }
    if (kTestSwappedBit & mask) {
      std::swap(radius1, radius2);
      radius2 = 0;
    }

    // Make sure that the radii are different
    if (SkScalarNearlyZero(radius1 - radius2)) {
      radius2 += offset;
    }
  }

  if (SkScalarNearlyZero(radius1 - radius2) &&
      SkScalarNearlyZero(SkPoint::Distance(center1, center2))) {
    radius2 += offset;  // make sure that we're not degenerated
  }

  GrGradientShader::RandomParams params(d->fRandom);
  auto shader = params.fUseColors4f
                    ? SkGradientShader::MakeTwoPointConical(
                          center1, radius1, center2, radius2, params.fColors4f, params.fColorSpace,
                          params.fStops, params.fColorCount, params.fTileMode)
                    : SkGradientShader::MakeTwoPointConical(
                          center1, radius1, center2, radius2, params.fColors, params.fStops,
                          params.fColorCount, params.fTileMode);
  GrTest::TestAsFPArgs asFPArgs(d);
  std::unique_ptr<GrFragmentProcessor> fp = as_SB(shader)->asFragmentProcessor(asFPArgs.args());

  SkASSERT_RELEASE(fp);
  return fp;
}
#endif

// .fp files do not let you reference outside enum definitions, so we have to explicitly map
// between the two compatible enum defs
GrTwoPointConicalGradientLayout::Type convert_type(SkTwoPointConicalGradient::Type type) {
  switch (type) {
    case SkTwoPointConicalGradient::Type::kRadial:
      return GrTwoPointConicalGradientLayout::Type::kRadial;
    case SkTwoPointConicalGradient::Type::kStrip:
      return GrTwoPointConicalGradientLayout::Type::kStrip;
    case SkTwoPointConicalGradient::Type::kFocal:
      return GrTwoPointConicalGradientLayout::Type::kFocal;
  }
  SkDEBUGFAIL("Should not be reachable");
  return GrTwoPointConicalGradientLayout::Type::kRadial;
}

std::unique_ptr<GrFragmentProcessor> GrTwoPointConicalGradientLayout::Make(
    const SkTwoPointConicalGradient& grad, const GrFPArgs& args) {
  GrTwoPointConicalGradientLayout::Type grType = convert_type(grad.getType());

  // The focalData struct is only valid if isFocal is true
  const SkTwoPointConicalGradient::FocalData& focalData = grad.getFocalData();
  bool isFocal = grType == Type::kFocal;

  // Calculate optimization switches from gradient specification
  bool isFocalOnCircle = isFocal && focalData.isFocalOnCircle();
  bool isWellBehaved = isFocal && focalData.isWellBehaved();
  bool isSwapped = isFocal && focalData.isSwapped();
  bool isNativelyFocal = isFocal && focalData.isNativelyFocal();

  // Type-specific calculations: isRadiusIncreasing, focalParams, and the gradient matrix.
  // However, all types start with the total inverse local matrix calculated from the shader
  // and args
  bool isRadiusIncreasing;
  SkPoint focalParams;  // really just a 2D tuple
  SkMatrix matrix;

  // Initialize the base matrix
  if (!grad.totalLocalMatrix(args.fPreLocalMatrix)->invert(&matrix)) {
    return nullptr;
  }

  if (isFocal) {
    isRadiusIncreasing = (1 - focalData.fFocalX) > 0;

    focalParams.set(1.0 / focalData.fR1, focalData.fFocalX);

    matrix.postConcat(grad.getGradientMatrix());
  } else if (grType == Type::kRadial) {
    SkScalar dr = grad.getDiffRadius();
    isRadiusIncreasing = dr >= 0;

    SkScalar r0 = grad.getStartRadius() / dr;
    focalParams.set(r0, r0 * r0);

    // GPU radial matrix is different from the original matrix, since we map the diff radius
    // to have |dr| = 1, so manually compute the final gradient matrix here.

    // Map center to (0, 0)
    matrix.postTranslate(-grad.getStartCenter().fX, -grad.getStartCenter().fY);

    // scale |diffRadius| to 1
    matrix.postScale(1 / dr, 1 / dr);
  } else {                       // kStrip
    isRadiusIncreasing = false;  // kStrip doesn't use this flag

    SkScalar r0 = grad.getStartRadius() / grad.getCenterX1();
    focalParams.set(r0, r0 * r0);

    matrix.postConcat(grad.getGradientMatrix());
  }

  return GrMatrixEffect::Make(
      matrix, std::unique_ptr<GrFragmentProcessor>(new GrTwoPointConicalGradientLayout(
                  grType, isRadiusIncreasing, isFocalOnCircle, isWellBehaved, isSwapped,
                  isNativelyFocal, focalParams)));
}
