/*
 * Copyright 2014 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef GrMatrixConvolutionEffect_DEFINED
#define GrMatrixConvolutionEffect_DEFINED

#include "src/gpu/effects/GrTextureDomain.h"

// A little bit less than the minimum # uniforms required by DX9SM2 (32).
// Allows for a 5x5 kernel (or 25x1, for that matter).
#define MAX_KERNEL_SIZE 25

class GrMatrixConvolutionEffect : public GrFragmentProcessor {
 public:
  static std::unique_ptr<GrFragmentProcessor> Make(
      sk_sp<GrTextureProxy> srcProxy, const SkIRect& srcBounds, const SkISize& kernelSize,
      const SkScalar* kernel, SkScalar gain, SkScalar bias, const SkIPoint& kernelOffset,
      GrTextureDomain::Mode tileMode, bool convolveAlpha) {
    return std::unique_ptr<GrFragmentProcessor>(new GrMatrixConvolutionEffect(
        std::move(srcProxy), srcBounds, kernelSize, kernel, gain, bias, kernelOffset, tileMode,
        convolveAlpha));
  }

  static std::unique_ptr<GrFragmentProcessor> MakeGaussian(
      sk_sp<GrTextureProxy> srcProxy, const SkIRect& srcBounds, const SkISize& kernelSize,
      SkScalar gain, SkScalar bias, const SkIPoint& kernelOffset, GrTextureDomain::Mode tileMode,
      bool convolveAlpha, SkScalar sigmaX, SkScalar sigmaY);

  const SkIRect& bounds() const noexcept { return fBounds; }
  const SkISize& kernelSize() const noexcept { return fKernelSize; }
  const float* kernelOffset() const noexcept { return fKernelOffset; }
  const float* kernel() const noexcept { return fKernel; }
  float gain() const noexcept { return fGain; }
  float bias() const noexcept { return fBias; }
  bool convolveAlpha() const noexcept { return fConvolveAlpha; }
  const GrTextureDomain& domain() const noexcept { return fDomain; }

  const char* name() const noexcept override { return "MatrixConvolution"; }

  std::unique_ptr<GrFragmentProcessor> clone() const override;

 private:
  // srcProxy is the texture that is going to be convolved
  // srcBounds is the subset of 'srcProxy' that will be used (e.g., for clamp mode)
  GrMatrixConvolutionEffect(
      sk_sp<GrTextureProxy> srcProxy, const SkIRect& srcBounds, const SkISize& kernelSize,
      const SkScalar* kernel, SkScalar gain, SkScalar bias, const SkIPoint& kernelOffset,
      GrTextureDomain::Mode tileMode, bool convolveAlpha);

  GrMatrixConvolutionEffect(const GrMatrixConvolutionEffect&) noexcept;

  GrGLSLFragmentProcessor* onCreateGLSLInstance() const override;

  void onGetGLSLProcessorKey(const GrShaderCaps&, GrProcessorKeyBuilder*) const noexcept override;

  bool onIsEqual(const GrFragmentProcessor&) const noexcept override;

  const TextureSampler& onTextureSampler(int i) const noexcept override { return fTextureSampler; }

  GrCoordTransform fCoordTransform;
  GrTextureDomain fDomain;
  TextureSampler fTextureSampler;
  SkIRect fBounds;
  SkISize fKernelSize;
  float fKernel[MAX_KERNEL_SIZE];
  float fGain;
  float fBias;
  float fKernelOffset[2];
  bool fConvolveAlpha;

  GR_DECLARE_FRAGMENT_PROCESSOR_TEST

  typedef GrFragmentProcessor INHERITED;
};

#endif
