/*
 * Copyright 2014 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "src/gpu/GrPrimitiveProcessor.h"

#include "src/gpu/GrCoordTransform.h"
#include "src/gpu/GrFragmentProcessor.h"

/**
 * We specialize the vertex or fragment coord transform code for these matrix types.
 * Some specializations are only applied when the coord transform is applied in the fragment
 * shader.
 */
enum MatrixType {
  kNone_MatrixType = 0,            // Used only in FS for explicitly sampled FPs
  kScaleTranslate_MatrixType = 1,  // Used only in FS for explicitly sampled FPs
  kNoPersp_MatrixType = 2,
  kGeneral_MatrixType = 3,
};

GrPrimitiveProcessor::GrPrimitiveProcessor(ClassID classID) : GrProcessor(classID) {}

const GrPrimitiveProcessor::TextureSampler& GrPrimitiveProcessor::textureSampler(int i) const {
  SkASSERT(i >= 0 && i < this->numTextureSamplers());
  return this->onTextureSampler(i);
}

uint32_t GrPrimitiveProcessor::computeCoordTransformsKey(const GrFragmentProcessor& fp) const {
  // This is highly coupled with the code in GrGLSLGeometryProcessor::emitTransforms().
  SkASSERT(fp.numCoordTransforms() * 2 <= 32);
  uint32_t totalKey = 0;
  for (int t = 0; t < fp.numCoordTransforms(); ++t) {
    uint32_t key = 0;
    const GrCoordTransform& coordTransform = fp.coordTransform(t);
    if (fp.isSampledWithExplicitCoords() && coordTransform.isNoOp()) {
      key = kNone_MatrixType;
    } else if (fp.isSampledWithExplicitCoords() && coordTransform.matrix().isScaleTranslate()) {
      key = kScaleTranslate_MatrixType;
    } else if (!coordTransform.matrix().hasPerspective()) {
      key = kNoPersp_MatrixType;
    } else {
      // Note that we can also have homogeneous varyings as a result of a GP local matrix or
      // homogeneous local coords generated by GP. We're relying on the GP to include any
      // variability in those in its key.
      key = kGeneral_MatrixType;
    }
    key <<= 2 * t;
    SkASSERT(0 == (totalKey & key));  // keys for each transform ought not to overlap
    totalKey |= key;
  }
  return totalKey;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

static inline GrSamplerState::Filter clamp_filter(
    GrTextureType type, GrSamplerState::Filter requestedFilter) {
  if (GrTextureTypeHasRestrictedSampling(type)) {
    return std::min(requestedFilter, GrSamplerState::Filter::kBilerp);
  }
  return requestedFilter;
}

GrPrimitiveProcessor::TextureSampler::TextureSampler(
    GrSamplerState samplerState, const GrBackendFormat& backendFormat, const GrSwizzle& swizzle) {
  this->reset(samplerState, backendFormat, swizzle);
}

void GrPrimitiveProcessor::TextureSampler::reset(
    GrSamplerState samplerState, const GrBackendFormat& backendFormat, const GrSwizzle& swizzle) {
  fSamplerState = samplerState;
  fSamplerState.setFilterMode(clamp_filter(backendFormat.textureType(), samplerState.filter()));
  fBackendFormat = backendFormat;
  fSwizzle = swizzle;
  fIsInitialized = true;
}
