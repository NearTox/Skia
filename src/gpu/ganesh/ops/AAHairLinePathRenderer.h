/*
 * Copyright 2011 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef AAHairLinePathRenderer_DEFINED
#define AAHairLinePathRenderer_DEFINED

#include "src/gpu/ganesh/v1/PathRenderer.h"

namespace skgpu::v1 {

class AAHairLinePathRenderer final : public PathRenderer {
 public:
  constexpr AAHairLinePathRenderer() noexcept = default;

  const char* name() const noexcept override { return "AAHairline"; }

 private:
  CanDrawPath onCanDrawPath(const CanDrawPathArgs&) const override;

  bool onDrawPath(const DrawPathArgs&) override;
};

}  // namespace skgpu::v1

#endif  // AAHairLinePathRenderer_DEFINED
