/*
 * Copyright 2021 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef GrDrawIndirectCommand_DEFINED
#define GrDrawIndirectCommand_DEFINED

#include <stdint.h>
#include <utility>

struct GrDrawIndirectCommand {
  uint32_t fVertexCount;
  uint32_t fInstanceCount;
  int32_t fBaseVertex;
  uint32_t fBaseInstance;
};

static_assert(sizeof(GrDrawIndirectCommand) == 16, "GrDrawIndirectCommand must be tightly packed");

struct GrDrawIndexedIndirectCommand {
  uint32_t fIndexCount;
  uint32_t fInstanceCount;
  uint32_t fBaseIndex;
  int32_t fBaseVertex;
  uint32_t fBaseInstance;
};

static_assert(
    sizeof(GrDrawIndexedIndirectCommand) == 20,
    "GrDrawIndexedIndirectCommand must be tightly packed");

// Helper for writing commands to an indirect draw buffer. Usage:
//
//    GrDrawIndirectWriter indirectWriter = target->makeDrawIndirectSpace(...);
//    indirectWriter.write(...);
//    indirectWriter.write(...);
struct GrDrawIndirectWriter {
 public:
  GrDrawIndirectWriter() = default;
  GrDrawIndirectWriter(void* data) noexcept : fData(static_cast<GrDrawIndirectCommand*>(data)) {}
  GrDrawIndirectWriter(const GrDrawIndirectWriter&) = delete;
  GrDrawIndirectWriter(GrDrawIndirectWriter&& that) noexcept { *this = std::move(that); }

  GrDrawIndirectWriter& operator=(const GrDrawIndirectWriter&) = delete;
  GrDrawIndirectWriter& operator=(GrDrawIndirectWriter&& that) noexcept {
    fData = that.fData;
    that.fData = nullptr;
    return *this;
  }

  bool operator==(const GrDrawIndirectWriter& that) noexcept { return fData == that.fData; }

  explicit operator bool() const noexcept { return fData != nullptr; }

  GrDrawIndirectWriter makeOffset(int drawCount) const noexcept { return {fData + drawCount}; }

  inline void write(
      uint32_t instanceCount, uint32_t baseInstance, uint32_t vertexCount,
      int32_t baseVertex) noexcept {
    *fData++ = {vertexCount, instanceCount, baseVertex, baseInstance};
  }

 private:
  GrDrawIndirectCommand* fData;
};

// Helper for writing commands to an indexed indirect draw buffer. Usage:
//
//    GrDrawIndexedIndirectWriter indirectWriter = target->makeDrawIndexedIndirectSpace(...);
//    indirectWriter.writeIndexed(...);
//    indirectWriter.writeIndexed(...);
struct GrDrawIndexedIndirectWriter {
 public:
  GrDrawIndexedIndirectWriter() = default;
  GrDrawIndexedIndirectWriter(void* data) noexcept 
      : fData(static_cast<GrDrawIndexedIndirectCommand*>(data)) {}
  GrDrawIndexedIndirectWriter(const GrDrawIndexedIndirectWriter&) = delete;
  GrDrawIndexedIndirectWriter(GrDrawIndexedIndirectWriter&& that) noexcept {
    *this = std::move(that);
  }

  GrDrawIndexedIndirectWriter& operator=(const GrDrawIndexedIndirectWriter&) = delete;
  GrDrawIndexedIndirectWriter& operator=(GrDrawIndexedIndirectWriter&& that) noexcept {
    fData = that.fData;
    that.fData = nullptr;
    return *this;
  }

  bool operator==(const GrDrawIndexedIndirectWriter& that) noexcept { return fData == that.fData; }

  explicit operator bool() const noexcept { return fData != nullptr; }

  GrDrawIndexedIndirectWriter makeOffset(int drawCount) const { return {fData + drawCount}; }

  inline void writeIndexed(
      uint32_t indexCount, uint32_t baseIndex, uint32_t instanceCount, uint32_t baseInstance,
      int32_t baseVertex) noexcept {
    *fData++ = {indexCount, instanceCount, baseIndex, baseVertex, baseInstance};
  }

 private:
  GrDrawIndexedIndirectCommand* fData;
};

#endif
