/*
 * Copyright 2020 Google LLC
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef SkSLSampleUsage_DEFINED
#define SkSLSampleUsage_DEFINED

#include "include/core/SkTypes.h"

#include <string>

namespace SkSL {

/**
 * Represents all of the ways that a fragment processor is sampled by its parent.
 */
class SampleUsage {
 public:
  enum class Kind {
    // Child is never sampled
    kNone,
    // Child is only sampled at the same coordinates as the parent
    kPassThrough,
    // Child is sampled with a matrix whose value is uniform
    kUniformMatrix,
    // Child is sampled with sk_FragCoord.xy
    kFragCoord,
    // Child is sampled using explicit coordinates
    kExplicit,
  };

  // Make a SampleUsage that corresponds to no sampling of the child at all
  SampleUsage() noexcept = default;

  SampleUsage(Kind kind, bool hasPerspective) noexcept
      : fKind(kind), fHasPerspective(hasPerspective) {
    if (kind != Kind::kUniformMatrix) {
      SkASSERT(!fHasPerspective);
    }
  }

  // Child is sampled with a matrix whose value is uniform. The name is fixed.
  static SampleUsage UniformMatrix(bool hasPerspective) noexcept {
    return SampleUsage(Kind::kUniformMatrix, hasPerspective);
  }

  static SampleUsage Explicit() noexcept { return SampleUsage(Kind::kExplicit, false); }

  static SampleUsage PassThrough() noexcept { return SampleUsage(Kind::kPassThrough, false); }

  static SampleUsage FragCoord() noexcept { return SampleUsage(Kind::kFragCoord, false); }

  bool operator==(const SampleUsage& that) const noexcept {
    return fKind == that.fKind && fHasPerspective == that.fHasPerspective;
  }

  bool operator!=(const SampleUsage& that) const noexcept { return !(*this == that); }

  // Arbitrary name used by all uniform sampling matrices
  static const char* MatrixUniformName() noexcept { return "matrix"; }

  SampleUsage merge(const SampleUsage& other);

  Kind kind() const noexcept { return fKind; }

  bool hasPerspective() const noexcept { return fHasPerspective; }

  bool isSampled() const noexcept { return fKind != Kind::kNone; }
  bool isPassThrough() const noexcept { return fKind == Kind::kPassThrough; }
  bool isExplicit() const noexcept { return fKind == Kind::kExplicit; }
  bool isUniformMatrix() const noexcept { return fKind == Kind::kUniformMatrix; }
  bool isFragCoord() const noexcept { return fKind == Kind::kFragCoord; }

  std::string constructor() const;

 private:
  Kind fKind = Kind::kNone;
  bool fHasPerspective = false;  // Only valid if fKind is kUniformMatrix
};

}  // namespace SkSL

#endif
