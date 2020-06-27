/*
 * Copyright 2006 The Android Open Source Project
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

//  Inspired by Rob Johnson's most excellent QuickDraw GX sample code

#ifndef SkCamera_DEFINED
#define SkCamera_DEFINED

#include "include/core/SkMatrix.h"
#include "include/private/SkNoncopyable.h"

// NOTE -- This entire header / impl is deprecated, and will be removed from Skia soon.
//
// Skia now has support for a 4x matrix (SkM44) in SkCanvas.
//

class SkCanvas;

// DEPRECATED
struct SkUnit3D {
  SkScalar fX, fY, fZ;

  constexpr void set(SkScalar x, SkScalar y, SkScalar z) noexcept {
    fX = x;
    fY = y;
    fZ = z;
  }
  static SkScalar Dot(const SkUnit3D&, const SkUnit3D&) noexcept;
  static void Cross(const SkUnit3D&, const SkUnit3D&, SkUnit3D* cross) noexcept;
};

// DEPRECATED
struct SkPoint3D {
  SkScalar fX, fY, fZ;

  constexpr void set(SkScalar x, SkScalar y, SkScalar z) noexcept {
    fX = x;
    fY = y;
    fZ = z;
  }
  SkScalar normalize(SkUnit3D*) const noexcept;
};
typedef SkPoint3D SkVector3D;

// DEPRECATED
struct SkMatrix3D {
  SkScalar fMat[3][4];

  void reset() noexcept;

  void setRow(int row, SkScalar a, SkScalar b, SkScalar c, SkScalar d = 0) noexcept {
    SkASSERT((unsigned)row < 3);
    fMat[row][0] = a;
    fMat[row][1] = b;
    fMat[row][2] = c;
    fMat[row][3] = d;
  }

  void setRotateX(SkScalar deg) noexcept;
  void setRotateY(SkScalar deg) noexcept;
  void setRotateZ(SkScalar deg) noexcept;
  void setTranslate(SkScalar x, SkScalar y, SkScalar z) noexcept;

  void preRotateX(SkScalar deg) noexcept;
  void preRotateY(SkScalar deg) noexcept;
  void preRotateZ(SkScalar deg) noexcept;
  void preTranslate(SkScalar x, SkScalar y, SkScalar z) noexcept;

  void setConcat(const SkMatrix3D& a, const SkMatrix3D& b) noexcept;
  void mapPoint(const SkPoint3D& src, SkPoint3D* dst) const noexcept;
  void mapVector(const SkVector3D& src, SkVector3D* dst) const noexcept;

  void mapPoint(SkPoint3D* v) const noexcept { this->mapPoint(*v, v); }

  void mapVector(SkVector3D* v) const noexcept { this->mapVector(*v, v); }
};

// DEPRECATED
class SkPatch3D {
 public:
  SkPatch3D() noexcept;

  void reset() noexcept;
  void transform(const SkMatrix3D&, SkPatch3D* dst = nullptr) const noexcept;

  // dot a unit vector with the patch's normal
  SkScalar dotWith(SkScalar dx, SkScalar dy, SkScalar dz) const noexcept;
  SkScalar dotWith(const SkVector3D& v) const noexcept { return this->dotWith(v.fX, v.fY, v.fZ); }

  // deprecated, but still here for animator (for now)
  void rotate(SkScalar /*x*/, SkScalar /*y*/, SkScalar /*z*/) {}
  void rotateDegrees(SkScalar /*x*/, SkScalar /*y*/, SkScalar /*z*/) {}

 private:
 public:  // make public for SkDraw3D for now
  SkVector3D fU, fV;
  SkPoint3D fOrigin;

  friend class SkCamera3D;
};

// DEPRECATED
class SkCamera3D {
 public:
  SkCamera3D() noexcept;

  void reset() noexcept;
  void update() noexcept;
  void patchToMatrix(const SkPatch3D&, SkMatrix* matrix) const;

  SkPoint3D fLocation;  // origin of the camera's space
  SkPoint3D fAxis;      // view direction
  SkPoint3D fZenith;    // up direction
  SkPoint3D fObserver;  // eye position (may not be the same as the origin)

 private:
  mutable SkMatrix fOrientation;
  mutable bool fNeedToUpdate;

  void doUpdate() const noexcept;
};

// DEPRECATED
class SK_API Sk3DView : SkNoncopyable {
 public:
  Sk3DView() noexcept;
  ~Sk3DView();

  void save();
  void restore() noexcept;

  void translate(SkScalar x, SkScalar y, SkScalar z) noexcept;
  void rotateX(SkScalar deg) noexcept;
  void rotateY(SkScalar deg) noexcept;
  void rotateZ(SkScalar deg) noexcept;

#ifdef SK_BUILD_FOR_ANDROID_FRAMEWORK
  void setCameraLocation(SkScalar x, SkScalar y, SkScalar z);
  SkScalar getCameraLocationX() const;
  SkScalar getCameraLocationY() const;
  SkScalar getCameraLocationZ() const;
#endif

  void getMatrix(SkMatrix*) const;
  void applyToCanvas(SkCanvas*) const;

  SkScalar dotWithNormal(SkScalar dx, SkScalar dy, SkScalar dz) const noexcept;

 private:
  struct Rec {
    Rec* fNext;
    SkMatrix3D fMatrix;
  };
  Rec* fRec;
  Rec fInitialRec;
  SkCamera3D fCamera;
};

#endif
