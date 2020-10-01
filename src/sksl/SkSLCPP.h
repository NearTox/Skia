/*
 * Copyright 2017 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef SKSL_CPP
#define SKSL_CPP

// functions used by CPP programs created by skslc

#include <cmath>
#include "include/core/SkPoint.h"
#include "include/core/SkRect.h"

using std::abs;

struct Float4 {
  constexpr Float4(float x, float y, float z, float w) noexcept : fX(x), fY(y), fZ(z), fW(w) {}

  constexpr operator SkRect() const noexcept { return SkRect::MakeLTRB(fX, fY, fZ, fW); }

 private:
  float fX;
  float fY;
  float fZ;
  float fW;
};

// macros to make sk_Caps.<cap name> work from C++ code
#define sk_Caps (*args.fShaderCaps)

#define floatIs32Bits floatIs32Bits()

// functions to make GLSL constructors work from C++ code
inline constexpr SkPoint float2(float xy) noexcept { return SkPoint::Make(xy, xy); }

inline constexpr SkPoint float2(float x, float y) noexcept { return SkPoint::Make(x, y); }

inline constexpr Float4 float4(float xyzw) noexcept { return Float4(xyzw, xyzw, xyzw, xyzw); }

inline constexpr Float4 float4(float x, float y, float z, float w) noexcept {
  return Float4(x, y, z, w);
}

#define half2 float2

#define half3 float3

#define half4 float4

#endif
