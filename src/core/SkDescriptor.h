/*
 * Copyright 2006 The Android Open Source Project
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef SkDescriptor_DEFINED
#define SkDescriptor_DEFINED

#include <memory>
#include <new>

#include "include/private/SkMacros.h"
#include "include/private/SkNoncopyable.h"
#include "src/core/SkScalerContext.h"

class SkDescriptor : SkNoncopyable {
 public:
  static constexpr size_t ComputeOverhead(int entryCount) noexcept {
    SkASSERT(entryCount >= 0);
    return sizeof(SkDescriptor) + entryCount * sizeof(Entry);
  }

  static std::unique_ptr<SkDescriptor> Alloc(size_t length);

  //
  // Ensure the unsized delete is called.
  void operator delete(void* p) noexcept;
  void* operator new(size_t);
  void* operator new(size_t, void* p) { return p; }

  uint32_t getLength() const noexcept { return fLength; }
  void* addEntry(uint32_t tag, size_t length, const void* data = nullptr) noexcept;
  void computeChecksum();

  // Assumes that getLength <= capacity of this SkDescriptor.
  bool isValid() const noexcept;

#ifdef SK_DEBUG
  void assertChecksum() const { SkASSERT(SkDescriptor::ComputeChecksum(this) == fChecksum); }
#endif

  const void* findEntry(uint32_t tag, uint32_t* length) const noexcept;

  std::unique_ptr<SkDescriptor> copy() const;

  // This assumes that all memory added has a length that is a multiple of 4. This is checked
  // by the assert in addEntry.
  bool operator==(const SkDescriptor& other) const noexcept;
  bool operator!=(const SkDescriptor& other) const noexcept { return !(*this == other); }

  uint32_t getChecksum() const noexcept { return fChecksum; }

  struct Entry {
    uint32_t fTag;
    uint32_t fLen;
  };

#ifdef SK_DEBUG
  uint32_t getCount() const { return fCount; }
#endif

 private:
  constexpr SkDescriptor() noexcept = default;
  friend class SkDescriptorTestHelper;
  friend class SkAutoDescriptor;

  static uint32_t ComputeChecksum(const SkDescriptor* desc);

  uint32_t fChecksum{0};                   // must be first
  uint32_t fLength{sizeof(SkDescriptor)};  // must be second
  uint32_t fCount{0};
};

class SkAutoDescriptor {
 public:
  SkAutoDescriptor() noexcept;
  explicit SkAutoDescriptor(size_t size);
  explicit SkAutoDescriptor(const SkDescriptor& desc);
  SkAutoDescriptor(const SkAutoDescriptor& ad);
  SkAutoDescriptor& operator=(const SkAutoDescriptor& ad);
  SkAutoDescriptor(SkAutoDescriptor&&) = delete;
  SkAutoDescriptor& operator=(SkAutoDescriptor&&) = delete;

  ~SkAutoDescriptor();

  void reset(size_t size);
  void reset(const SkDescriptor& desc);
  SkDescriptor* getDesc() const noexcept {
    SkASSERT(fDesc);
    return fDesc;
  }

 private:
  void free() noexcept;
  static constexpr size_t kStorageSize =
      sizeof(SkDescriptor) + sizeof(SkDescriptor::Entry) + sizeof(SkScalerContextRec)  // for rec
      + sizeof(SkDescriptor::Entry) + sizeof(void*)  // for typeface
      + 32;                                          // slop for occasional small extras

  SkDescriptor* fDesc{nullptr};
  std::aligned_storage<kStorageSize, alignof(uint32_t)>::type fStorage;
};

#endif  // SkDescriptor_DEFINED
