/*
 * Copyright 2021 Google LLC
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "src/gpu/graphite/Gpu.h"

#include "include/gpu/graphite/BackendTexture.h"
#include "include/gpu/graphite/TextureInfo.h"
#include "src/gpu/graphite/Caps.h"
#include "src/gpu/graphite/CommandBuffer.h"
#include "src/gpu/graphite/GpuWorkSubmission.h"
#include "src/gpu/graphite/Log.h"
#include "src/gpu/graphite/ResourceProvider.h"
#include "src/sksl/SkSLCompiler.h"

namespace skgpu::graphite {

// This constant determines how many OutstandingSubmissions are allocated together as a block in
// the deque. As such it needs to balance allocating too much memory vs. incurring
// allocation/deallocation thrashing. It should roughly correspond to the max number of outstanding
// submissions we expect to see.
static constexpr int kDefaultOutstandingAllocCnt = 8;

Gpu::Gpu(sk_sp<const Caps> caps)
    : fCaps(std::move(caps)),
      fOutstandingSubmissions(sizeof(OutstandingSubmission), kDefaultOutstandingAllocCnt) {
  // subclasses create their own subclassed resource provider
}

Gpu::~Gpu() {
  // TODO: add disconnect?

  // TODO: destroyResources instead?
  // TODO: how do we handle command buffers that haven't been submitted yet?
  this->checkForFinishedWork(SyncToCpu::kYes);
}

void Gpu::initCompiler() { fCompiler = std::make_unique<SkSL::Compiler>(fCaps->shaderCaps()); }

sk_sp<const Caps> Gpu::refCaps() const { return fCaps; }

bool Gpu::submit(sk_sp<CommandBuffer> commandBuffer) {
  if (!commandBuffer) {
    return false;
  }

#ifdef SK_DEBUG
  if (!commandBuffer->hasWork()) {
    SKGPU_LOG_W("Submitting empty command buffer!");
  }
#endif

  auto submission = this->onSubmit(std::move(commandBuffer));
  if (!submission) {
    return false;
  }

  new (fOutstandingSubmissions.push_back()) OutstandingSubmission(std::move(submission));
  return true;
}

void Gpu::checkForFinishedWork(SyncToCpu sync) {
  if (sync == SyncToCpu::kYes) {
    // wait for the last submission to finish
    OutstandingSubmission* back = (OutstandingSubmission*)fOutstandingSubmissions.back();
    if (back) {
      (*back)->waitUntilFinished(this);
    }
  }

  // Iterate over all the outstanding submissions to see if any have finished. The work
  // submissions are in order from oldest to newest, so we start at the front to check if they
  // have finished. If so we pop it off and move onto the next.
  // Repeat till we find a submission that has not finished yet (and all others afterwards are
  // also guaranteed to not have finished).
  OutstandingSubmission* front = (OutstandingSubmission*)fOutstandingSubmissions.front();
  while (front && (*front)->isFinished()) {
    // Make sure we remove before deleting as deletion might try to kick off another submit
    // (though hopefully *not* in Graphite).
    fOutstandingSubmissions.pop_front();
    // Since we used placement new we are responsible for calling the destructor manually.
    front->~OutstandingSubmission();
    front = (OutstandingSubmission*)fOutstandingSubmissions.front();
  }
  SkASSERT(sync == SyncToCpu::kNo || fOutstandingSubmissions.empty());
}

BackendTexture Gpu::createBackendTexture(SkISize dimensions, const TextureInfo& info) {
  if (dimensions.isEmpty() || dimensions.width() > this->caps()->maxTextureSize() ||
      dimensions.height() > this->caps()->maxTextureSize()) {
    return {};
  }

  return this->onCreateBackendTexture(dimensions, info);
}

void Gpu::deleteBackendTexture(BackendTexture& texture) {
  this->onDeleteBackendTexture(texture);
  // Invalidate the texture;
  texture = BackendTexture();
}

}  // namespace skgpu::graphite
