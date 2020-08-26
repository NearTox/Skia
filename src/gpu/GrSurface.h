/*
 * Copyright 2012 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef GrSurface_DEFINED
#define GrSurface_DEFINED

#include "include/core/SkImageInfo.h"
#include "include/core/SkRect.h"
#include "include/gpu/GrBackendSurface.h"
#include "include/gpu/GrTypes.h"
#include "src/gpu/GrGpuResource.h"

class GrRenderTarget;
class GrSurfacePriv;
class GrTexture;

class GrSurface : public GrGpuResource {
 public:
  /**
   * Retrieves the dimensions of the surface.
   */
  SkISize dimensions() const noexcept { return fDimensions; }

  /**
   * Retrieves the width of the surface.
   */
  int width() const noexcept { return fDimensions.width(); }

  /**
   * Retrieves the height of the surface.
   */
  int height() const noexcept { return fDimensions.height(); }

  /**
   * Helper that gets the width and height of the surface as a bounding rectangle.
   */
  SkRect getBoundsRect() const noexcept { return SkRect::Make(this->dimensions()); }

  virtual GrBackendFormat backendFormat() const noexcept = 0;

  void setRelease(sk_sp<GrRefCntedCallback> releaseHelper) {
    this->onSetRelease(releaseHelper);
    fReleaseHelper = std::move(releaseHelper);
  }

  // These match the definitions in SkImage, from whence they came.
  // TODO: Remove Chrome's need to call this on a GrTexture
  typedef void* ReleaseCtx;
  typedef void (*ReleaseProc)(ReleaseCtx);
  void setRelease(ReleaseProc proc, ReleaseCtx ctx) {
    sk_sp<GrRefCntedCallback> helper(new GrRefCntedCallback(proc, ctx));
    this->setRelease(std::move(helper));
  }

  /**
   * @return the texture associated with the surface, may be null.
   */
  virtual GrTexture* asTexture() noexcept { return nullptr; }
  virtual const GrTexture* asTexture() const noexcept { return nullptr; }

  /**
   * @return the render target underlying this surface, may be null.
   */
  virtual GrRenderTarget* asRenderTarget() noexcept { return nullptr; }
  virtual const GrRenderTarget* asRenderTarget() const noexcept { return nullptr; }

  /** Access methods that are only to be used within Skia code. */
  inline GrSurfacePriv surfacePriv() noexcept;
  inline const GrSurfacePriv surfacePriv() const noexcept;

  static size_t ComputeSize(
      const GrCaps&, const GrBackendFormat&, SkISize dimensions, int colorSamplesPerPixel,
      GrMipMapped, bool binSize = false) noexcept;

  /**
   * The pixel values of this surface cannot be modified (e.g. doesn't support write pixels or
   * MIP map level regen).
   */
  bool readOnly() const noexcept { return fSurfaceFlags & GrInternalSurfaceFlags::kReadOnly; }

  bool framebufferOnly() const noexcept {
    return fSurfaceFlags & GrInternalSurfaceFlags::kFramebufferOnly;
  }

  // Returns true if we are working with protected content.
  bool isProtected() const noexcept { return fIsProtected == GrProtected::kYes; }

  void setFramebufferOnly() noexcept {
    SkASSERT(this->asRenderTarget());
    fSurfaceFlags |= GrInternalSurfaceFlags::kFramebufferOnly;
  }

 protected:
  void setGLRTFBOIDIs0() noexcept {
    SkASSERT(!this->requiresManualMSAAResolve());
    SkASSERT(!this->asTexture());
    SkASSERT(this->asRenderTarget());
    fSurfaceFlags |= GrInternalSurfaceFlags::kGLRTFBOIDIs0;
  }
  bool glRTFBOIDis0() const noexcept {
    return fSurfaceFlags & GrInternalSurfaceFlags::kGLRTFBOIDIs0;
  }

  void setRequiresManualMSAAResolve() noexcept {
    SkASSERT(!this->glRTFBOIDis0());
    SkASSERT(this->asRenderTarget());
    fSurfaceFlags |= GrInternalSurfaceFlags::kRequiresManualMSAAResolve;
  }
  bool requiresManualMSAAResolve() const noexcept {
    return fSurfaceFlags & GrInternalSurfaceFlags::kRequiresManualMSAAResolve;
  }

  void setReadOnly() noexcept {
    SkASSERT(!this->asRenderTarget());
    fSurfaceFlags |= GrInternalSurfaceFlags::kReadOnly;
  }

  // Provides access to methods that should be public within Skia code.
  friend class GrSurfacePriv;

  GrSurface(GrGpu* gpu, const SkISize& dimensions, GrProtected isProtected) noexcept
      : INHERITED(gpu),
        fDimensions(dimensions),
        fSurfaceFlags(GrInternalSurfaceFlags::kNone),
        fIsProtected(isProtected) {}

  ~GrSurface() override {
    // check that invokeReleaseProc has been called (if needed)
    SkASSERT(!fReleaseHelper);
  }

  void onRelease() override;
  void onAbandon() override;

 private:
  const char* getResourceType() const noexcept override { return "Surface"; }

  // Unmanaged backends (e.g. Vulkan) may want to specially handle the release proc in order to
  // ensure it isn't called until GPU work related to the resource is completed.
  virtual void onSetRelease(sk_sp<GrRefCntedCallback>) {}

  void invokeReleaseProc() noexcept {
    // Depending on the ref count of fReleaseHelper this may or may not actually trigger the
    // ReleaseProc to be called.
    fReleaseHelper.reset();
  }

  SkISize fDimensions;
  GrInternalSurfaceFlags fSurfaceFlags;
  GrProtected fIsProtected;
  sk_sp<GrRefCntedCallback> fReleaseHelper;

  typedef GrGpuResource INHERITED;
};

#endif
