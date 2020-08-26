/*
 * Copyright 2019 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "modules/skottie/src/text/TextValue.h"

#include "modules/skottie/src/SkottieJson.h"
#include "modules/skottie/src/SkottiePriv.h"
#include "modules/skottie/src/SkottieValue.h"

namespace skottie::internal {

bool Parse(const skjson::Value& jv, const internal::AnimationBuilder& abuilder, TextValue* v) {
  const skjson::ObjectValue* jtxt = jv;
  if (!jtxt) {
    return false;
  }

  const skjson::StringValue* font_name = (*jtxt)["f"];
  const skjson::StringValue* text = (*jtxt)["t"];
  const skjson::NumberValue* text_size = (*jtxt)["s"];
  const skjson::NumberValue* line_height = (*jtxt)["lh"];
  if (!font_name || !text || !text_size || !line_height) {
    return false;
  }

  const auto* font = abuilder.findFont(SkString(font_name->begin(), font_name->size()));
  if (!font) {
    abuilder.log(Logger::Level::kError, nullptr, "Unknown font: \"%s\".", font_name->begin());
    return false;
  }

  v->fText.set(text->begin(), text->size());
  v->fTextSize = **text_size;
  v->fLineHeight = **line_height;
  v->fTypeface = font->fTypeface;
  v->fAscent = font->fAscentPct * -0.01f * v->fTextSize;  // negative ascent per SkFontMetrics

  static constexpr SkTextUtils::Align gAlignMap[] = {
      SkTextUtils::kLeft_Align,   // 'j': 0
      SkTextUtils::kRight_Align,  // 'j': 1
      SkTextUtils::kCenter_Align  // 'j': 2
  };
  v->fHAlign =
      gAlignMap[std::min<size_t>(ParseDefault<size_t>((*jtxt)["j"], 0), SK_ARRAY_COUNT(gAlignMap))];

  // Optional text box size.
  if (const skjson::ArrayValue* jsz = (*jtxt)["sz"]) {
    if (jsz->size() == 2) {
      v->fBox.setWH(ParseDefault<SkScalar>((*jsz)[0], 0), ParseDefault<SkScalar>((*jsz)[1], 0));
    }
  }

  // Optional text box position.
  if (const skjson::ArrayValue* jps = (*jtxt)["ps"]) {
    if (jps->size() == 2) {
      v->fBox.offset(ParseDefault<SkScalar>((*jps)[0], 0), ParseDefault<SkScalar>((*jps)[1], 0));
    }
  }

  static constexpr Shaper::ResizePolicy gResizeMap[] = {
      Shaper::ResizePolicy::kNone,            // 'rs': 0
      Shaper::ResizePolicy::kScaleToFit,      // 'rs': 1
      Shaper::ResizePolicy::kDownscaleToFit,  // 'rs': 2
  };
  // TODO: remove "sk_rs" support after migrating clients.
  v->fResize = gResizeMap[std::min(
      std::max(ParseDefault<size_t>((*jtxt)["rs"], 0), ParseDefault<size_t>((*jtxt)["sk_rs"], 0)),
      SK_ARRAY_COUNT(gResizeMap))];

  // In point mode, the text is baseline-aligned.
  v->fVAlign = v->fBox.isEmpty() ? Shaper::VAlign::kTopBaseline : Shaper::VAlign::kTop;

  static constexpr Shaper::VAlign gVAlignMap[] = {
      Shaper::VAlign::kVisualTop,     // 'vj': 0
      Shaper::VAlign::kVisualCenter,  // 'vj': 1
      Shaper::VAlign::kVisualBottom,  // 'vj': 2
  };
  size_t vj;
  if (skottie::Parse((*jtxt)["vj"], &vj) ||
      skottie::Parse((*jtxt)["sk_vj"], &vj)) {  // TODO: remove after migrating clients.
    if (vj < SK_ARRAY_COUNT(gVAlignMap)) {
      v->fVAlign = gVAlignMap[vj];
    } else {
      // Legacy sk_vj values.
      // TODO: remove after clients update.
      switch (vj) {
        case 3:
          // 'sk_vj': 3 -> kVisualCenter/kScaleToFit
          v->fVAlign = Shaper::VAlign::kVisualCenter;
          v->fResize = Shaper::ResizePolicy::kScaleToFit;
          break;
        case 4:
          // 'sk_vj': 4 -> kVisualCenter/kDownscaleToFit
          v->fVAlign = Shaper::VAlign::kVisualCenter;
          v->fResize = Shaper::ResizePolicy::kDownscaleToFit;
          break;
        default:
          abuilder.log(Logger::Level::kWarning, nullptr, "Ignoring unknown 'vj' value: %zu", vj);
          break;
      }
    }
  }

  if (v->fResize != Shaper::ResizePolicy::kNone && v->fBox.isEmpty()) {
    abuilder.log(Logger::Level::kWarning, jtxt, "Auto-scaled text requires a paragraph box.");
    v->fResize = Shaper::ResizePolicy::kNone;
  }

  const auto& parse_color = [](const skjson::ArrayValue* jcolor, SkColor* c) {
    if (!jcolor) {
      return false;
    }

    VectorValue color_vec;
    if (!skottie::Parse(*jcolor, &color_vec)) {
      return false;
    }

    *c = color_vec;
    return true;
  };

  v->fHasFill = parse_color((*jtxt)["fc"], &v->fFillColor);
  v->fHasStroke = parse_color((*jtxt)["sc"], &v->fStrokeColor);

  if (v->fHasStroke) {
    v->fStrokeWidth = ParseDefault((*jtxt)["s"], 0.0f);
  }

  return true;
}

}  // namespace skottie::internal
