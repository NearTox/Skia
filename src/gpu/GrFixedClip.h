/*
 * Copyright 2016 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef GrFixedClip_DEFINED
#define GrFixedClip_DEFINED

#include "src/gpu/GrClip.h"
#include "src/gpu/GrScissorState.h"
#include "src/gpu/GrWindowRectsState.h"

/**
 * Implements GrHardClip with scissor and window rectangles.
 */
class GrFixedClip final : public GrHardClip {
 public:
  explicit GrFixedClip(const SkISize& rtDims) noexcept : fScissorState(rtDims) {}
  GrFixedClip(const SkISize& rtDims, const SkIRect& scissorRect) noexcept : GrFixedClip(rtDims) {
    SkAssertResult(fScissorState.set(scissorRect));
  }

  const GrScissorState& scissorState() const noexcept { return fScissorState; }
  bool scissorEnabled() const noexcept { return fScissorState.enabled(); }
  // Returns the scissor rect or rt bounds if the scissor test is not enabled.
  const SkIRect& scissorRect() const noexcept { return fScissorState.rect(); }

  void disableScissor() noexcept { fScissorState.setDisabled(); }

  bool SK_WARN_UNUSED_RESULT setScissor(const SkIRect& irect) noexcept {
    return fScissorState.set(irect);
  }
  bool SK_WARN_UNUSED_RESULT intersect(const SkIRect& irect) noexcept {
    return fScissorState.intersect(irect);
  }

  const GrWindowRectsState& windowRectsState() const noexcept { return fWindowRectsState; }
  bool hasWindowRectangles() const noexcept { return fWindowRectsState.enabled(); }

  void disableWindowRectangles() noexcept { fWindowRectsState.setDisabled(); }

  void setWindowRectangles(
      const GrWindowRectangles& windows, GrWindowRectsState::Mode mode) noexcept {
    fWindowRectsState.set(windows, mode);
  }

  bool quickContains(const SkRect&) const override;
  SkIRect getConservativeBounds() const override;
  bool isRRect(SkRRect* rr, GrAA*) const override;
  bool apply(GrAppliedHardClip*, SkRect*) const override;

 private:
  GrScissorState fScissorState;
  GrWindowRectsState fWindowRectsState;
};

#endif
