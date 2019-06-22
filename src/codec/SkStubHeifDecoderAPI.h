/*
 * Copyright 2017 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef SkStubHeifDecoderAPI_DEFINED
#define SkStubHeifDecoderAPI_DEFINED

// This stub implementation of HeifDecoderAPI.h lets us compile SkHeifCodec.cpp
// even when libheif is not available.  It, of course, does nothing and fails to decode.

#include <stddef.h>
#include <stdint.h>
#include <memory>

enum HeifColorFormat {
  kHeifColorFormat_RGB565,
  kHeifColorFormat_RGBA_8888,
  kHeifColorFormat_BGRA_8888,
};

struct HeifStream {
  virtual ~HeifStream() {}

  virtual size_t read(void*, size_t) noexcept = 0;
  virtual bool rewind() noexcept = 0;
  virtual bool seek(size_t) noexcept = 0;
  virtual bool hasLength() const noexcept = 0;
  virtual size_t getLength() const noexcept = 0;
};

struct HeifFrameInfo {
  int mRotationAngle;
  int mWidth;
  int mHeight;
  int mBytesPerPixel;

  size_t mIccSize;
  std::unique_ptr<char[]> mIccData;
};

struct HeifDecoder {
  bool init(HeifStream* stream, HeifFrameInfo*) noexcept {
    delete stream;
    return false;
  }

  bool decode(HeifFrameInfo*) noexcept { return false; }

  bool setOutputColor(HeifColorFormat) noexcept { return false; }

  bool getScanline(uint8_t*) noexcept { return false; }

  int skipScanlines(int) noexcept { return 0; }
};

static inline HeifDecoder* createHeifDecoder() { return new HeifDecoder; }

#endif  // SkStubHeifDecoderAPI_DEFINED
