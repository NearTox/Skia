/*
 * Copyright 2016 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef GrAppliedClip_DEFINED
#define GrAppliedClip_DEFINED

#include "src/gpu/GrFragmentProcessor.h"
#include "src/gpu/GrScissorState.h"
#include "src/gpu/GrWindowRectsState.h"

#include "src/core/SkClipStack.h"

/**
 * Produced by GrHardClip. It provides a set of modifications to the hardware drawing state that
 * implement the clip.
 */
class GrAppliedHardClip {
 public:
  static const GrAppliedHardClip& Disabled() noexcept {
    // The size doesn't really matter here since it's returned as const& so an actual scissor
    // will never be set on it, and applied clips are not used to query or bounds test like
    // the GrClip is.
    static const GrAppliedHardClip kDisabled({1 << 29, 1 << 29});
    return kDisabled;
  }

  GrAppliedHardClip(const SkISize& rtDims) noexcept : fScissorState(rtDims) {}
  GrAppliedHardClip(const SkISize& logicalRTDims, const SkISize& backingStoreDims) noexcept
      : fScissorState(backingStoreDims) {
    fScissorState.set(SkIRect::MakeSize(logicalRTDims));
  }

  GrAppliedHardClip(GrAppliedHardClip&& that) noexcept = default;
  GrAppliedHardClip(const GrAppliedHardClip&) noexcept = delete;

  const GrScissorState& scissorState() const noexcept { return fScissorState; }
  const GrWindowRectsState& windowRectsState() const noexcept { return fWindowRectsState; }
  uint32_t stencilStackID() const noexcept { return fStencilStackID; }
  bool hasStencilClip() const noexcept { return SkClipStack::kInvalidGenID != fStencilStackID; }

  /**
   * Intersects the applied clip with the provided rect. Returns false if the draw became empty.
   * 'clippedDrawBounds' will be intersected with 'irect'. This returns false if the clip becomes
   * empty or the draw no longer intersects the clip. In either case the draw can be skipped.
   */
  bool addScissor(const SkIRect& irect, SkRect* clippedDrawBounds) noexcept {
    return fScissorState.intersect(irect) && clippedDrawBounds->intersect(SkRect::Make(irect));
  }

  void addWindowRectangles(const GrWindowRectsState& windowState) noexcept {
    SkASSERT(!fWindowRectsState.enabled());
    fWindowRectsState = windowState;
  }

  void addWindowRectangles(
      const GrWindowRectangles& windows, GrWindowRectsState::Mode mode) noexcept {
    SkASSERT(!fWindowRectsState.enabled());
    fWindowRectsState.set(windows, mode);
  }

  void addStencilClip(uint32_t stencilStackID) noexcept {
    SkASSERT(SkClipStack::kInvalidGenID == fStencilStackID);
    fStencilStackID = stencilStackID;
  }

  bool doesClip() const noexcept {
    return fScissorState.enabled() || this->hasStencilClip() || fWindowRectsState.enabled();
  }

  bool operator==(const GrAppliedHardClip& that) const noexcept {
    return fScissorState == that.fScissorState && fWindowRectsState == that.fWindowRectsState &&
           fStencilStackID == that.fStencilStackID;
  }
  bool operator!=(const GrAppliedHardClip& that) const noexcept { return !(*this == that); }

 private:
  GrScissorState fScissorState;
  GrWindowRectsState fWindowRectsState;
  uint32_t fStencilStackID = SkClipStack::kInvalidGenID;
};

/**
 * Produced by GrClip. It provides a set of modifications to GrPipeline that implement the clip.
 */
class GrAppliedClip {
 public:
  static GrAppliedClip Disabled() noexcept { return GrAppliedClip({1 << 29, 1 << 29}); }

  GrAppliedClip(const SkISize& rtDims) noexcept : fHardClip(rtDims) {}
  GrAppliedClip(const SkISize& logicalRTDims, const SkISize& backingStoreDims) noexcept
      : fHardClip(logicalRTDims, backingStoreDims) {}

  GrAppliedClip(GrAppliedClip&& that) noexcept = default;
  GrAppliedClip(const GrAppliedClip&) noexcept = delete;

  const GrScissorState& scissorState() const noexcept { return fHardClip.scissorState(); }
  const GrWindowRectsState& windowRectsState() const noexcept {
    return fHardClip.windowRectsState();
  }
  uint32_t stencilStackID() const noexcept { return fHardClip.stencilStackID(); }
  bool hasStencilClip() const noexcept { return fHardClip.hasStencilClip(); }
  int numClipCoverageFragmentProcessors() const noexcept { return fClipCoverageFPs.count(); }
  const GrFragmentProcessor* clipCoverageFragmentProcessor(int i) const noexcept {
    SkASSERT(fClipCoverageFPs[i]);
    return fClipCoverageFPs[i].get();
  }
  std::unique_ptr<const GrFragmentProcessor> detachClipCoverageFragmentProcessor(int i) noexcept {
    SkASSERT(fClipCoverageFPs[i]);
    return std::move(fClipCoverageFPs[i]);
  }

  const GrAppliedHardClip& hardClip() const noexcept { return fHardClip; }
  GrAppliedHardClip& hardClip() noexcept { return fHardClip; }

  void addCoverageFP(std::unique_ptr<GrFragmentProcessor> fp) noexcept {
    SkASSERT(fp);
    fClipCoverageFPs.push_back(std::move(fp));
  }

  bool doesClip() const noexcept { return fHardClip.doesClip() || !fClipCoverageFPs.empty(); }

  bool operator==(const GrAppliedClip& that) const {
    if (fHardClip != that.fHardClip || fClipCoverageFPs.count() != that.fClipCoverageFPs.count()) {
      return false;
    }
    for (int i = 0; i < fClipCoverageFPs.count(); ++i) {
      if (!fClipCoverageFPs[i] || !that.fClipCoverageFPs[i]) {
        if (fClipCoverageFPs[i] == that.fClipCoverageFPs[i]) {
          continue;  // Both are null.
        }
        return false;
      }
      if (!fClipCoverageFPs[i]->isEqual(*that.fClipCoverageFPs[i])) {
        return false;
      }
    }
    return true;
  }
  bool operator!=(const GrAppliedClip& that) const { return !(*this == that); }

  void visitProxies(const GrOp::VisitProxyFunc& func) const {
    for (const std::unique_ptr<GrFragmentProcessor>& fp : fClipCoverageFPs) {
      if (fp) {  // This might be called after detach.
        fp->visitProxies(func);
      }
    }
  }

 private:
  GrAppliedHardClip fHardClip;
  SkSTArray<4, std::unique_ptr<GrFragmentProcessor>> fClipCoverageFPs;
};

#endif
