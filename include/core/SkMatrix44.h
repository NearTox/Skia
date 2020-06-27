/*
 * Copyright 2011 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef SkMatrix44_DEFINED
#define SkMatrix44_DEFINED

#include "include/core/SkMatrix.h"
#include "include/core/SkScalar.h"

#include <atomic>
#include <cstring>

// This entire file is DEPRECATED, and will be removed at some point.
// SkCanvas has full support for 4x4 matrices using SkM44

// DEPRECATED
struct SkVector4 {
  SkScalar fData[4];

  SkVector4() noexcept { this->set(0, 0, 0, 1); }
  SkVector4(const SkVector4& src) noexcept { memcpy(fData, src.fData, sizeof(fData)); }
  SkVector4(SkScalar x, SkScalar y, SkScalar z, SkScalar w = SK_Scalar1) noexcept {
    fData[0] = x;
    fData[1] = y;
    fData[2] = z;
    fData[3] = w;
  }

  SkVector4& operator=(const SkVector4& src) noexcept {
    memcpy(fData, src.fData, sizeof(fData));
    return *this;
  }

  bool operator==(const SkVector4& v) const noexcept {
    return fData[0] == v.fData[0] && fData[1] == v.fData[1] && fData[2] == v.fData[2] &&
           fData[3] == v.fData[3];
  }
  bool operator!=(const SkVector4& v) const noexcept { return !(*this == v); }
  bool equals(SkScalar x, SkScalar y, SkScalar z, SkScalar w = SK_Scalar1) noexcept {
    return fData[0] == x && fData[1] == y && fData[2] == z && fData[3] == w;
  }

  void set(SkScalar x, SkScalar y, SkScalar z, SkScalar w = SK_Scalar1) noexcept {
    fData[0] = x;
    fData[1] = y;
    fData[2] = z;
    fData[3] = w;
  }
};

// DEPRECATED
class SK_API SkMatrix44 {
 public:
  enum Uninitialized_Constructor { kUninitialized_Constructor };
  enum Identity_Constructor { kIdentity_Constructor };
  enum NaN_Constructor { kNaN_Constructor };

  SkMatrix44(Uninitialized_Constructor) noexcept {}  // ironically, cannot be constexpr

  constexpr SkMatrix44(Identity_Constructor)noexcept
        : fMat{{ 1, 0, 0, 0, },
               { 0, 1, 0, 0, },
               { 0, 0, 1, 0, },
               { 0, 0, 0, 1, }}
        , fTypeMask(kIdentity_Mask) {}

  constexpr SkMatrix44(NaN_Constructor) noexcept
      : fMat{{SK_ScalarNaN, SK_ScalarNaN, SK_ScalarNaN, SK_ScalarNaN}, {SK_ScalarNaN, SK_ScalarNaN, SK_ScalarNaN, SK_ScalarNaN}, {SK_ScalarNaN, SK_ScalarNaN, SK_ScalarNaN, SK_ScalarNaN}, {SK_ScalarNaN, SK_ScalarNaN, SK_ScalarNaN, SK_ScalarNaN}},
        fTypeMask(kTranslate_Mask | kScale_Mask | kAffine_Mask | kPerspective_Mask) {}

  constexpr SkMatrix44() noexcept : SkMatrix44{kIdentity_Constructor} {}

  SkMatrix44(const SkMatrix44& src) noexcept = default;

  SkMatrix44& operator=(const SkMatrix44& src) noexcept = default;

  SkMatrix44(const SkMatrix44& a, const SkMatrix44& b) noexcept { this->setConcat(a, b); }

  bool operator==(const SkMatrix44& other) const noexcept;
  bool operator!=(const SkMatrix44& other) const noexcept { return !(other == *this); }

  /* When converting from SkMatrix44 to SkMatrix, the third row and
   * column is dropped.  When converting from SkMatrix to SkMatrix44
   * the third row and column remain as identity:
   * [ a b c ]      [ a b 0 c ]
   * [ d e f ]  ->  [ d e 0 f ]
   * [ g h i ]      [ 0 0 1 0 ]
   *                [ g h 0 i ]
   */
  SkMatrix44(const SkMatrix&) noexcept;
  SkMatrix44& operator=(const SkMatrix& src) noexcept;

  // TODO: make this explicit (will need to guard that change to update chrome, etc.
#ifndef SK_SUPPORT_LEGACY_IMPLICIT_CONVERSION_MATRIX44
  explicit
#endif
  operator SkMatrix() const noexcept;

  /**
   *  Return a reference to a const identity matrix
   */
  static const SkMatrix44& I() noexcept;

  using TypeMask = uint8_t;
  enum : TypeMask {
    kIdentity_Mask = 0,
    kTranslate_Mask = 1 << 0,    //!< set if the matrix has translation
    kScale_Mask = 1 << 1,        //!< set if the matrix has any scale != 1
    kAffine_Mask = 1 << 2,       //!< set if the matrix skews or rotates
    kPerspective_Mask = 1 << 3,  //!< set if the matrix is in perspective
  };

  /**
   *  Returns a bitfield describing the transformations the matrix may
   *  perform. The bitfield is computed conservatively, so it may include
   *  false positives. For example, when kPerspective_Mask is true, all
   *  other bits may be set to true even in the case of a pure perspective
   *  transform.
   */
  inline TypeMask getType() const noexcept { return fTypeMask; }

  /**
   *  Return true if the matrix is identity.
   */
  inline bool isIdentity() const noexcept { return kIdentity_Mask == this->getType(); }

  /**
   *  Return true if the matrix contains translate or is identity.
   */
  inline bool isTranslate() const noexcept { return !(this->getType() & ~kTranslate_Mask); }

  /**
   *  Return true if the matrix only contains scale or translate or is identity.
   */
  inline bool isScaleTranslate() const noexcept {
    return !(this->getType() & ~(kScale_Mask | kTranslate_Mask));
  }

  /**
   *  Returns true if the matrix only contains scale or is identity.
   */
  inline bool isScale() const noexcept { return !(this->getType() & ~kScale_Mask); }

  inline bool hasPerspective() const noexcept {
    return SkToBool(this->getType() & kPerspective_Mask);
  }

  void setIdentity() noexcept;
  inline void reset() noexcept { this->setIdentity(); }

  /**
   *  get a value from the matrix. The row,col parameters work as follows:
   *  (0, 0)  scale-x
   *  (0, 3)  translate-x
   *  (3, 0)  perspective-x
   */
  inline SkScalar get(int row, int col) const noexcept {
    SkASSERT((unsigned)row <= 3);
    SkASSERT((unsigned)col <= 3);
    return fMat[col][row];
  }

  /**
   *  set a value in the matrix. The row,col parameters work as follows:
   *  (0, 0)  scale-x
   *  (0, 3)  translate-x
   *  (3, 0)  perspective-x
   */
  inline void set(int row, int col, SkScalar value) noexcept {
    SkASSERT((unsigned)row <= 3);
    SkASSERT((unsigned)col <= 3);
    fMat[col][row] = value;
    this->recomputeTypeMask();
  }

  inline double getDouble(int row, int col) const noexcept { return double(this->get(row, col)); }
  inline void setDouble(int row, int col, double value) noexcept {
    this->set(row, col, SkScalar(value));
  }
  inline float getFloat(int row, int col) const noexcept { return float(this->get(row, col)); }
  inline void setFloat(int row, int col, float value) noexcept { this->set(row, col, value); }

  /** These methods allow one to efficiently read matrix entries into an
   *  array. The given array must have room for exactly 16 entries. Whenever
   *  possible, they will try to use memcpy rather than an entry-by-entry
   *  copy.
   *
   *  Col major indicates that consecutive elements of columns will be stored
   *  contiguously in memory.  Row major indicates that consecutive elements
   *  of rows will be stored contiguously in memory.
   */
  void asColMajorf(float[]) const noexcept;
  void asColMajord(double[]) const noexcept;
  void asRowMajorf(float[]) const noexcept;
  void asRowMajord(double[]) const noexcept;

  /** These methods allow one to efficiently set all matrix entries from an
   *  array. The given array must have room for exactly 16 entries. Whenever
   *  possible, they will try to use memcpy rather than an entry-by-entry
   *  copy.
   *
   *  Col major indicates that input memory will be treated as if consecutive
   *  elements of columns are stored contiguously in memory.  Row major
   *  indicates that input memory will be treated as if consecutive elements
   *  of rows are stored contiguously in memory.
   */
  void setColMajorf(const float[]) noexcept;
  void setColMajord(const double[]) noexcept;
  void setRowMajorf(const float[]) noexcept;
  void setRowMajord(const double[]) noexcept;

  void setColMajor(const SkScalar data[]) noexcept { this->setColMajorf(data); }
  void setRowMajor(const SkScalar data[]) noexcept { this->setRowMajorf(data); }

  /* This sets the top-left of the matrix and clears the translation and
   * perspective components (with [3][3] set to 1).  m_ij is interpreted
   * as the matrix entry at row = i, col = j. */
  void set3x3(
      SkScalar m_00, SkScalar m_10, SkScalar m_20, SkScalar m_01, SkScalar m_11, SkScalar m_21,
      SkScalar m_02, SkScalar m_12, SkScalar m_22) noexcept;
  void set3x3RowMajorf(const float[]) noexcept;

  void set4x4(
      SkScalar m_00, SkScalar m_10, SkScalar m_20, SkScalar m_30, SkScalar m_01, SkScalar m_11,
      SkScalar m_21, SkScalar m_31, SkScalar m_02, SkScalar m_12, SkScalar m_22, SkScalar m_32,
      SkScalar m_03, SkScalar m_13, SkScalar m_23, SkScalar m_33) noexcept;

  SkMatrix44& setTranslate(SkScalar dx, SkScalar dy, SkScalar dz) noexcept;
  SkMatrix44& preTranslate(SkScalar dx, SkScalar dy, SkScalar dz) noexcept;
  SkMatrix44& postTranslate(SkScalar dx, SkScalar dy, SkScalar dz) noexcept;

  SkMatrix44& setScale(SkScalar sx, SkScalar sy, SkScalar sz) noexcept;
  SkMatrix44& preScale(SkScalar sx, SkScalar sy, SkScalar sz) noexcept;
  SkMatrix44& postScale(SkScalar sx, SkScalar sy, SkScalar sz) noexcept;

  inline SkMatrix44& setScale(SkScalar scale) noexcept {
    return this->setScale(scale, scale, scale);
  }
  inline SkMatrix44& preScale(SkScalar scale) noexcept {
    return this->preScale(scale, scale, scale);
  }
  inline SkMatrix44& postScale(SkScalar scale) noexcept {
    return this->postScale(scale, scale, scale);
  }

  void setRotateDegreesAbout(SkScalar x, SkScalar y, SkScalar z, SkScalar degrees) noexcept {
    this->setRotateAbout(x, y, z, degrees * SK_ScalarPI / 180);
  }

  /** Rotate about the vector [x,y,z]. If that vector is not unit-length,
      it will be automatically resized.
   */
  void setRotateAbout(SkScalar x, SkScalar y, SkScalar z, SkScalar radians) noexcept;
  /** Rotate about the vector [x,y,z]. Does not check the length of the
      vector, assuming it is unit-length.
   */
  void setRotateAboutUnit(SkScalar x, SkScalar y, SkScalar z, SkScalar radians) noexcept;

  void setConcat(const SkMatrix44& a, const SkMatrix44& b) noexcept;
  inline void preConcat(const SkMatrix44& m) noexcept { this->setConcat(*this, m); }
  inline void postConcat(const SkMatrix44& m) noexcept { this->setConcat(m, *this); }

  friend SkMatrix44 operator*(const SkMatrix44& a, const SkMatrix44& b) noexcept {
    return SkMatrix44(a, b);
  }

  /** If this is invertible, return that in inverse and return true. If it is
      not invertible, return false and leave the inverse parameter in an
      unspecified state.
   */
  bool invert(SkMatrix44* inverse) const noexcept;

  /** Transpose this matrix in place. */
  void transpose() noexcept;

  /** Apply the matrix to the src vector, returning the new vector in dst.
      It is legal for src and dst to point to the same memory.
   */
  void mapScalars(const SkScalar src[4], SkScalar dst[4]) const noexcept;
  inline void mapScalars(SkScalar vec[4]) const noexcept { this->mapScalars(vec, vec); }

  friend SkVector4 operator*(const SkMatrix44& m, const SkVector4& src) noexcept {
    SkVector4 dst;
    m.mapScalars(src.fData, dst.fData);
    return dst;
  }

  /**
   *  map an array of [x, y, 0, 1] through the matrix, returning an array
   *  of [x', y', z', w'].
   *
   *  @param src2     array of [x, y] pairs, with implied z=0 and w=1
   *  @param count    number of [x, y] pairs in src2
   *  @param dst4     array of [x', y', z', w'] quads as the output.
   */
  void map2(const float src2[], int count, float dst4[]) const;
  void map2(const double src2[], int count, double dst4[]) const;

  /** Returns true if transformating an axis-aligned square in 2d by this matrix
      will produce another 2d axis-aligned square; typically means the matrix
      is a scale with perhaps a 90-degree rotation. A 3d rotation through 90
      degrees into a perpendicular plane collapses a square to a line, but
      is still considered to be axis-aligned.

      By default, tolerates very slight error due to float imprecisions;
      a 90-degree rotation can still end up with 10^-17 of
      "non-axis-aligned" result.
   */
  bool preserves2dAxisAlignment(SkScalar epsilon = SK_ScalarNearlyZero) const noexcept;

  void dump() const;

  double determinant() const noexcept;

 private:
  /* This is indexed by [col][row]. */
  SkScalar fMat[4][4];
  TypeMask fTypeMask;

  static constexpr int kAllPublic_Masks = 0xF;

  void as3x4RowMajorf(float[]) const noexcept;
  void set3x4RowMajorf(const float[]) noexcept;

  SkScalar transX() const noexcept { return fMat[3][0]; }
  SkScalar transY() const noexcept { return fMat[3][1]; }
  SkScalar transZ() const noexcept { return fMat[3][2]; }

  SkScalar scaleX() const noexcept { return fMat[0][0]; }
  SkScalar scaleY() const noexcept { return fMat[1][1]; }
  SkScalar scaleZ() const noexcept { return fMat[2][2]; }

  SkScalar perspX() const noexcept { return fMat[0][3]; }
  SkScalar perspY() const noexcept { return fMat[1][3]; }
  SkScalar perspZ() const noexcept { return fMat[2][3]; }

  void recomputeTypeMask() noexcept;

  inline void setTypeMask(TypeMask mask) noexcept {
    SkASSERT(0 == (~kAllPublic_Masks & mask));
    fTypeMask = mask;
  }

  inline const SkScalar* values() const noexcept { return &fMat[0][0]; }

  friend class SkColorSpace;
  friend class SkCanvas;
  friend class SkM44;
};

#endif
