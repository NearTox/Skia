/*
 * Copyright 2015 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef SkBigPicture_DEFINED
#define SkBigPicture_DEFINED

#include "include/core/SkPicture.h"
#include "include/core/SkRect.h"
#include "include/private/SkNoncopyable.h"
#include "include/private/SkOnce.h"
#include "include/private/SkTemplates.h"

class SkBBoxHierarchy;
class SkMatrix;
class SkRecord;

// An implementation of SkPicture supporting an arbitrary number of drawing commands.
class SkBigPicture final : public SkPicture {
 public:
  // An array of refcounted const SkPicture pointers.
  class SnapshotArray : ::SkNoncopyable {
   public:
    SnapshotArray(const SkPicture* pics[], int count) noexcept : fPics(pics), fCount(count) {}
    ~SnapshotArray() {
      for (int i = 0; i < fCount; i++) {
        fPics[i]->unref();
      }
    }

    const SkPicture* const* begin() const noexcept { return fPics; }
    int count() const noexcept { return fCount; }

   private:
    SkAutoTMalloc<const SkPicture*> fPics;
    int fCount;
  };

  SkBigPicture(
      const SkRect& cull, sk_sp<SkRecord>, std::unique_ptr<SnapshotArray>, sk_sp<SkBBoxHierarchy>,
      size_t approxBytesUsedBySubPictures) noexcept;

  // SkPicture overrides
  void playback(SkCanvas*, AbortCallback*) const override;
  SkRect cullRect() const noexcept override;
  int approximateOpCount() const noexcept override;
  size_t approximateBytesUsed() const noexcept override;
  const SkBigPicture* asSkBigPicture() const noexcept override { return this; }

  // Used by GrLayerHoister
  void partialPlayback(SkCanvas*, int start, int stop, const SkMatrix& initialCTM) const;
  // Used by GrRecordReplaceDraw
  const SkBBoxHierarchy* bbh() const noexcept { return fBBH.get(); }
  const SkRecord* record() const noexcept { return fRecord.get(); }

 private:
  int drawableCount() const noexcept;
  SkPicture const* const* drawablePicts() const noexcept;

  const SkRect fCullRect;
  const size_t fApproxBytesUsedBySubPictures;
  sk_sp<const SkRecord> fRecord;
  std::unique_ptr<const SnapshotArray> fDrawablePicts;
  sk_sp<const SkBBoxHierarchy> fBBH;
};

#endif  // SkBigPicture_DEFINED
