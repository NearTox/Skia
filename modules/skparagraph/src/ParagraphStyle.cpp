// Copyright 2019 Google LLC.

#include "modules/skparagraph/include/DartTypes.h"
#include "modules/skparagraph/include/ParagraphStyle.h"
#include "modules/skparagraph/src/ParagraphUtil.h"

namespace skia {
namespace textlayout {

StrutStyle::StrutStyle() noexcept {
  fFontStyle = SkFontStyle::Normal();
  fFontSize = 14;
  fHeight = 1;
  fLeading = -1;
  fForceHeight = false;
  fHeightOverride = false;
  fEnabled = false;
}

ParagraphStyle::ParagraphStyle() noexcept {
  fTextAlign = TextAlign::kStart;
  fTextDirection = TextDirection::kLtr;
  fLinesLimit = std::numeric_limits<size_t>::max();
  fHeight = 1;
  fTextHeightBehavior = TextHeightBehavior::kAll;
  fHintingIsOn = true;
}

TextAlign ParagraphStyle::effective_align() const noexcept {
  if (fTextAlign == TextAlign::kStart) {
    return (fTextDirection == TextDirection::kLtr) ? TextAlign::kLeft : TextAlign::kRight;
  } else if (fTextAlign == TextAlign::kEnd) {
    return (fTextDirection == TextDirection::kLtr) ? TextAlign::kRight : TextAlign::kLeft;
  } else {
    return fTextAlign;
  }
}

void ParagraphStyle::setEllipsis(const std::u16string& ellipsis) {
  fEllipsis = SkStringFromU16String(ellipsis);
}
}  // namespace textlayout
}  // namespace skia
