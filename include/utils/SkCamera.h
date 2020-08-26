/*
 * Copyright 2006 The Android Open Source Project
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

//  Inspired by Rob Johnson's most excellent QuickDraw GX sample code

#ifndef SkCamera_DEFINED
#define SkCamera_DEFINED

#include "include/core/SkM44.h"
#include "include/core/SkMatrix.h"
#include "include/private/SkNoncopyable.h"

// NOTE -- This entire header / impl is deprecated, and will be removed from Skia soon.
//
// Skia now has support for a 4x matrix (SkM44) in SkCanvas.
//

class SkCanvas;

// DEPRECATED
class SkPatch3D {
 public:
  SkPatch3D() noexcept;

  void reset() noexcept;
  void transform(const SkM44&, SkPatch3D* dst = nullptr) const noexcept;

  // dot a unit vector with the patch's normal
  SkScalar dotWith(SkScalar dx, SkScalar dy, SkScalar dz) const noexcept;
  SkScalar dotWith(const SkV3& v) const noexcept { return this->dotWith(v.x, v.y, v.z); }

  // deprecated, but still here for animator (for now)
  void rotate(SkScalar /*x*/, SkScalar /*y*/, SkScalar /*z*/) noexcept {}
  void rotateDegrees(SkScalar /*x*/, SkScalar /*y*/, SkScalar /*z*/) noexcept {}

 private:
 public:  // make public for SkDraw3D for now
  SkV3 fU, fV;
  SkV3 fOrigin;

  friend class SkCamera3D;
};

// DEPRECATED
class SkCamera3D {
 public:
  SkCamera3D() noexcept;

  void reset() noexcept;
  void update() noexcept;
  void patchToMatrix(const SkPatch3D&, SkMatrix* matrix) const noexcept;

  SkV3 fLocation;  // origin of the camera's space
  SkV3 fAxis;      // view direction
  SkV3 fZenith;    // up direction
  SkV3 fObserver;  // eye position (may not be the same as the origin)

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

  void getMatrix(SkMatrix*) const noexcept;
  void applyToCanvas(SkCanvas*) const;

  SkScalar dotWithNormal(SkScalar dx, SkScalar dy, SkScalar dz) const noexcept;

 private:
  struct Rec {
    Rec* fNext;
    SkM44 fMatrix;
  };
  Rec* fRec;
  Rec fInitialRec;
  SkCamera3D fCamera;
};

#endif
