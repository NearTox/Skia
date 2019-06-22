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

#define SK_DECLARE_STATIC_MUTEX(name) static SkBaseMutex name;

class SkBaseMutex {
 public:
  constexpr SkBaseMutex() noexcept = default;

  void acquire() noexcept {
    fSemaphore.wait();
    SkDEBUGCODE(fOwner = SkGetThreadID());
  }

  void release() noexcept {
    this->assertHeld();
    SkDEBUGCODE(fOwner = kIllegalThreadID);
    fSemaphore.signal();
  }

  void assertHeld() noexcept { SkASSERT(fOwner == SkGetThreadID()); }

 protected:
  SkBaseSemaphore fSemaphore{1};
  SkDEBUGCODE(SkThreadID fOwner{kIllegalThreadID});
};

class SK_CAPABILITY("mutex") SkMutex {
 public:
  constexpr SkMutex() noexcept = default;

  void acquire() SK_ACQUIRE() noexcept {
    fSemaphore.wait();
    SkDEBUGCODE(fOwner = SkGetThreadID());
  }

  void release() SK_RELEASE_CAPABILITY() noexcept {
    this->assertHeld();
    SkDEBUGCODE(fOwner = kIllegalThreadID);
    fSemaphore.signal();
  }

  void assertHeld() SK_ASSERT_CAPABILITY(this) noexcept { SkASSERT(fOwner == SkGetThreadID()); }

 private:
  SkSemaphore fSemaphore{1};
  SkDEBUGCODE(SkThreadID fOwner{kIllegalThreadID});
};

template <typename T>
class SkAutoMutexAcquire {
 public:
  SkAutoMutexAcquire(T* mutex) noexcept : fMutex(mutex) {
    if (mutex) {
      mutex->acquire();
    }
  }

  SkAutoMutexAcquire(T& mutex) noexcept : SkAutoMutexAcquire(&mutex) {}

  ~SkAutoMutexAcquire() { this->release(); }

  void release() noexcept {
    if (fMutex) {
      fMutex->release();
    }
    fMutex = nullptr;
  }

 private:
  T* fMutex;
};
#define SkAutoMutexAcquire(...) SK_REQUIRE_LOCAL_VAR(SkAutoMutexAcquire)

class SK_SCOPED_CAPABILITY SkAutoMutexExclusive {
 public:
  SkAutoMutexExclusive(SkMutex& mutex) SK_ACQUIRE(mutex) noexcept : fMutex(mutex) {
    fMutex.acquire();
  }
  ~SkAutoMutexExclusive() SK_RELEASE_CAPABILITY() { fMutex.release(); }

 private:
  SkMutex& fMutex;
};

#define SkAutoMutexExclusive(...) SK_REQUIRE_LOCAL_VAR(SkAutoMutexExclusive)

#endif  // SkMutex_DEFINED
