/*
 * Copyright 2021 Google LLC
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef SkSLSharedCompiler_DEFINED
#define SkSLSharedCompiler_DEFINED

#include "include/core/SkTypes.h"

#ifdef SK_ENABLE_SKSL

#  include "include/private/SkMutex.h"

namespace SkSL {

class Compiler;

/** A shared compiler instance for runtime client SkSL that is internally guarded by a mutex. */
class SharedCompiler {
 public:
  SharedCompiler();

  SkSL::Compiler* operator->() const;

 private:
  SkAutoMutexExclusive fLock;

  static SkMutex& compiler_mutex();

  struct Impl;
  static Impl* gImpl;
};

}  // namespace SkSL

#endif  // SK_ENABLE_SKSL

#endif
