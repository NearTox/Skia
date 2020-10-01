/*
 * Copyright 2015 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef GrSamplerState_DEFINED
#define GrSamplerState_DEFINED

#include "include/gpu/GrTypes.h"
#include <limits>

/**
 * Represents the filtering and tile modes used to access a texture.
 */
class GrSamplerState {
 public:
  enum class Filter : uint8_t { kNearest, kLinear, kLast = kLinear };
  enum class MipmapMode : uint8_t { kNone, kNearest, kLinear, kLast = kLinear };

  enum class WrapMode : uint8_t {
    kClamp,
    kRepeat,
    kMirrorRepeat,
    kClampToBorder,
    kLast = kClampToBorder
  };

  static constexpr int kFilterCount = static_cast<int>(Filter::kLast) + 1;
  static constexpr int kWrapModeCount = static_cast<int>(WrapMode::kLast) + 1;

  constexpr GrSamplerState() noexcept = default;

  constexpr GrSamplerState(
      WrapMode wrapXAndY, Filter filter, MipmapMode mm = MipmapMode::kNone) noexcept
      : fWrapModes{wrapXAndY, wrapXAndY}, fFilter(filter), fMipmapMode(mm) {}

  constexpr GrSamplerState(
      WrapMode wrapX, WrapMode wrapY, Filter filter, MipmapMode mm = MipmapMode::kNone) noexcept
      : fWrapModes{wrapX, wrapY}, fFilter(filter), fMipmapMode(mm) {}

  constexpr GrSamplerState(
      const WrapMode wrapModes[2], Filter filter, MipmapMode mm = MipmapMode::kNone) noexcept
      : fWrapModes{wrapModes[0], wrapModes[1]}, fFilter(filter), fMipmapMode(mm) {}

  constexpr /*explicit*/ GrSamplerState(Filter filter) noexcept : fFilter(filter) {}
  constexpr GrSamplerState(Filter filter, MipmapMode mm) noexcept
      : fFilter(filter), fMipmapMode(mm) {}

  constexpr GrSamplerState(const GrSamplerState&) noexcept = default;

  constexpr GrSamplerState& operator=(const GrSamplerState&) noexcept = default;

  constexpr WrapMode wrapModeX() const noexcept { return fWrapModes[0]; }

  constexpr WrapMode wrapModeY() const noexcept { return fWrapModes[1]; }

  constexpr bool isRepeated() const noexcept {
    return fWrapModes[0] == WrapMode::kRepeat || fWrapModes[0] == WrapMode::kMirrorRepeat ||
           fWrapModes[1] == WrapMode::kRepeat || fWrapModes[1] == WrapMode::kMirrorRepeat;
  }

  constexpr Filter filter() const noexcept { return fFilter; }

  constexpr MipmapMode mipmapMode() const noexcept { return fMipmapMode; }

  constexpr GrMipmapped mipmapped() const noexcept {
    return GrMipmapped(fMipmapMode != MipmapMode::kNone);
  }

  constexpr void setFilterMode(Filter filterMode) noexcept { fFilter = filterMode; }

  constexpr void setMipmapMode(MipmapMode mm) noexcept { fMipmapMode = mm; }

  constexpr void setWrapModeX(const WrapMode wrap) noexcept { fWrapModes[0] = wrap; }

  constexpr void setWrapModeY(const WrapMode wrap) noexcept { fWrapModes[1] = wrap; }

  constexpr bool operator==(GrSamplerState that) const noexcept {
    return fWrapModes[0] == that.fWrapModes[0] && fWrapModes[1] == that.fWrapModes[1] &&
           fFilter == that.fFilter && fMipmapMode == that.fMipmapMode;
  }

  constexpr bool operator!=(const GrSamplerState& that) const noexcept { return !(*this == that); }

  /**
   * Turn the sampler state into an integer from a tightly packed range for use as an index
   * (or key)
   */
  constexpr uint8_t asIndex() const noexcept {
    constexpr int kNumWraps = static_cast<int>(WrapMode::kLast) + 1;
    constexpr int kNumFilters = static_cast<int>(Filter::kLast) + 1;
    int result = static_cast<int>(fWrapModes[0]) * 1 + static_cast<int>(fWrapModes[1]) * kNumWraps +
                 static_cast<int>(fFilter) * kNumWraps * kNumWraps +
                 static_cast<int>(fMipmapMode) * kNumWraps * kNumWraps * kNumFilters;
    SkASSERT(result <= kNumUniqueSamplers);
    return static_cast<uint8_t>(result);
  }

  static constexpr int kNumUniqueSamplers =
      (static_cast<int>(WrapMode::kLast) + 1) * (static_cast<int>(WrapMode::kLast) + 1) *
      (static_cast<int>(Filter::kLast) + 1) * (static_cast<int>(MipmapMode::kLast) + 1);

 private:
  WrapMode fWrapModes[2] = {WrapMode::kClamp, WrapMode::kClamp};
  Filter fFilter = GrSamplerState::Filter::kNearest;
  MipmapMode fMipmapMode = GrSamplerState::MipmapMode::kNone;
};

static_assert(
    GrSamplerState::kNumUniqueSamplers <=
    std::numeric_limits<decltype(GrSamplerState{}.asIndex())>::max());

#endif
