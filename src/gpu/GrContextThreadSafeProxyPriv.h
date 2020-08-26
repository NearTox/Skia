/*
 * Copyright 2018 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef GrContextThreadSafeProxyPriv_DEFINED
#define GrContextThreadSafeProxyPriv_DEFINED

#include "include/gpu/GrContextThreadSafeProxy.h"
#include "include/private/GrContext_Base.h"

#include "src/gpu/GrCaps.h"

/**
 * Class that adds methods to GrContextThreadSafeProxy that are only intended for use internal to
 * Skia. This class is purely a privileged window into GrContextThreadSafeProxy. It should never
 * have additional data members or virtual methods.
 */
class GrContextThreadSafeProxyPriv {
 public:
  void init(sk_sp<const GrCaps> caps) const noexcept { fProxy->init(std::move(caps)); }

  bool matches(GrContext_Base* candidate) const {
    return fProxy == candidate->threadSafeProxy().get();
  }

  GrBackend backend() const noexcept { return fProxy->fBackend; }
  const GrContextOptions& options() const noexcept { return fProxy->fOptions; }
  uint32_t contextID() const noexcept { return fProxy->fContextID; }

  const GrCaps* caps() const noexcept { return fProxy->fCaps.get(); }
  sk_sp<const GrCaps> refCaps() const noexcept { return fProxy->fCaps; }

  void abandonContext() noexcept { fProxy->abandonContext(); }
  bool abandoned() const noexcept { return fProxy->abandoned(); }

  // GrContextThreadSafeProxyPriv
  static sk_sp<GrContextThreadSafeProxy> Make(GrBackendApi, const GrContextOptions&);

 private:
  explicit GrContextThreadSafeProxyPriv(GrContextThreadSafeProxy* proxy) noexcept : fProxy(proxy) {}
  GrContextThreadSafeProxyPriv(const GrContextThreadSafeProxy&) = delete;
  GrContextThreadSafeProxyPriv& operator=(const GrContextThreadSafeProxyPriv&) = delete;

  // No taking addresses of this type.
  const GrContextThreadSafeProxyPriv* operator&() const = delete;
  GrContextThreadSafeProxyPriv* operator&() = delete;

  GrContextThreadSafeProxy* fProxy;

  friend class GrContextThreadSafeProxy;  // to construct/copy this type.
};

inline GrContextThreadSafeProxyPriv GrContextThreadSafeProxy::priv() noexcept {
  return GrContextThreadSafeProxyPriv(this);
}

inline const GrContextThreadSafeProxyPriv GrContextThreadSafeProxy::priv() const noexcept {
  return GrContextThreadSafeProxyPriv(const_cast<GrContextThreadSafeProxy*>(this));
}

#endif
