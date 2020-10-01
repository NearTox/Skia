// Copyright 2019 Google LLC.
#ifndef ParagraphImpl_DEFINED
#define ParagraphImpl_DEFINED

#include "include/core/SkFont.h"
#include "include/core/SkPaint.h"
#include "include/core/SkPicture.h"
#include "include/core/SkPoint.h"
#include "include/core/SkRect.h"
#include "include/core/SkRefCnt.h"
#include "include/core/SkScalar.h"
#include "include/core/SkString.h"
#include "include/core/SkTypes.h"
#include "include/private/SkBitmaskEnum.h"
#include "include/private/SkTArray.h"
#include "include/private/SkTHash.h"
#include "include/private/SkTemplates.h"
#include "modules/skparagraph/include/DartTypes.h"
#include "modules/skparagraph/include/FontCollection.h"
#include "modules/skparagraph/include/Paragraph.h"
#include "modules/skparagraph/include/ParagraphCache.h"
#include "modules/skparagraph/include/ParagraphStyle.h"
#include "modules/skparagraph/include/TextShadow.h"
#include "modules/skparagraph/include/TextStyle.h"
#include "modules/skparagraph/src/Run.h"
#include "modules/skparagraph/src/TextLine.h"
#include "modules/skshaper/src/SkUnicode.h"
#include "src/core/SkSpan.h"

#include <memory>
#include <string>
#include <vector>

class SkCanvas;

namespace skia {
namespace textlayout {

enum CodeUnitFlags {
  kNoCodeUnitFlag = 0x0,
  kPartOfWhiteSpace = 0x1,
  kGraphemeStart = 0x2,
  kSoftLineBreakBefore = 0x4,
  kHardLineBreakBefore = 0x8,
};
}  // namespace textlayout
}  // namespace skia

namespace sknonstd {
template <>
struct is_bitmask_enum<skia::textlayout::CodeUnitFlags> : std::true_type {};
}  // namespace sknonstd

namespace skia {
namespace textlayout {

class LineMetrics;
class TextLine;

template <typename T>
bool operator==(const SkSpan<T>& a, const SkSpan<T>& b) {
  return a.size() == b.size() && a.begin() == b.begin();
}

template <typename T>
bool operator<=(const SkSpan<T>& a, const SkSpan<T>& b) {
  return a.begin() >= b.begin() && a.end() <= b.end();
}

template <typename TStyle>
struct StyleBlock {
  StyleBlock() : fRange(EMPTY_RANGE), fStyle() {}
  StyleBlock(size_t start, size_t end, const TStyle& style) : fRange(start, end), fStyle(style) {}
  StyleBlock(TextRange textRange, const TStyle& style) : fRange(textRange), fStyle(style) {}
  void add(TextRange tail) {
    SkASSERT(fRange.end == tail.start);
    fRange = TextRange(fRange.start, fRange.start + fRange.width() + tail.width());
  }
  TextRange fRange;
  TStyle fStyle;
};

struct ResolvedFontDescriptor {
  ResolvedFontDescriptor(TextIndex index, SkFont font) noexcept : fFont(font), fTextStart(index) {}
  SkFont fFont;
  TextIndex fTextStart;
};
/*
struct BidiRegion {
    BidiRegion(size_t start, size_t end, uint8_t dir)
        : text(start, end), direction(dir) { }
    TextRange text;
    uint8_t direction;
};
*/
class ParagraphImpl final : public Paragraph {
 public:
  ParagraphImpl(
      const SkString& text, ParagraphStyle style, SkTArray<Block, true> blocks,
      SkTArray<Placeholder, true> placeholders, sk_sp<FontCollection> fonts);

  ParagraphImpl(
      const std::u16string& utf16text, ParagraphStyle style, SkTArray<Block, true> blocks,
      SkTArray<Placeholder, true> placeholders, sk_sp<FontCollection> fonts);
  ~ParagraphImpl() override;

  void layout(SkScalar width) override;
  void paint(SkCanvas* canvas, SkScalar x, SkScalar y) override;
  std::vector<TextBox> getRectsForRange(
      unsigned start, unsigned end, RectHeightStyle rectHeightStyle,
      RectWidthStyle rectWidthStyle) override;
  std::vector<TextBox> getRectsForPlaceholders() override;
  void getLineMetrics(std::vector<LineMetrics>&) override;
  PositionWithAffinity getGlyphPositionAtCoordinate(SkScalar dx, SkScalar dy) override;
  SkRange<size_t> getWordBoundary(unsigned offset) override;

  size_t lineNumber() noexcept override { return fLines.size(); }

  TextLine& addLine(
      SkVector offset, SkVector advance, TextRange text, TextRange textWithSpaces,
      ClusterRange clusters, ClusterRange clustersWithGhosts, SkScalar widthWithSpaces,
      InternalLineMetrics sizes);

  SkSpan<const char> text() const noexcept {
    return SkSpan<const char>(fText.c_str(), fText.size());
  }
  InternalState state() const noexcept { return fState; }
  SkSpan<Run> runs() noexcept { return SkSpan<Run>(fRuns.data(), fRuns.size()); }
  SkSpan<Block> styles() noexcept { return SkSpan<Block>(fTextStyles.data(), fTextStyles.size()); }
  SkSpan<Placeholder> placeholders() noexcept {
    return SkSpan<Placeholder>(fPlaceholders.data(), fPlaceholders.size());
  }
  SkSpan<TextLine> lines() noexcept { return SkSpan<TextLine>(fLines.data(), fLines.size()); }
  const ParagraphStyle& paragraphStyle() const noexcept { return fParagraphStyle; }
  SkSpan<Cluster> clusters() noexcept {
    return SkSpan<Cluster>(fClusters.begin(), fClusters.size());
  }
  sk_sp<FontCollection> fontCollection() const noexcept { return fFontCollection; }
  void formatLines(SkScalar maxWidth);
  void ensureUTF16Mapping();
  TextIndex findGraphemeStart(TextIndex index);
  size_t getUTF16Index(TextIndex index) noexcept { return fUTF16IndexForUTF8Index[index]; }

  bool strutEnabled() const noexcept { return paragraphStyle().getStrutStyle().getStrutEnabled(); }
  bool strutForceHeight() const noexcept {
    return paragraphStyle().getStrutStyle().getForceStrutHeight();
  }
  bool strutHeightOverride() const noexcept {
    return paragraphStyle().getStrutStyle().getHeightOverride();
  }
  InternalLineMetrics strutMetrics() const noexcept { return fStrutMetrics; }

  SkSpan<const char> text(TextRange textRange) noexcept;
  SkSpan<Cluster> clusters(ClusterRange clusterRange) noexcept;
  Cluster& cluster(ClusterIndex clusterIndex) noexcept;
  ClusterIndex clusterIndex(TextIndex textIndex) noexcept {
    auto clusterIndex = this->fClustersIndexFromCodeUnit[textIndex];
    SkASSERT(clusterIndex != EMPTY_INDEX);
    return clusterIndex;
  }
  Run& run(RunIndex runIndex) noexcept {
    SkASSERT(runIndex < fRuns.size());
    return fRuns[runIndex];
  }

  Run& runByCluster(ClusterIndex clusterIndex);
  SkSpan<Block> blocks(BlockRange blockRange) noexcept;
  Block& block(BlockIndex blockIndex) noexcept;
  SkTArray<ResolvedFontDescriptor> resolvedFonts() const noexcept { return fFontSwitches; }

  void markDirty() noexcept override { fState = kUnknown; }

  int32_t unresolvedGlyphs() noexcept override;

  void setState(InternalState state);
  sk_sp<SkPicture> getPicture() noexcept { return fPicture; }
  SkRect getBoundaries() const noexcept { return fOrigin; }

  SkScalar widthWithTrailingSpaces() noexcept { return fMaxWidthWithTrailingSpaces; }

  void resetContext() noexcept;
  void resolveStrut();

  bool computeCodeUnitProperties();

  void buildClusterTable();
  void spaceGlyphs();
  bool shapeTextIntoEndlessLine();
  void breakShapedTextIntoLines(SkScalar maxWidth);
  void paintLinesIntoPicture();

  void updateTextAlign(TextAlign textAlign) noexcept override;
  void updateText(size_t from, SkString text) override;
  void updateFontSize(size_t from, size_t to, SkScalar fontSize) override;
  void updateForegroundPaint(size_t from, size_t to, SkPaint paint) override;
  void updateBackgroundPaint(size_t from, size_t to, SkPaint paint) override;

  InternalLineMetrics getEmptyMetrics() const noexcept { return fEmptyMetrics; }
  InternalLineMetrics getStrutMetrics() const noexcept { return fStrutMetrics; }

  BlockRange findAllBlocks(TextRange textRange);

  void resetShifts() noexcept {
    for (auto& run : fRuns) {
      run.resetJustificationShifts();
      run.resetShifts();
    }
  }

  using CodeUnitRangeVisitor = std::function<bool(TextRange textRange)>;
  void forEachCodeUnitPropertyRange(CodeUnitFlags property, CodeUnitRangeVisitor visitor);
  size_t getWhitespacesLength(TextRange textRange);

  bool codeUnitHasProperty(size_t index, CodeUnitFlags property) const {
    return (fCodeUnitProperties[index] & property) == property;
  }

  SkUnicode* getICU() noexcept { return fICU.get(); }

 private:
  friend class ParagraphBuilder;
  friend class ParagraphCacheKey;
  friend class ParagraphCacheValue;
  friend class ParagraphCache;

  friend class TextWrapper;
  friend class OneLineShaper;

  void calculateBoundaries();

  void computeEmptyMetrics();

  // Input
  SkTArray<StyleBlock<SkScalar>> fLetterSpaceStyles;
  SkTArray<StyleBlock<SkScalar>> fWordSpaceStyles;
  SkTArray<StyleBlock<SkPaint>> fBackgroundStyles;
  SkTArray<StyleBlock<SkPaint>> fForegroundStyles;
  SkTArray<StyleBlock<std::vector<TextShadow>>> fShadowStyles;
  SkTArray<StyleBlock<Decoration>> fDecorationStyles;
  SkTArray<Block, true> fTextStyles;  // TODO: take out only the font stuff
  SkTArray<Placeholder, true> fPlaceholders;
  SkString fText;

  // Internal structures
  InternalState fState;
  SkTArray<Run, false> fRuns;  // kShaped
  SkTArray<Cluster, true>
      fClusters;  // kClusterized (cached: text, word spacing, letter spacing, resolved fonts)
  SkTArray<CodeUnitFlags> fCodeUnitProperties;
  SkTArray<size_t> fClustersIndexFromCodeUnit;
  std::vector<size_t> fWords;
  std::vector<BidiRegion> fBidiRegions;
  // These two arrays are used in measuring methods (getRectsForRange, getGlyphPositionAtCoordinate)
  // They are filled lazily whenever they need and cached
  SkTArray<TextIndex, true> fUTF8IndexForUTF16Index;
  SkTArray<size_t, true> fUTF16IndexForUTF8Index;
  size_t fUnresolvedGlyphs;

  SkTArray<TextLine, false>
      fLines;                 // kFormatted   (cached: width, max lines, ellipsis, text align)
  sk_sp<SkPicture> fPicture;  // kRecorded    (cached: text styles)

  SkTArray<ResolvedFontDescriptor> fFontSwitches;

  InternalLineMetrics fEmptyMetrics;
  InternalLineMetrics fStrutMetrics;

  SkScalar fOldWidth;
  SkScalar fOldHeight;
  SkScalar fMaxWidthWithTrailingSpaces;
  SkRect fOrigin;

  std::unique_ptr<SkUnicode> fICU;
};
}  // namespace textlayout
}  // namespace skia

#endif  // ParagraphImpl_DEFINED
