/*
 * Copyright 2017 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef GrMockGpuCommandBuffer_DEFINED
#define GrMockGpuCommandBuffer_DEFINED

#include "src/gpu/GrGpuCommandBuffer.h"
#include "src/gpu/mock/GrMockGpu.h"

class GrMockGpuTextureCommandBuffer : public GrGpuTextureCommandBuffer {
 public:
  GrMockGpuTextureCommandBuffer(GrTexture* texture, GrSurfaceOrigin origin) noexcept
      : INHERITED(texture, origin) {}

  ~GrMockGpuTextureCommandBuffer() override {}

  void copy(
      GrSurface* src, GrSurfaceOrigin srcOrigin, const SkIRect& srcRect,
      const SkIPoint& dstPoint) noexcept override {}
  void transferFrom(
      const SkIRect& srcRect, GrColorType bufferColorType, GrGpuBuffer* transferBuffer,
      size_t offset) noexcept override {}
  void insertEventMarker(const char*) noexcept override {}

 private:
  typedef GrGpuTextureCommandBuffer INHERITED;
};

class GrMockGpuRTCommandBuffer : public GrGpuRTCommandBuffer {
 public:
  GrMockGpuRTCommandBuffer(GrMockGpu* gpu, GrRenderTarget* rt, GrSurfaceOrigin origin) noexcept
      : INHERITED(rt, origin), fGpu(gpu) {}

  GrGpu* gpu() noexcept override { return fGpu; }
  void inlineUpload(GrOpFlushState*, GrDeferredTextureUploadFn&) noexcept override {}
  void discard() noexcept override {}
  void insertEventMarker(const char*) override {}
  void begin() noexcept override {}
  void end() noexcept override {}
  void copy(
      GrSurface* src, GrSurfaceOrigin srcOrigin, const SkIRect& srcRect,
      const SkIPoint& dstPoint) noexcept override {}
  void transferFrom(
      const SkIRect& srcRect, GrColorType bufferColorType, GrGpuBuffer* transferBuffer,
      size_t offset) noexcept override {}

  int numDraws() const noexcept { return fNumDraws; }

 private:
  void onDraw(
      const GrPrimitiveProcessor&, const GrPipeline&, const GrPipeline::FixedDynamicState*,
      const GrPipeline::DynamicStateArrays*, const GrMesh[], int meshCount,
      const SkRect& bounds) noexcept override {
    ++fNumDraws;
  }
  void onClear(const GrFixedClip&, const SkPMColor4f&) noexcept override {}
  void onClearStencilClip(const GrFixedClip&, bool insideStencilMask) noexcept override {}

  GrMockGpu* fGpu;
  int fNumDraws = 0;

  typedef GrGpuRTCommandBuffer INHERITED;
};

#endif
