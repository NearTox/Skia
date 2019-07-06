/*
 * Copyright 2015 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "include/core/SkBitmap.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkImageInfo.h"
#include "include/core/SkSurface.h"
#include "tests/CodecPriv.h"
#include "tests/Test.h"

// A 20x1 image with 8 bits per pixel and a palette size of 2. Pixel values are 255, 254... Run
// this test with ASAN to make sure we don't try to access before/after any palette-sized buffers.
unsigned char gPng[] = {
    0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d, 0x49, 0x48, 0x44, 0x52,
    0x00, 0x00, 0x00, 0x14, 0x00, 0x00, 0x00, 0x01, 0x08, 0x03, 0x00, 0x00, 0x00, 0xe9, 0x4c, 0x7e,
    0x17, 0x00, 0x00, 0x00, 0x09, 0x70, 0x48, 0x59, 0x73, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x00, 0x00,
    0x1c, 0x00, 0x0f, 0x01, 0xb9, 0x8f, 0x00, 0x00, 0x00, 0x06, 0x50, 0x4c, 0x54, 0x45, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0xa5, 0x67, 0xb9, 0xcf, 0x00, 0x00, 0x00, 0x20, 0x49, 0x44, 0x41, 0x54,
    0x78, 0xda, 0xed, 0xfd, 0x07, 0x01, 0x00, 0x20, 0x08, 0x00, 0x41, 0xbc, 0x5b, 0xe8, 0xdf, 0x97,
    0x99, 0xe3, 0x92, 0xa0, 0xf2, 0xdf, 0x3d, 0x7b, 0x0d, 0xda, 0x04, 0x1c, 0x03, 0xad, 0x00, 0x38,
    0x5c, 0x2e, 0xad, 0x12, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4e, 0x44, 0xae, 0x42, 0x60, 0x82};

DEF_TEST(IndexedPngOverflow, reporter) {
  SkBitmap image;
  bool success = decode_memory(gPng, sizeof(gPng), &image);
  REPORTER_ASSERT(reporter, success);

  auto surface(SkSurface::MakeRaster(SkImageInfo::MakeN32Premul(20, 1)));
  SkCanvas* canvas = surface->getCanvas();
  SkRect destRect = SkRect::MakeXYWH(0, 0, 20, 1);
  canvas->drawBitmapRect(image, destRect, nullptr);
}
