/*
 * Copyright 2018 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef SkTextBlobPriv_DEFINED
#define SkTextBlobPriv_DEFINED

#include "include/core/SkColorFilter.h"
#include "include/core/SkFont.h"
#include "include/core/SkImageFilter.h"
#include "include/core/SkMaskFilter.h"
#include "include/core/SkPathEffect.h"
#include "include/core/SkShader.h"
#include "include/core/SkTextBlob.h"
#include "include/core/SkTypeface.h"
#include "src/core/SkPaintPriv.h"
#include "src/core/SkSafeMath.h"

class SkReadBuffer;
class SkWriteBuffer;

class SkTextBlobPriv {
 public:
  /**
   *  Serialize to a buffer.
   */
  static void Flatten(const SkTextBlob&, SkWriteBuffer&);

  /**
   *  Recreate an SkTextBlob that was serialized into a buffer.
   *
   *  @param  SkReadBuffer Serialized blob data.
   *  @return A new SkTextBlob representing the serialized data, or NULL if the buffer is
   *          invalid.
   */
  static sk_sp<SkTextBlob> MakeFromBuffer(SkReadBuffer&);
};

class SkTextBlobBuilderPriv {
 public:
  static const SkTextBlobBuilder::RunBuffer& AllocRunText(
      SkTextBlobBuilder* builder, const SkFont& font, int count, SkScalar x, SkScalar y,
      int textByteCount, SkString lang, const SkRect* bounds = nullptr) {
    return builder->allocRunText(font, count, x, y, textByteCount, lang, bounds);
  }
  static const SkTextBlobBuilder::RunBuffer& AllocRunTextPosH(
      SkTextBlobBuilder* builder, const SkFont& font, int count, SkScalar y, int textByteCount,
      SkString lang, const SkRect* bounds = nullptr) {
    return builder->allocRunTextPosH(font, count, y, textByteCount, lang, bounds);
  }
  static const SkTextBlobBuilder::RunBuffer& AllocRunTextPos(
      SkTextBlobBuilder* builder, const SkFont& font, int count, int textByteCount, SkString lang,
      const SkRect* bounds = nullptr) {
    return builder->allocRunTextPos(font, count, textByteCount, lang, bounds);
  }
};

//
// Textblob data is laid out into externally-managed storage as follows:
//
//    -----------------------------------------------------------------------------
//   | SkTextBlob | RunRecord | Glyphs[] | Pos[] | RunRecord | Glyphs[] | Pos[] | ...
//    -----------------------------------------------------------------------------
//
//  Each run record describes a text blob run, and can be used to determine the (implicit)
//  location of the following record.
//
// Extended Textblob runs have more data after the Pos[] array:
//
//    -------------------------------------------------------------------------
//    ... | RunRecord | Glyphs[] | Pos[] | TextSize | Clusters[] | Text[] | ...
//    -------------------------------------------------------------------------
//
// To determine the length of the extended run data, the TextSize must be read.
//
// Extended Textblob runs may be mixed with non-extended runs.

SkDEBUGCODE(static const unsigned kRunRecordMagic = 0xb10bcafe);

class SkTextBlob::RunRecord {
 public:
  RunRecord(
      uint32_t count, uint32_t textSize, const SkPoint& offset, const SkFont& font,
      GlyphPositioning pos) noexcept
      : fFont(font), fCount(count), fOffset(offset), fFlags(pos) {
    SkASSERT(static_cast<unsigned>(pos) <= Flags::kPositioning_Mask);

    SkDEBUGCODE(fMagic = kRunRecordMagic);
    if (textSize > 0) {
      fFlags |= kExtended_Flag;
      *this->textSizePtr() = textSize;
    }
  }

  uint32_t glyphCount() const noexcept { return fCount; }

  const SkPoint& offset() const noexcept { return fOffset; }

  const SkFont& font() const noexcept { return fFont; }

  GlyphPositioning positioning() const noexcept {
    return static_cast<GlyphPositioning>(fFlags & kPositioning_Mask);
  }

  uint16_t* glyphBuffer() const noexcept {
    static_assert(SkIsAlignPtr(sizeof(RunRecord)), "");
    // Glyphs are stored immediately following the record.
    return reinterpret_cast<uint16_t*>(const_cast<RunRecord*>(this) + 1);
  }

  // can be aliased with pointBuffer() or xformBuffer()
  SkScalar* posBuffer() const noexcept {
    // Position scalars follow the (aligned) glyph buffer.
    return reinterpret_cast<SkScalar*>(
        reinterpret_cast<uint8_t*>(this->glyphBuffer()) + SkAlign4(fCount * sizeof(uint16_t)));
  }

  // alias for posBuffer()
  SkPoint* pointBuffer() const noexcept {
    SkASSERT(this->positioning() == (GlyphPositioning)2);
    return reinterpret_cast<SkPoint*>(this->posBuffer());
  }

  // alias for posBuffer()
  SkRSXform* xformBuffer() const noexcept {
    SkASSERT(this->positioning() == (GlyphPositioning)3);
    return reinterpret_cast<SkRSXform*>(this->posBuffer());
  }

  uint32_t textSize() const noexcept { return isExtended() ? *this->textSizePtr() : 0; }

  uint32_t* clusterBuffer() const noexcept {
    // clusters follow the textSize.
    return isExtended() ? 1 + this->textSizePtr() : nullptr;
  }

  char* textBuffer() const noexcept {
    return isExtended() ? reinterpret_cast<char*>(this->clusterBuffer() + fCount) : nullptr;
  }

  bool isLastRun() const noexcept { return SkToBool(fFlags & kLast_Flag); }

  static size_t StorageSize(
      uint32_t glyphCount, uint32_t textSize, SkTextBlob::GlyphPositioning positioning,
      SkSafeMath* safe) noexcept;

  static const RunRecord* First(const SkTextBlob* blob) noexcept;

  static const RunRecord* Next(const RunRecord* run) noexcept;

  void validate(const uint8_t* storageTop) const noexcept;

 private:
  friend class SkTextBlobBuilder;

  enum Flags {
    kPositioning_Mask = 0x03,  // bits 0-1 reserved for positioning
    kLast_Flag = 0x04,         // set for the last blob run
    kExtended_Flag = 0x08,     // set for runs with text/cluster info
  };

  static const RunRecord* NextUnchecked(const RunRecord* run) noexcept;

  static size_t PosCount(
      uint32_t glyphCount, SkTextBlob::GlyphPositioning positioning, SkSafeMath* safe) noexcept;

  uint32_t* textSizePtr() const noexcept;

  void grow(uint32_t count) noexcept;

  bool isExtended() const noexcept { return fFlags & kExtended_Flag; }

  SkFont fFont;
  uint32_t fCount;
  SkPoint fOffset;
  uint32_t fFlags;

  SkDEBUGCODE(unsigned fMagic);
};

/**
 *  Iterate through all of the text runs of the text blob.  For example:
 *    for (SkTextBlobRunIterator it(blob); !it.done(); it.next()) {
 *         .....
 *    }
 */
class SkTextBlobRunIterator {
 public:
  SkTextBlobRunIterator(const SkTextBlob* blob) noexcept;

  enum GlyphPositioning : uint8_t {
    kDefault_Positioning = 0,     // Default glyph advances -- zero scalars per glyph.
    kHorizontal_Positioning = 1,  // Horizontal positioning -- one scalar per glyph.
    kFull_Positioning = 2,        // Point positioning -- two scalars per glyph.
    kRSXform_Positioning = 3,     // RSXform positioning -- four scalars per glyph.
  };

  bool done() const noexcept { return !fCurrentRun; }
  void next() noexcept;

  uint32_t glyphCount() const noexcept {
    SkASSERT(!this->done());
    return fCurrentRun->glyphCount();
  }
  const uint16_t* glyphs() const noexcept {
    SkASSERT(!this->done());
    return fCurrentRun->glyphBuffer();
  }
  const SkScalar* pos() const noexcept {
    SkASSERT(!this->done());
    return fCurrentRun->posBuffer();
  }
  // alias for pos()
  const SkPoint* points() const noexcept { return fCurrentRun->pointBuffer(); }
  // alias for pos()
  const SkRSXform* xforms() const noexcept { return fCurrentRun->xformBuffer(); }
  const SkPoint& offset() const noexcept {
    SkASSERT(!this->done());
    return fCurrentRun->offset();
  }
  const SkFont& font() const noexcept {
    SkASSERT(!this->done());
    return fCurrentRun->font();
  }
  GlyphPositioning positioning() const noexcept;
  uint32_t* clusters() const noexcept {
    SkASSERT(!this->done());
    return fCurrentRun->clusterBuffer();
  }
  uint32_t textSize() const noexcept {
    SkASSERT(!this->done());
    return fCurrentRun->textSize();
  }
  char* text() const noexcept {
    SkASSERT(!this->done());
    return fCurrentRun->textBuffer();
  }

  bool isLCD() const noexcept;

 private:
  const SkTextBlob::RunRecord* fCurrentRun;

  SkDEBUGCODE(uint8_t* fStorageTop);
};

#endif  // SkTextBlobPriv_DEFINED
