/*
 * Copyright 2017 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef GrMockGpu_DEFINED
#define GrMockGpu_DEFINED

#include "include/private/SkTHash.h"
#include "src/gpu/GrGpu.h"
#include "src/gpu/GrRenderTarget.h"
#include "src/gpu/GrSemaphore.h"
#include "src/gpu/GrTexture.h"

class GrMockOpsRenderPass;
struct GrMockOptions;
class GrPipeline;

class GrMockGpu : public GrGpu {
 public:
  static sk_sp<GrGpu> Make(const GrMockOptions*, const GrContextOptions&, GrContext*);

  ~GrMockGpu() override {}

  GrOpsRenderPass* getOpsRenderPass(
      GrRenderTarget*, GrStencilAttachment*, GrSurfaceOrigin, const SkIRect&,
      const GrOpsRenderPass::LoadAndStoreInfo&, const GrOpsRenderPass::StencilLoadAndStoreInfo&,
      const SkTArray<GrSurfaceProxy*, true>& sampledProxies) override;

  GrFence SK_WARN_UNUSED_RESULT insertFence() noexcept override { return 0; }
  bool waitFence(GrFence) noexcept override { return true; }
  void deleteFence(GrFence) const noexcept override {}

  std::unique_ptr<GrSemaphore> SK_WARN_UNUSED_RESULT makeSemaphore(bool isOwned) noexcept override {
    return nullptr;
  }
  std::unique_ptr<GrSemaphore> wrapBackendSemaphore(
      const GrBackendSemaphore& semaphore, GrResourceProvider::SemaphoreWrapType wrapType,
      GrWrapOwnership ownership) noexcept override {
    return nullptr;
  }
  void insertSemaphore(GrSemaphore* semaphore) noexcept override {}
  void waitSemaphore(GrSemaphore* semaphore) noexcept override {}
  std::unique_ptr<GrSemaphore> prepareTextureForCrossContextUsage(GrTexture*) noexcept override {
    return nullptr;
  }

  void submit(GrOpsRenderPass* renderPass) override;

  void checkFinishProcs() noexcept override {}

 private:
  GrMockGpu(GrContext* context, const GrMockOptions&, const GrContextOptions&);

  void onResetContext(uint32_t resetBits) noexcept override {}

  void querySampleLocations(GrRenderTarget*, SkTArray<SkPoint>* sampleLocations) override;

  void xferBarrier(GrRenderTarget*, GrXferBarrierType) noexcept override {}

  sk_sp<GrTexture> onCreateTexture(
      SkISize, const GrBackendFormat&, GrRenderable, int renderTargetSampleCnt, SkBudgeted,
      GrProtected, int mipLevelCount, uint32_t levelClearMask) override;

  sk_sp<GrTexture> onCreateCompressedTexture(
      SkISize dimensions, const GrBackendFormat&, SkBudgeted, GrMipMapped, GrProtected,
      const void* data, size_t dataSize) override;

  sk_sp<GrTexture> onWrapBackendTexture(
      const GrBackendTexture&, GrWrapOwnership, GrWrapCacheable, GrIOType) override;
  sk_sp<GrTexture> onWrapCompressedBackendTexture(
      const GrBackendTexture&, GrWrapOwnership, GrWrapCacheable) override;

  sk_sp<GrTexture> onWrapRenderableBackendTexture(
      const GrBackendTexture&, int sampleCnt, GrWrapOwnership, GrWrapCacheable) override;

  sk_sp<GrRenderTarget> onWrapBackendRenderTarget(const GrBackendRenderTarget&) override;

  sk_sp<GrRenderTarget> onWrapBackendTextureAsRenderTarget(
      const GrBackendTexture&, int sampleCnt) override;

  sk_sp<GrGpuBuffer> onCreateBuffer(
      size_t sizeInBytes, GrGpuBufferType, GrAccessPattern, const void*) override;

  bool onReadPixels(
      GrSurface* surface, int left, int top, int width, int height, GrColorType surfaceColorType,
      GrColorType dstColorType, void* buffer, size_t rowBytes) noexcept override {
    return true;
  }

  bool onWritePixels(
      GrSurface* surface, int left, int top, int width, int height, GrColorType surfaceColorType,
      GrColorType srcColorType, const GrMipLevel texels[], int mipLevelCount,
      bool prepForTexSampling) noexcept override {
    return true;
  }

  bool onTransferPixelsTo(
      GrTexture* texture, int left, int top, int width, int height, GrColorType surfaceColorType,
      GrColorType bufferColorType, GrGpuBuffer* transferBuffer, size_t offset,
      size_t rowBytes) noexcept override {
    return true;
  }
  bool onTransferPixelsFrom(
      GrSurface* surface, int left, int top, int width, int height, GrColorType surfaceColorType,
      GrColorType bufferColorType, GrGpuBuffer* transferBuffer, size_t offset) noexcept override {
    return true;
  }
  bool onCopySurface(
      GrSurface* dst, GrSurface* src, const SkIRect& srcRect,
      const SkIPoint& dstPoint) noexcept override {
    return true;
  }

  bool onRegenerateMipMapLevels(GrTexture*) noexcept override { return true; }

  void onResolveRenderTarget(
      GrRenderTarget* target, const SkIRect&, ForExternalIO) noexcept override {}

  void addFinishedProc(
      GrGpuFinishedProc finishedProc, GrGpuFinishedContext finishedContext) noexcept override {
    SkASSERT(finishedProc);
    finishedProc(finishedContext);
  }

  bool onSubmitToGpu(bool syncCpu) noexcept override { return true; }

  GrStencilAttachment* createStencilAttachmentForRenderTarget(
      const GrRenderTarget*, int width, int height, int numStencilSamples) override;
  GrBackendTexture onCreateBackendTexture(
      SkISize dimensions, const GrBackendFormat&, GrRenderable, GrMipMapped, GrProtected) override;

  bool onUpdateBackendTexture(
      const GrBackendTexture&, sk_sp<GrRefCntedCallback> finishedCallback,
      const BackendTextureData*) noexcept override {
    return true;
  }

  GrBackendTexture onCreateCompressedBackendTexture(
      SkISize dimensions, const GrBackendFormat&, GrMipMapped, GrProtected,
      sk_sp<GrRefCntedCallback> finishedCallback, const BackendTextureData*) override;
  void deleteBackendTexture(const GrBackendTexture&) override;

  bool compile(const GrProgramDesc&, const GrProgramInfo&) noexcept override { return false; }

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
