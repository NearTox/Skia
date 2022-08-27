/*
 * Copyright 2020 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef SkImageSampling_DEFINED
#define SkImageSampling_DEFINED

#include "include/core/SkTypes.h"

#include <algorithm>
#include <new>

enum class SkFilterMode {
  kNearest,  // single sample point (nearest neighbor)
  kLinear,   // interporate between 2x2 sample points (bilinear interpolation)

  kLast = kLinear,
};

enum class SkMipmapMode {
  kNone,     // ignore mipmap levels, sample from the "base"
  kNearest,  // sample from the nearest level
  kLinear,   // interpolate between the two nearest levels

  kLast = kLinear,
};

/*
 *  Specify B and C (each between 0...1) to create a shader that applies the corresponding
 *  cubic reconstruction filter to the image.
 *
 *  Example values:
 *      B = 1/3, C = 1/3        "Mitchell" filter
 *      B = 0,   C = 1/2        "Catmull-Rom" filter
 *
 *  See "Reconstruction Filters in Computer Graphics"
 *          Don P. Mitchell
 *          Arun N. Netravali
 *          1988
 *  https://www.cs.utexas.edu/~fussell/courses/cs384g-fall2013/lectures/mitchell/Mitchell.pdf
 *
 *  Desmos worksheet https://www.desmos.com/calculator/aghdpicrvr
 *  Nice overview https://entropymine.com/imageworsener/bicubic/
 */
struct SkCubicResampler {
  float B, C;

  // Historic default for kHigh_SkFilterQuality
  static constexpr SkCubicResampler Mitchell() noexcept { return {1 / 3.0f, 1 / 3.0f}; }
  static constexpr SkCubicResampler CatmullRom() noexcept { return {0.0f, 1 / 2.0f}; }
};

struct SK_API SkSamplingOptions {
  const int maxAniso = 0;
  const bool useCubic = false;
  const SkCubicResampler cubic = {0, 0};
  const SkFilterMode filter = SkFilterMode::kNearest;
  const SkMipmapMode mipmap = SkMipmapMode::kNone;

  constexpr SkSamplingOptions() noexcept = default;
  SkSamplingOptions(const SkSamplingOptions&) noexcept = default;
  SkSamplingOptions& operator=(const SkSamplingOptions& that) noexcept {
    this->~SkSamplingOptions();  // A pedantic no-op.
    new (this) SkSamplingOptions(that);
    return *this;
  }

  constexpr SkSamplingOptions(SkFilterMode fm, SkMipmapMode mm) noexcept : filter(fm), mipmap(mm) {}

  constexpr explicit SkSamplingOptions(SkFilterMode fm) noexcept
      : filter(fm), mipmap(SkMipmapMode::kNone) {}

  constexpr explicit SkSamplingOptions(const SkCubicResampler& c) noexcept
      : useCubic(true), cubic(c) {}

  static constexpr SkSamplingOptions Aniso(int maxAniso) noexcept {
    return SkSamplingOptions{std::max(maxAniso, 1)};
  }

  bool operator==(const SkSamplingOptions& other) const noexcept {
    return maxAniso == other.maxAniso && useCubic == other.useCubic && cubic.B == other.cubic.B &&
           cubic.C == other.cubic.C && filter == other.filter && mipmap == other.mipmap;
  }
  bool operator!=(const SkSamplingOptions& other) const noexcept { return !(*this == other); }

  bool isAniso() const noexcept { return maxAniso != 0; }

 private:
  constexpr SkSamplingOptions(int maxAniso) noexcept : maxAniso(maxAniso) {}
};

#endif
