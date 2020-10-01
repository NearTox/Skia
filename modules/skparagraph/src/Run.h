// Copyright 2019 Google LLC.
#ifndef Run_DEFINED
#define Run_DEFINED

#include "include/core/SkFont.h"
#include "include/core/SkFontMetrics.h"
#include "include/core/SkPoint.h"
#include "include/core/SkRect.h"
#include "include/core/SkScalar.h"
#include "include/core/SkTypes.h"
#include "include/private/SkTArray.h"
#include "modules/skparagraph/include/DartTypes.h"
#include "modules/skparagraph/include/TextStyle.h"
#include "modules/skshaper/include/SkShaper.h"
#include "src/core/SkSpan.h"

#include <math.h>
#include <algorithm>
#include <functional>
#include <limits>
#include <tuple>

class SkTextBlobBuilder;

namespace skia {
namespace textlayout {

class Cluster;
class InternalLineMetrics;
class ParagraphImpl;

typedef size_t RunIndex;
constexpr size_t EMPTY_RUN = EMPTY_INDEX;

typedef size_t ClusterIndex;
typedef SkRange<size_t> ClusterRange;
constexpr size_t EMPTY_CLUSTER = EMPTY_INDEX;
constexpr SkRange<size_t> EMPTY_CLUSTERS = EMPTY_RANGE;

typedef size_t GraphemeIndex;
typedef SkRange<GraphemeIndex> GraphemeRange;

typedef size_t GlyphIndex;
typedef SkRange<GlyphIndex> GlyphRange;

// LTR: [start: end) where start <= end
// RTL: [end: start) where start >= end
class DirText {
  DirText(bool dir, size_t s, size_t e) noexcept : start(s), end(e) {}
  bool isLeftToRight() const noexcept { return start <= end; }
  size_t start;
  size_t end;
};

class Run {
 public:
  Run(ParagraphImpl* owner, const SkShaper::RunHandler::RunInfo& info, size_t firstChar,
      SkScalar heightMultiplier, size_t index, SkScalar shiftX);
  Run(const Run&) = default;
  Run& operator=(const Run&) = delete;
  Run(Run&&) noexcept = default;
  Run& operator=(Run&&) = delete;
  ~Run() = default;

  void setOwner(ParagraphImpl* owner) noexcept { fOwner = owner; }

  SkShaper::RunHandler::Buffer newRunBuffer() noexcept;

  SkScalar posX(size_t index) const noexcept { return fPositions[index].fX; }
  void addX(size_t index, SkScalar shift) noexcept { fPositions[index].fX += shift; }
  SkScalar posY(size_t index) const noexcept { return fPositions[index].fY; }
  size_t size() const noexcept { return fGlyphs.size(); }
  void setWidth(SkScalar width) noexcept { fAdvance.fX = width; }
  void setHeight(SkScalar height) noexcept { fAdvance.fY = height; }
  void shift(SkScalar shiftX, SkScalar shiftY) noexcept {
    fOffset.fX += shiftX;
    fOffset.fY += shiftY;
  }
  SkVector advance() const noexcept {
    return SkVector::Make(
        fAdvance.fX, fFontMetrics.fDescent - fFontMetrics.fAscent + fFontMetrics.fLeading);
  }
  SkVector offset() const noexcept { return fOffset; }
  SkScalar ascent() const noexcept { return fFontMetrics.fAscent; }
  SkScalar descent() const noexcept { return fFontMetrics.fDescent; }
  SkScalar leading() const noexcept { return fFontMetrics.fLeading; }
  SkScalar correctAscent() const noexcept { return fCorrectAscent; }
  SkScalar correctDescent() const noexcept { return fCorrectDescent; }
  SkScalar correctLeading() const noexcept { return fCorrectLeading; }
  const SkFont& font() const noexcept { return fFont; }
  bool leftToRight() const noexcept { return fBidiLevel % 2 == 0; }
  TextDirection getTextDirection() const noexcept {
    return leftToRight() ? TextDirection::kLtr : TextDirection::kRtl;
  }
  size_t index() const noexcept { return fIndex; }
  SkScalar heightMultiplier() const noexcept { return fHeightMultiplier; }
  PlaceholderStyle* placeholderStyle() const noexcept;
  bool isPlaceholder() const noexcept {
    return fPlaceholderIndex != std::numeric_limits<size_t>::max();
  }
  size_t clusterIndex(size_t pos) const noexcept { return fClusterIndexes[pos]; }
  size_t globalClusterIndex(size_t pos) const noexcept {
    return fClusterStart + fClusterIndexes[pos];
  }
  SkScalar positionX(size_t pos) const noexcept;

  TextRange textRange() const noexcept { return fTextRange; }
  ClusterRange clusterRange() const noexcept { return fClusterRange; }

  ParagraphImpl* owner() const noexcept { return fOwner; }

  bool isEllipsis() const noexcept { return fEllipsis; }

  void calculateMetrics() noexcept;
  void updateMetrics(InternalLineMetrics* endlineMetrics);

  void setClusterRange(size_t from, size_t to) noexcept { fClusterRange = ClusterRange(from, to); }
  SkRect clip() const noexcept {
    return SkRect::MakeXYWH(fOffset.fX, fOffset.fY, fAdvance.fX, fAdvance.fY);
  }

  SkScalar addSpacesAtTheEnd(SkScalar space, Cluster* cluster) noexcept;
  SkScalar addSpacesEvenly(SkScalar space, Cluster* cluster);
  void shift(const Cluster* cluster, SkScalar offset) noexcept;

  SkScalar calculateHeight(
      LineMetricStyle ascentStyle, LineMetricStyle descentStyle) const noexcept {
    auto ascent =
        ascentStyle == LineMetricStyle::Typographic ? this->ascent() : this->correctAscent();
    auto descent =
        descentStyle == LineMetricStyle::Typographic ? this->descent() : this->correctDescent();
    return descent - ascent;
  }
  SkScalar calculateWidth(size_t start, size_t end, bool clip) const noexcept;

  void copyTo(SkTextBlobBuilder& builder, size_t pos, size_t size) const;

  using ClusterTextVisitor = std::function<void(
      size_t glyphStart, size_t glyphEnd, size_t charStart, size_t charEnd, SkScalar width,
      SkScalar height)>;
  void iterateThroughClustersInTextOrder(const ClusterTextVisitor& visitor);

  using ClusterVisitor = std::function<void(Cluster* cluster)>;
  void iterateThroughClusters(const ClusterVisitor& visitor);

  std::tuple<bool, ClusterIndex, ClusterIndex> findLimitingClusters(TextRange text) const;
  SkSpan<const SkGlyphID> glyphs() const noexcept {
    return SkSpan<const SkGlyphID>(fGlyphs.begin(), fGlyphs.size());
  }
  SkSpan<const SkPoint> positions() const noexcept {
    return SkSpan<const SkPoint>(fPositions.begin(), fPositions.size());
  }
  SkSpan<const uint32_t> clusterIndexes() const noexcept {
    return SkSpan<const uint32_t>(fClusterIndexes.begin(), fClusterIndexes.size());
  }
  SkSpan<const SkScalar> shifts() const noexcept {
    return SkSpan<const SkScalar>(fShifts.begin(), fShifts.size());
  }

  void commit();

  SkRect getBounds(size_t pos) const noexcept { return fBounds[pos]; }

  void resetShifts() noexcept {
    for (auto& r : fShifts) {
      r = 0;
    }
    fSpaced = false;
  }

  void resetJustificationShifts() noexcept { fJustificationShifts.reset(); }

 private:
  friend class ParagraphImpl;
  friend class TextLine;
  friend class InternalLineMetrics;
  friend class ParagraphCache;
  friend class OneLineShaper;

  ParagraphImpl* fOwner;
  TextRange fTextRange;
  ClusterRange fClusterRange;

  SkFont fFont;
  size_t fPlaceholderIndex;
  size_t fIndex;
  SkVector fAdvance;
  SkVector fOffset;
  TextIndex fClusterStart;
  SkShaper::RunHandler::Range fUtf8Range;
  SkSTArray<128, SkGlyphID, true> fGlyphs;
  SkSTArray<128, SkPoint, true> fPositions;
  SkSTArray<128, SkPoint, true>
      fJustificationShifts;  // For justification (current and prev shifts)
  SkSTArray<128, uint32_t, true> fClusterIndexes;
  SkSTArray<128, SkRect, true> fBounds;

  SkSTArray<128, SkScalar, true> fShifts;  // For formatting (letter/word spacing)

  SkFontMetrics fFontMetrics;
  const SkScalar fHeightMultiplier;
  SkScalar fCorrectAscent;
  SkScalar fCorrectDescent;
  SkScalar fCorrectLeading;

  bool fSpaced;
  bool fEllipsis;
  uint8_t fBidiLevel;
};

class Cluster {
 public:
  enum BreakType {
    None,
    GraphemeBreak,  // calculated for all clusters (UBRK_CHARACTER)
    SoftLineBreak,  // calculated for all clusters (UBRK_LINE & UBRK_CHARACTER)
    HardLineBreak,  // calculated for all clusters (UBRK_LINE)
  };

  Cluster() noexcept
      : fOwner(nullptr),
        fRunIndex(EMPTY_RUN),
        fTextRange(EMPTY_TEXT),
        fGraphemeRange(EMPTY_RANGE),
        fStart(0),
        fEnd(),
        fWidth(),
        fSpacing(0),
        fHeight(),
        fHalfLetterSpacing(0.0) {}

  Cluster(
      ParagraphImpl* owner, RunIndex runIndex, size_t start, size_t end, SkSpan<const char> text,
      SkScalar width, SkScalar height);

  Cluster(TextRange textRange) noexcept : fTextRange(textRange), fGraphemeRange(EMPTY_RANGE) {}

  ~Cluster() = default;

  SkScalar sizeToChar(TextIndex ch) const noexcept;
  SkScalar sizeFromChar(TextIndex ch) const noexcept;

  size_t roundPos(SkScalar s) const noexcept;

  void space(SkScalar shift, SkScalar space) noexcept {
    fSpacing += space;
    fWidth += shift;
  }

  bool isWhitespaces() const noexcept { return fIsWhiteSpaces; }
  bool isHardBreak() const;
  bool isSoftBreak() const;
  bool isGraphemeBreak() const;
  bool canBreakLineAfter() const { return isHardBreak() || isSoftBreak(); }
  size_t startPos() const noexcept { return fStart; }
  size_t endPos() const noexcept { return fEnd; }
  SkScalar width() const noexcept { return fWidth; }
  SkScalar height() const noexcept { return fHeight; }
  size_t size() const noexcept { return fEnd - fStart; }

  void setHalfLetterSpacing(SkScalar halfLetterSpacing) noexcept {
    fHalfLetterSpacing = halfLetterSpacing;
  }
  SkScalar getHalfLetterSpacing() const noexcept { return fHalfLetterSpacing; }

  TextRange textRange() const noexcept { return fTextRange; }

  RunIndex runIndex() const noexcept { return fRunIndex; }
  ParagraphImpl* owner() const noexcept { return fOwner; }

  Run* run() const noexcept;
  SkFont font() const noexcept;

  SkScalar trimmedWidth(size_t pos) const noexcept;

  bool contains(TextIndex ch) const noexcept {
    return ch >= fTextRange.start && ch < fTextRange.end;
  }

  bool belongs(TextRange text) const noexcept {
    return fTextRange.start >= text.start && fTextRange.end <= text.end;
  }

  bool startsIn(TextRange text) const noexcept {
    return fTextRange.start >= text.start && fTextRange.start < text.end;
  }

 private:
  friend ParagraphImpl;

  ParagraphImpl* fOwner;
  RunIndex fRunIndex;
  TextRange fTextRange;
  GraphemeRange fGraphemeRange;

  size_t fStart;
  size_t fEnd;
  SkScalar fWidth;
  SkScalar fSpacing;
  SkScalar fHeight;
  SkScalar fHalfLetterSpacing;
  bool fIsWhiteSpaces;
};

class InternalLineMetrics {
 public:
  InternalLineMetrics() noexcept {
    clean();
    fForceStrut = false;
  }

  InternalLineMetrics(bool forceStrut) noexcept {
    clean();
    fForceStrut = forceStrut;
  }

  InternalLineMetrics(SkScalar a, SkScalar d, SkScalar l) noexcept {
    fAscent = a;
    fDescent = d;
    fLeading = l;
    fForceStrut = false;
  }

  InternalLineMetrics(const SkFont& font, bool forceStrut) {
    SkFontMetrics metrics;
    font.getMetrics(&metrics);
    fAscent = metrics.fAscent;
    fDescent = metrics.fDescent;
    fLeading = metrics.fLeading;
    fForceStrut = forceStrut;
  }

  void add(Run* run) noexcept {
    if (fForceStrut) {
      return;
    }

    fAscent = std::min(fAscent, run->correctAscent());
    fDescent = std::max(fDescent, run->correctDescent());
    fLeading = std::max(fLeading, run->correctLeading());
  }

  void add(InternalLineMetrics other) noexcept {
    fAscent = std::min(fAscent, other.fAscent);
    fDescent = std::max(fDescent, other.fDescent);
    fLeading = std::max(fLeading, other.fLeading);
  }
  void clean() noexcept {
    fAscent = 0;
    fDescent = 0;
    fLeading = 0;
  }

  SkScalar delta() const noexcept { return height() - ideographicBaseline(); }

  void updateLineMetrics(InternalLineMetrics& metrics) noexcept {
    if (metrics.fForceStrut) {
      metrics.fAscent = fAscent;
      metrics.fDescent = fDescent;
      metrics.fLeading = fLeading;
    } else {
      // This is another of those flutter changes. To be removed...
      metrics.fAscent = std::min(metrics.fAscent, fAscent - fLeading / 2.0f);
      metrics.fDescent = std::max(metrics.fDescent, fDescent + fLeading / 2.0f);
    }
  }

  SkScalar runTop(const Run* run, LineMetricStyle ascentStyle) const noexcept {
    return fLeading / 2 - fAscent +
           (ascentStyle == LineMetricStyle::Typographic ? run->ascent() : run->correctAscent()) +
           delta();
  }

  SkScalar height() const noexcept { return ::round((double)fDescent - fAscent + fLeading); }

  SkScalar alphabeticBaseline() const noexcept { return fLeading / 2 - fAscent; }
  SkScalar ideographicBaseline() const noexcept { return fDescent - fAscent + fLeading; }
  SkScalar deltaBaselines() const noexcept { return fLeading / 2 + fDescent; }
  SkScalar baseline() const noexcept { return fLeading / 2 - fAscent; }
  SkScalar ascent() const noexcept { return fAscent; }
  SkScalar descent() const noexcept { return fDescent; }
  SkScalar leading() const noexcept { return fLeading; }
  void setForceStrut(bool value) noexcept { fForceStrut = value; }
  bool getForceStrut() const noexcept { return fForceStrut; }

 private:
  friend class TextWrapper;
  friend class TextLine;

  SkScalar fAscent;
  SkScalar fDescent;
  SkScalar fLeading;
  bool fForceStrut;
};
}  // namespace textlayout
}  // namespace skia

#endif  // Run_DEFINED
