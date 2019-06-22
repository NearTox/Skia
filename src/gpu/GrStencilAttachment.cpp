/*
 * Copyright 2011 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "src/gpu/GrStencilAttachment.h"
#include "include/private/GrResourceKey.h"

void GrStencilAttachment::ComputeSharedStencilAttachmentKey(
    int width, int height, int sampleCnt, GrUniqueKey* key) noexcept {
  static const GrUniqueKey::Domain kDomain = GrUniqueKey::GenerateDomain();
  GrUniqueKey::Builder builder(key, kDomain, 3);
  builder[0] = width;
  builder[1] = height;
  builder[2] = sampleCnt;
}
