/*
 * Copyright 2017 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef GrColorInfo_DEFINED
#define GrColorInfo_DEFINED

#include "include/core/SkColorSpace.h"
#include "include/core/SkRefCnt.h"
#include "include/gpu/GrTypes.h"
#include "src/gpu/GrColorSpaceXform.h"

/**
 * All the info needed to interpret a color: Color type + alpha type + color space. Also caches
 * the GrColorSpaceXform from sRGB. */
class GrColorInfo {
 public:
  constexpr GrColorInfo() noexcept = default;
  GrColorInfo(const GrColorInfo&) noexcept;
  GrColorInfo(GrColorInfo&&) noexcept;
  GrColorInfo& operator=(const GrColorInfo&) noexcept;
  GrColorInfo& operator=(GrColorInfo&&) noexcept;
  GrColorInfo(GrColorType, SkAlphaType, sk_sp<SkColorSpace>);
  /* implicit */ GrColorInfo(const SkColorInfo&);

  bool isLinearlyBlended() const noexcept { return fColorSpace && fColorSpace->gammaIsLinear(); }

  SkColorSpace* colorSpace() const noexcept { return fColorSpace.get(); }
  sk_sp<SkColorSpace> refColorSpace() const noexcept { return fColorSpace; }

  GrColorSpaceXform* colorSpaceXformFromSRGB() const noexcept { return fColorXformFromSRGB.get(); }
  sk_sp<GrColorSpaceXform> refColorSpaceXformFromSRGB() const noexcept {
    return fColorXformFromSRGB;
  }

  GrColorType colorType() const noexcept { return fColorType; }
  SkAlphaType alphaType() const noexcept { return fAlphaType; }

  bool isValid() const noexcept {
    return fColorType != GrColorType::kUnknown && fAlphaType != kUnknown_SkAlphaType;
  }

 private:
  sk_sp<SkColorSpace> fColorSpace;
  sk_sp<GrColorSpaceXform> fColorXformFromSRGB;
  GrColorType fColorType = GrColorType::kUnknown;
  SkAlphaType fAlphaType = kUnknown_SkAlphaType;
};

#endif
