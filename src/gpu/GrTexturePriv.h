/*
 * Copyright 2014 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef GrTexturePriv_DEFINED
#define GrTexturePriv_DEFINED

#include "include/gpu/GrTexture.h"

/** Class that adds methods to GrTexture that are only intended for use internal to Skia.
    This class is purely a privileged window into GrTexture. It should never have additional data
    members or virtual methods.
    Non-static methods that are not trivial inlines should be spring-boarded (e.g. declared and
    implemented privately in GrTexture with a inline public method here). */
class GrTexturePriv {
 public:
  void markMipMapsDirty() noexcept { fTexture->markMipMapsDirty(); }

  void markMipMapsClean() noexcept { fTexture->markMipMapsClean(); }

  bool mipMapsAreDirty() const noexcept {
    return GrMipMapsStatus::kValid != fTexture->fMipMapsStatus;
  }

  GrMipMapped mipMapped() const noexcept {
    if (GrMipMapsStatus::kNotAllocated != fTexture->fMipMapsStatus) {
      return GrMipMapped::kYes;
    }
    return GrMipMapped::kNo;
  }

  int maxMipMapLevel() const noexcept { return fTexture->fMaxMipMapLevel; }

  GrTextureType textureType() const noexcept { return fTexture->fTextureType; }
  bool hasRestrictedSampling() const noexcept {
    return GrTextureTypeHasRestrictedSampling(this->textureType());
  }
  /** Filtering is clamped to this value. */
  GrSamplerState::Filter highestFilterMode() const {
    return this->hasRestrictedSampling() ? GrSamplerState::Filter::kBilerp
                                         : GrSamplerState::Filter::kMipMap;
  }

  static void ComputeScratchKey(const GrSurfaceDesc&, GrScratchKey*);
  static void ComputeScratchKey(
      GrPixelConfig config, int width, int height, bool isRenderTarget, int sampleCnt, GrMipMapped,
      GrScratchKey* key);

 private:
  GrTexturePriv(GrTexture* texture) noexcept : fTexture(texture) {}
  GrTexturePriv(const GrTexturePriv& that) noexcept : fTexture(that.fTexture) {}
  GrTexturePriv& operator=(const GrTexturePriv&);  // unimpl

  // No taking addresses of this type.
  const GrTexturePriv* operator&() const;
  GrTexturePriv* operator&();

  GrTexture* fTexture;

  friend class GrTexture;  // to construct/copy this type.
};

inline GrTexturePriv GrTexture::texturePriv() noexcept { return GrTexturePriv(this); }

inline const GrTexturePriv GrTexture::texturePriv() const noexcept {
  return GrTexturePriv(const_cast<GrTexture*>(this));
}

#endif
