
/*
 * Copyright 2006 The Android Open Source Project
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef SkDeque_DEFINED
#define SkDeque_DEFINED

#include "include/core/SkTypes.h"

/*
 * The deque class works by blindly creating memory space of a specified element
 * size. It manages the memory as a doubly linked list of blocks each of which
 * can contain multiple elements. Pushes and pops add/remove blocks from the
 * beginning/end of the list as necessary while each block tracks the used
 * portion of its memory.
 * One behavior to be aware of is that the pops do not immediately remove an
 * empty block from the beginning/end of the list (Presumably so push/pop pairs
 * on the block boundaries don't cause thrashing). This can result in the first/
 * last element not residing in the first/last block.
 */
class SK_API SkDeque {
 public:
  /**
   * elemSize specifies the size of each individual element in the deque
   * allocCount specifies how many elements are to be allocated as a block
   */
  explicit SkDeque(size_t elemSize, int allocCount = 1) noexcept;
  SkDeque(size_t elemSize, void* storage, size_t storageSize, int allocCount = 1) noexcept;
  ~SkDeque();

  bool empty() const noexcept { return 0 == fCount; }
  int count() const noexcept { return fCount; }
  size_t elemSize() const noexcept { return fElemSize; }

  const void* front() const noexcept { return fFront; }
  const void* back() const noexcept { return fBack; }

  void* front() noexcept { return fFront; }
  void* back() noexcept { return fBack; }

  /**
   * push_front and push_back return a pointer to the memory space
   * for the new element
   */
  void* push_front() noexcept;
  void* push_back() noexcept;

  void pop_front() noexcept;
  void pop_back() noexcept;

 private:
  struct Block;

 public:
  class Iter {
   public:
    enum IterStart {
      kFront_IterStart,
      kBack_IterStart,
    };

    /**
     * Creates an uninitialized iterator. Must be reset()
     */
    Iter() noexcept;

    Iter(const SkDeque& d, IterStart startLoc) noexcept;
    void* next() noexcept;
    void* prev() noexcept;

    void reset(const SkDeque& d, IterStart startLoc) noexcept;

   private:
    SkDeque::Block* fCurBlock;
    char* fPos;
    size_t fElemSize;
  };

  // Inherit privately from Iter to prevent access to reverse iteration
  class F2BIter : private Iter {
   public:
    F2BIter() noexcept = default;

    /**
     * Wrap Iter's 2 parameter ctor to force initialization to the
     * beginning of the deque
     */
    F2BIter(const SkDeque& d) noexcept : INHERITED(d, kFront_IterStart) {}

    using Iter::next;

    /**
     * Wrap Iter::reset to force initialization to the beginning of the
     * deque
     */
    void reset(const SkDeque& d) noexcept { this->INHERITED::reset(d, kFront_IterStart); }

   private:
    using INHERITED = Iter;
  };

 private:
  // allow unit test to call numBlocksAllocated
  friend class DequeUnitTestHelper;

  void* fFront;
  void* fBack;

  Block* fFrontBlock;
  Block* fBackBlock;
  size_t fElemSize;
  void* fInitialStorage;
  int fCount;       // number of elements in the deque
  int fAllocCount;  // number of elements to allocate per block

  Block* allocateBlock(int allocCount) noexcept;
  void freeBlock(Block* block) noexcept;

  /**
   * This returns the number of chunk blocks allocated by the deque. It
   * can be used to gauge the effectiveness of the selected allocCount.
   */
  int numBlocksAllocated() const noexcept;

  SkDeque(const SkDeque&) = delete;
  SkDeque& operator=(const SkDeque&) = delete;
};

#endif
