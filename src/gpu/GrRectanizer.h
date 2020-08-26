/*
 * Copyright 2010 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef GrRectanizer_DEFINED
#define GrRectanizer_DEFINED

#include "include/gpu/GrTypes.h"

struct SkIPoint16;

class GrRectanizer {
 public:
  constexpr GrRectanizer(int width, int height) noexcept : fWidth(width), fHeight(height) {
    SkASSERT(width >= 0);
    SkASSERT(height >= 0);
  }

  virtual ~GrRectanizer() = default;

  virtual void reset() noexcept = 0;

  int width() const noexcept { return fWidth; }
  int height() const noexcept { return fHeight; }

  // Attempt to add a rect. Return true on success; false on failure. If
  // successful the position in the atlas is returned in 'loc'.
  virtual bool addRect(int width, int height, SkIPoint16* loc) noexcept = 0;
  virtual float percentFull() const noexcept = 0;

  /**
   *  Our factory, which returns the subclass du jour
   */
  static GrRectanizer* Factory(int width, int height);

 private:
  const int fWidth;
  const int fHeight;
};

#endif
