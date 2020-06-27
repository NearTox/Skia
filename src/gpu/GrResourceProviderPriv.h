/*
 * Copyright 2018 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef GrResourceProviderPriv_DEFINED
#define GrResourceProviderPriv_DEFINED

#include "src/gpu/GrResourceProvider.h"

/** Class that adds methods to GrResourceProvider that are only intended for use internal to Skia.
    This class is purely a privileged window into GrResourceProvider. It should never have
    additional data members or virtual methods. */
class GrResourceProviderPriv {
 public:
  GrGpu* gpu() noexcept { return fResourceProvider->gpu(); }

 private:
  explicit GrResourceProviderPriv(GrResourceProvider* provider) noexcept
      : fResourceProvider(provider) {}
  GrResourceProviderPriv(const GrResourceProviderPriv&);             // unimpl
  GrResourceProviderPriv& operator=(const GrResourceProviderPriv&);  // unimpl

  // No taking addresses of this type.
  const GrResourceProviderPriv* operator&() const;
  GrResourceProviderPriv* operator&();

  GrResourceProvider* fResourceProvider;
  friend class GrResourceProvider;  // to construct/copy this type
};

inline GrResourceProviderPriv GrResourceProvider::priv() noexcept {
  return GrResourceProviderPriv(this);
}

inline const GrResourceProviderPriv GrResourceProvider::priv() const noexcept {
  return GrResourceProviderPriv(const_cast<GrResourceProvider*>(this));
}

#endif
