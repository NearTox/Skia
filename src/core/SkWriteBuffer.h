/*
 * Copyright 2011 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef SkWriteBuffer_DEFINED
#define SkWriteBuffer_DEFINED

#include "include/core/SkData.h"
#include "include/core/SkFlattenable.h"
#include "include/core/SkSerialProcs.h"
#include "include/private/SkTHash.h"
#include "src/core/SkWriter32.h"

class SkFactorySet;
class SkImage;
class SkPath;
class SkRefCntSet;

class SkWriteBuffer {
 public:
  constexpr SkWriteBuffer() noexcept {}
  virtual ~SkWriteBuffer() {}

  virtual void writePad32(const void* buffer, size_t bytes) noexcept = 0;

  virtual void writeByteArray(const void* data, size_t size) noexcept = 0;
  void writeDataAsByteArray(SkData* data) noexcept {
    this->writeByteArray(data->data(), data->size());
  }
  virtual void writeBool(bool value) noexcept = 0;
  virtual void writeScalar(SkScalar value) noexcept = 0;
  virtual void writeScalarArray(const SkScalar* value, uint32_t count) noexcept = 0;
  virtual void writeInt(int32_t value) noexcept = 0;
  virtual void writeIntArray(const int32_t* value, uint32_t count) noexcept = 0;
  virtual void writeUInt(uint32_t value) noexcept = 0;
  void write32(int32_t value) noexcept { this->writeInt(value); }
  virtual void writeString(const char* value) noexcept = 0;

  virtual void writeFlattenable(const SkFlattenable* flattenable) = 0;
  virtual void writeColor(SkColor color) noexcept = 0;
  virtual void writeColorArray(const SkColor* color, uint32_t count) noexcept = 0;
  virtual void writeColor4f(const SkColor4f& color) noexcept = 0;
  virtual void writeColor4fArray(const SkColor4f* color, uint32_t count) noexcept = 0;
  virtual void writePoint(const SkPoint& point) noexcept = 0;
  virtual void writePointArray(const SkPoint* point, uint32_t count) noexcept = 0;
  virtual void writePoint3(const SkPoint3& point) noexcept = 0;
  virtual void writeMatrix(const SkMatrix& matrix) noexcept = 0;
  virtual void writeIRect(const SkIRect& rect) noexcept = 0;
  virtual void writeRect(const SkRect& rect) noexcept = 0;
  virtual void writeRegion(const SkRegion& region) noexcept = 0;
  virtual void writePath(const SkPath& path) noexcept = 0;
  virtual size_t writeStream(SkStream* stream, size_t length) noexcept = 0;
  virtual void writeImage(const SkImage*) = 0;
  virtual void writeTypeface(SkTypeface* typeface) = 0;
  virtual void writePaint(const SkPaint& paint) = 0;

  void setSerialProcs(const SkSerialProcs& procs) noexcept { fProcs = procs; }

 protected:
  SkSerialProcs fProcs;

  friend class SkPicturePriv;  // fProcs
};

/**
 * Concrete implementation that serializes to a flat binary blob.
 */
class SkBinaryWriteBuffer : public SkWriteBuffer {
 public:
  SkBinaryWriteBuffer();
  SkBinaryWriteBuffer(void* initialStorage, size_t storageSize);
  ~SkBinaryWriteBuffer() override;

  void write(const void* buffer, size_t bytes) noexcept { fWriter.write(buffer, bytes); }
  void writePad32(const void* buffer, size_t bytes) noexcept override {
    fWriter.writePad(buffer, bytes);
  }

  void reset(void* storage = nullptr, size_t storageSize = 0) noexcept {
    fWriter.reset(storage, storageSize);
  }

  size_t bytesWritten() const noexcept { return fWriter.bytesWritten(); }

  // Returns true iff all of the bytes written so far are stored in the initial storage
  // buffer provided in the constructor or the most recent call to reset.
  bool usingInitialStorage() const noexcept;

  void writeByteArray(const void* data, size_t size) noexcept override;
  void writeBool(bool value) noexcept override;
  void writeScalar(SkScalar value) noexcept override;
  void writeScalarArray(const SkScalar* value, uint32_t count) noexcept override;
  void writeInt(int32_t value) noexcept override;
  void writeIntArray(const int32_t* value, uint32_t count) noexcept override;
  void writeUInt(uint32_t value) noexcept override;
  void writeString(const char* value) noexcept override;

  void writeFlattenable(const SkFlattenable* flattenable) override;
  void writeColor(SkColor color) noexcept override;
  void writeColorArray(const SkColor* color, uint32_t count) noexcept override;
  void writeColor4f(const SkColor4f& color) noexcept override;
  void writeColor4fArray(const SkColor4f* color, uint32_t count) noexcept override;
  void writePoint(const SkPoint& point) noexcept override;
  void writePointArray(const SkPoint* point, uint32_t count) noexcept override;
  void writePoint3(const SkPoint3& point) noexcept override;
  void writeMatrix(const SkMatrix& matrix) noexcept override;
  void writeIRect(const SkIRect& rect) noexcept override;
  void writeRect(const SkRect& rect) noexcept override;
  void writeRegion(const SkRegion& region) noexcept override;
  void writePath(const SkPath& path) noexcept override;
  size_t writeStream(SkStream* stream, size_t length) noexcept override;
  void writeImage(const SkImage*) override;
  void writeTypeface(SkTypeface* typeface) override;
  void writePaint(const SkPaint& paint) override;

  bool writeToStream(SkWStream*) const noexcept;
  void writeToMemory(void* dst) const noexcept { fWriter.flatten(dst); }

  void setFactoryRecorder(sk_sp<SkFactorySet>) noexcept;
  void setTypefaceRecorder(sk_sp<SkRefCntSet>) noexcept;

 private:
  sk_sp<SkFactorySet> fFactorySet;
  sk_sp<SkRefCntSet> fTFSet;

  SkWriter32 fWriter;

  // Only used if we do not have an fFactorySet
  SkTHashMap<SkFlattenable::Factory, uint32_t> fFlattenableDict;
};

#endif  // SkWriteBuffer_DEFINED
