/*
 * Copyright 2017 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef GrSurfaceProxyPriv_DEFINED
#define GrSurfaceProxyPriv_DEFINED

#include "src/gpu/GrSurfaceProxy.h"

#include "src/gpu/GrResourceProvider.h"

/** Class that adds methods to GrSurfaceProxy that are only intended for use internal to Skia.
    This class is purely a privileged window into GrSurfaceProxy. It should never have additional
    data members or virtual methods. */
class GrSurfaceProxyPriv {
 public:
  void computeScratchKey(const GrCaps& caps, GrScratchKey* key) const {
    return fProxy->computeScratchKey(caps, key);
  }

  // Create a GrSurface-derived class that meets the requirements (i.e, desc, renderability)
  // of the GrSurfaceProxy.
  sk_sp<GrSurface> createSurface(GrResourceProvider* resourceProvider) const {
    return fProxy->createSurface(resourceProvider);
  }

  // Assign this proxy the provided GrSurface as its backing surface
  void assign(sk_sp<GrSurface> surface) { fProxy->assign(std::move(surface)); }

  // Don't abuse this call!!!!!!!
  bool isExact() const noexcept { return SkBackingFit::kExact == fProxy->fFit; }

  // Don't. Just don't.
  void exactify(bool allocatedCaseOnly) noexcept;

  void setLazyDimensions(SkISize dimensions) noexcept { fProxy->setLazyDimensions(dimensions); }

  bool doLazyInstantiation(GrResourceProvider*);

 private:
  explicit GrSurfaceProxyPriv(GrSurfaceProxy* proxy) noexcept : fProxy(proxy) {}
  GrSurfaceProxyPriv(const GrSurfaceProxyPriv&) = delete;             // unimpl
  GrSurfaceProxyPriv& operator=(const GrSurfaceProxyPriv&) = delete;  // unimpl

  // No taking addresses of this type.
  const GrSurfaceProxyPriv* operator&() const = delete;
  GrSurfaceProxyPriv* operator&() = delete;

  GrSurfaceProxy* fProxy;

  friend class GrSurfaceProxy;  // to construct/copy this type.
};

inline GrSurfaceProxyPriv GrSurfaceProxy::priv() noexcept { return GrSurfaceProxyPriv(this); }

inline const GrSurfaceProxyPriv GrSurfaceProxy::priv() const noexcept {
  return GrSurfaceProxyPriv(const_cast<GrSurfaceProxy*>(this));
}

#endif
