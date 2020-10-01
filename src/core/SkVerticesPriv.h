/*
 * Copyright 2020 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef SkVerticesPriv_DEFINED
#define SkVerticesPriv_DEFINED

#include "include/core/SkVertices.h"

class SkReadBuffer;
class SkWriteBuffer;

struct SkVertices_DeprecatedBone {
  float values[6];
};

/** Class that adds methods to SkVertices that are only intended for use internal to Skia.
    This class is purely a privileged window into SkVertices. It should never have additional
    data members or virtual methods. */
class SkVerticesPriv {
 public:
  SkVertices::VertexMode mode() const noexcept { return fVertices->fMode; }

  bool hasCustomData() const noexcept { return SkToBool(fVertices->fCustomData); }
  bool hasColors() const noexcept { return SkToBool(fVertices->fColors); }
  bool hasTexCoords() const noexcept { return SkToBool(fVertices->fTexs); }
  bool hasIndices() const noexcept { return SkToBool(fVertices->fIndices); }

  bool hasUsage(SkVertices::Attribute::Usage) const;

  int vertexCount() const noexcept { return fVertices->fVertexCount; }
  int indexCount() const noexcept { return fVertices->fIndexCount; }
  int attributeCount() const noexcept { return fVertices->fAttributeCount; }
  size_t customDataSize() const;

  const SkPoint* positions() const noexcept { return fVertices->fPositions; }
  const void* customData() const noexcept { return fVertices->fCustomData; }
  const SkPoint* texCoords() const noexcept { return fVertices->fTexs; }
  const SkColor* colors() const noexcept { return fVertices->fColors; }
  const uint16_t* indices() const noexcept { return fVertices->fIndices; }
  const SkVertices::Attribute* attributes() const noexcept { return fVertices->fAttributes; }

  // Never called due to RVO in priv(), but must exist for MSVC 2017.
  SkVerticesPriv(const SkVerticesPriv&) = default;

  void encode(SkWriteBuffer&) const;
  static sk_sp<SkVertices> Decode(SkReadBuffer&);

 private:
  explicit SkVerticesPriv(SkVertices* vertices) noexcept : fVertices(vertices) {}
  SkVerticesPriv& operator=(const SkVerticesPriv&) = delete;

  // No taking addresses of this type
  const SkVerticesPriv* operator&() const = delete;
  SkVerticesPriv* operator&() = delete;

  SkVertices* fVertices;

  friend class SkVertices;  // to construct this type
};

inline SkVerticesPriv SkVertices::priv() noexcept { return SkVerticesPriv(this); }

inline const SkVerticesPriv SkVertices::priv()
    const noexcept {  // NOLINT(readability-const-return-type)
  return SkVerticesPriv(const_cast<SkVertices*>(this));
}

#endif
