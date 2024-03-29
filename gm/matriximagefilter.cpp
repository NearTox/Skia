/*
 * Copyright 2014 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "gm/gm.h"
#include "include/core/SkBitmap.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkColor.h"
#include "include/core/SkImage.h"
#include "include/core/SkMatrix.h"
#include "include/core/SkPaint.h"
#include "include/core/SkRect.h"
#include "include/core/SkScalar.h"
#include "include/effects/SkImageFilters.h"

static void draw(
    SkCanvas* canvas, const SkRect& rect, const SkBitmap& bitmap, const SkMatrix& matrix,
    const SkSamplingOptions& sampling) {
  SkPaint paint;
  paint.setImageFilter(SkImageFilters::MatrixTransform(matrix, sampling, nullptr));
  canvas->saveLayer(&rect, &paint);
  canvas->drawImage(bitmap.asImage(), 0, 0, sampling);
  canvas->restore();
}

static void make_checkerboard(SkBitmap* bitmap) {
  bitmap->allocN32Pixels(64, 64);
  SkCanvas canvas(*bitmap);
  SkPaint darkPaint;
  darkPaint.setColor(0xFF404040);
  SkPaint lightPaint;
  lightPaint.setColor(0xFFA0A0A0);
  for (int y = 0; y < 64; y += 32) {
    for (int x = 0; x < 64; x += 32) {
      canvas.save();
      canvas.translate(SkIntToScalar(x), SkIntToScalar(y));
      canvas.drawRect(SkRect::MakeXYWH(0, 0, 16, 16), darkPaint);
      canvas.drawRect(SkRect::MakeXYWH(16, 0, 16, 16), lightPaint);
      canvas.drawRect(SkRect::MakeXYWH(0, 16, 16, 16), lightPaint);
      canvas.drawRect(SkRect::MakeXYWH(16, 16, 16, 16), darkPaint);
      canvas.restore();
    }
  }
}

DEF_SIMPLE_GM_BG(matriximagefilter, canvas, 420, 100, SK_ColorBLACK) {
  SkMatrix matrix;
  SkScalar margin = SkIntToScalar(10);
  matrix.setSkew(SkDoubleToScalar(0.5), SkDoubleToScalar(0.2));
  SkBitmap checkerboard;
  make_checkerboard(&checkerboard);

  SkRect srcRect = SkRect::MakeWH(96, 96);

  canvas->translate(margin, margin);
  draw(canvas, srcRect, checkerboard, matrix, SkSamplingOptions());

  canvas->translate(srcRect.width() + margin, 0);
  draw(canvas, srcRect, checkerboard, matrix, SkSamplingOptions(SkFilterMode::kLinear));
}
