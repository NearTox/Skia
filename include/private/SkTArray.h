/*
 * Copyright 2011 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef SkTArray_DEFINED
#define SkTArray_DEFINED

#include "include/core/SkMath.h"
#include "include/core/SkTypes.h"
#include "include/private/SkMalloc.h"
#include "include/private/SkSafe32.h"
#include "include/private/SkTLogic.h"
#include "include/private/SkTemplates.h"

#include <string.h>
#include <memory>
#include <new>
#include <utility>

/** SkTArray<T> implements a typical, mostly std::vector-like array.
    Each T will be default-initialized on allocation, and ~T will be called on destruction.

    MEM_MOVE controls the behavior when a T needs to be moved (e.g. when the array is resized)
      - true: T will be bit-copied via memcpy.
      - false: T will be moved via move-constructors.

    Modern implementations of std::vector<T> will generally provide similar performance
    characteristics when used with appropriate care. Consider using std::vector<T> in new code.
*/
template <typename T, bool MEM_MOVE = false>
class SkTArray {
 public:
  /**
   * Creates an empty array with no initial storage
   */
  SkTArray() noexcept { this->init(); }

  /**
   * Creates an empty array that will preallocate space for reserveCount
   * elements.
   */
  explicit SkTArray(int reserveCount) noexcept { this->init(0, reserveCount); }

  /**
   * Copies one array to another. The new array will be heap allocated.
   */
  SkTArray(const SkTArray& that) noexcept(std::is_nothrow_copy_constructible_v<T>) {
    this->init(that.fCount);
    this->copy(that.fItemArray);
  }

  SkTArray(SkTArray&& that) noexcept(MEM_MOVE || std::is_nothrow_move_constructible_v<T>) {
    if (that.fOwnMemory) {
      fItemArray = that.fItemArray;
      fCount = that.fCount;
      fAllocCount = that.fAllocCount;
      fOwnMemory = true;
      fReserved = that.fReserved;

      that.fItemArray = nullptr;
      that.fCount = 0;
      that.fAllocCount = 0;
      that.fOwnMemory = true;
      that.fReserved = false;
    } else {
      this->init(that.fCount);
      that.move(fItemArray);
      that.fCount = 0;
    }
  }

  /**
   * Creates a SkTArray by copying contents of a standard C array. The new
   * array will be heap allocated. Be careful not to use this constructor
   * when you really want the (void*, int) version.
   */
  SkTArray(const T* array, int count) noexcept(std::is_nothrow_copy_constructible_v<T>) {
    this->init(count);
    this->copy(array);
  }

  SkTArray& operator=(const SkTArray& that) noexcept(
      (MEM_MOVE ||
       std::is_nothrow_move_constructible_v<T>)&&std::is_nothrow_copy_constructible_v<T>) {
    if (this == &that) {
      return *this;
    }
    for (int i = 0; i < fCount; ++i) {
      fItemArray[i].~T();
    }
    fCount = 0;
    this->checkRealloc(that.count());
    fCount = that.count();
    this->copy(that.fItemArray);
    return *this;
  }
  SkTArray& operator=(SkTArray&& that) noexcept(
      MEM_MOVE || std::is_nothrow_move_constructible_v<T>) {
    if (this == &that) {
      return *this;
    }
    for (int i = 0; i < fCount; ++i) {
      fItemArray[i].~T();
    }
    fCount = 0;
    this->checkRealloc(that.count());
    fCount = that.count();
    that.move(fItemArray);
    that.fCount = 0;
    return *this;
  }

  ~SkTArray() {
    for (int i = 0; i < fCount; ++i) {
      fItemArray[i].~T();
    }
    if (fOwnMemory) {
      sk_free(fItemArray);
    }
  }

  /**
   * Resets to count() == 0 and resets any reserve count.
   */
  void reset() noexcept(MEM_MOVE || std::is_nothrow_move_constructible_v<T>) {
    this->pop_back_n(fCount);
    fReserved = false;
  }

  /**
   * Resets to count() = n newly constructed T objects and resets any reserve count.
   */
  void reset(int n) noexcept(
      (MEM_MOVE ||
       std::is_nothrow_move_constructible_v<T>)&&std::is_nothrow_default_constructible_v<T>) {
    SkASSERT(n >= 0);
    for (int i = 0; i < fCount; ++i) {
      fItemArray[i].~T();
    }
    // Set fCount to 0 before calling checkRealloc so that no elements are moved.
    fCount = 0;
    this->checkRealloc(n);
    fCount = n;
    for (int i = 0; i < fCount; ++i) {
      new (fItemArray + i) T;
    }
    fReserved = false;
  }

  /**
   * Resets to a copy of a C array and resets any reserve count.
   */
  void reset(const T* array, int count) noexcept(
      (MEM_MOVE ||
       std::is_nothrow_move_constructible_v<T>)&&std::is_nothrow_copy_constructible_v<T>) {
    for (int i = 0; i < fCount; ++i) {
      fItemArray[i].~T();
    }
    fCount = 0;
    this->checkRealloc(count);
    fCount = count;
    this->copy(array);
    fReserved = false;
  }

  /**
   * Ensures there is enough reserved space for n additional elements. The is guaranteed at least
   * until the array size grows above n and subsequently shrinks below n, any version of reset()
   * is called, or reserve() is called again.
   */
  void reserve(int n) noexcept(MEM_MOVE || std::is_nothrow_move_constructible_v<T>) {
    SkASSERT(n >= 0);
    if (n > 0) {
      this->checkRealloc(n);
      fReserved = fOwnMemory;
    } else {
      fReserved = false;
    }
  }

  void removeShuffle(int n) noexcept(MEM_MOVE || std::is_nothrow_move_constructible_v<T>) {
    SkASSERT(n < fCount);
    int newCount = fCount - 1;
    fCount = newCount;
    fItemArray[n].~T();
    if (n != newCount) {
      this->move(n, newCount);
    }
  }

  /**
   * Number of elements in the array.
   */
  int count() const noexcept { return fCount; }

  /**
   * Is the array empty.
   */
  bool empty() const noexcept { return !fCount; }

  /**
   * Adds 1 new default-initialized T value and returns it by reference. Note
   * the reference only remains valid until the next call that adds or removes
   * elements.
   */
  T& push_back() noexcept(
      (MEM_MOVE ||
       std::is_nothrow_move_constructible_v<T>)&&std::is_nothrow_default_constructible_v<T>) {
    void* newT = this->push_back_raw(1);
    return *new (newT) T;
  }

  /**
   * Version of above that uses a copy constructor to initialize the new item
   */
  T& push_back(const T& t) noexcept(
      (MEM_MOVE ||
       std::is_nothrow_move_constructible_v<T>)&&std::is_nothrow_copy_constructible_v<T>) {
    void* newT = this->push_back_raw(1);
    return *new (newT) T(t);
  }

  /**
   * Version of above that uses a move constructor to initialize the new item
   */
  T& push_back(T&& t) noexcept(MEM_MOVE || std::is_nothrow_move_constructible_v<T>) {
    void* newT = this->push_back_raw(1);
    return *new (newT) T(std::move(t));
  }

  /**
   *  Construct a new T at the back of this array.
   */
  template <class... Args>
  T& emplace_back(Args&&... args) noexcept(
      (MEM_MOVE ||
       std::is_nothrow_move_constructible_v<T>)&&std::is_nothrow_constructible_v<T, Args...>) {
    void* newT = this->push_back_raw(1);
    return *new (newT) T(std::forward<Args>(args)...);
  }

  /*template <>
  T& emplace_back<>() noexcept(std::is_nothrow_default_constructible_v<T>) {
    void* newT = this->push_back_raw(1);
    return *new (newT) T();
  }

  template <>
  T& emplace_back<T&&>(T&& move) noexcept(std::is_nothrow_move_constructible_v<T>) {
    void* newT = this->push_back_raw(1);
    return *new (newT) T(std::move(move));
  }

  template <>
  T& emplace_back<const T&>(const T& copy) noexcept(
      (MEM_MOVE ||
       std::is_nothrow_move_constructible_v<T>)&&std::is_nothrow_copy_constructible_v<T>) {
    void* newT = this->push_back_raw(1);
    return *new (newT) T(copy);
  }*/

  /**
   * Allocates n more default-initialized T values, and returns the address of
   * the start of that new range. Note: this address is only valid until the
   * next API call made on the array that might add or remove elements.
   */
  T* push_back_n(int n) noexcept(
      (MEM_MOVE ||
       std::is_nothrow_move_constructible_v<T>)&&std::is_nothrow_default_constructible_v<T>) {
    SkASSERT(n >= 0);
    void* newTs = this->push_back_raw(n);
    for (int i = 0; i < n; ++i) {
      new (static_cast<char*>(newTs) + i * sizeof(T)) T;
    }
    return static_cast<T*>(newTs);
  }

  /**
   * Version of above that uses a copy constructor to initialize all n items
   * to the same T.
   */
  T* push_back_n(int n, const T& t) noexcept(
      (MEM_MOVE ||
       std::is_nothrow_move_constructible_v<T>)&&std::is_nothrow_copy_constructible_v<T>) {
    SkASSERT(n >= 0);
    void* newTs = this->push_back_raw(n);
    for (int i = 0; i < n; ++i) {
      new (static_cast<char*>(newTs) + i * sizeof(T)) T(t);
    }
    return static_cast<T*>(newTs);
  }

  /**
   * Version of above that uses a copy constructor to initialize the n items
   * to separate T values.
   */
  T* push_back_n(int n, const T t[]) noexcept(
      (MEM_MOVE ||
       std::is_nothrow_move_constructible_v<T>)&&std::is_nothrow_copy_constructible_v<T>) {
    SkASSERT(n >= 0);
    this->checkRealloc(n);
    for (int i = 0; i < n; ++i) {
      new (fItemArray + fCount + i) T(t[i]);
    }
    fCount += n;
    return fItemArray + fCount - n;
  }

  /**
   * Version of above that uses the move constructor to set n items.
   */
  T* move_back_n(int n, T* t) noexcept(std::is_nothrow_move_constructible_v<T>) {
    SkASSERT(n >= 0);
    this->checkRealloc(n);
    for (int i = 0; i < n; ++i) {
      new (fItemArray + fCount + i) T(std::move(t[i]));
    }
    fCount += n;
    return fItemArray + fCount - n;
  }

  /**
   * Removes the last element. Not safe to call when count() == 0.
   */
  void pop_back() noexcept(MEM_MOVE || std::is_nothrow_move_constructible_v<T>) {
    SkASSERT(fCount > 0);
    --fCount;
    fItemArray[fCount].~T();
    this->checkRealloc(0);
  }

  /**
   * Removes the last n elements. Not safe to call when count() < n.
   */
  void pop_back_n(int n) noexcept(MEM_MOVE || std::is_nothrow_move_constructible_v<T>) {
    SkASSERT(n >= 0);
    SkASSERT(fCount >= n);
    fCount -= n;
    for (int i = 0; i < n; ++i) {
      fItemArray[fCount + i].~T();
    }
    this->checkRealloc(0);
  }

  /**
   * Pushes or pops from the back to resize. Pushes will be default
   * initialized.
   */
  void resize_back(int newCount) noexcept(
      (MEM_MOVE ||
       std::is_nothrow_move_constructible_v<T>)&&std::is_nothrow_copy_constructible_v<T>) {
    SkASSERT(newCount >= 0);

    if (newCount > fCount) {
      this->push_back_n(newCount - fCount);
    } else if (newCount < fCount) {
      this->pop_back_n(fCount - newCount);
    }
  }

  /** Swaps the contents of this array with that array. Does a pointer swap if possible,
      otherwise copies the T values. */
  void swap(SkTArray& that) noexcept(MEM_MOVE || std::is_nothrow_move_constructible_v<T>) {
    using std::swap;
    if (this == &that) {
      return;
    }
    if (fOwnMemory && that.fOwnMemory) {
      swap(fItemArray, that.fItemArray);
      swap(fCount, that.fCount);
      swap(fAllocCount, that.fAllocCount);
    } else {
      // This could be more optimal...
      SkTArray copy(std::move(that));
      that = std::move(*this);
      *this = std::move(copy);
    }
  }

  T* begin() noexcept { return fItemArray; }
  const T* begin() const noexcept { return fItemArray; }
  T* end() noexcept { return fItemArray ? fItemArray + fCount : nullptr; }
  const T* end() const noexcept { return fItemArray ? fItemArray + fCount : nullptr; }
  T* data() noexcept { return fItemArray; }
  const T* data() const noexcept { return fItemArray; }
  size_t size() const noexcept { return (size_t)fCount; }
  void resize(size_t count) noexcept(
      (MEM_MOVE ||
       std::is_nothrow_move_constructible_v<T>)&&std::is_nothrow_copy_constructible_v<T>) {
    this->resize_back((int)count);
  }

  /**
   * Get the i^th element.
   */
  T& operator[](int i) noexcept {
    SkASSERT(i < fCount);
    SkASSERT(i >= 0);
    return fItemArray[i];
  }

  const T& operator[](int i) const noexcept {
    SkASSERT(i < fCount);
    SkASSERT(i >= 0);
    return fItemArray[i];
  }

  T& at(int i) noexcept { return (*this)[i]; }
  const T& at(int i) const noexcept { return (*this)[i]; }

  /**
   * equivalent to operator[](0)
   */
  T& front() noexcept {
    SkASSERT(fCount > 0);
    return fItemArray[0];
  }

  const T& front() const noexcept {
    SkASSERT(fCount > 0);
    return fItemArray[0];
  }

  /**
   * equivalent to operator[](count() - 1)
   */
  T& back() noexcept {
    SkASSERT(fCount);
    return fItemArray[fCount - 1];
  }

  const T& back() const noexcept {
    SkASSERT(fCount > 0);
    return fItemArray[fCount - 1];
  }

  /**
   * equivalent to operator[](count()-1-i)
   */
  T& fromBack(int i) noexcept {
    SkASSERT(i >= 0);
    SkASSERT(i < fCount);
    return fItemArray[fCount - i - 1];
  }

  const T& fromBack(int i) const noexcept {
    SkASSERT(i >= 0);
    SkASSERT(i < fCount);
    return fItemArray[fCount - i - 1];
  }

  bool operator==(const SkTArray<T, MEM_MOVE>& right) const {
    int leftCount = this->count();
    if (leftCount != right.count()) {
      return false;
    }
    for (int index = 0; index < leftCount; ++index) {
      if (fItemArray[index] != right.fItemArray[index]) {
        return false;
      }
    }
    return true;
  }

  bool operator!=(const SkTArray<T, MEM_MOVE>& right) const { return !(*this == right); }

  inline int allocCntForTest() const;

 protected:
  /**
   * Creates an empty array that will use the passed storage block until it
   * is insufficiently large to hold the entire array.
   */
  template <int N>
  SkTArray(SkAlignedSTStorage<N, T>* storage) noexcept {
    this->initWithPreallocatedStorage(0, storage->get(), N);
  }

  /**
   * Copy another array, using preallocated storage if preAllocCount >=
   * array.count(). Otherwise storage will only be used when array shrinks
   * to fit.
   */
  template <int N>
  SkTArray(const SkTArray& array, SkAlignedSTStorage<N, T>* storage) noexcept(
      std::is_nothrow_copy_constructible_v<T>) {
    this->initWithPreallocatedStorage(array.fCount, storage->get(), N);
    this->copy(array.fItemArray);
  }

  /**
   * Move another array, using preallocated storage if preAllocCount >=
   * array.count(). Otherwise storage will only be used when array shrinks
   * to fit.
   */
  template <int N>
  SkTArray(SkTArray&& array, SkAlignedSTStorage<N, T>* storage) noexcept(
      MEM_MOVE || std::is_nothrow_move_constructible_v<T>) {
    this->initWithPreallocatedStorage(array.fCount, storage->get(), N);
    array.move(fItemArray);
    array.fCount = 0;
  }

  /**
   * Copy a C array, using preallocated storage if preAllocCount >=
   * count. Otherwise storage will only be used when array shrinks
   * to fit.
   */
  template <int N>
  SkTArray(const T* array, int count, SkAlignedSTStorage<N, T>* storage) noexcept(
      std::is_nothrow_copy_constructible_v<T>) {
    this->initWithPreallocatedStorage(count, storage->get(), N);
    this->copy(array);
  }

 private:
  void init(int count = 0, int reserveCount = 0) noexcept {
    SkASSERT(count >= 0);
    SkASSERT(reserveCount >= 0);
    fCount = count;
    if (!count && !reserveCount) {
      fAllocCount = 0;
      fItemArray = nullptr;
      fOwnMemory = true;
      fReserved = false;
    } else {
      fAllocCount = std::max(count, std::max(kMinHeapAllocCount, reserveCount));
      fItemArray = (T*)sk_malloc_throw((size_t)fAllocCount, sizeof(T));
      fOwnMemory = true;
      fReserved = reserveCount > 0;
    }
  }

  void initWithPreallocatedStorage(int count, void* preallocStorage, int preallocCount) noexcept {
    SkASSERT(count >= 0);
    SkASSERT(preallocCount > 0);
    SkASSERT(preallocStorage);
    fCount = count;
    fItemArray = nullptr;
    fReserved = false;
    if (count > preallocCount) {
      fAllocCount = std::max(count, kMinHeapAllocCount);
      fItemArray = (T*)sk_malloc_throw(fAllocCount, sizeof(T));
      fOwnMemory = true;
    } else {
      fAllocCount = preallocCount;
      fItemArray = (T*)preallocStorage;
      fOwnMemory = false;
    }
  }

  /** In the following move and copy methods, 'dst' is assumed to be uninitialized raw storage.
   *  In the following move methods, 'src' is destroyed leaving behind uninitialized raw storage.
   */
  void copy(const T* src) noexcept(std::is_nothrow_copy_constructible_v<T>) {
    // Some types may be trivially copyable, in which case we *could* use memcopy; but
    // MEM_MOVE == true implies that the type is trivially movable, and not necessarily
    // trivially copyable (think sk_sp<>).  So short of adding another template arg, we
    // must be conservative and use copy construction.
    for (int i = 0; i < fCount; ++i) {
      new (fItemArray + i) T(src[i]);
    }
  }

  template <bool E = MEM_MOVE>
  std::enable_if_t<E, void> move(int dst, int src) noexcept {
    memcpy(&fItemArray[dst], &fItemArray[src], sizeof(T));
  }
  template <bool E = MEM_MOVE>
  std::enable_if_t<E, void> move(void* dst) noexcept {
    sk_careful_memcpy(dst, fItemArray, fCount * sizeof(T));
  }

  template <bool E = MEM_MOVE>
  std::enable_if_t<!E, void> move(int dst, int src) noexcept(
      MEM_MOVE || std::is_nothrow_move_constructible_v<T>) {
    new (&fItemArray[dst]) T(std::move(fItemArray[src]));
    fItemArray[src].~T();
  }
  template <bool E = MEM_MOVE>
  std::enable_if_t<!E, void> move(void* dst) noexcept(
      MEM_MOVE || std::is_nothrow_move_constructible_v<T>) {
    for (int i = 0; i < fCount; ++i) {
      new (static_cast<char*>(dst) + sizeof(T) * (size_t)i) T(std::move(fItemArray[i]));
      fItemArray[i].~T();
    }
  }

  static constexpr int kMinHeapAllocCount = 8;

  // Helper function that makes space for n objects, adjusts the count, but does not initialize
  // the new objects.
  void* push_back_raw(int n) noexcept(MEM_MOVE || std::is_nothrow_move_constructible_v<T>) {
    this->checkRealloc(n);
    void* ptr = fItemArray + fCount;
    fCount += n;
    return ptr;
  }

  void checkRealloc(int delta) noexcept(MEM_MOVE || std::is_nothrow_move_constructible_v<T>) {
    SkASSERT(fCount >= 0);
    SkASSERT(fAllocCount >= 0);
    SkASSERT(-delta <= fCount);

    // Move into 64bit math temporarily, to avoid local overflows
    int64_t newCount = fCount + delta;

    // We allow fAllocCount to be in the range [newCount, 3*newCount]. We also never shrink
    // when we're currently using preallocated memory, would allocate less than
    // kMinHeapAllocCount, or a reserve count was specified that has yet to be exceeded.
    bool mustGrow = newCount > fAllocCount;
    bool shouldShrink = fAllocCount > 3 * newCount && fOwnMemory && !fReserved;
    if (!mustGrow && !shouldShrink) {
      return;
    }

    // Whether we're growing or shrinking, we leave at least 50% extra space for future growth.
    int64_t newAllocCount = newCount + ((newCount + 1) >> 1);
    // Align the new allocation count to kMinHeapAllocCount.
    static_assert(SkIsPow2(kMinHeapAllocCount), "min alloc count not power of two.");
    newAllocCount = (newAllocCount + (kMinHeapAllocCount - 1)) & ~(kMinHeapAllocCount - 1);
    // At small sizes the old and new alloc count can both be kMinHeapAllocCount.
    if (newAllocCount == fAllocCount) {
      return;
    }

    fAllocCount = Sk64_pin_to_s32(newAllocCount);
    SkASSERT(fAllocCount >= newCount);
    T* newItemArray = (T*)sk_malloc_throw((size_t)fAllocCount, sizeof(T));
    this->move(newItemArray);
    if (fOwnMemory) {
      sk_free(fItemArray);
    }
    fItemArray = newItemArray;
    fOwnMemory = true;
    fReserved = false;
  }

  T* fItemArray;
  int fCount;
  int fAllocCount;
  bool fOwnMemory : 1;
  bool fReserved : 1;
};

template <typename T, bool M>
static inline void swap(SkTArray<T, M>& a, SkTArray<T, M>& b) noexcept(
    M || std::is_nothrow_move_constructible_v<T>) {
  a.swap(b);
}

/**
 * Subclass of SkTArray that contains a preallocated memory block for the array.
 */
template <int N, typename T, bool MEM_MOVE = false>
class SkSTArray : public SkTArray<T, MEM_MOVE> {
 private:
  typedef SkTArray<T, MEM_MOVE> INHERITED;

 public:
  SkSTArray() noexcept : INHERITED(&fStorage) {}

  SkSTArray(const SkSTArray& array) noexcept(std::is_nothrow_copy_constructible_v<T>)
      : INHERITED(array, &fStorage) {}

  SkSTArray(SkSTArray&& array) noexcept(MEM_MOVE || std::is_nothrow_move_constructible_v<T>)
      : INHERITED(std::move(array), &fStorage) {}

  explicit SkSTArray(const INHERITED& array) noexcept(std::is_nothrow_copy_constructible_v<T>)
      : INHERITED(array, &fStorage) {}

  explicit SkSTArray(INHERITED&& array) noexcept(
      MEM_MOVE || std::is_nothrow_move_constructible_v<T>)
      : INHERITED(std::move(array), &fStorage) {}

  explicit SkSTArray(int reserveCount) noexcept : INHERITED(reserveCount) {}

  SkSTArray(const T* array, int count) noexcept(std::is_nothrow_copy_constructible_v<T>)
      : INHERITED(array, count, &fStorage) {}

  SkSTArray& operator=(const SkSTArray& array) noexcept(
      std::is_nothrow_copy_assignable_v<INHERITED>) {
    INHERITED::operator=(array);
    return *this;
  }

  SkSTArray& operator=(SkSTArray&& array) noexcept(
      MEM_MOVE || std::is_nothrow_move_assignable_v<INHERITED>) {
    INHERITED::operator=(std::move(array));
    return *this;
  }

  SkSTArray& operator=(const INHERITED& array) noexcept(
      std::is_nothrow_copy_assignable_v<INHERITED>) {
    INHERITED::operator=(array);
    return *this;
  }

  SkSTArray& operator=(INHERITED&& array) noexcept(
      MEM_MOVE || std::is_nothrow_move_assignable_v<INHERITED>) {
    INHERITED::operator=(std::move(array));
    return *this;
  }

 private:
  SkAlignedSTStorage<N, T> fStorage;
};

#endif
