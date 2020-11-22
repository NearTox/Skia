/*
 * Copyright 2020 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */
#include "include/core/SkString.h"
#include "include/private/SkTFitsIn.h"
#include "include/private/SkTemplates.h"
#include "modules/skshaper/src/SkUnicode.h"
#include "src/utils/SkUTF.h"
#include <unicode/ubidi.h>
#include <unicode/ubrk.h>
#include <unicode/ustring.h>
#include <unicode/utext.h>
#include <unicode/utypes.h>
#include <vector>
#include <functional>

#if defined(SK_USING_THIRD_PARTY_ICU)
#  include "SkLoadICU.h"
#endif

using SkUnicodeBidi = std::unique_ptr<UBiDi, SkFunctionWrapper<decltype(ubidi_close), ubidi_close>>;
using ICUUText = std::unique_ptr<UText, SkFunctionWrapper<decltype(utext_close), utext_close>>;
using ICUBreakIterator =
    std::unique_ptr<UBreakIterator, SkFunctionWrapper<decltype(ubrk_close), ubrk_close>>;

/** Replaces invalid utf-8 sequences with REPLACEMENT CHARACTER U+FFFD. */
static inline SkUnichar utf8_next(const char** ptr, const char* end) {
  SkUnichar val = SkUTF::NextUTF8(ptr, end);
  return val < 0 ? 0xFFFD : val;
}

class SkBidiIterator_icu : public SkBidiIterator {
  SkUnicodeBidi fBidi;

 public:
  explicit SkBidiIterator_icu(SkUnicodeBidi bidi) : fBidi(std::move(bidi)) {}
  Position getLength() override { return ubidi_getLength(fBidi.get()); }
  Level getLevelAt(Position pos) override { return ubidi_getLevelAt(fBidi.get(), pos); }

  static std::unique_ptr<SkBidiIterator> makeBidiIterator(
      const uint16_t utf16[], int utf16Units, Direction dir) {
    UErrorCode status = U_ZERO_ERROR;
    SkUnicodeBidi bidi(ubidi_openSized(utf16Units, 0, &status));
    if (U_FAILURE(status)) {
      SkDEBUGF("Bidi error: %s", u_errorName(status));
      return nullptr;
    }
    SkASSERT(bidi);
    uint8_t bidiLevel = (dir == SkBidiIterator::kLTR) ? UBIDI_LTR : UBIDI_RTL;
    // The required lifetime of utf16 isn't well documented.
    // It appears it isn't used after ubidi_setPara except through ubidi_getText.
    ubidi_setPara(bidi.get(), (const UChar*)utf16, utf16Units, bidiLevel, nullptr, &status);
    if (U_FAILURE(status)) {
      SkDEBUGF("Bidi error: %s", u_errorName(status));
      return nullptr;
    }
    return std::unique_ptr<SkBidiIterator>(new SkBidiIterator_icu(std::move(bidi)));
  }

  // ICU bidi iterator works with utf16 but clients (Flutter for instance) may work with utf8
  // This method allows the clients not to think about all these details
  static std::unique_ptr<SkBidiIterator> makeBidiIterator(
      const char utf8[], int utf8Units, Direction dir) {
    // Convert utf8 into utf16 since ubidi only accepts utf16
    if (!SkTFitsIn<int32_t>(utf8Units)) {
      SkDEBUGF("Bidi error: text too long");
      return nullptr;
    }

    // Getting the length like this seems to always set U_BUFFER_OVERFLOW_ERROR
    int utf16Units = SkUTF::UTF8ToUTF16(nullptr, 0, utf8, utf8Units);
    if (utf16Units < 0) {
      SkDEBUGF("Bidi error: Invalid utf8 input");
      return nullptr;
    }
    std::unique_ptr<uint16_t[]> utf16(new uint16_t[utf16Units]);
    SkDEBUGCODE(int dstLen =)
    SkUTF::UTF8ToUTF16(utf16.get(), utf16Units, utf8, utf8Units);
    SkASSERT(dstLen == utf16Units);

    return makeBidiIterator(utf16.get(), utf16Units, dir);
  }

  // This method returns the final results only: a list of bidi regions
  // (this is all SkParagraph really needs; SkShaper however uses the iterator itself)
  static std::vector<Region> getBidiRegions(const char utf8[], int utf8Units, Direction dir) {
    auto bidiIterator = makeBidiIterator(utf8, utf8Units, dir);
    std::vector<Region> bidiRegions;
    const char* start8 = utf8;
    const char* end8 = utf8 + utf8Units;
    SkBidiIterator::Level currentLevel = 0;

    Position pos8 = 0;
    Position pos16 = 0;
    Position end16 = bidiIterator->getLength();
    while (pos16 < end16) {
      auto level = bidiIterator->getLevelAt(pos16);
      if (pos16 == 0) {
        currentLevel = level;
      } else if (level != currentLevel) {
        auto end = start8 - utf8;
        bidiRegions.emplace_back(pos8, end, currentLevel);
        currentLevel = level;
        pos8 = end;
      }
      SkUnichar u = utf8_next(&start8, end8);
      pos16 += SkUTF::ToUTF16(u);
    }
    auto end = start8 - utf8;
    if (end != pos8) {
      bidiRegions.emplace_back(pos8, end, currentLevel);
    }
    return bidiRegions;
  }
};

void SkBidiIterator::ReorderVisual(
    const Level runLevels[], int levelsCount, int32_t logicalFromVisual[]) {
  ubidi_reorderVisual(runLevels, levelsCount, logicalFromVisual);
}

class SkBreakIterator_icu : public SkBreakIterator {
  ICUBreakIterator fBreakIterator;
  Position fLastResult;

 public:
  explicit SkBreakIterator_icu(ICUBreakIterator iter)
      : fBreakIterator(std::move(iter)), fLastResult(0) {}
  Position first() override { return fLastResult = ubrk_first(fBreakIterator.get()); }
  Position current() override { return fLastResult = ubrk_current(fBreakIterator.get()); }
  Position next() override { return fLastResult = ubrk_next(fBreakIterator.get()); }
  Position preceding(Position offset) override {
    return fLastResult = ubrk_preceding(fBreakIterator.get(), offset);
  }
  Position following(Position offset) override {
    return fLastResult = ubrk_following(fBreakIterator.get(), offset);
  }
  Status status() override { return ubrk_getRuleStatus(fBreakIterator.get()); }
  bool isDone() override { return fLastResult == UBRK_DONE; }

  bool setText(const char utftext8[], int utf8Units) override {
    UErrorCode status = U_ZERO_ERROR;

    UText sUtf8UText = UTEXT_INITIALIZER;
    ICUUText text(utext_openUTF8(&sUtf8UText, &utftext8[0], utf8Units, &status));

    if (U_FAILURE(status)) {
      SkDEBUGF("Break error: %s", u_errorName(status));
      return false;
    }
    SkASSERT(text);
    ubrk_setUText(fBreakIterator.get(), text.get(), &status);
    if (U_FAILURE(status)) {
      SkDEBUGF("Break error: %s", u_errorName(status));
      return false;
    }
    fLastResult = 0;
    return true;
  }

  static UBreakIteratorType convertType(SkUnicode::BreakType type) {
    switch (type) {
      case SkUnicode::BreakType::kLines: return UBRK_LINE;
      case SkUnicode::BreakType::kGraphemes: return UBRK_CHARACTER;
      case SkUnicode::BreakType::kWords: return UBRK_WORD;
      default: return UBRK_CHARACTER;
    }
  }

  static std::unique_ptr<SkBreakIterator> makeUtf8BreakIterator(
      const char locale[], SkUnicode::BreakType type) {
    UErrorCode status = U_ZERO_ERROR;
    ICUBreakIterator iterator(ubrk_open(convertType(type), locale, nullptr, 0, &status));
    if (U_FAILURE(status)) {
      SkDEBUGF("Break error: %s", u_errorName(status));
      return nullptr;
    }
    return std::unique_ptr<SkBreakIterator>(new SkBreakIterator_icu(std::move(iterator)));
  }
};

class SkUnicode_icu : public SkUnicode {
  static UBreakIteratorType convertType(BreakType type) {
    switch (type) {
      case BreakType::kLines: return UBRK_LINE;
      case BreakType::kGraphemes: return UBRK_CHARACTER;
      case BreakType::kWords: return UBRK_WORD;
      default: SkDEBUGF("Convert error: wrong break type"); return UBRK_CHARACTER;
    }
  }

  static bool extractBidi(
      const char utf8[], int utf8Units, TextDirection dir, std::vector<BidiRegion>* bidiRegions) {
    // Convert to UTF16 since for now bidi iterator only operates on utf16
    std::unique_ptr<uint16_t[]> utf16;
    auto utf16Units = utf8ToUtf16(utf8, utf8Units, &utf16);
    if (utf16Units < 0) {
      return false;
    }

    // Create bidi iterator
    UErrorCode status = U_ZERO_ERROR;
    SkUnicodeBidi bidi(ubidi_openSized(utf16Units, 0, &status));
    if (U_FAILURE(status)) {
      SkDEBUGF("Bidi error: %s", u_errorName(status));
      return false;
    }
    SkASSERT(bidi);
    uint8_t bidiLevel = (dir == TextDirection::kLTR) ? UBIDI_LTR : UBIDI_RTL;
    // The required lifetime of utf16 isn't well documented.
    // It appears it isn't used after ubidi_setPara except through ubidi_getText.
    ubidi_setPara(bidi.get(), (const UChar*)utf16.get(), utf16Units, bidiLevel, nullptr, &status);
    if (U_FAILURE(status)) {
      SkDEBUGF("Bidi error: %s", u_errorName(status));
      return false;
    }

    // Iterate through bidi regions and the result positions into utf8
    const char* start8 = utf8;
    const char* end8 = utf8 + utf8Units;
    BidiLevel currentLevel = 0;

    Position pos8 = 0;
    Position pos16 = 0;
    Position end16 = ubidi_getLength(bidi.get());
    while (pos16 < end16) {
      auto level = ubidi_getLevelAt(bidi.get(), pos16);
      if (pos16 == 0) {
        currentLevel = level;
      } else if (level != currentLevel) {
        Position end = start8 - utf8;
        bidiRegions->emplace_back(pos8, end, currentLevel);
        currentLevel = level;
        pos8 = end;
      }
      SkUnichar u = utf8_next(&start8, end8);
      pos16 += SkUTF::ToUTF16(u);
    }
    Position end = start8 - utf8;
    if (end != pos8) {
      bidiRegions->emplace_back(pos8, end, currentLevel);
    }
    return true;
  }

  static bool extractWords(uint16_t utf16[], int utf16Units, std::vector<Position>* words) {
    UErrorCode status = U_ZERO_ERROR;

    UBreakIteratorType breakType = convertType(BreakType::kWords);
    ICUBreakIterator iterator(ubrk_open(breakType, uloc_getDefault(), nullptr, 0, &status));
    if (U_FAILURE(status)) {
      SkDEBUGF("Break error: %s", u_errorName(status));
      return false;
    }
    SkASSERT(iterator);

    UText sUtf16UText = UTEXT_INITIALIZER;
    ICUUText utf16UText(utext_openUChars(&sUtf16UText, (UChar*)utf16, utf16Units, &status));
    if (U_FAILURE(status)) {
      SkDEBUGF("Break error: %s", u_errorName(status));
      return false;
    }

    ubrk_setUText(iterator.get(), utf16UText.get(), &status);
    if (U_FAILURE(status)) {
      SkDEBUGF("Break error: %s", u_errorName(status));
      return false;
    }

    // Get the words
    int32_t pos = ubrk_first(iterator.get());
    while (pos != UBRK_DONE) {
      words->emplace_back(pos);
      pos = ubrk_next(iterator.get());
    }

    return true;
  }

  static bool extractPositions(
      const char utf8[], int utf8Units, BreakType type, std::function<void(int, int)> add) {
    UErrorCode status = U_ZERO_ERROR;
    UText sUtf8UText = UTEXT_INITIALIZER;
    ICUUText text(utext_openUTF8(&sUtf8UText, &utf8[0], utf8Units, &status));

    if (U_FAILURE(status)) {
      SkDEBUGF("Break error: %s", u_errorName(status));
      return false;
    }
    SkASSERT(text);

    ICUBreakIterator iterator(ubrk_open(convertType(type), uloc_getDefault(), nullptr, 0, &status));
    if (U_FAILURE(status)) {
      SkDEBUGF("Break error: %s", u_errorName(status));
    }

    ubrk_setUText(iterator.get(), text.get(), &status);
    if (U_FAILURE(status)) {
      SkDEBUGF("Break error: %s", u_errorName(status));
      return false;
    }

    auto iter = iterator.get();
    int32_t pos = ubrk_first(iter);
    while (pos != UBRK_DONE) {
      add(pos, ubrk_getRuleStatus(iter));
      pos = ubrk_next(iter);
    }
    return true;
  }

  static bool extractWhitespaces(
      const char utf8[], int utf8Units, std::vector<Position>* whitespaces) {
    const char* start = utf8;
    const char* end = utf8 + utf8Units;
    const char* ch = start;
    while (ch < end) {
      auto index = ch - start;
      auto unichar = utf8_next(&ch, end);
      if (u_isWhitespace(unichar)) {
        auto ending = ch - start;
        for (auto k = index; k < ending; ++k) {
          whitespaces->emplace_back(k);
        }
      }
    }
    return true;
  }

  static int utf8ToUtf16(const char* utf8, size_t utf8Units, std::unique_ptr<uint16_t[]>* utf16) {
    int utf16Units = SkUTF::UTF8ToUTF16(nullptr, 0, utf8, utf8Units);
    if (utf16Units < 0) {
      SkDEBUGF("Convert error: Invalid utf8 input");
      return utf16Units;
    }
    *utf16 = std::unique_ptr<uint16_t[]>(new uint16_t[utf16Units]);
    SkDEBUGCODE(int dstLen =)
    SkUTF::UTF8ToUTF16(utf16->get(), utf16Units, utf8, utf8Units);
    SkASSERT(dstLen == utf16Units);
    return utf16Units;
  }

  static int utf16ToUtf8(const uint16_t* utf16, size_t utf16Units, std::unique_ptr<char[]>* utf8) {
    int utf8Units = SkUTF::UTF16ToUTF8(nullptr, 0, utf16, utf16Units);
    if (utf8Units < 0) {
      SkDEBUGF("Convert error: Invalid utf16 input");
      return utf8Units;
    }
    *utf8 = std::unique_ptr<char[]>(new char[utf8Units]);
    SkDEBUGCODE(int dstLen =)
    SkUTF::UTF16ToUTF8(utf8->get(), utf8Units, utf16, utf16Units);
    SkASSERT(dstLen == utf8Units);
    return utf8Units;
  }

 public:
  ~SkUnicode_icu() override {}
  std::unique_ptr<SkBidiIterator> makeBidiIterator(
      const uint16_t text[], int count, SkBidiIterator::Direction dir) override {
    return SkBidiIterator_icu::makeBidiIterator(text, count, dir);
  }
  std::unique_ptr<SkBidiIterator> makeBidiIterator(
      const char text[], int count, SkBidiIterator::Direction dir) override {
    return SkBidiIterator_icu::makeBidiIterator(text, count, dir);
  }
  std::unique_ptr<SkBreakIterator> makeBreakIterator(
      const char locale[], BreakType breakType) override {
    return SkBreakIterator_icu::makeUtf8BreakIterator(locale, breakType);
  }

  // TODO: Use ICU data file to detect controls and whitespaces
  bool isControl(SkUnichar utf8) override { return u_iscntrl(utf8); }

  bool isWhitespace(SkUnichar utf8) override { return u_isWhitespace(utf8); }

  SkString convertUtf16ToUtf8(const std::u16string& utf16) override {
    std::unique_ptr<char[]> utf8;
    auto utf8Units = SkUnicode_icu::utf16ToUtf8((uint16_t*)utf16.data(), utf16.size(), &utf8);
    if (utf8Units >= 0) {
      return SkString(utf8.get(), utf8Units);
    } else {
      return SkString();
    }
  }

  bool getBidiRegions(
      const char utf8[], int utf8Units, TextDirection dir,
      std::vector<BidiRegion>* results) override {
    return extractBidi(utf8, utf8Units, dir, results);
  }

  bool getLineBreaks(
      const char utf8[], int utf8Units, std::vector<LineBreakBefore>* results) override {
    return extractPositions(utf8, utf8Units, BreakType::kLines, [results](int pos, int status) {
      results->emplace_back(
          pos,
          status == UBRK_LINE_HARD ? LineBreakType::kHardLineBreak : LineBreakType::kSoftLineBreak);
    });
  }

  bool getWords(const char utf8[], int utf8Units, std::vector<Position>* results) override {
    // Convert to UTF16 since we want the results in utf16
    std::unique_ptr<uint16_t[]> utf16;
    auto utf16Units = utf8ToUtf16(utf8, utf8Units, &utf16);
    if (utf16Units < 0) {
      return false;
    }

    return extractWords(utf16.get(), utf16Units, results);
  }

  bool getGraphemes(const char utf8[], int utf8Units, std::vector<Position>* results) override {
    return extractPositions(utf8, utf8Units, BreakType::kGraphemes, [results](int pos, int status) {
      results->emplace_back(pos);
    });
  }

  bool getWhitespaces(const char utf8[], int utf8Units, std::vector<Position>* results) override {
    return extractWhitespaces(utf8, utf8Units, results);
  }

  void reorderVisual(
      const BidiLevel runLevels[], int levelsCount, int32_t logicalFromVisual[]) override {
    ubidi_reorderVisual(runLevels, levelsCount, logicalFromVisual);
  }
};

std::unique_ptr<SkUnicode> SkUnicode::Make() {
#if defined(SK_USING_THIRD_PARTY_ICU)
  if (!SkLoadICU()) {
    SkDEBUGF("SkLoadICU() failed!\n");
    return nullptr;
  }
#endif
  return std::make_unique<SkUnicode_icu>();
}
