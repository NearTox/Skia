/*
 * Copyright 2015 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef DashLinePathRenderer_DEFINED
#define DashLinePathRenderer_DEFINED

#include "src/gpu/ganesh/v1/PathRenderer.h"

class GrGpu;

namespace skgpu::v1 {

class DashLinePathRenderer final : public PathRenderer {
 public:
  constexpr DashLinePathRenderer() noexcept = default;

  const char* name() const noexcept override { return "DashLine"; }

 private:
  CanDrawPath onCanDrawPath(const CanDrawPathArgs&) const override;

  StencilSupport onGetStencilSupport(const GrStyledShape&) const override {
    return kNoSupport_StencilSupport;
  }

  bool onDrawPath(const DrawPathArgs&) override;

  sk_sp<GrGpu> fGpu;
};

}  // namespace skgpu::v1

#endif  // DashLinePathRenderer_DEFINED
