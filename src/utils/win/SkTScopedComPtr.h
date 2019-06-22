/*
 * Copyright 2011 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef SkTScopedComPtr_DEFINED
#define SkTScopedComPtr_DEFINED

#include "src/core/SkLeanWindows.h"

#ifdef SK_BUILD_FOR_WIN

template <typename T>
class SkBlockComRef : public T {
 private:
  virtual ULONG STDMETHODCALLTYPE AddRef(void) noexcept = 0;
  virtual ULONG STDMETHODCALLTYPE Release(void) noexcept = 0;
  virtual ~SkBlockComRef() {}
};

template <typename T>
T* SkRefComPtr(T* ptr) {
  ptr->AddRef();
  return ptr;
}

template <typename T>
T* SkSafeRefComPtr(T* ptr) {
  if (ptr) {
    ptr->AddRef();
  }
  return ptr;
}

template <typename T>
class SkTScopedComPtr {
 private:
  T* fPtr;

 public:
  constexpr SkTScopedComPtr() noexcept : fPtr(nullptr) {}
  constexpr SkTScopedComPtr(std::nullptr_t) noexcept : fPtr(nullptr) {}
  constexpr explicit SkTScopedComPtr(T* ptr) noexcept : fPtr(ptr) {}
  constexpr SkTScopedComPtr(SkTScopedComPtr&& that) noexcept : fPtr(that.release()) {}
  SkTScopedComPtr(const SkTScopedComPtr&) = delete;

  ~SkTScopedComPtr() { this->reset(); }

  SkTScopedComPtr& operator=(SkTScopedComPtr&& that) noexcept(noexcept(this->reset())) {
    this->reset(that.release());
    return *this;
  }
  SkTScopedComPtr& operator=(const SkTScopedComPtr&) = delete;
  SkTScopedComPtr& operator=(std::nullptr_t) noexcept(noexcept(this->reset())) {
    this->reset();
    return *this;
  }

  T& operator*() const {
    SkASSERT(fPtr != nullptr);
    return *fPtr;
  }

  explicit operator bool() const noexcept { return fPtr != nullptr; }

  SkBlockComRef<T>* operator->() const noexcept { return static_cast<SkBlockComRef<T>*>(fPtr); }

  /**
   * Returns the address of the underlying pointer.
   * This is dangerous -- it breaks encapsulation and the reference escapes.
   * Must only be used on instances currently pointing to NULL,
   * and only to initialize the instance.
   */
  T** operator&() noexcept {
    SkASSERT(fPtr == nullptr);
    return &fPtr;
  }

  T* get() const noexcept { return fPtr; }

  void reset(T* ptr = nullptr) noexcept(noexcept(fPtr->Release())) {
    if (fPtr) {
      fPtr->Release();
    }
    fPtr = ptr;
  }

  void swap(SkTScopedComPtr<T>& that) noexcept {
    T* temp = this->fPtr;
    this->fPtr = that.fPtr;
    that.fPtr = temp;
  }

  constexpr T* release() noexcept {
    T* temp = this->fPtr;
    this->fPtr = nullptr;
    return temp;
  }
};

#endif  // SK_BUILD_FOR_WIN
#endif  // SkTScopedComPtr_DEFINED
