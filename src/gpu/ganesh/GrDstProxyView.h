/*
 * Copyright 2021 Google LLC
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef GrDstProxyView_DEFINED
#define GrDstProxyView_DEFINED

#include "include/gpu/GrTypes.h"
#include "include/private/gpu/ganesh/GrTypesPriv.h"
#include "src/gpu/ganesh/GrSurfaceProxyView.h"

/**
 * GrDstProxyView holds a texture containing the destination pixel values, and an integer-coordinate
 * offset from device-space to the space of the texture. When framebuffer fetch is not available, a
 * GrDstProxyView may be used to support blending in the fragment shader/xfer processor.
 */
class GrDstProxyView {
 public:
  GrDstProxyView() noexcept = default;

  GrDstProxyView(const GrDstProxyView& other) noexcept { *this = other; }

  GrDstProxyView& operator=(const GrDstProxyView& other) noexcept {
    fProxyView = other.fProxyView;
    fOffset = other.fOffset;
    fDstSampleFlags = other.fDstSampleFlags;
    return *this;
  }

  bool operator==(const GrDstProxyView& that) const noexcept {
    return fProxyView == that.fProxyView && fOffset == that.fOffset &&
           fDstSampleFlags == that.fDstSampleFlags;
  }
  bool operator!=(const GrDstProxyView& that) const noexcept { return !(*this == that); }

  const SkIPoint& offset() const noexcept { return fOffset; }

  void setOffset(const SkIPoint& offset) noexcept { fOffset = offset; }
  void setOffset(int ox, int oy) noexcept { fOffset.set(ox, oy); }

  GrSurfaceProxy* proxy() const noexcept { return fProxyView.proxy(); }
  const GrSurfaceProxyView& proxyView() const noexcept { return fProxyView; }

  void setProxyView(GrSurfaceProxyView view) noexcept {
    fProxyView = std::move(view);
    if (!fProxyView.proxy()) {
      fOffset = {0, 0};
    }
  }

  GrDstSampleFlags dstSampleFlags() const noexcept { return fDstSampleFlags; }

  void setDstSampleFlags(GrDstSampleFlags dstSampleFlags) noexcept {
    fDstSampleFlags = dstSampleFlags;
  }

 private:
  GrSurfaceProxyView fProxyView;
  SkIPoint fOffset = {0, 0};
  GrDstSampleFlags fDstSampleFlags = GrDstSampleFlags::kNone;
};

#endif
