/*
 * Copyright 2010 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "src/gpu/GrFixedClip.h"

#include "src/gpu/GrAppliedClip.h"
#include "src/gpu/GrRenderTargetContext.h"

bool GrFixedClip::quickContains(const SkRect& rect) const noexcept {
  if (fWindowRectsState.enabled()) {
    return false;
  }
  return !fScissorState.enabled() || GrClip::IsInsideClip(fScissorState.rect(), rect);
}

void GrFixedClip::getConservativeBounds(int w, int h, SkIRect* devResult, bool* iior) const
    noexcept {
  devResult->setXYWH(0, 0, w, h);
  if (fScissorState.enabled()) {
    if (!devResult->intersect(fScissorState.rect())) {
      devResult->setEmpty();
    }
  }
  if (iior) {
    *iior = true;
  }
}

bool GrFixedClip::isRRect(const SkRect& rtBounds, SkRRect* rr, GrAA* aa) const noexcept {
  if (fWindowRectsState.enabled()) {
    return false;
  }
  if (fScissorState.enabled()) {
    SkRect rect = SkRect::Make(fScissorState.rect());
    if (!rect.intersects(rtBounds)) {
      return false;
    }
    rr->setRect(rect);
    *aa = GrAA::kNo;
    return true;
  }
  return false;
};

bool GrFixedClip::apply(int rtWidth, int rtHeight, GrAppliedHardClip* out, SkRect* bounds) const
    noexcept {
  if (fScissorState.enabled()) {
    SkIRect tightScissor = SkIRect::MakeWH(rtWidth, rtHeight);
    if (!tightScissor.intersect(fScissorState.rect())) {
      return false;
    }
    if (IsOutsideClip(tightScissor, *bounds)) {
      return false;
    }
    if (!IsInsideClip(fScissorState.rect(), *bounds)) {
      out->addScissor(tightScissor, bounds);
    }
  }

  if (fWindowRectsState.enabled()) {
    out->addWindowRectangles(fWindowRectsState);
  }

  return true;
}

const GrFixedClip& GrFixedClip::Disabled() noexcept {
  static const GrFixedClip disabled = GrFixedClip();
  return disabled;
}
