/*
 * Copyright 2017 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/**************************************************************************************************
 *** This file was autogenerated from GrSimpleTextureEffect.fp; do not modify.
 **************************************************************************************************/
#ifndef GrSimpleTextureEffect_DEFINED
#define GrSimpleTextureEffect_DEFINED
#include "include/core/SkTypes.h"

#include "src/gpu/GrCoordTransform.h"
#include "src/gpu/GrFragmentProcessor.h"
class GrSimpleTextureEffect : public GrFragmentProcessor {
 public:
  static std::unique_ptr<GrFragmentProcessor> Make(
      sk_sp<GrTextureProxy> proxy, GrColorType srcColorType, const SkMatrix& matrix) {
    return std::unique_ptr<GrFragmentProcessor>(new GrSimpleTextureEffect(
        std::move(proxy), matrix, srcColorType,
        GrSamplerState(GrSamplerState::WrapMode::kClamp, GrSamplerState::Filter::kNearest)));
  }

  /* clamp mode */
  static std::unique_ptr<GrFragmentProcessor> Make(
      sk_sp<GrTextureProxy> proxy, GrColorType srcColorType, const SkMatrix& matrix,
      GrSamplerState::Filter filter) {
    return std::unique_ptr<GrFragmentProcessor>(new GrSimpleTextureEffect(
        std::move(proxy), matrix, srcColorType,
        GrSamplerState(GrSamplerState::WrapMode::kClamp, filter)));
  }

  static std::unique_ptr<GrFragmentProcessor> Make(
      sk_sp<GrTextureProxy> proxy, GrColorType srcColorType, const SkMatrix& matrix,
      const GrSamplerState& p) {
    return std::unique_ptr<GrFragmentProcessor>(
        new GrSimpleTextureEffect(std::move(proxy), matrix, srcColorType, p));
  }
  GrSimpleTextureEffect(const GrSimpleTextureEffect& src);
  std::unique_ptr<GrFragmentProcessor> clone() const override;
  const char* name() const override { return "SimpleTextureEffect"; }
  GrCoordTransform imageCoordTransform;
  TextureSampler image;
  SkMatrix44 matrix;

 private:
  GrSimpleTextureEffect(
      sk_sp<GrTextureProxy> image, SkMatrix44 matrix, GrColorType srcColorType,
      GrSamplerState samplerParams)
      : INHERITED(
            kGrSimpleTextureEffect_ClassID,
            (OptimizationFlags)ModulateForSamplerOptFlags(
                srcColorType,
                samplerParams.wrapModeX() == GrSamplerState::WrapMode::kClampToBorder ||
                    samplerParams.wrapModeY() == GrSamplerState::WrapMode::kClampToBorder)),
        imageCoordTransform(matrix, image.get()),
        image(std::move(image), samplerParams),
        matrix(matrix) {
    this->setTextureSamplerCnt(1);
    this->addCoordTransform(&imageCoordTransform);
  }
  GrGLSLFragmentProcessor* onCreateGLSLInstance() const override;
  void onGetGLSLProcessorKey(const GrShaderCaps&, GrProcessorKeyBuilder*) const override;
  bool onIsEqual(const GrFragmentProcessor&) const override;
  const TextureSampler& onTextureSampler(int) const override;
  GR_DECLARE_FRAGMENT_PROCESSOR_TEST
  typedef GrFragmentProcessor INHERITED;
};
#endif
