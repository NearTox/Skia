/*
 * Copyright 2021 Google LLC.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef SKSL_POSITION
#define SKSL_POSITION

#include "include/core/SkTypes.h"
#include <string_view>

namespace SkSL {

class Position {
 public:
  constexpr Position() noexcept : fStartOffset(-1), fLength(0) {}

  static Position Range(int startOffset, int endOffset) noexcept {
    SkASSERT(startOffset <= endOffset);
    SkASSERT(startOffset <= 0xFFFFFF);
    int length = endOffset - startOffset;
    Position result;
    result.fStartOffset = startOffset;
    result.fLength = length <= 0xFF ? length : 0xFF;
    return result;
  }

  bool valid() const noexcept { return fStartOffset != -1; }

  int line(std::string_view source) const;

  int startOffset() const noexcept {
    SkASSERT(this->valid());
    return fStartOffset;
  }

  int endOffset() const noexcept {
    SkASSERT(this->valid());
    return fStartOffset + fLength;
  }

  // Returns the position from this through, and including the entirety of, end.
  Position rangeThrough(Position end) const {
    if (fStartOffset == -1 || end.fStartOffset == -1) {
      return *this;
    }
    SkASSERTF(
        this->startOffset() <= end.startOffset() && this->endOffset() <= end.endOffset(),
        "Invalid range: (%d-%d) - (%d-%d)\n", this->startOffset(), this->endOffset(),
        end.startOffset(), end.endOffset());
    return Range(this->startOffset(), end.endOffset());
  }

  // Returns a position representing the character immediately after this position
  Position after() const {
    int endOffset = this->endOffset();
    return Range(endOffset, endOffset + 1);
  }

  bool operator==(const Position& other) const noexcept {
    return fStartOffset == other.fStartOffset && fLength == other.fLength;
  }

  bool operator!=(const Position& other) const noexcept { return !(*this == other); }

  bool operator>(const Position& other) const noexcept { return fStartOffset > other.fStartOffset; }

  bool operator>=(const Position& other) const noexcept {
    return fStartOffset >= other.fStartOffset;
  }

  bool operator<(const Position& other) const noexcept { return fStartOffset < other.fStartOffset; }

  bool operator<=(const Position& other) const noexcept {
    return fStartOffset <= other.fStartOffset;
  }

 private:
  int32_t fStartOffset : 24;
  uint32_t fLength : 8;
};

struct ForLoopPositions {
  Position initPosition = Position();
  Position conditionPosition = Position();
  Position nextPosition = Position();
};

}  // namespace SkSL

#endif
