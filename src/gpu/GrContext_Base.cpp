/*
 * Copyright 2019 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "include/private/GrContext_Base.h"

#include "include/private/GrSkSLFPFactoryCache.h"
#include "src/gpu/GrBaseContextPriv.h"
#include "src/gpu/GrCaps.h"

static int32_t next_id() noexcept {
  static std::atomic<int32_t> nextID{1};
  int32_t id;
  do {
    id = nextID++;
  } while (id == SK_InvalidGenID);
  return id;
}

GrContext_Base::GrContext_Base(
    GrBackendApi backend, const GrContextOptions& options, uint32_t contextID) noexcept
    : fBackend(backend),
      fOptions(options),
      fContextID(SK_InvalidGenID == contextID ? next_id() : contextID) {}

GrContext_Base::~GrContext_Base() {}

bool GrContext_Base::init(sk_sp<const GrCaps> caps, sk_sp<GrSkSLFPFactoryCache> FPFactoryCache) {
  SkASSERT(caps && FPFactoryCache);

  fCaps = caps;
  fFPFactoryCache = FPFactoryCache;
  return true;
}

const GrCaps* GrContext_Base::caps() const noexcept { return fCaps.get(); }
sk_sp<const GrCaps> GrContext_Base::refCaps() const noexcept { return fCaps; }

sk_sp<GrSkSLFPFactoryCache> GrContext_Base::fpFactoryCache() noexcept { return fFPFactoryCache; }

///////////////////////////////////////////////////////////////////////////////////////////////////
sk_sp<const GrCaps> GrBaseContextPriv::refCaps() const noexcept { return fContext->refCaps(); }

sk_sp<GrSkSLFPFactoryCache> GrBaseContextPriv::fpFactoryCache() noexcept {
  return fContext->fpFactoryCache();
}
