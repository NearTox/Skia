/*
 * Copyright 2011 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef GrStencilAttachment_DEFINED
#define GrStencilAttachment_DEFINED

#include "include/gpu/GrGpuResource.h"
#include "src/core/SkClipStack.h"

class GrRenderTarget;
class GrResourceKey;

class GrStencilAttachment : public GrGpuResource {
 public:
  ~GrStencilAttachment() override {
    // TODO: allow SB to be purged and detach itself from rts
  }

  int width() const noexcept { return fWidth; }
  int height() const noexcept { return fHeight; }
  int bits() const noexcept { return fBits; }
  int numSamples() const noexcept { return fSampleCnt; }
  bool isDirty() const noexcept { return fIsDirty; }

  void cleared() noexcept { fIsDirty = false; }

  // We create a unique stencil buffer at each width, height and sampleCnt and share it for
  // all render targets that require a stencil with those params.
  static void ComputeSharedStencilAttachmentKey(
      int width, int height, int sampleCnt, GrUniqueKey* key) noexcept;

 protected:
  GrStencilAttachment(GrGpu* gpu, int width, int height, int bits, int sampleCnt) noexcept
      : INHERITED(gpu),
        fWidth(width),
        fHeight(height),
        fBits(bits),
        fSampleCnt(sampleCnt),
        fIsDirty(true) {}

 private:
  const char* getResourceType() const noexcept override { return "Stencil"; }

  int fWidth;
  int fHeight;
  int fBits;
  int fSampleCnt;
  bool fIsDirty;

  typedef GrGpuResource INHERITED;
};

#endif
