/*
 * Copyright 2017 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef SkSGPath_DEFINED
#define SkSGPath_DEFINED

#include "modules/sksg/include/SkSGGeometryNode.h"

#include "include/core/SkPath.h"

class SkCanvas;
class SkPaint;

namespace sksg {

/**
 * Concrete Geometry node, wrapping an SkPath.
 */
class Path : public GeometryNode {
 public:
  static sk_sp<Path> Make() { return sk_sp<Path>(new Path(SkPath())); }
  static sk_sp<Path> Make(const SkPath& r) { return sk_sp<Path>(new Path(r)); }

  SG_ATTRIBUTE(Path, SkPath, fPath)

  // Temporarily inlined for SkPathFillType staging
  // SG_MAPPED_ATTRIBUTE(FillType, SkPathFillType, fPath)

  SkPathFillType getFillType() const noexcept { return fPath.getFillType(); }

  void setFillType(SkPathFillType fillType) noexcept {
    if (fillType != fPath.getFillType()) {
      fPath.setFillType(fillType);
      this->invalidate();
    }
  }

 protected:
  void onClip(SkCanvas*, bool antiAlias) const override;
  void onDraw(SkCanvas*, const SkPaint&) const override;
  bool onContains(const SkPoint&) const noexcept override;

  SkRect onRevalidate(InvalidationController*, const SkMatrix&) noexcept override;
  SkPath onAsPath() const noexcept override;

 private:
  explicit Path(const SkPath&) noexcept;

  SkPath fPath;

  using INHERITED = GeometryNode;
};

}  // namespace sksg

#endif  // SkSGPath_DEFINED
