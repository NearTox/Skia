/*
 * Copyright 2019 Google LLC
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef GrSurfaceProxyView_DEFINED
#define GrSurfaceProxyView_DEFINED

#include "include/core/SkRefCnt.h"
#include "include/gpu/GrTypes.h"
#include "src/gpu/Swizzle.h"
#include "src/gpu/ganesh/GrRenderTargetProxy.h"
#include "src/gpu/ganesh/GrSurfaceProxy.h"
#include "src/gpu/ganesh/GrTextureProxy.h"

class GrSurfaceProxyView {
 public:
  constexpr GrSurfaceProxyView() noexcept = default;

  GrSurfaceProxyView(
      sk_sp<GrSurfaceProxy> proxy, GrSurfaceOrigin origin, skgpu::Swizzle swizzle) noexcept
      : fProxy(std::move(proxy)), fOrigin(origin), fSwizzle(swizzle) {}

  // This entry point is used when we don't care about the origin or the swizzle.
  explicit GrSurfaceProxyView(sk_sp<GrSurfaceProxy> proxy) noexcept 
      : fProxy(std::move(proxy)), fOrigin(kTopLeft_GrSurfaceOrigin) {}

  GrSurfaceProxyView(GrSurfaceProxyView&& view) noexcept = default;
  GrSurfaceProxyView(const GrSurfaceProxyView&) noexcept = default;

  explicit operator bool() const noexcept { return SkToBool(fProxy.get()); }

  GrSurfaceProxyView& operator=(const GrSurfaceProxyView&) noexcept = default;
  GrSurfaceProxyView& operator=(GrSurfaceProxyView&& view) noexcept = default;

  bool operator==(const GrSurfaceProxyView& view) const noexcept {
    return fProxy->uniqueID() == view.fProxy->uniqueID() && fOrigin == view.fOrigin &&
           fSwizzle == view.fSwizzle;
  }
  bool operator!=(const GrSurfaceProxyView& other) const noexcept { return !(*this == other); }

  int width() const noexcept { return this->proxy()->width(); }
  int height() const noexcept { return this->proxy()->height(); }
  SkISize dimensions() const noexcept { return this->proxy()->dimensions(); }

  GrMipmapped mipmapped() const {
    if (const GrTextureProxy* proxy = this->asTextureProxy()) {
      return proxy->mipmapped();
    }
    return GrMipmapped::kNo;
  }

  GrSurfaceProxy* proxy() const noexcept { return fProxy.get(); }
  sk_sp<GrSurfaceProxy> refProxy() const noexcept { return fProxy; }

  GrTextureProxy* asTextureProxy() const noexcept {
    if (!fProxy) {
      return nullptr;
    }
    return fProxy->asTextureProxy();
  }
  sk_sp<GrTextureProxy> asTextureProxyRef() const {
    return sk_ref_sp<GrTextureProxy>(this->asTextureProxy());
  }

  GrRenderTargetProxy* asRenderTargetProxy() const noexcept {
    if (!fProxy) {
      return nullptr;
    }
    return fProxy->asRenderTargetProxy();
  }

  sk_sp<GrRenderTargetProxy> asRenderTargetProxyRef() const {
    return sk_ref_sp<GrRenderTargetProxy>(this->asRenderTargetProxy());
  }

  GrSurfaceOrigin origin() const noexcept { return fOrigin; }
  skgpu::Swizzle swizzle() const noexcept { return fSwizzle; }

  void concatSwizzle(skgpu::Swizzle swizzle) noexcept {
    fSwizzle = skgpu::Swizzle::Concat(fSwizzle, swizzle);
  }

  GrSurfaceProxyView makeSwizzle(skgpu::Swizzle swizzle) const& {
    return {fProxy, fOrigin, skgpu::Swizzle::Concat(fSwizzle, swizzle)};
  }

  GrSurfaceProxyView makeSwizzle(skgpu::Swizzle swizzle) && {
    return {std::move(fProxy), fOrigin, skgpu::Swizzle::Concat(fSwizzle, swizzle)};
  }

  void reset() noexcept { *this = {}; }

  // Helper that copies a rect of a src view'' proxy and then creates a view for the copy with
  // the same origin and swizzle as the src view.
  static GrSurfaceProxyView Copy(
      GrRecordingContext* context, GrSurfaceProxyView src, GrMipmapped mipmapped, SkIRect srcRect,
      SkBackingFit fit, SkBudgeted budgeted) {
    auto copy = GrSurfaceProxy::Copy(
        context, src.refProxy(), src.origin(), mipmapped, srcRect, fit, budgeted);
    return {std::move(copy), src.origin(), src.swizzle()};
  }

  static GrSurfaceProxyView Copy(
      GrRecordingContext* rContext, GrSurfaceProxyView src, GrMipmapped mipmapped, SkBackingFit fit,
      SkBudgeted budgeted) {
    auto copy =
        GrSurfaceProxy::Copy(rContext, src.refProxy(), src.origin(), mipmapped, fit, budgeted);
    return {std::move(copy), src.origin(), src.swizzle()};
  }

  // This does not reset the origin or swizzle, so the View can still be used to access those
  // properties associated with the detached proxy.
  sk_sp<GrSurfaceProxy> detachProxy() noexcept { return std::move(fProxy); }

 private:
  sk_sp<GrSurfaceProxy> fProxy;
  GrSurfaceOrigin fOrigin = kTopLeft_GrSurfaceOrigin;
  skgpu::Swizzle fSwizzle;
};

#endif
