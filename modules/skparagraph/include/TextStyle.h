// Copyright 2019 Google LLC.
#ifndef TextStyle_DEFINED
#define TextStyle_DEFINED

#include <vector>
#include "include/core/SkColor.h"
#include "include/core/SkFont.h"
#include "include/core/SkFontMetrics.h"
#include "include/core/SkFontStyle.h"
#include "include/core/SkPaint.h"
#include "include/core/SkScalar.h"
#include "modules/skparagraph/include/DartTypes.h"
#include "modules/skparagraph/include/TextShadow.h"

// TODO: Make it external so the other platforms (Android) could use it
#define DEFAULT_FONT_FAMILY "sans-serif"

namespace skia {
namespace textlayout {

static inline bool nearlyZero(SkScalar x, SkScalar tolerance = SK_ScalarNearlyZero) noexcept {
  if (SkScalarIsFinite(x)) {
    return SkScalarNearlyZero(x, tolerance);
  }
  return false;
}

static inline bool nearlyEqual(
    SkScalar x, SkScalar y, SkScalar tolerance = SK_ScalarNearlyZero) noexcept {
  if (SkScalarIsFinite(x) && SkScalarIsFinite(x)) {
    return SkScalarNearlyEqual(x, y, tolerance);
  }
  // Inf == Inf, anything else is false
  return x == y;
}

// Multiple decorations can be applied at once. Ex: Underline and overline is
// (0x1 | 0x2)
enum TextDecoration {
  kNoDecoration = 0x0,
  kUnderline = 0x1,
  kOverline = 0x2,
  kLineThrough = 0x4,
};
constexpr TextDecoration AllTextDecorations[] = {
    kNoDecoration,
    kUnderline,
    kOverline,
    kLineThrough,
};

enum TextDecorationStyle { kSolid, kDouble, kDotted, kDashed, kWavy };

enum TextDecorationMode { kGaps, kThrough };

enum StyleType {
  kNone,
  kAllAttributes,
  kFont,
  kForeground,
  kBackground,
  kShadow,
  kDecorations,
  kLetterSpacing,
  kWordSpacing
};

struct Decoration {
  TextDecoration fType;
  TextDecorationMode fMode;
  SkColor fColor;
  TextDecorationStyle fStyle;
  SkScalar fThicknessMultiplier;

  bool operator==(const Decoration& other) const noexcept {
    return this->fType == other.fType && this->fMode == other.fMode &&
           this->fColor == other.fColor && this->fStyle == other.fStyle &&
           this->fThicknessMultiplier == other.fThicknessMultiplier;
  }
};

/// Where to vertically align the placeholder relative to the surrounding text.
enum class PlaceholderAlignment {
  /// Match the baseline of the placeholder with the baseline.
  kBaseline,

  /// Align the bottom edge of the placeholder with the baseline such that the
  /// placeholder sits on top of the baseline.
  kAboveBaseline,

  /// Align the top edge of the placeholder with the baseline specified in
  /// such that the placeholder hangs below the baseline.
  kBelowBaseline,

  /// Align the top edge of the placeholder with the top edge of the font.
  /// When the placeholder is very tall, the extra space will hang from
  /// the top and extend through the bottom of the line.
  kTop,

  /// Align the bottom edge of the placeholder with the top edge of the font.
  /// When the placeholder is very tall, the extra space will rise from
  /// the bottom and extend through the top of the line.
  kBottom,

  /// Align the middle of the placeholder with the middle of the text. When the
  /// placeholder is very tall, the extra space will grow equally from
  /// the top and bottom of the line.
  kMiddle,
};

struct FontFeature {
  FontFeature(const SkString name, int value) noexcept : fName(name), fValue(value) {}
  bool operator==(const FontFeature& that) const noexcept {
    return fName == that.fName && fValue == that.fValue;
  }
  SkString fName;
  int fValue;
};

struct PlaceholderStyle {
  constexpr PlaceholderStyle() noexcept = default;
  constexpr PlaceholderStyle(
      SkScalar width, SkScalar height, PlaceholderAlignment alignment, TextBaseline baseline,
      SkScalar offset) noexcept
      : fWidth(width),
        fHeight(height),
        fAlignment(alignment),
        fBaseline(baseline),
        fBaselineOffset(offset) {}

  bool equals(const PlaceholderStyle&) const noexcept;

  SkScalar fWidth = 0;
  SkScalar fHeight = 0;
  PlaceholderAlignment fAlignment = PlaceholderAlignment::kBaseline;
  TextBaseline fBaseline = TextBaseline::kAlphabetic;
  // Distance from the top edge of the rect to the baseline position. This
  // baseline will be aligned against the alphabetic baseline of the surrounding
  // text.
  //
  // Positive values drop the baseline lower (positions the rect higher) and
  // small or negative values will cause the rect to be positioned underneath
  // the line. When baseline == height, the bottom edge of the rect will rest on
  // the alphabetic baseline.
  SkScalar fBaselineOffset = 0;
};

class TextStyle {
 public:
  TextStyle() noexcept = default;
  TextStyle(const TextStyle& other, bool placeholder);

  bool equals(const TextStyle& other) const noexcept;
  bool equalsByFonts(const TextStyle& that) const noexcept;
  bool matchOneAttribute(StyleType styleType, const TextStyle& other) const noexcept;
  bool operator==(const TextStyle& rhs) const noexcept { return this->equals(rhs); }

  // Colors
  SkColor getColor() const noexcept { return fColor; }
  void setColor(SkColor color) noexcept { fColor = color; }

  bool hasForeground() const noexcept { return fHasForeground; }
  SkPaint getForeground() const noexcept { return fForeground; }
  void setForegroundColor(SkPaint paint) noexcept {
    fHasForeground = true;
    fForeground = std::move(paint);
  }
  void clearForegroundColor() noexcept { fHasForeground = false; }

  bool hasBackground() const noexcept { return fHasBackground; }
  SkPaint getBackground() const noexcept { return fBackground; }
  void setBackgroundColor(SkPaint paint) noexcept {
    fHasBackground = true;
    fBackground = std::move(paint);
  }
  void clearBackgroundColor() noexcept { fHasBackground = false; }

  // Decorations
  Decoration getDecoration() const noexcept { return fDecoration; }
  TextDecoration getDecorationType() const noexcept { return fDecoration.fType; }
  TextDecorationMode getDecorationMode() const noexcept { return fDecoration.fMode; }
  SkColor getDecorationColor() const noexcept { return fDecoration.fColor; }
  TextDecorationStyle getDecorationStyle() const noexcept { return fDecoration.fStyle; }
  SkScalar getDecorationThicknessMultiplier() const noexcept {
    return fDecoration.fThicknessMultiplier;
  }
  void setDecoration(TextDecoration decoration) noexcept { fDecoration.fType = decoration; }
  void setDecorationMode(TextDecorationMode mode) noexcept { fDecoration.fMode = mode; }
  void setDecorationStyle(TextDecorationStyle style) noexcept { fDecoration.fStyle = style; }
  void setDecorationColor(SkColor color) noexcept { fDecoration.fColor = color; }
  void setDecorationThicknessMultiplier(SkScalar m) noexcept {
    fDecoration.fThicknessMultiplier = m;
  }

  // Weight/Width/Slant
  SkFontStyle getFontStyle() const noexcept { return fFontStyle; }
  void setFontStyle(SkFontStyle fontStyle) noexcept { fFontStyle = fontStyle; }

  // Shadows
  size_t getShadowNumber() const noexcept { return fTextShadows.size(); }
  const std::vector<TextShadow>& getShadows() const noexcept { return fTextShadows; }
  void addShadow(TextShadow shadow) { fTextShadows.emplace_back(shadow); }
  void resetShadows() noexcept { fTextShadows.clear(); }

  // Font features
  size_t getFontFeatureNumber() const noexcept { return fFontFeatures.size(); }
  const std::vector<FontFeature>& getFontFeatures() const noexcept { return fFontFeatures; }
  void addFontFeature(const SkString& fontFeature, int value) {
    fFontFeatures.emplace_back(fontFeature, value);
  }
  void resetFontFeatures() noexcept { fFontFeatures.clear(); }

  SkScalar getFontSize() const noexcept { return fFontSize; }
  void setFontSize(SkScalar size) noexcept { fFontSize = size; }

  const std::vector<SkString>& getFontFamilies() const noexcept { return fFontFamilies; }
  void setFontFamilies(std::vector<SkString> families) noexcept {
    fFontFamilies = std::move(families);
  }

  void setHeight(SkScalar height) noexcept { fHeight = height; }
  SkScalar getHeight() const noexcept { return fHeightOverride ? fHeight : 0; }

  void setHeightOverride(bool heightOverride) noexcept { fHeightOverride = heightOverride; }
  bool getHeightOverride() const noexcept { return fHeightOverride; }

  void setLetterSpacing(SkScalar letterSpacing) noexcept { fLetterSpacing = letterSpacing; }
  SkScalar getLetterSpacing() const noexcept { return fLetterSpacing; }

  void setWordSpacing(SkScalar wordSpacing) noexcept { fWordSpacing = wordSpacing; }
  SkScalar getWordSpacing() const noexcept { return fWordSpacing; }

  SkTypeface* getTypeface() const noexcept { return fTypeface.get(); }
  sk_sp<SkTypeface> refTypeface() const noexcept { return fTypeface; }
  void setTypeface(sk_sp<SkTypeface> typeface) noexcept { fTypeface = std::move(typeface); }

  SkString getLocale() const noexcept { return fLocale; }
  void setLocale(const SkString& locale) noexcept { fLocale = locale; }

  TextBaseline getTextBaseline() const noexcept { return fTextBaseline; }
  void setTextBaseline(TextBaseline baseline) noexcept { fTextBaseline = baseline; }

  void getFontMetrics(SkFontMetrics* metrics) const;

  bool isPlaceholder() const noexcept { return fIsPlaceholder; }
  void setPlaceholder() noexcept { fIsPlaceholder = true; }

 private:
  Decoration fDecoration = {
      TextDecoration::kNoDecoration,
      // TODO: switch back to kGaps when (if) switching flutter to skparagraph
      TextDecorationMode::kThrough,
      // It does not make sense to draw a transparent object, so we use this as a default
      // value to indicate no decoration color was set.
      SK_ColorTRANSPARENT, TextDecorationStyle::kSolid,
      // Thickness is applied as a multiplier to the default thickness of the font.
      1.0f};

  SkFontStyle fFontStyle;

  std::vector<SkString> fFontFamilies = {SkString(DEFAULT_FONT_FAMILY)};
  SkScalar fFontSize = 14.0;
  SkScalar fHeight = 1.0;
  bool fHeightOverride = false;
  SkString fLocale = {};
  SkScalar fLetterSpacing = 0.0;
  SkScalar fWordSpacing = 0.0;

  TextBaseline fTextBaseline = TextBaseline::kAlphabetic;

  SkColor fColor = SK_ColorWHITE;
  bool fHasBackground = false;
  SkPaint fBackground;
  bool fHasForeground = false;
  SkPaint fForeground;

  std::vector<TextShadow> fTextShadows;

  sk_sp<SkTypeface> fTypeface;
  bool fIsPlaceholder = false;

  std::vector<FontFeature> fFontFeatures;
};

typedef size_t TextIndex;
typedef SkRange<size_t> TextRange;
constexpr SkRange<size_t> EMPTY_TEXT = EMPTY_RANGE;

struct Block {
  Block() = default;
  Block(size_t start, size_t end, const TextStyle& style) : fRange(start, end), fStyle(style) {}
  Block(TextRange textRange, const TextStyle& style) : fRange(textRange), fStyle(style) {}

  void add(TextRange tail) noexcept {
    SkASSERT(fRange.end == tail.start);
    fRange = TextRange(fRange.start, fRange.start + fRange.width() + tail.width());
  }

  TextRange fRange = EMPTY_RANGE;
  TextStyle fStyle;
};

typedef size_t BlockIndex;
typedef SkRange<size_t> BlockRange;
constexpr size_t EMPTY_BLOCK = EMPTY_INDEX;
constexpr SkRange<size_t> EMPTY_BLOCKS = EMPTY_RANGE;

struct Placeholder {
  Placeholder() = default;
  Placeholder(
      size_t start, size_t end, const PlaceholderStyle& style, const TextStyle& textStyle,
      BlockRange blocksBefore, TextRange textBefore)
      : fRange(start, end),
        fStyle(style),
        fTextStyle(textStyle),
        fBlocksBefore(blocksBefore),
        fTextBefore(textBefore) {}

  TextRange fRange = EMPTY_RANGE;
  PlaceholderStyle fStyle;
  TextStyle fTextStyle;
  BlockRange fBlocksBefore;
  TextRange fTextBefore;
};

}  // namespace textlayout
}  // namespace skia

#endif  // TextStyle_DEFINED
