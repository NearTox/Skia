// Copyright 2019 Google LLC.
#ifndef TextShadow_DEFINED
#define TextShadow_DEFINED

#include "include/core/SkColor.h"
#include "include/core/SkPoint.h"

namespace skia {
namespace textlayout {

class TextShadow {
 public:
  SkColor fColor = SK_ColorBLACK;
  SkPoint fOffset;
  double fBlurRadius = 0.0;

  TextShadow() noexcept;

  TextShadow(SkColor color, SkPoint offset, double blurRadius) noexcept;

  bool operator==(const TextShadow& other) const noexcept;

  bool operator!=(const TextShadow& other) const noexcept;

  bool hasShadow() const noexcept;
};
}  // namespace textlayout
}  // namespace skia

#endif  // TextShadow_DEFINED
