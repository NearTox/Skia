/*
 * Copyright 2006 The Android Open Source Project
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "src/core/SkUtils.h"

template <typename T, SkUnichar (*fn)(const T**, const T*)>
static SkUnichar next(const T** srcPtr, unsigned N) noexcept {
  SkASSERT(srcPtr);
  const T* ptr = *srcPtr;
  SkUnichar c = fn(&ptr, ptr + N);
  if (c == -1) {
    SkASSERT(false);
    ++(*srcPtr);
    return 0xFFFD;  // REPLACEMENT CHARACTER
  }
  *srcPtr = ptr;
  return c;
}
SkUnichar SkUTF8_NextUnichar(const char** p) noexcept {
  return next<char, SkUTF::NextUTF8>(p, SkUTF::kMaxBytesInUTF8Sequence);
}
SkUnichar SkUTF16_NextUnichar(const uint16_t** p) noexcept {
  return next<uint16_t, SkUTF::NextUTF16>(p, 2);
}

///////////////////////////////////////////////////////////////////////////////

const char SkHexadecimalDigits::gUpper[16] = {'0', '1', '2', '3', '4', '5', '6', '7',
                                              '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
const char SkHexadecimalDigits::gLower[16] = {'0', '1', '2', '3', '4', '5', '6', '7',
                                              '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
