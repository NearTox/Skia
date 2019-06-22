/*
 * Copyright 2016 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "src/core/SkAutoPixmapStorage.h"
#include "include/core/SkData.h"

SkAutoPixmapStorage::SkAutoPixmapStorage() noexcept : fStorage(nullptr) {}

SkAutoPixmapStorage::~SkAutoPixmapStorage() { this->freeStorage(); }

SkAutoPixmapStorage& SkAutoPixmapStorage::operator=(SkAutoPixmapStorage&& other) noexcept {
  this->fStorage = other.fStorage;
  this->INHERITED::reset(other.info(), this->fStorage, other.rowBytes());

  other.fStorage = nullptr;
  other.INHERITED::reset();

  return *this;
}

size_t SkAutoPixmapStorage::AllocSize(const SkImageInfo& info, size_t* rowBytes) noexcept {
  size_t rb = info.minRowBytes();
  if (rowBytes) {
    *rowBytes = rb;
  }
  return info.computeByteSize(rb);
}

bool SkAutoPixmapStorage::tryAlloc(const SkImageInfo& info) noexcept {
  this->freeStorage();

  size_t rb;
  size_t size = AllocSize(info, &rb);
  if (SkImageInfo::ByteSizeOverflowed(size)) {
    return false;
  }
  void* pixels = sk_malloc_canfail(size);
  if (nullptr == pixels) {
    return false;
  }
  this->reset(info, pixels, rb);
  fStorage = pixels;
  return true;
}

void SkAutoPixmapStorage::alloc(const SkImageInfo& info) noexcept {
  SkASSERT_RELEASE(this->tryAlloc(info));
}

sk_sp<SkData> SkAutoPixmapStorage::detachPixelsAsData() {
  if (!fStorage) {
    return nullptr;
  }

  sk_sp<SkData> data = SkData::MakeFromMalloc(fStorage, this->computeByteSize());
  fStorage = nullptr;
  this->INHERITED::reset();

  return data;
}
