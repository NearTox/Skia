
/*
 * Copyright 2006 The Android Open Source Project
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef SkParse_DEFINED
#define SkParse_DEFINED

#include "include/core/SkColor.h"

class SK_API SkParse {
 public:
  static int Count(const char str[]) noexcept;  // number of scalars or int values
  static int Count(const char str[], char separator) noexcept;
  static const char* FindColor(const char str[], SkColor* value) noexcept;
  static const char* FindHex(const char str[], uint32_t* value) noexcept;
  static const char* FindMSec(const char str[], SkMSec* value) noexcept;
  static const char* FindNamedColor(const char str[], size_t len, SkColor* color) noexcept;
  static const char* FindS32(const char str[], int32_t* value) noexcept;
  static const char* FindScalar(const char str[], SkScalar* value) noexcept;
  static const char* FindScalars(const char str[], SkScalar value[], int count) noexcept;

  static bool FindBool(const char str[], bool* value) noexcept;
  // return the index of str in list[], or -1 if not found
  static int FindList(const char str[], const char list[]) noexcept;
};

#endif
