/*
 * Copyright 2018 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "include/gpu/GrDriverBugWorkarounds.h"

#include "include/core/SkTypes.h"

GrDriverBugWorkarounds::GrDriverBugWorkarounds() noexcept = default;

GrDriverBugWorkarounds::GrDriverBugWorkarounds(
    const std::vector<int>& enabled_driver_bug_workarounds) noexcept {
  for (auto id : enabled_driver_bug_workarounds) {
    switch (id) {
#define GPU_OP(type, name) \
  case GrDriverBugWorkaroundType::type: name = true; break;

      GPU_DRIVER_BUG_WORKAROUNDS(GPU_OP)
#undef GPU_OP
      default: SK_ABORT("Not implemented"); break;
    }
  }
}

void GrDriverBugWorkarounds::applyOverrides(const GrDriverBugWorkarounds& workarounds) noexcept {
#define GPU_OP(type, name) name |= workarounds.name;

  GPU_DRIVER_BUG_WORKAROUNDS(GPU_OP)
#undef GPU_OP
}

GrDriverBugWorkarounds::~GrDriverBugWorkarounds() = default;
