/*
 * Copyright 2017 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "src/gpu/GrColorInfo.h"

#include "src/core/SkColorSpacePriv.h"

GrColorInfo::GrColorInfo(
    GrColorType colorType, SkAlphaType alphaType, sk_sp<SkColorSpace> colorSpace)
    : fColorSpace(std::move(colorSpace)), fColorType(colorType), fAlphaType(alphaType) {
  // sRGB sources are very common (SkColor, etc...), so we cache that transformation
  fColorXformFromSRGB = GrColorSpaceXform::Make(
      sk_srgb_singleton(), kUnpremul_SkAlphaType, fColorSpace.get(), kUnpremul_SkAlphaType);
}

GrColorInfo::GrColorInfo(const SkColorInfo& ci)
    : GrColorInfo(SkColorTypeToGrColorType(ci.colorType()), ci.alphaType(), ci.refColorSpace()) {}

GrColorInfo::GrColorInfo(GrColorInfo&&) noexcept = default;
GrColorInfo& GrColorInfo::operator=(GrColorInfo&&) noexcept = default;

GrColorInfo::GrColorInfo(const GrColorInfo&) noexcept = default;
GrColorInfo& GrColorInfo::operator=(const GrColorInfo&) noexcept = default;
