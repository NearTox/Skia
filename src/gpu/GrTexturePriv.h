/*
 * Copyright 2014 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef GrTexturePriv_DEFINED
#define GrTexturePriv_DEFINED

#include "src/gpu/GrSamplerState.h"
#include "src/gpu/GrTexture.h"

/** Class that adds methods to GrTexture that are only intended for use internal to Skia.
    This class is purely a privileged window into GrTexture. It should never have additional data
    members or virtual methods.
    Non-static methods that are not trivial inlines should be spring-boarded (e.g. declared and
    implemented privately in GrTexture with a inline public method here). */
class GrTexturePriv {
 public:
  void markMipMapsDirty() { fTexture->markMipMapsDirty(); }

  void markMipMapsClean() { fTexture->markMipMapsClean(); }

  GrMipMapsStatus mipMapsStatus() const noexcept { return fTexture->fMipMapsStatus; }

  bool mipMapsAreDirty() const { return GrMipMapsStatus::kValid != this->mipMapsStatus(); }

  GrMipMapped mipMapped() const {
    if (GrMipMapsStatus::kNotAllocated != this->mipMapsStatus()) {
      return GrMipMapped::kYes;
    }
    return GrMipMapped::kNo;
  }

  int maxMipMapLevel() const noexcept { return fTexture->fMaxMipMapLevel; }

  GrTextureType textureType() const noexcept { return fTexture->fTextureType; }
  bool hasRestrictedSampling() const {
    return GrTextureTypeHasRestrictedSampling(this->textureType());
  }

  static void ComputeScratchKey(
      const GrCaps& caps, const GrBackendFormat& format, SkISize dimensions, GrRenderable,
      int sampleCnt, GrMipMapped, GrProtected, GrScratchKey* key);

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
