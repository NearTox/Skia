/*
 * Copyright 2013 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef SkFontStyle_DEFINED
#define SkFontStyle_DEFINED

#include "include/core/SkTypes.h"

class SK_API SkFontStyle {
 public:
  enum Weight {
    kInvisible_Weight = 0,
    kThin_Weight = 100,
    kExtraLight_Weight = 200,
    kLight_Weight = 300,
    kNormal_Weight = 400,
    kMedium_Weight = 500,
    kSemiBold_Weight = 600,
    kBold_Weight = 700,
    kExtraBold_Weight = 800,
    kBlack_Weight = 900,
    kExtraBlack_Weight = 1000,
  };

  enum Width {
    kUltraCondensed_Width = 1,
    kExtraCondensed_Width = 2,
    kCondensed_Width = 3,
    kSemiCondensed_Width = 4,
    kNormal_Width = 5,
    kSemiExpanded_Width = 6,
    kExpanded_Width = 7,
    kExtraExpanded_Width = 8,
    kUltraExpanded_Width = 9,
  };

  enum Slant {
    kUpright_Slant,
    kItalic_Slant,
    kOblique_Slant,
  };

  constexpr SkFontStyle(int weight, int width, Slant slant) noexcept
      : fValue(
            (SkTPin<int>(weight, kInvisible_Weight, kExtraBlack_Weight)) +
            (SkTPin<int>(width, kUltraCondensed_Width, kUltraExpanded_Width) << 16) +
            (SkTPin<int>(slant, kUpright_Slant, kOblique_Slant) << 24)) {}

  constexpr SkFontStyle() noexcept : SkFontStyle{kNormal_Weight, kNormal_Width, kUpright_Slant} {}

  constexpr bool operator==(const SkFontStyle& rhs) const noexcept { return fValue == rhs.fValue; }

  constexpr int weight() const noexcept { return fValue & 0xFFFF; }
  constexpr int width() const noexcept { return (fValue >> 16) & 0xFF; }
  constexpr Slant slant() const noexcept { return (Slant)((fValue >> 24) & 0xFF); }

  static constexpr SkFontStyle Normal() noexcept {
    return SkFontStyle(kNormal_Weight, kNormal_Width, kUpright_Slant);
  }
  static constexpr SkFontStyle Bold() noexcept {
    return SkFontStyle(kBold_Weight, kNormal_Width, kUpright_Slant);
  }
  static constexpr SkFontStyle Italic() noexcept {
    return SkFontStyle(kNormal_Weight, kNormal_Width, kItalic_Slant);
  }
  static constexpr SkFontStyle BoldItalic() noexcept {
    return SkFontStyle(kBold_Weight, kNormal_Width, kItalic_Slant);
  }

 private:
  int32_t fValue;
};

#endif
