/*
 * Copyright 2012 The Android Open Source Project
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "include/core/SkCanvas.h"
#include "include/core/SkColorFilter.h"
#include "include/effects/SkImageFilters.h"
#include "src/core/SkColorFilterBase.h"
#include "src/core/SkImageFilter_Base.h"
#include "src/core/SkReadBuffer.h"
#include "src/core/SkSpecialImage.h"
#include "src/core/SkSpecialSurface.h"
#include "src/core/SkWriteBuffer.h"

namespace {

class SkColorFilterImageFilter final : public SkImageFilter_Base {
 public:
  SkColorFilterImageFilter(
      sk_sp<SkColorFilter> cf, sk_sp<SkImageFilter> input, const SkRect* cropRect)
      : INHERITED(&input, 1, cropRect), fColorFilter(std::move(cf)) {}

 protected:
  void flatten(SkWriteBuffer&) const override;
  sk_sp<SkSpecialImage> onFilterImage(const Context&, SkIPoint* offset) const override;
  bool onIsColorFilterNode(SkColorFilter**) const override;
  MatrixCapability onGetCTMCapability() const override { return MatrixCapability::kComplex; }
  bool onAffectsTransparentBlack() const override;

 private:
  friend void ::SkRegisterColorFilterImageFilterFlattenable();
  SK_FLATTENABLE_HOOKS(SkColorFilterImageFilter)

  sk_sp<SkColorFilter> fColorFilter;

  using INHERITED = SkImageFilter_Base;
};

}  // end namespace

sk_sp<SkImageFilter> SkImageFilters::ColorFilter(
    sk_sp<SkColorFilter> cf, sk_sp<SkImageFilter> input, const CropRect& cropRect) {
  if (!cf) {
    return nullptr;
  }

  SkColorFilter* inputCF;
  if (input && input->isColorFilterNode(&inputCF)) {
    // This is an optimization, as it collapses the hierarchy by just combining the two
    // colorfilters into a single one, which the new imagefilter will wrap.
    sk_sp<SkColorFilter> newCF = cf->makeComposed(sk_sp<SkColorFilter>(inputCF));
    if (newCF) {
      return sk_sp<SkImageFilter>(
          new SkColorFilterImageFilter(std::move(newCF), sk_ref_sp(input->getInput(0)), cropRect));
    }
  }

  return sk_sp<SkImageFilter>(
      new SkColorFilterImageFilter(std::move(cf), std::move(input), cropRect));
}

void SkRegisterColorFilterImageFilterFlattenable() {
  SK_REGISTER_FLATTENABLE(SkColorFilterImageFilter);
  // TODO (michaelludwig) - Remove after grace period for SKPs to stop using old name
  SkFlattenable::Register("SkColorFilterImageFilterImpl", SkColorFilterImageFilter::CreateProc);
}

sk_sp<SkFlattenable> SkColorFilterImageFilter::CreateProc(SkReadBuffer& buffer) {
  SK_IMAGEFILTER_UNFLATTEN_COMMON(common, 1);
  sk_sp<SkColorFilter> cf(buffer.readColorFilter());
  return SkImageFilters::ColorFilter(std::move(cf), common.getInput(0), common.cropRect());
}

void SkColorFilterImageFilter::flatten(SkWriteBuffer& buffer) const {
  this->INHERITED::flatten(buffer);
  buffer.writeFlattenable(fColorFilter.get());
}

///////////////////////////////////////////////////////////////////////////////////////////////////

sk_sp<SkSpecialImage> SkColorFilterImageFilter::onFilterImage(
    const Context& ctx, SkIPoint* offset) const {
  SkIPoint inputOffset = SkIPoint::Make(0, 0);
  sk_sp<SkSpecialImage> input(this->filterInput(0, ctx, &inputOffset));

  SkIRect inputBounds;
  if (as_CFB(fColorFilter)->affectsTransparentBlack()) {
    // If the color filter affects transparent black, the bounds are the entire clip.
    inputBounds = ctx.clipBounds();
  } else if (!input) {
    return nullptr;
  } else {
    inputBounds =
        SkIRect::MakeXYWH(inputOffset.x(), inputOffset.y(), input->width(), input->height());
  }

  SkIRect bounds;
  if (!this->applyCropRect(ctx, inputBounds, &bounds)) {
    return nullptr;
  }

  sk_sp<SkSpecialSurface> surf(ctx.makeSurface(bounds.size()));
  if (!surf) {
    return nullptr;
  }

  SkCanvas* canvas = surf->getCanvas();
  SkASSERT(canvas);

  SkPaint paint;

  paint.setBlendMode(SkBlendMode::kSrc);
  paint.setColorFilter(fColorFilter);

  // TODO: it may not be necessary to clear or drawPaint inside the input bounds
  // (see skbug.com/5075)
  if (as_CFB(fColorFilter)->affectsTransparentBlack()) {
    // The subsequent input->draw() call may not fill the entire canvas. For filters which
    // affect transparent black, ensure that the filter is applied everywhere.
    paint.setColor(SK_ColorTRANSPARENT);
    canvas->drawPaint(paint);
    paint.setColor(SK_ColorBLACK);
  } else {
    canvas->clear(0x0);
  }

  if (input) {
    input->draw(
        canvas, SkIntToScalar(inputOffset.fX - bounds.fLeft),
        SkIntToScalar(inputOffset.fY - bounds.fTop), SkSamplingOptions(), &paint);
  }

  offset->fX = bounds.fLeft;
  offset->fY = bounds.fTop;
  return surf->makeImageSnapshot();
}

bool SkColorFilterImageFilter::onIsColorFilterNode(SkColorFilter** filter) const {
  SkASSERT(1 == this->countInputs());
  if (!this->cropRectIsSet()) {
    if (filter) {
      *filter = SkRef(fColorFilter.get());
    }
    return true;
  }
  return false;
}

bool SkColorFilterImageFilter::onAffectsTransparentBlack() const {
  return as_CFB(fColorFilter)->affectsTransparentBlack();
}
