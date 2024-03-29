/*
 * Copyright 2021 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "src/gpu/graphite/mtl/MtlBuffer.h"

#include "src/gpu/graphite/mtl/MtlGpu.h"

namespace skgpu::graphite {

#ifdef SK_ENABLE_MTL_DEBUG_INFO
NSString* kBufferTypeNames[kBufferTypeCount] = {
    @"Vertex", @"Index", @"Xfer CPU to GPU", @"Xfer GPU to CPU", @"Uniform",
};
#endif

sk_sp<Buffer> MtlBuffer::Make(
    const MtlGpu* gpu, size_t size, BufferType type, PrioritizeGpuReads prioritizeGpuReads) {
  if (size <= 0) {
    return nullptr;
  }

  const MtlCaps& mtlCaps = gpu->mtlCaps();

  NSUInteger options = 0;
  if (@available(macOS 10.11, iOS 9.0, *)) {
    if (prioritizeGpuReads == PrioritizeGpuReads::kNo) {
#ifdef SK_BUILD_FOR_MAC
      if (mtlCaps.isMac()) {
        options |= MTLResourceStorageModeManaged;
      } else {
        SkASSERT(mtlCaps.isApple());
        options |= MTLResourceStorageModeShared;
      }
#else
      options |= MTLResourceStorageModeShared;
#endif
    } else {
      options |= MTLResourceStorageModePrivate;
    }
  }

  size = SkAlignTo(size, mtlCaps.getMinBufferAlignment());
  sk_cfp<id<MTLBuffer>> buffer([gpu->device() newBufferWithLength:size options:options]);
#ifdef SK_ENABLE_MTL_DEBUG_INFO
  (*buffer).label = kBufferTypeNames[(int)type];
#endif

  return sk_sp<Buffer>(new MtlBuffer(gpu, size, type, prioritizeGpuReads, std::move(buffer)));
}

MtlBuffer::MtlBuffer(
    const MtlGpu* gpu, size_t size, BufferType type, PrioritizeGpuReads prioritizeGpuReads,
    sk_cfp<id<MTLBuffer>> buffer)
    : Buffer(gpu, size, type, prioritizeGpuReads), fBuffer(std::move(buffer)) {}

void MtlBuffer::onMap() {
  SkASSERT(fBuffer);
  SkASSERT(!this->isMapped());

  if ((*fBuffer).storageMode == MTLStorageModePrivate) {
    return;
  }

  fMapPtr = static_cast<char*>((*fBuffer).contents);
}

void MtlBuffer::onUnmap() {
  SkASSERT(fBuffer);
  SkASSERT(this->isMapped());
#ifdef SK_BUILD_FOR_MAC
  if ((*fBuffer).storageMode == MTLStorageModeManaged) {
    [*fBuffer didModifyRange:NSMakeRange(0, this->size())];
  }
#endif
  fMapPtr = nullptr;
}

void MtlBuffer::freeGpuData() { fBuffer.reset(); }

}  // namespace skgpu::graphite
