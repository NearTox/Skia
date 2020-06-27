/*
 * Copyright 2017 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef SkArenaAllocList_DEFINED
#define SkArenaAllocList_DEFINED

#include "include/core/SkTypes.h"
#include "src/core/SkArenaAlloc.h"

/**
 * A singly linked list of Ts stored in a SkArenaAlloc. The arena rather than the list owns
 * the elements. This supports forward iteration and range based for loops.
 */
template <typename T>
class SkArenaAllocList {
 private:
  struct Node;

 public:
  constexpr SkArenaAllocList() noexcept = default;

  constexpr void reset() noexcept { fHead = fTail = nullptr; }

  template <typename... Args>
  inline T& append(SkArenaAlloc* arena, Args... args);

  class Iter {
   public:
    constexpr Iter() noexcept = default;
    inline Iter& operator++() noexcept;
    T& operator*() const noexcept { return fCurr->fT; }
    T* operator->() const noexcept { return &fCurr->fT; }
    bool operator==(const Iter& that) const noexcept { return fCurr == that.fCurr; }
    bool operator!=(const Iter& that) const noexcept { return !(*this == that); }

   private:
    friend class SkArenaAllocList;
    explicit Iter(Node* node) noexcept : fCurr(node) {}
    Node* fCurr = nullptr;
  };

  Iter begin() noexcept { return Iter(fHead); }
  Iter end() noexcept { return Iter(); }
  Iter tail() noexcept { return Iter(fTail); }

 private:
  struct Node {
    template <typename... Args>
    Node(Args... args) : fT(std::forward<Args>(args)...) {}
    T fT;
    Node* fNext = nullptr;
  };
  Node* fHead = nullptr;
  Node* fTail = nullptr;
};

template <typename T>
template <typename... Args>
T& SkArenaAllocList<T>::append(SkArenaAlloc* arena, Args... args) {
  SkASSERT(!fHead == !fTail);
  auto* n = arena->make<Node>(std::forward<Args>(args)...);
  if (!fTail) {
    fHead = fTail = n;
  } else {
    fTail = fTail->fNext = n;
  }
  return fTail->fT;
}

template <typename T>
typename SkArenaAllocList<T>::Iter& SkArenaAllocList<T>::Iter::operator++() noexcept {
  fCurr = fCurr->fNext;
  return *this;
}

#endif
