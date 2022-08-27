/*
 * Copyright 2006 The Android Open Source Project
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef SkGlyph_DEFINED
#define SkGlyph_DEFINED

#include "include/core/SkPath.h"
#include "include/core/SkTypes.h"
#include "include/private/SkChecksum.h"
#include "include/private/SkFixed.h"
#include "include/private/SkTo.h"
#include "include/private/SkVx.h"
#include "src/core/SkMask.h"
#include "src/core/SkMathPriv.h"

class SkArenaAlloc;
class SkDrawable;
class SkScalerContext;

// -- SkPackedGlyphID ------------------------------------------------------------------------------
// A combination of SkGlyphID and sub-pixel position information.
struct SkPackedGlyphID {
  inline static constexpr uint32_t kImpossibleID = ~0u;
  enum {
    // Lengths
    kGlyphIDLen = 16u,
    kSubPixelPosLen = 2u,

    // Bit positions
    kSubPixelX = 0u,
    kGlyphID = kSubPixelPosLen,
    kSubPixelY = kGlyphIDLen + kSubPixelPosLen,
    kEndData = kGlyphIDLen + 2 * kSubPixelPosLen,

    // Masks
    kGlyphIDMask = (1u << kGlyphIDLen) - 1,
    kSubPixelPosMask = (1u << kSubPixelPosLen) - 1,
    kMaskAll = (1u << kEndData) - 1,

    // Location of sub pixel info in a fixed pointer number.
    kFixedPointBinaryPointPos = 16u,
    kFixedPointSubPixelPosBits = kFixedPointBinaryPointPos - kSubPixelPosLen,
  };

  inline static const constexpr SkScalar kSubpixelRound =
      1.f / (1u << (SkPackedGlyphID::kSubPixelPosLen + 1));

  inline static const constexpr SkIPoint kXYFieldMask{
      kSubPixelPosMask << kSubPixelX, kSubPixelPosMask << kSubPixelY};

  struct Hash {
    uint32_t operator()(SkPackedGlyphID packedID) const noexcept { return packedID.hash(); }
  };

  constexpr explicit SkPackedGlyphID(SkGlyphID glyphID) noexcept
      : fID{(uint32_t)glyphID << kGlyphID} {}

  constexpr SkPackedGlyphID(SkGlyphID glyphID, SkFixed x, SkFixed y) noexcept 
      : fID{PackIDXY(glyphID, x, y)} {}

  constexpr SkPackedGlyphID(SkGlyphID glyphID, uint32_t x, uint32_t y) noexcept 
      : fID{PackIDSubXSubY(glyphID, x, y)} {}

  SkPackedGlyphID(SkGlyphID glyphID, SkPoint pt, SkIPoint mask) noexcept 
      : fID{PackIDSkPoint(glyphID, pt, mask)} {}

  constexpr explicit SkPackedGlyphID(uint32_t v) noexcept : fID{v & kMaskAll} {}
  constexpr SkPackedGlyphID() noexcept : fID{kImpossibleID} {}

  bool operator==(const SkPackedGlyphID& that) const noexcept { return fID == that.fID; }
  bool operator!=(const SkPackedGlyphID& that) const noexcept { return !(*this == that); }
  bool operator<(SkPackedGlyphID that) const noexcept { return this->fID < that.fID; }

  SkGlyphID glyphID() const noexcept { return (fID >> kGlyphID) & kGlyphIDMask; }

  uint32_t value() const noexcept { return fID; }

  SkFixed getSubXFixed() const noexcept { return this->subToFixed(kSubPixelX); }

  SkFixed getSubYFixed() const noexcept { return this->subToFixed(kSubPixelY); }

  uint32_t hash() const noexcept { return SkChecksum::CheapMix(fID); }

  SkString dump() const {
    SkString str;
    str.appendf("glyphID: %d, x: %d, y:%d", glyphID(), getSubXFixed(), getSubYFixed());
    return str;
  }

  SkString shortDump() const {
    SkString str;
    str.appendf(
        "0x%x|%1d|%1d", this->glyphID(), this->subPixelField(kSubPixelX),
        this->subPixelField(kSubPixelY));
    return str;
  }

 private:
  static constexpr uint32_t PackIDSubXSubY(SkGlyphID glyphID, uint32_t x, uint32_t y) noexcept {
    SkASSERT(x < (1u << kSubPixelPosLen));
    SkASSERT(y < (1u << kSubPixelPosLen));

    return (x << kSubPixelX) | (y << kSubPixelY) | (glyphID << kGlyphID);
  }

  // Assumptions: pt is properly rounded. mask is set for the x or y fields.
  //
  // A sub-pixel field is a number on the interval [2^kSubPixel, 2^(kSubPixel + kSubPixelPosLen)).
  // Where kSubPixel is either kSubPixelX or kSubPixelY. Given a number x on [0, 1) we can
  // generate a sub-pixel field using:
  //    sub-pixel-field = x * 2^(kSubPixel + kSubPixelPosLen)
  //
  // We can generate the integer sub-pixel field by &-ing the integer part of sub-filed with the
  // sub-pixel field mask.
  //    int-sub-pixel-field = int(sub-pixel-field) & (kSubPixelPosMask << kSubPixel)
  //
  // The last trick is to extend the range from [0, 1) to [0, 2). The extend range is
  // necessary because the modulo 1 calculation (pt - floor(pt)) generates numbers on [-1, 1).
  // This does not round (floor) properly when converting to integer. Adding one to the range
  // causes truncation and floor to be the same. Coincidentally, masking to produce the field also
  // removes the +1.
  static uint32_t PackIDSkPoint(SkGlyphID glyphID, SkPoint pt, SkIPoint mask) noexcept {
#if 0
        // TODO: why does this code not work on GCC 8.3 x86 Debug builds?
        using namespace skvx;
        using XY = Vec<2, float>;
        using SubXY = Vec<2, int>;

        const XY magic = {1.f * (1u << (kSubPixelPosLen + kSubPixelX)),
                          1.f * (1u << (kSubPixelPosLen + kSubPixelY))};
        XY pos{pt.x(), pt.y()};
        XY subPos = (pos - floor(pos)) + 1.0f;
        SubXY sub = cast<int>(subPos * magic) & SubXY{mask.x(), mask.y()};
#else
    constexpr float magicX = 1.f * (1u << (kSubPixelPosLen + kSubPixelX)),
                magicY = 1.f * (1u << (kSubPixelPosLen + kSubPixelY));

    float x = pt.x(), y = pt.y();
    x = (x - floorf(x)) + 1.0f;
    y = (y - floorf(y)) + 1.0f;
    int sub[] = {
        (int)(x * magicX) & mask.x(),
        (int)(y * magicY) & mask.y(),
    };
#endif

    SkASSERT(sub[0] / (1u << kSubPixelX) < (1u << kSubPixelPosLen));
    SkASSERT(sub[1] / (1u << kSubPixelY) < (1u << kSubPixelPosLen));
    return (glyphID << kGlyphID) | sub[0] | sub[1];
  }

  static constexpr uint32_t PackIDXY(SkGlyphID glyphID, SkFixed x, SkFixed y) noexcept {
    return PackIDSubXSubY(glyphID, FixedToSub(x), FixedToSub(y));
  }

  static constexpr uint32_t FixedToSub(SkFixed n) noexcept {
    return ((uint32_t)n >> kFixedPointSubPixelPosBits) & kSubPixelPosMask;
  }

  constexpr uint32_t subPixelField(uint32_t subPixelPosBit) const noexcept {
    return (fID >> subPixelPosBit) & kSubPixelPosMask;
  }

  constexpr SkFixed subToFixed(uint32_t subPixelPosBit) const noexcept {
    uint32_t subPixelPosition = this->subPixelField(subPixelPosBit);
    return subPixelPosition << kFixedPointSubPixelPosBits;
  }

  uint32_t fID;
};

// -- SkAxisAlignment ------------------------------------------------------------------------------
// SkAxisAlignment specifies the x component of a glyph's position is rounded when kX, and the y
// component is rounded when kY. If kNone then neither are rounded.
enum class SkAxisAlignment : uint32_t {
  kNone,
  kX,
  kY,
};

// round and ignorePositionMask are used to calculate the subpixel position of a glyph.
// The per component (x or y) calculation is:
//
//   subpixelOffset = (floor((viewportPosition + rounding) & mask) >> 14) & 3
//
// where mask is either 0 or ~0, and rounding is either
// 1/2 for non-subpixel or 1/8 for subpixel.
struct SkGlyphPositionRoundingSpec {
  SkGlyphPositionRoundingSpec(bool isSubpixel, SkAxisAlignment axisAlignment);
  const SkVector halfAxisSampleFreq;
  const SkIPoint ignorePositionMask;
  const SkIPoint ignorePositionFieldMask;

 private:
  static SkVector HalfAxisSampleFreq(bool isSubpixel, SkAxisAlignment axisAlignment);
  static SkIPoint IgnorePositionMask(bool isSubpixel, SkAxisAlignment axisAlignment);
  static SkIPoint IgnorePositionFieldMask(bool isSubpixel, SkAxisAlignment axisAlignment);
};

class SkGlyphRect;
namespace skglyph {
SkGlyphRect rect_union(SkGlyphRect, SkGlyphRect) noexcept;
SkGlyphRect rect_intersection(SkGlyphRect, SkGlyphRect) noexcept;
}  // namespace skglyph

// SkGlyphRect encodes rectangles with coordinates on [-32767, 32767]. It is specialized for
// rectangle union and intersection operations.
class SkGlyphRect {
 public:
  SkGlyphRect() = default;
  SkGlyphRect(int16_t left, int16_t top, int16_t right, int16_t bottom) noexcept 
      : fRect{(int16_t)-left, (int16_t)-top, right, bottom} {
    SkDEBUGCODE(const int32_t min = std::numeric_limits<int16_t>::min());
    SkASSERT(left != min && top != min && right != min && bottom != min);
  }
  bool empty() const noexcept { return -fRect[0] >= fRect[2] || -fRect[1] >= fRect[3]; }
  SkRect rect() const noexcept {
    return SkRect::MakeLTRB(-fRect[0], -fRect[1], fRect[2], fRect[3]);
  }
  SkIRect iRect() const noexcept {
    return SkIRect::MakeLTRB(-fRect[0], -fRect[1], fRect[2], fRect[3]);
  }
  SkGlyphRect offset(int16_t x, int16_t y) const noexcept {
    return SkGlyphRect{fRect + Storage{SkTo<int16_t>(-x), SkTo<int16_t>(-y), x, y}};
  }
  skvx::Vec<2, int16_t> leftTop() const noexcept { return -this->negLeftTop(); }
  skvx::Vec<2, int16_t> rightBottom() const noexcept { return {fRect[2], fRect[3]}; }
  skvx::Vec<2, int16_t> widthHeight() const noexcept { return this->rightBottom() + negLeftTop(); }
  friend SkGlyphRect skglyph::rect_union(SkGlyphRect, SkGlyphRect) noexcept;
  friend SkGlyphRect skglyph::rect_intersection(SkGlyphRect, SkGlyphRect) noexcept;

 private:
  skvx::Vec<2, int16_t> negLeftTop() const noexcept { return {fRect[0], fRect[1]}; }
  using Storage = skvx::Vec<4, int16_t>;
  SkGlyphRect(Storage rect) noexcept : fRect{rect} {}
  Storage fRect;
};

namespace skglyph {
inline SkGlyphRect empty_rect() noexcept {
  constexpr int16_t max = std::numeric_limits<int16_t>::max();
  return {max, max, -max, -max};
}
inline SkGlyphRect full_rect() noexcept {
  constexpr int16_t max = std::numeric_limits<int16_t>::max();
  return {-max, -max, max, max};
}
inline SkGlyphRect rect_union(SkGlyphRect a, SkGlyphRect b) noexcept {
  return skvx::max(a.fRect, b.fRect);
}
inline SkGlyphRect rect_intersection(SkGlyphRect a, SkGlyphRect b) noexcept {
  return skvx::min(a.fRect, b.fRect);
}
}  // namespace skglyph

class SkGlyph;

// SkGlyphDigest contains a digest of information for making GPU drawing decisions. It can be
// referenced instead of the glyph itself in many situations. In the remote glyphs cache the
// SkGlyphDigest is the only information that needs to be stored in the cache.
class SkGlyphDigest {
 public:
  // Default ctor is only needed for the hash table.
  SkGlyphDigest() = default;
  SkGlyphDigest(size_t index, const SkGlyph& glyph);
  int index() const noexcept { return fIndex; }
  bool isEmpty() const noexcept { return fIsEmpty; }
  bool isColor() const noexcept { return fIsColor; }
  bool canDrawAsMask() const noexcept { return fCanDrawAsMask; }
  bool canDrawAsSDFT() const noexcept { return fCanDrawAsSDFT; }
  uint16_t maxDimension() const noexcept { return std::max(fWidth, fHeight); }

 private:
  static_assert(SkPackedGlyphID::kEndData == 20);
  struct {
    uint32_t fIndex : SkPackedGlyphID::kEndData;
    uint32_t fIsEmpty : 1;
    uint32_t fIsColor : 1;
    uint32_t fCanDrawAsMask : 1;
    uint32_t fCanDrawAsSDFT : 1;
  };
  int16_t fLeft, fTop;
  uint16_t fWidth, fHeight;
};

class SkGlyph {
 public:
  // SkGlyph() is used for testing.
  constexpr SkGlyph() noexcept : SkGlyph{SkPackedGlyphID()} {}
  SkGlyph(const SkGlyph&) noexcept;
  SkGlyph& operator=(const SkGlyph&) noexcept;
  SkGlyph(SkGlyph&&) noexcept;
  SkGlyph& operator=(SkGlyph&&) noexcept;
  ~SkGlyph();
  constexpr explicit SkGlyph(SkPackedGlyphID id) : fID{id} {}

  SkVector advanceVector() const noexcept { return SkVector{fAdvanceX, fAdvanceY}; }
  SkScalar advanceX() const noexcept { return fAdvanceX; }
  SkScalar advanceY() const noexcept { return fAdvanceY; }

  SkGlyphID getGlyphID() const noexcept { return fID.glyphID(); }
  SkPackedGlyphID getPackedID() const noexcept { return fID; }
  SkFixed getSubXFixed() const { return fID.getSubXFixed(); }
  SkFixed getSubYFixed() const { return fID.getSubYFixed(); }

  size_t rowBytes() const;
  size_t rowBytesUsingFormat(SkMask::Format format) const;

  // Call this to set all of the metrics fields to 0 (e.g. if the scaler
  // encounters an error measuring a glyph). Note: this does not alter the
  // fImage, fPath, fID, fMaskFormat fields.
  void zeroMetrics();

  SkMask mask() const;

  SkMask mask(SkPoint position) const;

  // Image
  // If we haven't already tried to associate an image with this glyph
  // (i.e. setImageHasBeenCalled() returns false), then use the
  // SkScalerContext or const void* argument to set the image.
  bool setImage(SkArenaAlloc* alloc, SkScalerContext* scalerContext);
  bool setImage(SkArenaAlloc* alloc, const void* image);

  // Merge the from glyph into this glyph using alloc to allocate image data. Return the number
  // of bytes allocated. Copy the width, height, top, left, format, and image into this glyph
  // making a copy of the image using the alloc.
  size_t setMetricsAndImage(SkArenaAlloc* alloc, const SkGlyph& from);

  // Returns true if the image has been set.
  bool setImageHasBeenCalled() const noexcept {
    return fImage != nullptr || this->isEmpty() || this->imageTooLarge();
  }

  // Return a pointer to the path if the image exists, otherwise return nullptr.
  const void* image() const noexcept {
    SkASSERT(this->setImageHasBeenCalled());
    return fImage;
  }

  // Return the size of the image.
  size_t imageSize() const;

  // Path
  // If we haven't already tried to associate a path to this glyph
  // (i.e. setPathHasBeenCalled() returns false), then use the
  // SkScalerContext or SkPath argument to try to do so.  N.B. this
  // may still result in no path being associated with this glyph,
  // e.g. if you pass a null SkPath or the typeface is bitmap-only.
  //
  // This setPath() call is sticky... once you call it, the glyph
  // stays in its state permanently, ignoring any future calls.
  //
  // Returns true if this is the first time you called setPath()
  // and there actually is a path; call path() to get it.
  bool setPath(SkArenaAlloc* alloc, SkScalerContext* scalerContext);
  bool setPath(SkArenaAlloc* alloc, const SkPath* path, bool hairline);

  // Returns true if that path has been set.
  bool setPathHasBeenCalled() const noexcept { return fPathData != nullptr; }

  // Return a pointer to the path if it exists, otherwise return nullptr. Only works if the
  // path was previously set.
  const SkPath* path() const;
  bool pathIsHairline() const;

  bool setDrawable(SkArenaAlloc* alloc, SkScalerContext* scalerContext);
  bool setDrawable(SkArenaAlloc* alloc, sk_sp<SkDrawable> drawable);
  bool setDrawableHasBeenCalled() const noexcept { return fDrawableData != nullptr; }
  SkDrawable* drawable() const;

  // Format
  bool isColor() const noexcept { return fMaskFormat == SkMask::kARGB32_Format; }
  SkMask::Format maskFormat() const noexcept { return fMaskFormat; }
  size_t formatAlignment() const;

  // Bounds
  int maxDimension() const noexcept { return std::max(fWidth, fHeight); }
  SkIRect iRect() const noexcept { return SkIRect::MakeXYWH(fLeft, fTop, fWidth, fHeight); }
  SkRect rect() const noexcept { return SkRect::MakeXYWH(fLeft, fTop, fWidth, fHeight); }
  SkGlyphRect glyphRect() const noexcept {
    return {fLeft, fTop, SkTo<int16_t>(fLeft + fWidth), SkTo<int16_t>(fTop + fHeight)};
  }
  int left() const noexcept { return fLeft; }
  int top() const noexcept { return fTop; }
  int width() const noexcept { return fWidth; }
  int height() const noexcept { return fHeight; }
  bool isEmpty() const noexcept {
    // fHeight == 0 -> fWidth == 0;
    SkASSERT(fHeight != 0 || fWidth == 0);
    return fWidth == 0;
  }
  bool imageTooLarge() const noexcept { return fWidth >= kMaxGlyphWidth; }

  // Make sure that the intercept information is on the glyph and return it, or return it if it
  // already exists.
  // * bounds - either end of the gap for the character.
  // * scale, xPos - information about how wide the gap is.
  // * array - accumulated gaps for many characters if not null.
  // * count - the number of gaps.
  void ensureIntercepts(
      const SkScalar bounds[2], SkScalar scale, SkScalar xPos, SkScalar* array, int* count,
      SkArenaAlloc* alloc);

  void setImage(void* image) { fImage = image; }

 private:
  // There are two sides to an SkGlyph, the scaler side (things that create glyph data) have
  // access to all the fields. Scalers are assumed to maintain all the SkGlyph invariants. The
  // consumer side has a tighter interface.
  friend class RandomScalerContext;
  friend class SkScalerContext;
  friend class SkScalerContextProxy;
  friend class SkScalerContext_Empty;
  friend class SkScalerContext_FreeType;
  friend class SkScalerContext_FreeType_Base;
  friend class SkScalerContext_DW;
  friend class SkScalerContext_GDI;
  friend class SkScalerContext_Mac;
  friend class SkStrikeClientImpl;
  friend class SkTestScalerContext;
  friend class SkTestSVGScalerContext;
  friend class SkUserScalerContext;
  friend class TestSVGTypeface;
  friend class TestTypeface;

  inline static constexpr uint16_t kMaxGlyphWidth = 1u << 13u;

  // Support horizontal and vertical skipping strike-through / underlines.
  // The caller walks the linked list looking for a match. For a horizontal underline,
  // the fBounds contains the top and bottom of the underline. The fInterval pair contains the
  // beginning and end of the intersection of the bounds and the glyph's path.
  // If interval[0] >= interval[1], no intersection was found.
  struct Intercept {
    Intercept* fNext;
    SkScalar fBounds[2];    // for horz underlines, the boundaries in Y
    SkScalar fInterval[2];  // the outside intersections of the axis and the glyph
  };

  struct PathData {
    Intercept* fIntercept{nullptr};
    SkPath fPath;
    bool fHasPath{false};
    // A normal user-path will have patheffects applied to it and eventually become a dev-path.
    // A dev-path is always a fill-path, except when it is hairline.
    // The fPath is a dev-path, so sidecar the paths hairline status.
    // This allows the user to avoid filling paths which should not be filled.
    bool fHairline{false};
  };

  struct DrawableData {
    Intercept* fIntercept{nullptr};
    sk_sp<SkDrawable> fDrawable;
    bool fHasDrawable{false};
  };

  size_t allocImage(SkArenaAlloc* alloc);

  // path == nullptr indicates that there is no path.
  void installPath(SkArenaAlloc* alloc, const SkPath* path, bool hairline);

  // drawable == nullptr indicates that there is no path.
  void installDrawable(SkArenaAlloc* alloc, sk_sp<SkDrawable> drawable);

  // The width and height of the glyph mask.
  uint16_t fWidth = 0, fHeight = 0;

  // The offset from the glyphs origin on the baseline to the top left of the glyph mask.
  int16_t fTop = 0, fLeft = 0;

  // fImage must remain null if the glyph is empty or if width > kMaxGlyphWidth.
  void* fImage = nullptr;

  // Path data has tricky state. If the glyph isEmpty, then fPathData should always be nullptr,
  // else if fPathData is not null, then a path has been requested. The fPath field of fPathData
  // may still be null after the request meaning that there is no path for this glyph.
  PathData* fPathData = nullptr;
  DrawableData* fDrawableData = nullptr;

  // The advance for this glyph.
  float fAdvanceX = 0, fAdvanceY = 0;

  SkMask::Format fMaskFormat{SkMask::kBW_Format};

  // Used by the SkScalerContext to pass state from generateMetrics to generateImage.
  // Usually specifies which glyph representation was used to generate the metrics.
  uint16_t fScalerContextBits = 0;

  // An SkGlyph can be created with just a packedID, but generally speaking some glyph factory
  // needs to actually fill out the glyph before it can be used as part of that system.
  SkDEBUGCODE(bool fAdvancesBoundsFormatAndInitialPathDone{false};)

  SkPackedGlyphID fID;
};

#endif
