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
  GrFixedClip() noexcept = default;
  explicit GrFixedClip(const SkIRect& scissorRect) noexcept : fScissorState(scissorRect) {}

  const GrScissorState& scissorState() const noexcept { return fScissorState; }
  bool scissorEnabled() const noexcept { return fScissorState.enabled(); }
  const SkIRect& scissorRect() const noexcept {
    SkASSERT(scissorEnabled());
    return fScissorState.rect();
  }

  void disableScissor() noexcept { fScissorState.setDisabled(); }

  void setScissor(const SkIRect& irect) noexcept { fScissorState.set(irect); }
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
  void getConservativeBounds(int w, int h, SkIRect* devResult, bool* iior) const override;
  bool isRRect(const SkRect& rtBounds, SkRRect* rr, GrAA*) const override;
  bool apply(int rtWidth, int rtHeight, GrAppliedHardClip*, SkRect*) const override;

  static const GrFixedClip& Disabled();

 private:
  GrScissorState fScissorState;
  GrWindowRectsState fWindowRectsState;
};

#endif
