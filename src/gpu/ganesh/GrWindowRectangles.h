/*
 * Copyright 2016 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef GrWindowRectangles_DEFINED
#define GrWindowRectangles_DEFINED

#include "include/core/SkRect.h"
#include "src/gpu/ganesh/GrNonAtomicRef.h"

class GrWindowRectangles {
 public:
  constexpr static int kMaxWindows = 8;

  GrWindowRectangles() : fCount(0) {}
  GrWindowRectangles(const GrWindowRectangles& that) noexcept : fCount(0) { *this = that; }
  ~GrWindowRectangles();

  GrWindowRectangles makeOffset(int dx, int dy) const;

  bool empty() const noexcept { return !fCount; }
  int count() const noexcept { return fCount; }
  const SkIRect* data() const noexcept;

  void reset() noexcept;
  GrWindowRectangles& operator=(const GrWindowRectangles&) noexcept;

  SkIRect& addWindow(const SkIRect& window) { return this->addWindow() = window; }
  SkIRect& addWindow();

  bool operator!=(const GrWindowRectangles& that) const noexcept { return !(*this == that); }
  bool operator==(const GrWindowRectangles&) const noexcept;

 private:
  struct Rec;

  const Rec* rec() const noexcept { return fCount <= 1 ? nullptr : fRec; }

  int fCount;
  union {
    SkIRect fLocalWindow;  // If fCount <= 1
    Rec* fRec;             // If fCount >  1.
  };
};

struct GrWindowRectangles::Rec : public GrNonAtomicRef<Rec> {
  Rec(const SkIRect* windows, int numWindows) {
    SkASSERT(numWindows <= kMaxWindows);
    memcpy(fData, windows, sizeof(SkIRect) * numWindows);
  }
  Rec() = default;

  SkIRect fData[kMaxWindows];
};

inline GrWindowRectangles::~GrWindowRectangles() { SkSafeUnref(this->rec()); }

inline const SkIRect* GrWindowRectangles::data() const noexcept {
  return fCount <= 1 ? &fLocalWindow : fRec->fData;
}

inline void GrWindowRectangles::reset() noexcept {
  SkSafeUnref(this->rec());
  fCount = 0;
}

inline GrWindowRectangles& GrWindowRectangles::operator=(const GrWindowRectangles& that) noexcept {
  SkSafeUnref(this->rec());
  fCount = that.fCount;
  if (fCount <= 1) {
    fLocalWindow = that.fLocalWindow;
  } else {
    fRec = SkRef(that.fRec);
  }
  return *this;
}

inline GrWindowRectangles GrWindowRectangles::makeOffset(int dx, int dy) const {
  if (!dx && !dy) {
    return *this;
  }
  GrWindowRectangles result;
  result.fCount = fCount;
  SkIRect* windows;
  if (result.fCount > 1) {
    result.fRec = new Rec();
    windows = result.fRec->fData;
  } else {
    windows = &result.fLocalWindow;
  }
  for (int i = 0; i < fCount; ++i) {
    windows[i] = this->data()[i].makeOffset(dx, dy);
  }
  return result;
}

inline SkIRect& GrWindowRectangles::addWindow() {
  SkASSERT(fCount < kMaxWindows);
  if (fCount == 0) {
    fCount = 1;
    return fLocalWindow;
  }
  if (fCount == 1) {
    fRec = new Rec(&fLocalWindow, 1);
  } else if (!fRec->unique()) {  // Simple copy-on-write.
    fRec->unref();
    fRec = new Rec(fRec->fData, fCount);
  }
  return fRec->fData[fCount++];
}

inline bool GrWindowRectangles::operator==(const GrWindowRectangles& that) const noexcept {
  if (fCount != that.fCount) {
    return false;
  }
  if (fCount > 1 && fRec == that.fRec) {
    return true;
  }
  return !fCount || !memcmp(this->data(), that.data(), sizeof(SkIRect) * fCount);
}

#endif
