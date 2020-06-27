/*
 * Copyright 2012 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef SkFontDescriptor_DEFINED
#define SkFontDescriptor_DEFINED

#include "include/core/SkStream.h"
#include "include/core/SkString.h"
#include "include/core/SkTypeface.h"
#include "include/private/SkFixed.h"
#include "include/private/SkNoncopyable.h"

class SkFontData {
 public:
  /** Makes a copy of the data in 'axis'. */
  SkFontData(
      std::unique_ptr<SkStreamAsset> stream, int index, const SkFixed axis[],
      int axisCount) noexcept
      : fStream(std::move(stream)), fIndex(index), fAxisCount(axisCount), fAxis(axisCount) {
    for (int i = 0; i < axisCount; ++i) {
      fAxis[i] = axis[i];
    }
  }
  SkFontData(const SkFontData& that)
      : fStream(that.fStream->duplicate()),
        fIndex(that.fIndex),
        fAxisCount(that.fAxisCount),
        fAxis(fAxisCount) {
    for (int i = 0; i < fAxisCount; ++i) {
      fAxis[i] = that.fAxis[i];
    }
  }
  bool hasStream() const noexcept { return fStream.get() != nullptr; }
  std::unique_ptr<SkStreamAsset> detachStream() noexcept { return std::move(fStream); }
  SkStreamAsset* getStream() noexcept { return fStream.get(); }
  SkStreamAsset const* getStream() const noexcept { return fStream.get(); }
  int getIndex() const noexcept { return fIndex; }
  int getAxisCount() const noexcept { return fAxisCount; }
  const SkFixed* getAxis() const noexcept { return fAxis.get(); }

 private:
  std::unique_ptr<SkStreamAsset> fStream;
  int fIndex;
  int fAxisCount;
  SkAutoSTMalloc<4, SkFixed> fAxis;
};

class SkFontDescriptor : SkNoncopyable {
 public:
  SkFontDescriptor();
  // Does not affect ownership of SkStream.
  static bool Deserialize(SkStream*, SkFontDescriptor* result);

  void serialize(SkWStream*) const;

  SkFontStyle getStyle() const noexcept { return fStyle; }
  void setStyle(SkFontStyle style) noexcept { fStyle = style; }

  const char* getFamilyName() const noexcept { return fFamilyName.c_str(); }
  const char* getFullName() const noexcept { return fFullName.c_str(); }
  const char* getPostscriptName() const noexcept { return fPostscriptName.c_str(); }
  bool hasFontData() const noexcept { return fFontData.get() != nullptr; }
  std::unique_ptr<SkFontData> detachFontData() noexcept { return std::move(fFontData); }

  void setFamilyName(const char* name) { fFamilyName.set(name); }
  void setFullName(const char* name) { fFullName.set(name); }
  void setPostscriptName(const char* name) { fPostscriptName.set(name); }
  /** Set the font data only if it is necessary for serialization. */
  void setFontData(std::unique_ptr<SkFontData> data) noexcept { fFontData = std::move(data); }

 private:
  SkString fFamilyName;
  SkString fFullName;
  SkString fPostscriptName;
  std::unique_ptr<SkFontData> fFontData;

  SkFontStyle fStyle;
};

#endif  // SkFontDescriptor_DEFINED
