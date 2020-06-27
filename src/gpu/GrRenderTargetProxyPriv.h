/*
 * Copyright 2018 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef GrRenderTargetProxyPriv_DEFINED
#define GrRenderTargetProxyPriv_DEFINED

#include "src/gpu/GrRenderTargetProxy.h"

/**
 * This class hides the more specialized capabilities of GrRenderTargetProxy.
 */
class GrRenderTargetProxyPriv {
 public:
  void setGLRTFBOIDIs0() noexcept {
    // FBO0 should never be wrapped as a texture render target.
    SkASSERT(!fRenderTargetProxy->requiresManualMSAAResolve());
    SkASSERT(!fRenderTargetProxy->asTextureProxy());
    fRenderTargetProxy->setGLRTFBOIDIs0();
  }

  bool glRTFBOIDIs0() const noexcept { return fRenderTargetProxy->glRTFBOIDIs0(); }

 private:
  explicit GrRenderTargetProxyPriv(GrRenderTargetProxy* renderTargetProxy) noexcept
      : fRenderTargetProxy(renderTargetProxy) {}
  GrRenderTargetProxyPriv(const GrRenderTargetProxyPriv&) noexcept {}  // unimpl
  GrRenderTargetProxyPriv& operator=(const GrRenderTargetProxyPriv&);  // unimpl

  // No taking addresses of this type.
  const GrRenderTargetProxyPriv* operator&() const;
  GrRenderTargetProxyPriv* operator&();

  GrRenderTargetProxy* fRenderTargetProxy;

  friend class GrRenderTargetProxy;  // to construct/copy this type.
};

inline GrRenderTargetProxyPriv GrRenderTargetProxy::rtPriv() noexcept {
  return GrRenderTargetProxyPriv(this);
}

inline const GrRenderTargetProxyPriv GrRenderTargetProxy::rtPriv() const noexcept {
  return GrRenderTargetProxyPriv(const_cast<GrRenderTargetProxy*>(this));
}

#endif
