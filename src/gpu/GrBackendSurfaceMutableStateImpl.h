/*
 * Copyright 2020 Google LLC
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef GrBackendSurfaceMutableStateImpl_DEFINED
#define GrBackendSurfaceMutableStateImpl_DEFINED

#include "include/core/SkRefCnt.h"
#include "include/gpu/GrBackendSurfaceMutableState.h"

class GrBackendSurfaceMutableStateImpl : public SkRefCnt {
 public:
#ifdef SK_VULKAN
  GrBackendSurfaceMutableStateImpl(VkImageLayout layout, uint32_t queueFamilyIndex) noexcept
      : fState(layout, queueFamilyIndex) {}

  GrBackendSurfaceMutableStateImpl(GrVkSharedImageInfo sharedInfo) noexcept
      : fState(sharedInfo.getImageLayout(), sharedInfo.getQueueFamilyIndex()) {}
#endif

  void set(const GrBackendSurfaceMutableState& state) noexcept { fState = state; }

#ifdef SK_VULKAN
  VkImageLayout getImageLayout() const noexcept {
    SkASSERT(fState.fBackend == GrBackend::kVulkan);
    return fState.fVkState.getImageLayout();
  }

  void setImageLayout(VkImageLayout layout) noexcept {
    SkASSERT(fState.fBackend == GrBackend::kVulkan);
    fState.fVkState.setImageLayout(layout);
  }

  uint32_t getQueueFamilyIndex() const noexcept {
    SkASSERT(fState.fBackend == GrBackend::kVulkan);
    return fState.fVkState.getQueueFamilyIndex();
  }

  void setQueueFamilyIndex(uint32_t queueFamilyIndex) noexcept {
    SkASSERT(fState.fBackend == GrBackend::kVulkan);
    fState.fVkState.setQueueFamilyIndex(queueFamilyIndex);
  }

  const GrVkSharedImageInfo& getVkSharedImageInfo() noexcept {
    SkASSERT(fState.fBackend == GrBackend::kVulkan);
    return fState.fVkState;
  }
#endif

 private:
  GrBackendSurfaceMutableState fState;
};

#endif
