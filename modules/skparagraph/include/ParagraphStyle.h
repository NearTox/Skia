// Copyright 2019 Google LLC.
#ifndef ParagraphStyle_DEFINED
#define ParagraphStyle_DEFINED

#include "include/core/SkFontStyle.h"
#include "include/core/SkScalar.h"
#include "include/core/SkString.h"
#include "modules/skparagraph/include/DartTypes.h"
#include "modules/skparagraph/include/TextStyle.h"

#include <stddef.h>
#include <algorithm>
#include <limits>
#include <string>
#include <utility>
#include <vector>

namespace skia {
namespace textlayout {

struct StrutStyle {
  StrutStyle() noexcept;

  const std::vector<SkString>& getFontFamilies() const noexcept { return fFontFamilies; }
  void setFontFamilies(std::vector<SkString> families) noexcept { fFontFamilies = std::move(families); }

  SkFontStyle getFontStyle() const noexcept { return fFontStyle; }
  void setFontStyle(SkFontStyle fontStyle) noexcept { fFontStyle = fontStyle; }

  SkScalar getFontSize() const noexcept { return fFontSize; }
  void setFontSize(SkScalar size) noexcept { fFontSize = size; }

  void setHeight(SkScalar height) noexcept { fHeight = height; }
  SkScalar getHeight() const noexcept { return fHeight; }

  void setLeading(SkScalar Leading) noexcept { fLeading = Leading; }
  SkScalar getLeading() const noexcept { return fLeading; }

  bool getStrutEnabled() const noexcept { return fEnabled; }
  void setStrutEnabled(bool v) noexcept { fEnabled = v; }

  bool getForceStrutHeight() const noexcept { return fForceHeight; }
  void setForceStrutHeight(bool v) noexcept { fForceHeight = v; }

  bool getHeightOverride() const noexcept { return fHeightOverride; }
  void setHeightOverride(bool v) noexcept { fHeightOverride = v; }

  bool operator==(const StrutStyle& rhs) const noexcept {
    return this->fEnabled == rhs.fEnabled && this->fHeightOverride == rhs.fHeightOverride &&
           this->fForceHeight == rhs.fForceHeight && nearlyEqual(this->fLeading, rhs.fLeading) &&
           nearlyEqual(this->fHeight, rhs.fHeight) && nearlyEqual(this->fFontSize, rhs.fFontSize) &&
           this->fFontStyle == rhs.fFontStyle && this->fFontFamilies == rhs.fFontFamilies;
  }

 private:
  std::vector<SkString> fFontFamilies;
  SkFontStyle fFontStyle;
  SkScalar fFontSize;
  SkScalar fHeight;
  SkScalar fLeading;
  bool fForceHeight;
  bool fEnabled;
  bool fHeightOverride;
};

struct ParagraphStyle {
  ParagraphStyle() noexcept;

  bool operator==(const ParagraphStyle& rhs) const noexcept {
    return this->fHeight == rhs.fHeight && this->fEllipsis == rhs.fEllipsis &&
           this->fTextDirection == rhs.fTextDirection && this->fTextAlign == rhs.fTextAlign &&
           this->fDefaultTextStyle == rhs.fDefaultTextStyle;
  }

  const StrutStyle& getStrutStyle() const noexcept { return fStrutStyle; }
  void setStrutStyle(StrutStyle strutStyle) noexcept { fStrutStyle = std::move(strutStyle); }

  const TextStyle& getTextStyle() const noexcept { return fDefaultTextStyle; }
  void setTextStyle(const TextStyle& textStyle) noexcept { fDefaultTextStyle = textStyle; }

  TextDirection getTextDirection() const noexcept { return fTextDirection; }
  void setTextDirection(TextDirection direction) noexcept { fTextDirection = direction; }

  TextAlign getTextAlign() const noexcept { return fTextAlign; }
  void setTextAlign(TextAlign align) noexcept { fTextAlign = align; }

  size_t getMaxLines() const noexcept { return fLinesLimit; }
  void setMaxLines(size_t maxLines) noexcept { fLinesLimit = maxLines; }

  const SkString& getEllipsis() const noexcept { return fEllipsis; }
  void setEllipsis(const std::u16string& ellipsis);
  void setEllipsis(const SkString& ellipsis) noexcept { fEllipsis = ellipsis; }

  SkScalar getHeight() const noexcept { return fHeight; }
  void setHeight(SkScalar height) noexcept { fHeight = height; }

  TextHeightBehavior getTextHeightBehavior() const noexcept { return fTextHeightBehavior; }
  void setTextHeightBehavior(TextHeightBehavior v) noexcept { fTextHeightBehavior = v; }

  bool unlimited_lines() const noexcept { return fLinesLimit == std::numeric_limits<size_t>::max(); }
  bool ellipsized() const noexcept { return fEllipsis.size() != 0; }
  TextAlign effective_align() const noexcept;
  bool hintingIsOn() const noexcept { return fHintingIsOn; }
  void turnHintingOff() noexcept { fHintingIsOn = false; }

 private:
  StrutStyle fStrutStyle;
  TextStyle fDefaultTextStyle;
  TextAlign fTextAlign;
  TextDirection fTextDirection;
  size_t fLinesLimit;
  SkString fEllipsis;
  SkScalar fHeight;
  TextHeightBehavior fTextHeightBehavior;
  bool fHintingIsOn;
};
}  // namespace textlayout
}  // namespace skia

#endif  // ParagraphStyle_DEFINED
