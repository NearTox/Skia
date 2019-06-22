/*
 * Copyright 2017 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef GrMockGpu_DEFINED
#define GrMockGpu_DEFINED

#include "include/gpu/GrRenderTarget.h"
#include "include/gpu/GrTexture.h"
#include "include/private/SkTHash.h"
#include "src/gpu/GrGpu.h"
#include "src/gpu/GrSemaphore.h"

class GrMockGpuRTCommandBuffer;
struct GrMockOptions;
class GrPipeline;

class GrMockGpu : public GrGpu {
 public:
  static sk_sp<GrGpu> Make(const GrMockOptions*, const GrContextOptions&, GrContext*);

  ~GrMockGpu() override {}

  GrGpuRTCommandBuffer* getCommandBuffer(
      GrRenderTarget*, GrSurfaceOrigin, const SkRect&,
      const GrGpuRTCommandBuffer::LoadAndStoreInfo&,
      const GrGpuRTCommandBuffer::StencilLoadAndStoreInfo&) override;

  GrGpuTextureCommandBuffer* getCommandBuffer(GrTexture*, GrSurfaceOrigin) override;

  GrFence SK_WARN_UNUSED_RESULT insertFence() noexcept override { return 0; }
  bool waitFence(GrFence, uint64_t) noexcept override { return true; }
  void deleteFence(GrFence) const noexcept override {}

  sk_sp<GrSemaphore> SK_WARN_UNUSED_RESULT makeSemaphore(bool isOwned) noexcept override {
    return nullptr;
  }
  sk_sp<GrSemaphore> wrapBackendSemaphore(
      const GrBackendSemaphore& semaphore, GrResourceProvider::SemaphoreWrapType wrapType,
      GrWrapOwnership ownership) noexcept override {
    return nullptr;
  }
  void insertSemaphore(sk_sp<GrSemaphore> semaphore) noexcept override {}
  void waitSemaphore(sk_sp<GrSemaphore> semaphore) noexcept override {}
  sk_sp<GrSemaphore> prepareTextureForCrossContextUsage(GrTexture*) noexcept override {
    return nullptr;
  }

  void submit(GrGpuCommandBuffer* buffer) override;

  void checkFinishProcs() noexcept override {}

 private:
  GrMockGpu(GrContext* context, const GrMockOptions&, const GrContextOptions&);

  void submitCommandBuffer(const GrMockGpuRTCommandBuffer*) noexcept;

  void onResetContext(uint32_t resetBits) noexcept override {}

  void querySampleLocations(GrRenderTarget*, SkTArray<SkPoint>*) noexcept override {
    SkASSERT(!this->caps()->sampleLocationsSupport());
    SK_ABORT("Sample locations not implemented for mock GPU.");
  }

  void xferBarrier(GrRenderTarget*, GrXferBarrierType) noexcept override {}

  sk_sp<GrTexture> onCreateTexture(
      const GrSurfaceDesc&, SkBudgeted, const GrMipLevel[], int mipLevelCount) override;

  sk_sp<GrTexture> onWrapBackendTexture(
      const GrBackendTexture&, GrWrapOwnership, GrWrapCacheable, GrIOType) override;

  sk_sp<GrTexture> onWrapRenderableBackendTexture(
      const GrBackendTexture&, int sampleCnt, GrWrapOwnership, GrWrapCacheable) override;

  sk_sp<GrRenderTarget> onWrapBackendRenderTarget(const GrBackendRenderTarget&) override;

  sk_sp<GrRenderTarget> onWrapBackendTextureAsRenderTarget(
      const GrBackendTexture&, int sampleCnt) override;

  sk_sp<GrGpuBuffer> onCreateBuffer(
      size_t sizeInBytes, GrGpuBufferType, GrAccessPattern, const void*) override;

  bool onReadPixels(
      GrSurface* surface, int left, int top, int width, int height, GrColorType, void* buffer,
      size_t rowBytes) noexcept override {
    return true;
  }

  bool onWritePixels(
      GrSurface* surface, int left, int top, int width, int height, GrColorType,
      const GrMipLevel texels[], int mipLevelCount) noexcept override {
    return true;
  }

  bool onTransferPixelsTo(
      GrTexture* texture, int left, int top, int width, int height, GrColorType,
      GrGpuBuffer* transferBuffer, size_t offset, size_t rowBytes) noexcept override {
    return true;
  }
  bool onTransferPixelsFrom(
      GrSurface* surface, int left, int top, int width, int height, GrColorType,
      GrGpuBuffer* transferBuffer, size_t offset) noexcept override {
    return true;
  }
  bool onCopySurface(
      GrSurface* dst, GrSurfaceOrigin dstOrigin, GrSurface* src, GrSurfaceOrigin srcOrigin,
      const SkIRect& srcRect, const SkIPoint& dstPoint,
      bool canDiscardOutsideDstRect) noexcept override {
    return true;
  }

  bool onRegenerateMipMapLevels(GrTexture*) noexcept override { return true; }

  void onResolveRenderTarget(GrRenderTarget* target) noexcept override { return; }

  void onFinishFlush(
      GrSurfaceProxy*[], int n, SkSurface::BackendSurfaceAccess access, const GrFlushInfo& info,
      const GrPrepareForExternalIORequests&) noexcept override {
    if (info.fFinishedProc) {
      info.fFinishedProc(info.fFinishedContext);
    }
  }

  GrStencilAttachment* createStencilAttachmentForRenderTarget(
      const GrRenderTarget*, int width, int height) override;
  GrBackendTexture createBackendTexture(
      int w, int h, const GrBackendFormat&, GrMipMapped, GrRenderable, const void* pixels,
      size_t rowBytes, const SkColor4f& color = SkColors::kTransparent) override;
  void deleteBackendTexture(const GrBackendTexture&) override;

#if GR_TEST_UTILS
  bool isTestingOnlyBackendTexture(const GrBackendTexture&) const override;

  GrBackendRenderTarget createTestingOnlyBackendRenderTarget(int w, int h, GrColorType) override;
  void deleteTestingOnlyBackendRenderTarget(const GrBackendRenderTarget&) override;

  void testingOnly_flushGpuAndSync() override {}
#endif

  const GrMockOptions fMockOptions;

  static int NextInternalTextureID() noexcept;
  static int NextExternalTextureID() noexcept;
  static int NextInternalRenderTargetID() noexcept;
  static int NextExternalRenderTargetID() noexcept;

  SkTHashSet<int> fOutstandingTestingOnlyTextureIDs;

  typedef GrGpu INHERITED;
};

#endif
