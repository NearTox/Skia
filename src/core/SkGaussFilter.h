/*
 * Copyright 2017 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef SkGaussFilter_DEFINED
#define SkGaussFilter_DEFINED

#include <cstddef>

// Define gaussian filters for values of sigma < 2. Produce values good to 1 part in 1,000,000.
// Produces values as defined in "Scale-Space for Discrete Signals" by Tony Lindeberg.
class SkGaussFilter {
 public:
  static constexpr int kGaussArrayMax = 6;

  explicit SkGaussFilter(double sigma) noexcept;

  size_t size() const noexcept { return fN; }
  int radius() const noexcept { return fN - 1; }
  int width() const noexcept { return 2 * this->radius() + 1; }

  // Allow a filter to be used in a C++ ranged-for loop.
  const double* begin() const noexcept { return &fBasis[0]; }
  const double* end() const noexcept { return &fBasis[fN]; }

 private:
  double fBasis[kGaussArrayMax];
  int fN;
};

#endif  // SkGaussFilter_DEFINED
