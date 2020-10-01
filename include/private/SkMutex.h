/*
 * Copyright 2015 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef SkMutex_DEFINED
#define SkMutex_DEFINED

#include "include/core/SkTypes.h"
#include "include/private/SkMacros.h"
#include "include/private/SkSemaphore.h"
#include "include/private/SkThreadAnnotations.h"
#include "include/private/SkThreadID.h"

class SK_CAPABILITY("mutex") SkMutex {
 public:
  constexpr SkMutex() noexcept = default;

  void acquire() noexcept SK_ACQUIRE() {
    fSemaphore.wait();
    SkDEBUGCODE(fOwner = SkGetThreadID());
  }

  void release() noexcept SK_RELEASE_CAPABILITY() {
    this->assertHeld();
    SkDEBUGCODE(fOwner = kIllegalThreadID);
    fSemaphore.signal();
  }

  void assertHeld() noexcept SK_ASSERT_CAPABILITY(this) { SkASSERT(fOwner == SkGetThreadID()); }

 private:
  SkSemaphore fSemaphore{1};
  SkDEBUGCODE(SkThreadID fOwner{kIllegalThreadID});
};

class SK_SCOPED_CAPABILITY SkAutoMutexExclusive {
 public:
  SkAutoMutexExclusive(SkMutex& mutex) noexcept SK_ACQUIRE(mutex) : fMutex(mutex) {
    fMutex.acquire();
  }
  ~SkAutoMutexExclusive() SK_RELEASE_CAPABILITY() { fMutex.release(); }

 private:
  SkMutex& fMutex;
};

#endif  // SkMutex_DEFINED
