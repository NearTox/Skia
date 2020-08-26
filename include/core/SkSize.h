/*
 * Copyright 2011 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef SkSize_DEFINED
#define SkSize_DEFINED

#include "include/core/SkScalar.h"

struct SkISize {
  int32_t fWidth;
  int32_t fHeight;

  static constexpr SkISize Make(int32_t w, int32_t h) noexcept { return {w, h}; }

  static constexpr SkISize MakeEmpty() noexcept { return {0, 0}; }

  constexpr void set(int32_t w, int32_t h) noexcept { *this = SkISize{w, h}; }

  /** Returns true iff fWidth == 0 && fHeight == 0
   */
  constexpr bool isZero() const noexcept { return 0 == fWidth && 0 == fHeight; }

  /** Returns true if either width or height are <= 0 */
  constexpr bool isEmpty() const noexcept { return fWidth <= 0 || fHeight <= 0; }

  /** Set the width and height to 0 */
  constexpr void setEmpty() noexcept { fWidth = fHeight = 0; }

  constexpr int32_t width() const noexcept { return fWidth; }
  constexpr int32_t height() const noexcept { return fHeight; }

  constexpr int64_t area() const noexcept { return fWidth * fHeight; }

  constexpr bool equals(int32_t w, int32_t h) const noexcept { return fWidth == w && fHeight == h; }
};

static constexpr inline bool operator==(const SkISize& a, const SkISize& b) noexcept {
  return a.fWidth == b.fWidth && a.fHeight == b.fHeight;
}

static constexpr inline bool operator!=(const SkISize& a, const SkISize& b) noexcept {
  return !(a == b);
}

///////////////////////////////////////////////////////////////////////////////

struct SkSize {
  SkScalar fWidth;
  SkScalar fHeight;

  static constexpr SkSize Make(SkScalar w, SkScalar h) noexcept { return {w, h}; }

  static constexpr SkSize Make(const SkISize& src) noexcept {
    return {SkIntToScalar(src.width()), SkIntToScalar(src.height())};
  }

  static constexpr SkSize MakeEmpty() noexcept { return {0, 0}; }

  constexpr void set(SkScalar w, SkScalar h) noexcept { *this = SkSize{w, h}; }

  /** Returns true iff fWidth == 0 && fHeight == 0
   */
  constexpr bool isZero() const noexcept { return 0 == fWidth && 0 == fHeight; }

  /** Returns true if either width or height are <= 0 */
  constexpr bool isEmpty() const noexcept { return fWidth <= 0 || fHeight <= 0; }

  /** Set the width and height to 0 */
  constexpr void setEmpty() noexcept { *this = SkSize{0, 0}; }

  constexpr SkScalar width() const noexcept { return fWidth; }
  constexpr SkScalar height() const noexcept { return fHeight; }

  bool equals(SkScalar w, SkScalar h) const noexcept { return fWidth == w && fHeight == h; }

  SkISize toRound() const noexcept {
    return {SkScalarRoundToInt(fWidth), SkScalarRoundToInt(fHeight)};
  }

  SkISize toCeil() const noexcept {
    return {SkScalarCeilToInt(fWidth), SkScalarCeilToInt(fHeight)};
  }

  SkISize toFloor() const noexcept {
    return {SkScalarFloorToInt(fWidth), SkScalarFloorToInt(fHeight)};
  }
};

static constexpr inline bool operator==(const SkSize& a, const SkSize& b) noexcept {
  return a.fWidth == b.fWidth && a.fHeight == b.fHeight;
}

static constexpr inline bool operator!=(const SkSize& a, const SkSize& b) noexcept {
  return !(a == b);
}
#endif
