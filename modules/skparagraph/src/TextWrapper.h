// Copyright 2019 Google LLC.
#ifndef TextWrapper_DEFINED
#define TextWrapper_DEFINED

#include <string>
#include "modules/skparagraph/src/TextLine.h"
#include "src/core/SkSpan.h"

namespace skia {
namespace textlayout {

class ParagraphImpl;

class TextWrapper {
  class ClusterPos {
   public:
    constexpr ClusterPos() noexcept : fCluster(nullptr), fPos(0) {}
    ClusterPos(Cluster* cluster, size_t pos) noexcept : fCluster(cluster), fPos(pos) {}
    inline Cluster* cluster() const noexcept { return fCluster; }
    inline size_t position() const noexcept { return fPos; }
    inline void setPosition(size_t pos) noexcept { fPos = pos; }
    void clean() noexcept {
      fCluster = nullptr;
      fPos = 0;
    }
    void move(bool up) noexcept {
      fCluster += up ? 1 : -1;
      fPos = up ? 0 : fCluster->endPos();
    }

   private:
    Cluster* fCluster;
    size_t fPos;
  };
  class TextStretch {
   public:
    TextStretch() noexcept : fStart(), fEnd(), fWidth(0), fWidthWithGhostSpaces(0) {}
    TextStretch(Cluster* s, Cluster* e, bool forceStrut) noexcept
        : fStart(s, 0),
          fEnd(e, e->endPos()),
          fMetrics(forceStrut),
          fWidth(0),
          fWidthWithGhostSpaces(0) {
      for (auto c = s; c <= e; ++c) {
        if (c->run() != nullptr) {
          fMetrics.add(c->run());
        }
      }
    }

    inline SkScalar width() const noexcept { return fWidth; }
    SkScalar widthWithGhostSpaces() const noexcept { return fWidthWithGhostSpaces; }
    inline Cluster* startCluster() const noexcept { return fStart.cluster(); }
    inline Cluster* endCluster() const noexcept { return fEnd.cluster(); }
    inline Cluster* breakCluster() const noexcept { return fBreak.cluster(); }
    inline InternalLineMetrics& metrics() noexcept { return fMetrics; }
    inline size_t startPos() const noexcept { return fStart.position(); }
    inline size_t endPos() const noexcept { return fEnd.position(); }
    bool endOfCluster() noexcept { return fEnd.position() == fEnd.cluster()->endPos(); }
    bool endOfWord() {
      return endOfCluster() && (fEnd.cluster()->isHardBreak() || fEnd.cluster()->isSoftBreak());
    }

    void extend(TextStretch& stretch) noexcept {
      fMetrics.add(stretch.fMetrics);
      fEnd = stretch.fEnd;
      fWidth += stretch.fWidth;
      stretch.clean();
    }

    bool empty() noexcept {
      return fStart.cluster() == fEnd.cluster() && fStart.position() == fEnd.position();
    }

    void setMetrics(const InternalLineMetrics& metrics) noexcept { fMetrics = metrics; }

    void extend(Cluster* cluster) noexcept {
      if (fStart.cluster() == nullptr) {
        fStart = ClusterPos(cluster, cluster->startPos());
      }
      fEnd = ClusterPos(cluster, cluster->endPos());
      // TODO: Make sure all the checks are correct and there are no unnecessary checks
      if (!cluster->run()->isPlaceholder()) {
        fMetrics.add(cluster->run());
      }
      fWidth += cluster->width();
    }

    void extend(Cluster* cluster, size_t pos) noexcept {
      fEnd = ClusterPos(cluster, pos);
      if (cluster->run() != nullptr) {
        fMetrics.add(cluster->run());
      }
    }

    void startFrom(Cluster* cluster, size_t pos) noexcept {
      fStart = ClusterPos(cluster, pos);
      fEnd = ClusterPos(cluster, pos);
      if (cluster->run() != nullptr) {
        fMetrics.add(cluster->run());
      }
      fWidth = 0;
    }

    void saveBreak() noexcept {
      fWidthWithGhostSpaces = fWidth;
      fBreak = fEnd;
    }

    void restoreBreak() noexcept {
      fWidth = fWidthWithGhostSpaces;
      fEnd = fBreak;
    }

    void trim() {
      if (fEnd.cluster() != nullptr && fEnd.cluster()->owner() != nullptr &&
          fEnd.cluster()->run() != nullptr &&
          fEnd.cluster()->run()->placeholderStyle() == nullptr && fWidth > 0) {
        fWidth -= (fEnd.cluster()->width() - fEnd.cluster()->trimmedWidth(fEnd.position()));
      }
    }

    void trim(Cluster* cluster) noexcept {
      SkASSERT(fEnd.cluster() == cluster);
      if (fEnd.cluster() > fStart.cluster()) {
        fEnd.move(false);
        fWidth -= cluster->width();
      } else {
        fWidth = 0;
      }
    }

    void clean() noexcept {
      fStart.clean();
      fEnd.clean();
      fWidth = 0;
      fMetrics.clean();
    }

   private:
    ClusterPos fStart;
    ClusterPos fEnd;
    ClusterPos fBreak;
    InternalLineMetrics fMetrics;
    SkScalar fWidth;
    SkScalar fWidthWithGhostSpaces;
  };

 public:
  TextWrapper() noexcept {
    fLineNumber = 1;
    fHardLineBreak = false;
    fExceededMaxLines = false;
  }

  using AddLineToParagraph = std::function<void(
      TextRange text, TextRange textWithSpaces, ClusterRange clusters,
      ClusterRange clustersWithGhosts, SkScalar AddLineToParagraph, size_t startClip,
      size_t endClip, SkVector offset, SkVector advance, InternalLineMetrics metrics,
      bool addEllipsis)>;
  void breakTextIntoLines(
      ParagraphImpl* parent, SkScalar maxWidth, const AddLineToParagraph& addLine);

  SkScalar height() const noexcept { return fHeight; }
  SkScalar minIntrinsicWidth() const noexcept { return fMinIntrinsicWidth; }
  SkScalar maxIntrinsicWidth() const noexcept { return fMaxIntrinsicWidth; }
  bool exceededMaxLines() const noexcept { return fExceededMaxLines; }

 private:
  TextStretch fWords;
  TextStretch fClusters;
  TextStretch fClip;
  TextStretch fEndLine;
  size_t fLineNumber;
  bool fTooLongWord;
  bool fTooLongCluster;

  bool fHardLineBreak;
  bool fExceededMaxLines;

  SkScalar fHeight;
  SkScalar fMinIntrinsicWidth;
  SkScalar fMaxIntrinsicWidth;

  void reset() noexcept {
    fWords.clean();
    fClusters.clean();
    fClip.clean();
    fTooLongCluster = false;
    fTooLongWord = false;
  }

  void lookAhead(SkScalar maxWidth, Cluster* endOfClusters);
  void moveForward(bool hasEllipsis);
  void trimEndSpaces(TextAlign align);
  std::tuple<Cluster*, size_t, SkScalar> trimStartSpaces(Cluster* endOfClusters);
  SkScalar getClustersTrimmedWidth();
};
}  // namespace textlayout
}  // namespace skia

#endif  // TextWrapper_DEFINED
