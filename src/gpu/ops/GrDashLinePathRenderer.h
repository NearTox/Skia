/*
 * Copyright 2015 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef GrDashLinePathRenderer_DEFINED
#define GrDashLinePathRenderer_DEFINED

#include "src/gpu/GrPathRenderer.h"

class GrGpu;

class GrDashLinePathRenderer : public GrPathRenderer {
 private:
  const char* name() const final { return "DashLine"; }

  CanDrawPath onCanDrawPath(const CanDrawPathArgs&) const override;

  StencilSupport onGetStencilSupport(const GrStyledShape&) const override {
    return kNoSupport_StencilSupport;
  }

  bool onDrawPath(const DrawPathArgs&) override;

  sk_sp<GrGpu> fGpu;
  using INHERITED = GrPathRenderer;
};

#endif
