/*
 * Copyright 2017 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef SkImageInfoPriv_DEFINED
#define SkImageInfoPriv_DEFINED

#include "include/core/SkColor.h"
#include "include/core/SkImageInfo.h"

static constexpr inline uint32_t SkColorTypeChannelFlags(SkColorType ct) noexcept {
  switch (ct) {
    case kUnknown_SkColorType: return 0;
    case kAlpha_8_SkColorType: return kAlpha_SkColorChannelFlag;
    case kRGB_565_SkColorType: return kRGB_SkColorChannelFlags;
    case kARGB_4444_SkColorType: return kRGBA_SkColorChannelFlags;
    case kRGBA_8888_SkColorType: return kRGBA_SkColorChannelFlags;
    case kRGB_888x_SkColorType: return kRGB_SkColorChannelFlags;
    case kBGRA_8888_SkColorType: return kRGBA_SkColorChannelFlags;
    case kRGBA_1010102_SkColorType: return kRGBA_SkColorChannelFlags;
    case kRGB_101010x_SkColorType: return kRGB_SkColorChannelFlags;
    case kBGRA_1010102_SkColorType: return kRGBA_SkColorChannelFlags;
    case kBGR_101010x_SkColorType: return kRGB_SkColorChannelFlags;
    case kGray_8_SkColorType: return kGray_SkColorChannelFlag;
    case kRGBA_F16Norm_SkColorType: return kRGBA_SkColorChannelFlags;
    case kRGBA_F16_SkColorType: return kRGBA_SkColorChannelFlags;
    case kRGBA_F32_SkColorType: return kRGBA_SkColorChannelFlags;
    case kR8G8_unorm_SkColorType: return kRG_SkColorChannelFlags;
    case kA16_unorm_SkColorType: return kAlpha_SkColorChannelFlag;
    case kR16G16_unorm_SkColorType: return kRG_SkColorChannelFlags;
    case kA16_float_SkColorType: return kAlpha_SkColorChannelFlag;
    case kR16G16_float_SkColorType: return kRG_SkColorChannelFlags;
    case kR16G16B16A16_unorm_SkColorType: return kRGBA_SkColorChannelFlags;
  }
  SkUNREACHABLE;
}

static constexpr inline bool SkColorTypeIsAlphaOnly(SkColorType ct) noexcept {
  return SkColorTypeChannelFlags(ct) == kAlpha_SkColorChannelFlag;
}

static constexpr inline bool SkAlphaTypeIsValid(unsigned value) noexcept {
  return value <= kLastEnum_SkAlphaType;
}

static int SkColorTypeShiftPerPixel(SkColorType ct) noexcept {
  switch (ct) {
    case kUnknown_SkColorType: return 0;
    case kAlpha_8_SkColorType: return 0;
    case kRGB_565_SkColorType: return 1;
    case kARGB_4444_SkColorType: return 1;
    case kRGBA_8888_SkColorType: return 2;
    case kRGB_888x_SkColorType: return 2;
    case kBGRA_8888_SkColorType: return 2;
    case kRGBA_1010102_SkColorType: return 2;
    case kRGB_101010x_SkColorType: return 2;
    case kBGRA_1010102_SkColorType: return 2;
    case kBGR_101010x_SkColorType: return 2;
    case kGray_8_SkColorType: return 0;
    case kRGBA_F16Norm_SkColorType: return 3;
    case kRGBA_F16_SkColorType: return 3;
    case kRGBA_F32_SkColorType: return 4;
    case kR8G8_unorm_SkColorType: return 1;
    case kA16_unorm_SkColorType: return 1;
    case kR16G16_unorm_SkColorType: return 2;
    case kA16_float_SkColorType: return 1;
    case kR16G16_float_SkColorType: return 2;
    case kR16G16B16A16_unorm_SkColorType: return 3;
  }
  SkUNREACHABLE;
}

static inline size_t SkColorTypeMinRowBytes(SkColorType ct, int width) noexcept {
  return (size_t)(width * SkColorTypeBytesPerPixel(ct));
}

static constexpr inline bool SkColorTypeIsValid(unsigned value) noexcept {
  return value <= kLastEnum_SkColorType;
}

static inline size_t SkColorTypeComputeOffset(
    SkColorType ct, int x, int y, size_t rowBytes) noexcept {
  if (kUnknown_SkColorType == ct) {
    return 0;
  }
  return (size_t)y * rowBytes + ((size_t)x << SkColorTypeShiftPerPixel(ct));
}

static inline bool SkColorTypeIsNormalized(SkColorType ct) noexcept {
  switch (ct) {
    case kUnknown_SkColorType:
    case kAlpha_8_SkColorType:
    case kRGB_565_SkColorType:
    case kARGB_4444_SkColorType:
    case kRGBA_8888_SkColorType:
    case kRGB_888x_SkColorType:
    case kBGRA_8888_SkColorType:
    case kRGBA_1010102_SkColorType:
    case kRGB_101010x_SkColorType:
    case kBGRA_1010102_SkColorType:
    case kBGR_101010x_SkColorType:
    case kGray_8_SkColorType:
    case kRGBA_F16Norm_SkColorType:
    case kR8G8_unorm_SkColorType:
    case kA16_unorm_SkColorType:
    case kA16_float_SkColorType: /*subtle... alpha is always [0,1]*/
    case kR16G16_unorm_SkColorType:
    case kR16G16B16A16_unorm_SkColorType: return true;

    case kRGBA_F16_SkColorType:
    case kRGBA_F32_SkColorType:
    case kR16G16_float_SkColorType: return false;
  }
  SkUNREACHABLE;
}

/**
 *  Returns true if |info| contains a valid colorType and alphaType.
 */
static inline bool SkColorInfoIsValid(const SkColorInfo& info) noexcept {
  return info.colorType() != kUnknown_SkColorType && info.alphaType() != kUnknown_SkAlphaType;
}

/**
 *  Returns true if |info| contains a valid combination of width, height and colorInfo.
 */
static inline bool SkImageInfoIsValid(const SkImageInfo& info) noexcept {
  if (info.width() <= 0 || info.height() <= 0) {
    return false;
  }

  constexpr int kMaxDimension = SK_MaxS32 >> 2;
  if (info.width() > kMaxDimension || info.height() > kMaxDimension) {
    return false;
  }

  return SkColorInfoIsValid(info.colorInfo());
}

/**
 *  Returns true if Skia has defined a pixel conversion from the |src| to the |dst|.
 *  Returns false otherwise.
 */
static inline bool SkImageInfoValidConversion(
    const SkImageInfo& dst, const SkImageInfo& src) noexcept {
  return SkImageInfoIsValid(dst) && SkImageInfoIsValid(src);
}
#endif  // SkImageInfoPriv_DEFINED
