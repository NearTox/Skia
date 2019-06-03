/*
 * Copyright 2006 The Android Open Source Project
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef SkMatrix_DEFINED
#define SkMatrix_DEFINED

#include "include/core/SkRect.h"
#include "include/private/SkMacros.h"
#include "include/private/SkTo.h"

struct SkRSXform;
struct SkPoint3;
class SkString;

/** \class SkMatrix
    SkMatrix holds a 3x3 matrix for transforming coordinates. This allows mapping
    SkPoint and vectors with translation, scaling, skewing, rotation, and
    perspective.

    SkMatrix elements are in row major order. SkMatrix does not have a constructor,
    so it must be explicitly initialized. setIdentity() initializes SkMatrix
    so it has no effect. setTranslate(), setScale(), setSkew(), setRotate(), set9 and setAll()
    initializes all SkMatrix elements with the corresponding mapping.

    SkMatrix includes a hidden variable that classifies the type of matrix to
    improve performance. SkMatrix is not thread safe unless getType() is called first.
*/
SK_BEGIN_REQUIRE_DENSE
class SK_API SkMatrix {
public:
    /** Creates an identity SkMatrix:

            | 1 0 0 |
            | 0 1 0 |
            | 0 0 1 |
    */
    constexpr SkMatrix() noexcept : fMat{1, 0, 0, 0, 1, 0, 0, 0, 1}, fTypeMask{kIdentity_Mask} {}

    /** Sets SkMatrix to scale by (sx, sy). Returned matrix is:

            | sx  0  0 |
            |  0 sy  0 |
            |  0  0  1 |

        @param sx  horizontal scale factor
        @param sy  vertical scale factor
        @return    SkMatrix with scale
    */
    static SkMatrix SK_WARN_UNUSED_RESULT MakeScale(SkScalar sx, SkScalar sy) noexcept {
        SkMatrix m;
        m.setScale(sx, sy);
        return m;
    }

    /** Sets SkMatrix to scale by (scale, scale). Returned matrix is:

            | scale   0   0 |
            |   0   scale 0 |
            |   0     0   1 |

        @param scale  horizontal and vertical scale factor
        @return       SkMatrix with scale
    */
    static SkMatrix SK_WARN_UNUSED_RESULT MakeScale(SkScalar scale) noexcept {
        SkMatrix m;
        m.setScale(scale, scale);
        return m;
    }

    /** Sets SkMatrix to translate by (dx, dy). Returned matrix is:

            | 1 0 dx |
            | 0 1 dy |
            | 0 0  1 |

        @param dx  horizontal translation
        @param dy  vertical translation
        @return    SkMatrix with translation
    */
    static SkMatrix SK_WARN_UNUSED_RESULT MakeTrans(SkScalar dx, SkScalar dy) noexcept {
        SkMatrix m;
        m.setTranslate(dx, dy);
        return m;
    }

    /** Sets SkMatrix to:

            | scaleX  skewX transX |
            |  skewY scaleY transY |
            |  pers0  pers1  pers2 |

        @param scaleX  horizontal scale factor
        @param skewX   horizontal skew factor
        @param transX  horizontal translation
        @param skewY   vertical skew factor
        @param scaleY  vertical scale factor
        @param transY  vertical translation
        @param pers0   input x-axis perspective factor
        @param pers1   input y-axis perspective factor
        @param pers2   perspective scale factor
        @return        SkMatrix constructed from parameters
    */
    static SkMatrix SK_WARN_UNUSED_RESULT MakeAll(SkScalar scaleX, SkScalar skewX, SkScalar transX,
                                                  SkScalar skewY, SkScalar scaleY, SkScalar transY,
                                                  SkScalar pers0, SkScalar pers1,
                                                  SkScalar pers2) noexcept {
        SkMatrix m;
        m.setAll(scaleX, skewX, transX, skewY, scaleY, transY, pers0, pers1, pers2);
        return m;
    }

    /** \enum SkMatrix::TypeMask
        Enum of bit fields for mask returned by getType().
        Used to identify the complexity of SkMatrix, to optimize performance.
    */
    enum TypeMask {
        kIdentity_Mask = 0,        //!< identity SkMatrix; all bits clear
        kTranslate_Mask = 0x01,    //!< translation SkMatrix
        kScale_Mask = 0x02,        //!< scale SkMatrix
        kAffine_Mask = 0x04,       //!< skew or rotate SkMatrix
        kPerspective_Mask = 0x08,  //!< perspective SkMatrix
    };

    /** Returns a bit field describing the transformations the matrix may
        perform. The bit field is computed conservatively, so it may include
        false positives. For example, when kPerspective_Mask is set, all
        other bits are set.

        @return  kIdentity_Mask, or combinations of: kTranslate_Mask, kScale_Mask,
                 kAffine_Mask, kPerspective_Mask
    */
    TypeMask getType() const noexcept {
        if (fTypeMask & kUnknown_Mask) {
            fTypeMask = this->computeTypeMask();
        }
        // only return the public masks
        return (TypeMask)(fTypeMask & 0xF);
    }

    /** Returns true if SkMatrix is identity.  Identity matrix is:

            | 1 0 0 |
            | 0 1 0 |
            | 0 0 1 |

        @return  true if SkMatrix has no effect
    */
    bool isIdentity() const noexcept { return this->getType() == 0; }

    /** Returns true if SkMatrix at most scales and translates. SkMatrix may be identity,
        contain only scale elements, only translate elements, or both. SkMatrix form is:

            | scale-x    0    translate-x |
            |    0    scale-y translate-y |
            |    0       0         1      |

        @return  true if SkMatrix is identity; or scales, translates, or both
    */
    bool isScaleTranslate() const noexcept {
        return !(this->getType() & ~(kScale_Mask | kTranslate_Mask));
    }

    /** Returns true if SkMatrix is identity, or translates. SkMatrix form is:

            | 1 0 translate-x |
            | 0 1 translate-y |
            | 0 0      1      |

        @return  true if SkMatrix is identity, or translates
    */
    bool isTranslate() const noexcept { return !(this->getType() & ~(kTranslate_Mask)); }

    /** Returns true SkMatrix maps SkRect to another SkRect. If true, SkMatrix is identity,
        or scales, or rotates a multiple of 90 degrees, or mirrors on axes. In all
        cases, SkMatrix may also have translation. SkMatrix form is either:

            | scale-x    0    translate-x |
            |    0    scale-y translate-y |
            |    0       0         1      |

        or

            |    0     rotate-x translate-x |
            | rotate-y    0     translate-y |
            |    0        0          1      |

        for non-zero values of scale-x, scale-y, rotate-x, and rotate-y.

        Also called preservesAxisAlignment(); use the one that provides better inline
        documentation.

        @return  true if SkMatrix maps one SkRect into another
    */
    bool rectStaysRect() const noexcept {
        if (fTypeMask & kUnknown_Mask) {
            fTypeMask = this->computeTypeMask();
        }
        return (fTypeMask & kRectStaysRect_Mask) != 0;
    }

    /** Returns true SkMatrix maps SkRect to another SkRect. If true, SkMatrix is identity,
        or scales, or rotates a multiple of 90 degrees, or mirrors on axes. In all
        cases, SkMatrix may also have translation. SkMatrix form is either:

            | scale-x    0    translate-x |
            |    0    scale-y translate-y |
            |    0       0         1      |

        or

            |    0     rotate-x translate-x |
            | rotate-y    0     translate-y |
            |    0        0          1      |

        for non-zero values of scale-x, scale-y, rotate-x, and rotate-y.

        Also called rectStaysRect(); use the one that provides better inline
        documentation.

        @return  true if SkMatrix maps one SkRect into another
    */
    bool preservesAxisAlignment() const noexcept { return this->rectStaysRect(); }

    /** Returns true if the matrix contains perspective elements. SkMatrix form is:

            |       --            --              --          |
            |       --            --              --          |
            | perspective-x  perspective-y  perspective-scale |

        where perspective-x or perspective-y is non-zero, or perspective-scale is
        not one. All other elements may have any value.

        @return  true if SkMatrix is in most general form
    */
    bool hasPerspective() const noexcept {
        return SkToBool(this->getPerspectiveTypeMaskOnly() & kPerspective_Mask);
    }

    /** Returns true if SkMatrix contains only translation, rotation, reflection, and
        uniform scale.
        Returns false if SkMatrix contains different scales, skewing, perspective, or
        degenerate forms that collapse to a line or point.

        Describes that the SkMatrix makes rendering with and without the matrix are
        visually alike; a transformed circle remains a circle. Mathematically, this is
        referred to as similarity of a Euclidean space, or a similarity transformation.

        Preserves right angles, keeping the arms of the angle equal lengths.

        @param tol  to be deprecated
        @return     true if SkMatrix only rotates, uniformly scales, translates
    */
    bool isSimilarity(SkScalar tol = SK_ScalarNearlyZero) const noexcept;

    /** Returns true if SkMatrix contains only translation, rotation, reflection, and
        scale. Scale may differ along rotated axes.
        Returns false if SkMatrix skewing, perspective, or degenerate forms that collapse
        to a line or point.

        Preserves right angles, but not requiring that the arms of the angle
        retain equal lengths.

        @param tol  to be deprecated
        @return     true if SkMatrix only rotates, scales, translates
    */
    bool preservesRightAngles(SkScalar tol = SK_ScalarNearlyZero) const noexcept;

    /** SkMatrix organizes its values in row order. These members correspond to
        each value in SkMatrix.
    */
    static constexpr int kMScaleX = 0;  //!< horizontal scale factor
    static constexpr int kMSkewX = 1;   //!< horizontal skew factor
    static constexpr int kMTransX = 2;  //!< horizontal translation
    static constexpr int kMSkewY = 3;   //!< vertical skew factor
    static constexpr int kMScaleY = 4;  //!< vertical scale factor
    static constexpr int kMTransY = 5;  //!< vertical translation
    static constexpr int kMPersp0 = 6;  //!< input x perspective factor
    static constexpr int kMPersp1 = 7;  //!< input y perspective factor
    static constexpr int kMPersp2 = 8;  //!< perspective bias

    /** Affine arrays are in column major order to match the matrix used by
        PDF and XPS.
    */
    static constexpr int kAScaleX = 0;  //!< horizontal scale factor
    static constexpr int kASkewY = 1;   //!< vertical skew factor
    static constexpr int kASkewX = 2;   //!< horizontal skew factor
    static constexpr int kAScaleY = 3;  //!< vertical scale factor
    static constexpr int kATransX = 4;  //!< horizontal translation
    static constexpr int kATransY = 5;  //!< vertical translation

    /** Returns one matrix value. Asserts if index is out of range and SK_DEBUG is
        defined.

        @param index  one of: kMScaleX, kMSkewX, kMTransX, kMSkewY, kMScaleY, kMTransY,
                      kMPersp0, kMPersp1, kMPersp2
        @return       value corresponding to index
    */
    SkScalar operator[](int index) const noexcept {
        SkASSERT((unsigned)index < 9);
        return fMat[index];
    }

    /** Returns one matrix value. Asserts if index is out of range and SK_DEBUG is
        defined.

        @param index  one of: kMScaleX, kMSkewX, kMTransX, kMSkewY, kMScaleY, kMTransY,
                      kMPersp0, kMPersp1, kMPersp2
        @return       value corresponding to index
    */
    SkScalar get(int index) const noexcept {
        SkASSERT((unsigned)index < 9);
        return fMat[index];
    }

    /** Returns scale factor multiplied by x-axis input, contributing to x-axis output.
        With mapPoints(), scales SkPoint along the x-axis.

        @return  horizontal scale factor
    */
    SkScalar getScaleX() const noexcept { return fMat[kMScaleX]; }

    /** Returns scale factor multiplied by y-axis input, contributing to y-axis output.
        With mapPoints(), scales SkPoint along the y-axis.

        @return  vertical scale factor
    */
    SkScalar getScaleY() const noexcept { return fMat[kMScaleY]; }

    /** Returns scale factor multiplied by x-axis input, contributing to y-axis output.
        With mapPoints(), skews SkPoint along the y-axis.
        Skewing both axes can rotate SkPoint.

        @return  vertical skew factor
    */
    SkScalar getSkewY() const noexcept { return fMat[kMSkewY]; }

    /** Returns scale factor multiplied by y-axis input, contributing to x-axis output.
        With mapPoints(), skews SkPoint along the x-axis.
        Skewing both axes can rotate SkPoint.

        @return  horizontal scale factor
    */
    SkScalar getSkewX() const noexcept { return fMat[kMSkewX]; }

    /** Returns translation contributing to x-axis output.
        With mapPoints(), moves SkPoint along the x-axis.

        @return  horizontal translation factor
    */
    SkScalar getTranslateX() const noexcept { return fMat[kMTransX]; }

    /** Returns translation contributing to y-axis output.
        With mapPoints(), moves SkPoint along the y-axis.

        @return  vertical translation factor
    */
    SkScalar getTranslateY() const noexcept { return fMat[kMTransY]; }

    /** Returns factor scaling input x-axis relative to input y-axis.

        @return  input x-axis perspective factor
    */
    SkScalar getPerspX() const noexcept { return fMat[kMPersp0]; }

    /** Returns factor scaling input y-axis relative to input x-axis.

        @return  input y-axis perspective factor
    */
    SkScalar getPerspY() const noexcept { return fMat[kMPersp1]; }

    /** Returns writable SkMatrix value. Asserts if index is out of range and SK_DEBUG is
        defined. Clears internal cache anticipating that caller will change SkMatrix value.

        Next call to read SkMatrix state may recompute cache; subsequent writes to SkMatrix
        value must be followed by dirtyMatrixTypeCache().

        @param index  one of: kMScaleX, kMSkewX, kMTransX, kMSkewY, kMScaleY, kMTransY,
                      kMPersp0, kMPersp1, kMPersp2
        @return       writable value corresponding to index
    */
    SkScalar& operator[](int index) noexcept {
        SkASSERT((unsigned)index < 9);
        this->setTypeMask(kUnknown_Mask);
        return fMat[index];
    }

    /** Sets SkMatrix value. Asserts if index is out of range and SK_DEBUG is
        defined. Safer than operator[]; internal cache is always maintained.

        @param index  one of: kMScaleX, kMSkewX, kMTransX, kMSkewY, kMScaleY, kMTransY,
                      kMPersp0, kMPersp1, kMPersp2
        @param value  scalar to store in SkMatrix
    */
    void set(int index, SkScalar value) noexcept {
        SkASSERT((unsigned)index < 9);
        fMat[index] = value;
        this->setTypeMask(kUnknown_Mask);
    }

    /** Sets horizontal scale factor.

        @param v  horizontal scale factor to store
    */
    void setScaleX(SkScalar v) noexcept { this->set(kMScaleX, v); }

    /** Sets vertical scale factor.

        @param v  vertical scale factor to store
    */
    void setScaleY(SkScalar v) noexcept { this->set(kMScaleY, v); }

    /** Sets vertical skew factor.

        @param v  vertical skew factor to store
    */
    void setSkewY(SkScalar v) noexcept { this->set(kMSkewY, v); }

    /** Sets horizontal skew factor.

        @param v  horizontal skew factor to store
    */
    void setSkewX(SkScalar v) noexcept { this->set(kMSkewX, v); }

    /** Sets horizontal translation.

        @param v  horizontal translation to store
    */
    void setTranslateX(SkScalar v) noexcept { this->set(kMTransX, v); }

    /** Sets vertical translation.

        @param v  vertical translation to store
    */
    void setTranslateY(SkScalar v) noexcept { this->set(kMTransY, v); }

    /** Sets input x-axis perspective factor, which causes mapXY() to vary input x-axis values
        inversely proportional to input y-axis values.

        @param v  perspective factor
    */
    void setPerspX(SkScalar v) noexcept { this->set(kMPersp0, v); }

    /** Sets input y-axis perspective factor, which causes mapXY() to vary input y-axis values
        inversely proportional to input x-axis values.

        @param v  perspective factor
    */
    void setPerspY(SkScalar v) noexcept { this->set(kMPersp1, v); }

    /** Sets all values from parameters. Sets matrix to:

            | scaleX  skewX transX |
            |  skewY scaleY transY |
            | persp0 persp1 persp2 |

        @param scaleX  horizontal scale factor to store
        @param skewX   horizontal skew factor to store
        @param transX  horizontal translation to store
        @param skewY   vertical skew factor to store
        @param scaleY  vertical scale factor to store
        @param transY  vertical translation to store
        @param persp0  input x-axis values perspective factor to store
        @param persp1  input y-axis values perspective factor to store
        @param persp2  perspective scale factor to store
    */
    void setAll(SkScalar scaleX, SkScalar skewX, SkScalar transX, SkScalar skewY, SkScalar scaleY,
                SkScalar transY, SkScalar persp0, SkScalar persp1, SkScalar persp2) noexcept {
        fMat[kMScaleX] = scaleX;
        fMat[kMSkewX] = skewX;
        fMat[kMTransX] = transX;
        fMat[kMSkewY] = skewY;
        fMat[kMScaleY] = scaleY;
        fMat[kMTransY] = transY;
        fMat[kMPersp0] = persp0;
        fMat[kMPersp1] = persp1;
        fMat[kMPersp2] = persp2;
        this->setTypeMask(kUnknown_Mask);
    }

    /** Copies nine scalar values contained by SkMatrix into buffer, in member value
        ascending order: kMScaleX, kMSkewX, kMTransX, kMSkewY, kMScaleY, kMTransY,
        kMPersp0, kMPersp1, kMPersp2.

        @param buffer  storage for nine scalar values
    */
    void get9(SkScalar buffer[9]) const noexcept { memcpy(buffer, fMat, 9 * sizeof(SkScalar)); }

    /** Sets SkMatrix to nine scalar values in buffer, in member value ascending order:
        kMScaleX, kMSkewX, kMTransX, kMSkewY, kMScaleY, kMTransY, kMPersp0, kMPersp1,
        kMPersp2.

        Sets matrix to:

            | buffer[0] buffer[1] buffer[2] |
            | buffer[3] buffer[4] buffer[5] |
            | buffer[6] buffer[7] buffer[8] |

        In the future, set9 followed by get9 may not return the same values. Since SkMatrix
        maps non-homogeneous coordinates, scaling all nine values produces an equivalent
        transformation, possibly improving precision.

        @param buffer  nine scalar values
    */
    void set9(const SkScalar buffer[9]) noexcept;

    /** Sets SkMatrix to identity; which has no effect on mapped SkPoint. Sets SkMatrix to:

            | 1 0 0 |
            | 0 1 0 |
            | 0 0 1 |

        Also called setIdentity(); use the one that provides better inline
        documentation.
    */
    void reset() noexcept;

    /** Sets SkMatrix to identity; which has no effect on mapped SkPoint. Sets SkMatrix to:

            | 1 0 0 |
            | 0 1 0 |
            | 0 0 1 |

        Also called reset(); use the one that provides better inline
        documentation.
    */
    void setIdentity() noexcept { this->reset(); }

    /** Sets SkMatrix to translate by (dx, dy).

        @param dx  horizontal translation
        @param dy  vertical translation
    */
    void setTranslate(SkScalar dx, SkScalar dy) noexcept;

    /** Sets SkMatrix to translate by (v.fX, v.fY).

        @param v  vector containing horizontal and vertical translation
    */
    void setTranslate(const SkVector& v) noexcept { this->setTranslate(v.fX, v.fY); }

    /** Sets SkMatrix to scale by sx and sy, about a pivot point at (px, py).
        The pivot point is unchanged when mapped with SkMatrix.

        @param sx  horizontal scale factor
        @param sy  vertical scale factor
        @param px  pivot on x-axis
        @param py  pivot on y-axis
    */
    void setScale(SkScalar sx, SkScalar sy, SkScalar px, SkScalar py) noexcept;

    /** Sets SkMatrix to scale by sx and sy about at pivot point at (0, 0).

        @param sx  horizontal scale factor
        @param sy  vertical scale factor
    */
    void setScale(SkScalar sx, SkScalar sy) noexcept;

    /** Sets SkMatrix to rotate by degrees about a pivot point at (px, py).
        The pivot point is unchanged when mapped with SkMatrix.

        Positive degrees rotates clockwise.

        @param degrees  angle of axes relative to upright axes
        @param px       pivot on x-axis
        @param py       pivot on y-axis
    */
    void setRotate(SkScalar degrees, SkScalar px, SkScalar py) noexcept;

    /** Sets SkMatrix to rotate by degrees about a pivot point at (0, 0).
        Positive degrees rotates clockwise.

        @param degrees  angle of axes relative to upright axes
    */
    void setRotate(SkScalar degrees) noexcept;

    /** Sets SkMatrix to rotate by sinValue and cosValue, about a pivot point at (px, py).
        The pivot point is unchanged when mapped with SkMatrix.

        Vector (sinValue, cosValue) describes the angle of rotation relative to (0, 1).
        Vector length specifies scale.

        @param sinValue  rotation vector x-axis component
        @param cosValue  rotation vector y-axis component
        @param px        pivot on x-axis
        @param py        pivot on y-axis
    */
    void setSinCos(SkScalar sinValue, SkScalar cosValue, SkScalar px, SkScalar py) noexcept;

    /** Sets SkMatrix to rotate by sinValue and cosValue, about a pivot point at (0, 0).

        Vector (sinValue, cosValue) describes the angle of rotation relative to (0, 1).
        Vector length specifies scale.

        @param sinValue  rotation vector x-axis component
        @param cosValue  rotation vector y-axis component
    */
    void setSinCos(SkScalar sinValue, SkScalar cosValue) noexcept;

    /** Sets SkMatrix to rotate, scale, and translate using a compressed matrix form.

        Vector (rsxForm.fSSin, rsxForm.fSCos) describes the angle of rotation relative
        to (0, 1). Vector length specifies scale. Mapped point is rotated and scaled
        by vector, then translated by (rsxForm.fTx, rsxForm.fTy).

        @param rsxForm  compressed SkRSXform matrix
        @return         reference to SkMatrix
    */
    SkMatrix& setRSXform(const SkRSXform& rsxForm) noexcept;

    /** Sets SkMatrix to skew by kx and ky, about a pivot point at (px, py).
        The pivot point is unchanged when mapped with SkMatrix.

        @param kx  horizontal skew factor
        @param ky  vertical skew factor
        @param px  pivot on x-axis
        @param py  pivot on y-axis
    */
    void setSkew(SkScalar kx, SkScalar ky, SkScalar px, SkScalar py) noexcept;

    /** Sets SkMatrix to skew by kx and ky, about a pivot point at (0, 0).

        @param kx  horizontal skew factor
        @param ky  vertical skew factor
    */
    void setSkew(SkScalar kx, SkScalar ky) noexcept;

    /** Sets SkMatrix to SkMatrix a multiplied by SkMatrix b. Either a or b may be this.

        Given:

                | A B C |      | J K L |
            a = | D E F |, b = | M N O |
                | G H I |      | P Q R |

        sets SkMatrix to:

                    | A B C |   | J K L |   | AJ+BM+CP AK+BN+CQ AL+BO+CR |
            a * b = | D E F | * | M N O | = | DJ+EM+FP DK+EN+FQ DL+EO+FR |
                    | G H I |   | P Q R |   | GJ+HM+IP GK+HN+IQ GL+HO+IR |

        @param a  SkMatrix on left side of multiply expression
        @param b  SkMatrix on right side of multiply expression
    */
    void setConcat(const SkMatrix& a, const SkMatrix& b) noexcept;

    /** Sets SkMatrix to SkMatrix multiplied by SkMatrix constructed from translation (dx, dy).
        This can be thought of as moving the point to be mapped before applying SkMatrix.

        Given:

                     | A B C |               | 1 0 dx |
            Matrix = | D E F |,  T(dx, dy) = | 0 1 dy |
                     | G H I |               | 0 0  1 |

        sets SkMatrix to:

                                 | A B C | | 1 0 dx |   | A B A*dx+B*dy+C |
            Matrix * T(dx, dy) = | D E F | | 0 1 dy | = | D E D*dx+E*dy+F |
                                 | G H I | | 0 0  1 |   | G H G*dx+H*dy+I |

        @param dx  x-axis translation before applying SkMatrix
        @param dy  y-axis translation before applying SkMatrix
    */
    void preTranslate(SkScalar dx, SkScalar dy) noexcept;

    /** Sets SkMatrix to SkMatrix multiplied by SkMatrix constructed from scaling by (sx, sy)
        about pivot point (px, py).
        This can be thought of as scaling about a pivot point before applying SkMatrix.

        Given:

                     | A B C |                       | sx  0 dx |
            Matrix = | D E F |,  S(sx, sy, px, py) = |  0 sy dy |
                     | G H I |                       |  0  0  1 |

        where

            dx = px - sx * px
            dy = py - sy * py

        sets SkMatrix to:

                                         | A B C | | sx  0 dx |   | A*sx B*sy A*dx+B*dy+C |
            Matrix * S(sx, sy, px, py) = | D E F | |  0 sy dy | = | D*sx E*sy D*dx+E*dy+F |
                                         | G H I | |  0  0  1 |   | G*sx H*sy G*dx+H*dy+I |

        @param sx  horizontal scale factor
        @param sy  vertical scale factor
        @param px  pivot on x-axis
        @param py  pivot on y-axis
    */
    void preScale(SkScalar sx, SkScalar sy, SkScalar px, SkScalar py) noexcept;

    /** Sets SkMatrix to SkMatrix multiplied by SkMatrix constructed from scaling by (sx, sy)
        about pivot point (0, 0).
        This can be thought of as scaling about the origin before applying SkMatrix.

        Given:

                     | A B C |               | sx  0  0 |
            Matrix = | D E F |,  S(sx, sy) = |  0 sy  0 |
                     | G H I |               |  0  0  1 |

        sets SkMatrix to:

                                 | A B C | | sx  0  0 |   | A*sx B*sy C |
            Matrix * S(sx, sy) = | D E F | |  0 sy  0 | = | D*sx E*sy F |
                                 | G H I | |  0  0  1 |   | G*sx H*sy I |

        @param sx  horizontal scale factor
        @param sy  vertical scale factor
    */
    void preScale(SkScalar sx, SkScalar sy) noexcept;

    /** Sets SkMatrix to SkMatrix multiplied by SkMatrix constructed from rotating by degrees
        about pivot point (px, py).
        This can be thought of as rotating about a pivot point before applying SkMatrix.

        Positive degrees rotates clockwise.

        Given:

                     | A B C |                        | c -s dx |
            Matrix = | D E F |,  R(degrees, px, py) = | s  c dy |
                     | G H I |                        | 0  0  1 |

        where

            c  = cos(degrees)
            s  = sin(degrees)
            dx =  s * py + (1 - c) * px
            dy = -s * px + (1 - c) * py

        sets SkMatrix to:

                                          | A B C | | c -s dx |   | Ac+Bs -As+Bc A*dx+B*dy+C |
            Matrix * R(degrees, px, py) = | D E F | | s  c dy | = | Dc+Es -Ds+Ec D*dx+E*dy+F |
                                          | G H I | | 0  0  1 |   | Gc+Hs -Gs+Hc G*dx+H*dy+I |

        @param degrees  angle of axes relative to upright axes
        @param px       pivot on x-axis
        @param py       pivot on y-axis
    */
    void preRotate(SkScalar degrees, SkScalar px, SkScalar py) noexcept;

    /** Sets SkMatrix to SkMatrix multiplied by SkMatrix constructed from rotating by degrees
        about pivot point (0, 0).
        This can be thought of as rotating about the origin before applying SkMatrix.

        Positive degrees rotates clockwise.

        Given:

                     | A B C |                        | c -s 0 |
            Matrix = | D E F |,  R(degrees, px, py) = | s  c 0 |
                     | G H I |                        | 0  0 1 |

        where

            c  = cos(degrees)
            s  = sin(degrees)

        sets SkMatrix to:

                                          | A B C | | c -s 0 |   | Ac+Bs -As+Bc C |
            Matrix * R(degrees, px, py) = | D E F | | s  c 0 | = | Dc+Es -Ds+Ec F |
                                          | G H I | | 0  0 1 |   | Gc+Hs -Gs+Hc I |

        @param degrees  angle of axes relative to upright axes
    */
    void preRotate(SkScalar degrees) noexcept;

    /** Sets SkMatrix to SkMatrix multiplied by SkMatrix constructed from skewing by (kx, ky)
        about pivot point (px, py).
        This can be thought of as skewing about a pivot point before applying SkMatrix.

        Given:

                     | A B C |                       |  1 kx dx |
            Matrix = | D E F |,  K(kx, ky, px, py) = | ky  1 dy |
                     | G H I |                       |  0  0  1 |

        where

            dx = -kx * py
            dy = -ky * px

        sets SkMatrix to:

                                         | A B C | |  1 kx dx |   | A+B*ky A*kx+B A*dx+B*dy+C |
            Matrix * K(kx, ky, px, py) = | D E F | | ky  1 dy | = | D+E*ky D*kx+E D*dx+E*dy+F |
                                         | G H I | |  0  0  1 |   | G+H*ky G*kx+H G*dx+H*dy+I |

        @param kx  horizontal skew factor
        @param ky  vertical skew factor
        @param px  pivot on x-axis
        @param py  pivot on y-axis
    */
    void preSkew(SkScalar kx, SkScalar ky, SkScalar px, SkScalar py) noexcept;

    /** Sets SkMatrix to SkMatrix multiplied by SkMatrix constructed from skewing by (kx, ky)
        about pivot point (0, 0).
        This can be thought of as skewing about the origin before applying SkMatrix.

        Given:

                     | A B C |               |  1 kx 0 |
            Matrix = | D E F |,  K(kx, ky) = | ky  1 0 |
                     | G H I |               |  0  0 1 |

        sets SkMatrix to:

                                 | A B C | |  1 kx 0 |   | A+B*ky A*kx+B C |
            Matrix * K(kx, ky) = | D E F | | ky  1 0 | = | D+E*ky D*kx+E F |
                                 | G H I | |  0  0 1 |   | G+H*ky G*kx+H I |

        @param kx  horizontal skew factor
        @param ky  vertical skew factor
    */
    void preSkew(SkScalar kx, SkScalar ky) noexcept;

    /** Sets SkMatrix to SkMatrix multiplied by SkMatrix other.
        This can be thought of mapping by other before applying SkMatrix.

        Given:

                     | A B C |          | J K L |
            Matrix = | D E F |, other = | M N O |
                     | G H I |          | P Q R |

        sets SkMatrix to:

                             | A B C |   | J K L |   | AJ+BM+CP AK+BN+CQ AL+BO+CR |
            Matrix * other = | D E F | * | M N O | = | DJ+EM+FP DK+EN+FQ DL+EO+FR |
                             | G H I |   | P Q R |   | GJ+HM+IP GK+HN+IQ GL+HO+IR |

        @param other  SkMatrix on right side of multiply expression
    */
    void preConcat(const SkMatrix& other) noexcept;

    /** Sets SkMatrix to SkMatrix constructed from translation (dx, dy) multiplied by SkMatrix.
        This can be thought of as moving the point to be mapped after applying SkMatrix.

        Given:

                     | J K L |               | 1 0 dx |
            Matrix = | M N O |,  T(dx, dy) = | 0 1 dy |
                     | P Q R |               | 0 0  1 |

        sets SkMatrix to:

                                 | 1 0 dx | | J K L |   | J+dx*P K+dx*Q L+dx*R |
            T(dx, dy) * Matrix = | 0 1 dy | | M N O | = | M+dy*P N+dy*Q O+dy*R |
                                 | 0 0  1 | | P Q R |   |      P      Q      R |

        @param dx  x-axis translation after applying SkMatrix
        @param dy  y-axis translation after applying SkMatrix
    */
    void postTranslate(SkScalar dx, SkScalar dy) noexcept;

    /** Sets SkMatrix to SkMatrix constructed from scaling by (sx, sy) about pivot point
        (px, py), multiplied by SkMatrix.
        This can be thought of as scaling about a pivot point after applying SkMatrix.

        Given:

                     | J K L |                       | sx  0 dx |
            Matrix = | M N O |,  S(sx, sy, px, py) = |  0 sy dy |
                     | P Q R |                       |  0  0  1 |

        where

            dx = px - sx * px
            dy = py - sy * py

        sets SkMatrix to:

                                         | sx  0 dx | | J K L |   | sx*J+dx*P sx*K+dx*Q sx*L+dx+R |
            S(sx, sy, px, py) * Matrix = |  0 sy dy | | M N O | = | sy*M+dy*P sy*N+dy*Q sy*O+dy*R |
                                         |  0  0  1 | | P Q R |   |         P         Q         R |

        @param sx  horizontal scale factor
        @param sy  vertical scale factor
        @param px  pivot on x-axis
        @param py  pivot on y-axis
    */
    void postScale(SkScalar sx, SkScalar sy, SkScalar px, SkScalar py) noexcept;

    /** Sets SkMatrix to SkMatrix constructed from scaling by (sx, sy) about pivot point
        (0, 0), multiplied by SkMatrix.
        This can be thought of as scaling about the origin after applying SkMatrix.

        Given:

                     | J K L |               | sx  0  0 |
            Matrix = | M N O |,  S(sx, sy) = |  0 sy  0 |
                     | P Q R |               |  0  0  1 |

        sets SkMatrix to:

                                 | sx  0  0 | | J K L |   | sx*J sx*K sx*L |
            S(sx, sy) * Matrix = |  0 sy  0 | | M N O | = | sy*M sy*N sy*O |
                                 |  0  0  1 | | P Q R |   |    P    Q    R |

        @param sx  horizontal scale factor
        @param sy  vertical scale factor
    */
    void postScale(SkScalar sx, SkScalar sy) noexcept;

    /** Sets SkMatrix to SkMatrix constructed from scaling by (1/divx, 1/divy),
        about pivot point (px, py), multiplied by SkMatrix.

        Returns false if either divx or divy is zero.

        Given:

                     | J K L |                   | sx  0  0 |
            Matrix = | M N O |,  I(divx, divy) = |  0 sy  0 |
                     | P Q R |                   |  0  0  1 |

        where

            sx = 1 / divx
            sy = 1 / divy

        sets SkMatrix to:

                                     | sx  0  0 | | J K L |   | sx*J sx*K sx*L |
            I(divx, divy) * Matrix = |  0 sy  0 | | M N O | = | sy*M sy*N sy*O |
                                     |  0  0  1 | | P Q R |   |    P    Q    R |

        @param divx  integer divisor for inverse scale in x
        @param divy  integer divisor for inverse scale in y
        @return      true on successful scale
    */
    bool postIDiv(int divx, int divy) noexcept;

    /** Sets SkMatrix to SkMatrix constructed from rotating by degrees about pivot point
        (px, py), multiplied by SkMatrix.
        This can be thought of as rotating about a pivot point after applying SkMatrix.

        Positive degrees rotates clockwise.

        Given:

                     | J K L |                        | c -s dx |
            Matrix = | M N O |,  R(degrees, px, py) = | s  c dy |
                     | P Q R |                        | 0  0  1 |

        where

            c  = cos(degrees)
            s  = sin(degrees)
            dx =  s * py + (1 - c) * px
            dy = -s * px + (1 - c) * py

        sets SkMatrix to:

                                          |c -s dx| |J K L|   |cJ-sM+dx*P cK-sN+dx*Q cL-sO+dx+R|
            R(degrees, px, py) * Matrix = |s  c dy| |M N O| = |sJ+cM+dy*P sK+cN+dy*Q sL+cO+dy*R|
                                          |0  0  1| |P Q R|   |         P          Q          R|

        @param degrees  angle of axes relative to upright axes
        @param px       pivot on x-axis
        @param py       pivot on y-axis
    */
    void postRotate(SkScalar degrees, SkScalar px, SkScalar py) noexcept;

    /** Sets SkMatrix to SkMatrix constructed from rotating by degrees about pivot point
        (0, 0), multiplied by SkMatrix.
        This can be thought of as rotating about the origin after applying SkMatrix.

        Positive degrees rotates clockwise.

        Given:

                     | J K L |                        | c -s 0 |
            Matrix = | M N O |,  R(degrees, px, py) = | s  c 0 |
                     | P Q R |                        | 0  0 1 |

        where

            c  = cos(degrees)
            s  = sin(degrees)

        sets SkMatrix to:

                                          | c -s dx | | J K L |   | cJ-sM cK-sN cL-sO |
            R(degrees, px, py) * Matrix = | s  c dy | | M N O | = | sJ+cM sK+cN sL+cO |
                                          | 0  0  1 | | P Q R |   |     P     Q     R |

        @param degrees  angle of axes relative to upright axes
    */
    void postRotate(SkScalar degrees) noexcept;

    /** Sets SkMatrix to SkMatrix constructed from skewing by (kx, ky) about pivot point
        (px, py), multiplied by SkMatrix.
        This can be thought of as skewing about a pivot point after applying SkMatrix.

        Given:

                     | J K L |                       |  1 kx dx |
            Matrix = | M N O |,  K(kx, ky, px, py) = | ky  1 dy |
                     | P Q R |                       |  0  0  1 |

        where

            dx = -kx * py
            dy = -ky * px

        sets SkMatrix to:

                                         | 1 kx dx| |J K L|   |J+kx*M+dx*P K+kx*N+dx*Q L+kx*O+dx+R|
            K(kx, ky, px, py) * Matrix = |ky  1 dy| |M N O| = |ky*J+M+dy*P ky*K+N+dy*Q ky*L+O+dy*R|
                                         | 0  0  1| |P Q R|   |          P           Q           R|

        @param kx  horizontal skew factor
        @param ky  vertical skew factor
        @param px  pivot on x-axis
        @param py  pivot on y-axis
    */
    void postSkew(SkScalar kx, SkScalar ky, SkScalar px, SkScalar py) noexcept;

    /** Sets SkMatrix to SkMatrix constructed from skewing by (kx, ky) about pivot point
        (0, 0), multiplied by SkMatrix.
        This can be thought of as skewing about the origin after applying SkMatrix.

        Given:

                     | J K L |               |  1 kx 0 |
            Matrix = | M N O |,  K(kx, ky) = | ky  1 0 |
                     | P Q R |               |  0  0 1 |

        sets SkMatrix to:

                                 |  1 kx 0 | | J K L |   | J+kx*M K+kx*N L+kx*O |
            K(kx, ky) * Matrix = | ky  1 0 | | M N O | = | ky*J+M ky*K+N ky*L+O |
                                 |  0  0 1 | | P Q R |   |      P      Q      R |

        @param kx  horizontal skew factor
        @param ky  vertical skew factor
    */
    void postSkew(SkScalar kx, SkScalar ky) noexcept;

    /** Sets SkMatrix to SkMatrix other multiplied by SkMatrix.
        This can be thought of mapping by other after applying SkMatrix.

        Given:

                     | J K L |           | A B C |
            Matrix = | M N O |,  other = | D E F |
                     | P Q R |           | G H I |

        sets SkMatrix to:

                             | A B C |   | J K L |   | AJ+BM+CP AK+BN+CQ AL+BO+CR |
            other * Matrix = | D E F | * | M N O | = | DJ+EM+FP DK+EN+FQ DL+EO+FR |
                             | G H I |   | P Q R |   | GJ+HM+IP GK+HN+IQ GL+HO+IR |

        @param other  SkMatrix on left side of multiply expression
    */
    void postConcat(const SkMatrix& other) noexcept;

    /** \enum SkMatrix::ScaleToFit
        ScaleToFit describes how SkMatrix is constructed to map one SkRect to another.
        ScaleToFit may allow SkMatrix to have unequal horizontal and vertical scaling,
        or may restrict SkMatrix to square scaling. If restricted, ScaleToFit specifies
        how SkMatrix maps to the side or center of the destination SkRect.
    */
    enum ScaleToFit {
        kFill_ScaleToFit,    //!< scales in x and y to fill destination SkRect
        kStart_ScaleToFit,   //!< scales and aligns to left and top
        kCenter_ScaleToFit,  //!< scales and aligns to center
        kEnd_ScaleToFit,     //!< scales and aligns to right and bottom
    };

    /** Sets SkMatrix to scale and translate src SkRect to dst SkRect. stf selects whether
        mapping completely fills dst or preserves the aspect ratio, and how to align
        src within dst. Returns false if src is empty, and sets SkMatrix to identity.
        Returns true if dst is empty, and sets SkMatrix to:

            | 0 0 0 |
            | 0 0 0 |
            | 0 0 1 |

        @param src  SkRect to map from
        @param dst  SkRect to map to
        @param stf  one of: kFill_ScaleToFit, kStart_ScaleToFit,
                    kCenter_ScaleToFit, kEnd_ScaleToFit
        @return     true if SkMatrix can represent SkRect mapping
    */
    bool setRectToRect(const SkRect& src, const SkRect& dst, ScaleToFit stf) noexcept;

    /** Returns SkMatrix set to scale and translate src SkRect to dst SkRect. stf selects
        whether mapping completely fills dst or preserves the aspect ratio, and how to
        align src within dst. Returns the identity SkMatrix if src is empty. If dst is
        empty, returns SkMatrix set to:

            | 0 0 0 |
            | 0 0 0 |
            | 0 0 1 |

        @param src  SkRect to map from
        @param dst  SkRect to map to
        @param stf  one of: kFill_ScaleToFit, kStart_ScaleToFit,
                    kCenter_ScaleToFit, kEnd_ScaleToFit
        @return     SkMatrix mapping src to dst
    */
    static SkMatrix MakeRectToRect(const SkRect& src, const SkRect& dst, ScaleToFit stf) noexcept {
        SkMatrix m;
        m.setRectToRect(src, dst, stf);
        return m;
    }

    /** Sets SkMatrix to map src to dst. count must be zero or greater, and four or less.

        If count is zero, sets SkMatrix to identity and returns true.
        If count is one, sets SkMatrix to translate and returns true.
        If count is two or more, sets SkMatrix to map SkPoint if possible; returns false
        if SkMatrix cannot be constructed. If count is four, SkMatrix may include
        perspective.

        @param src    SkPoint to map from
        @param dst    SkPoint to map to
        @param count  number of SkPoint in src and dst
        @return       true if SkMatrix was constructed successfully
    */
    bool setPolyToPoly(const SkPoint src[], const SkPoint dst[], int count) noexcept;

    /** Sets inverse to reciprocal matrix, returning true if SkMatrix can be inverted.
        Geometrically, if SkMatrix maps from source to destination, inverse SkMatrix
        maps from destination to source. If SkMatrix can not be inverted, inverse is
        unchanged.

        @param inverse  storage for inverted SkMatrix; may be nullptr
        @return         true if SkMatrix can be inverted
    */
    bool SK_WARN_UNUSED_RESULT invert(SkMatrix* inverse) const noexcept {
        // Allow the trivial case to be inlined.
        if (this->isIdentity()) {
            if (inverse) {
                inverse->reset();
            }
            return true;
        }
        return this->invertNonIdentity(inverse);
    }

    /** Fills affine with identity values in column major order.
        Sets affine to:

            | 1 0 0 |
            | 0 1 0 |

        Affine 3 by 2 matrices in column major order are used by OpenGL and XPS.

        @param affine  storage for 3 by 2 affine matrix
    */
    static void SetAffineIdentity(SkScalar affine[6]) noexcept;

    /** Fills affine in column major order. Sets affine to:

            | scale-x  skew-x translate-x |
            | skew-y  scale-y translate-y |

        If SkMatrix contains perspective, returns false and leaves affine unchanged.

        @param affine  storage for 3 by 2 affine matrix; may be nullptr
        @return        true if SkMatrix does not contain perspective
    */
    bool SK_WARN_UNUSED_RESULT asAffine(SkScalar affine[6]) const noexcept;

    /** Sets SkMatrix to affine values, passed in column major order. Given affine,
        column, then row, as:

            | scale-x  skew-x translate-x |
            |  skew-y scale-y translate-y |

        SkMatrix is set, row, then column, to:

            | scale-x  skew-x translate-x |
            |  skew-y scale-y translate-y |
            |       0       0           1 |

        @param affine  3 by 2 affine matrix
    */
    void setAffine(const SkScalar affine[6]) noexcept;

    /** Maps src SkPoint array of length count to dst SkPoint array of equal or greater
        length. SkPoint are mapped by multiplying each SkPoint by SkMatrix. Given:

                     | A B C |        | x |
            Matrix = | D E F |,  pt = | y |
                     | G H I |        | 1 |

        where

            for (i = 0; i < count; ++i) {
                x = src[i].fX
                y = src[i].fY
            }

        each dst SkPoint is computed as:

                          |A B C| |x|                               Ax+By+C   Dx+Ey+F
            Matrix * pt = |D E F| |y| = |Ax+By+C Dx+Ey+F Gx+Hy+I| = ------- , -------
                          |G H I| |1|                               Gx+Hy+I   Gx+Hy+I

        src and dst may point to the same storage.

        @param dst    storage for mapped SkPoint
        @param src    SkPoint to transform
        @param count  number of SkPoint to transform
    */
    void mapPoints(SkPoint dst[], const SkPoint src[], int count) const noexcept;

    /** Maps pts SkPoint array of length count in place. SkPoint are mapped by multiplying
        each SkPoint by SkMatrix. Given:

                     | A B C |        | x |
            Matrix = | D E F |,  pt = | y |
                     | G H I |        | 1 |

        where

            for (i = 0; i < count; ++i) {
                x = pts[i].fX
                y = pts[i].fY
            }

        each resulting pts SkPoint is computed as:

                          |A B C| |x|                               Ax+By+C   Dx+Ey+F
            Matrix * pt = |D E F| |y| = |Ax+By+C Dx+Ey+F Gx+Hy+I| = ------- , -------
                          |G H I| |1|                               Gx+Hy+I   Gx+Hy+I

        @param pts    storage for mapped SkPoint
        @param count  number of SkPoint to transform
    */
    void mapPoints(SkPoint pts[], int count) const noexcept { this->mapPoints(pts, pts, count); }

    /** Maps src SkPoint3 array of length count to dst SkPoint3 array, which must of length count or
        greater. SkPoint3 array is mapped by multiplying each SkPoint3 by SkMatrix. Given:

                     | A B C |         | x |
            Matrix = | D E F |,  src = | y |
                     | G H I |         | z |

        each resulting dst SkPoint is computed as:

                           |A B C| |x|
            Matrix * src = |D E F| |y| = |Ax+By+Cz Dx+Ey+Fz Gx+Hy+Iz|
                           |G H I| |z|

        @param dst    storage for mapped SkPoint3 array
        @param src    SkPoint3 array to transform
        @param count  items in SkPoint3 array to transform
    */
    void mapHomogeneousPoints(SkPoint3 dst[], const SkPoint3 src[], int count) const noexcept;

    /** Maps SkPoint (x, y) to result. SkPoint is mapped by multiplying by SkMatrix. Given:

                     | A B C |        | x |
            Matrix = | D E F |,  pt = | y |
                     | G H I |        | 1 |

        result is computed as:

                          |A B C| |x|                               Ax+By+C   Dx+Ey+F
            Matrix * pt = |D E F| |y| = |Ax+By+C Dx+Ey+F Gx+Hy+I| = ------- , -------
                          |G H I| |1|                               Gx+Hy+I   Gx+Hy+I

        @param x       x-axis value of SkPoint to map
        @param y       y-axis value of SkPoint to map
        @param result  storage for mapped SkPoint
    */
    void mapXY(SkScalar x, SkScalar y, SkPoint* result) const noexcept;

    /** Returns SkPoint (x, y) multiplied by SkMatrix. Given:

                     | A B C |        | x |
            Matrix = | D E F |,  pt = | y |
                     | G H I |        | 1 |

        result is computed as:

                          |A B C| |x|                               Ax+By+C   Dx+Ey+F
            Matrix * pt = |D E F| |y| = |Ax+By+C Dx+Ey+F Gx+Hy+I| = ------- , -------
                          |G H I| |1|                               Gx+Hy+I   Gx+Hy+I

        @param x  x-axis value of SkPoint to map
        @param y  y-axis value of SkPoint to map
        @return   mapped SkPoint
    */
    SkPoint mapXY(SkScalar x, SkScalar y) const noexcept {
        SkPoint result;
        this->mapXY(x, y, &result);
        return result;
    }

    /** Maps src vector array of length count to vector SkPoint array of equal or greater
        length. Vectors are mapped by multiplying each vector by SkMatrix, treating
        SkMatrix translation as zero. Given:

                     | A B 0 |         | x |
            Matrix = | D E 0 |,  src = | y |
                     | G H I |         | 1 |

        where

            for (i = 0; i < count; ++i) {
                x = src[i].fX
                y = src[i].fY
            }

        each dst vector is computed as:

                           |A B 0| |x|                            Ax+By     Dx+Ey
            Matrix * src = |D E 0| |y| = |Ax+By Dx+Ey Gx+Hy+I| = ------- , -------
                           |G H I| |1|                           Gx+Hy+I   Gx+Hy+I

        src and dst may point to the same storage.

        @param dst    storage for mapped vectors
        @param src    vectors to transform
        @param count  number of vectors to transform
    */
    void mapVectors(SkVector dst[], const SkVector src[], int count) const noexcept;

    /** Maps vecs vector array of length count in place, multiplying each vector by
        SkMatrix, treating SkMatrix translation as zero. Given:

                     | A B 0 |         | x |
            Matrix = | D E 0 |,  vec = | y |
                     | G H I |         | 1 |

        where

            for (i = 0; i < count; ++i) {
                x = vecs[i].fX
                y = vecs[i].fY
            }

        each result vector is computed as:

                           |A B 0| |x|                            Ax+By     Dx+Ey
            Matrix * vec = |D E 0| |y| = |Ax+By Dx+Ey Gx+Hy+I| = ------- , -------
                           |G H I| |1|                           Gx+Hy+I   Gx+Hy+I

        @param vecs   vectors to transform, and storage for mapped vectors
        @param count  number of vectors to transform
    */
    void mapVectors(SkVector vecs[], int count) const noexcept {
        this->mapVectors(vecs, vecs, count);
    }

    /** Maps vector (dx, dy) to result. Vector is mapped by multiplying by SkMatrix,
        treating SkMatrix translation as zero. Given:

                     | A B 0 |         | dx |
            Matrix = | D E 0 |,  vec = | dy |
                     | G H I |         |  1 |

        each result vector is computed as:

                       |A B 0| |dx|                                        A*dx+B*dy     D*dx+E*dy
        Matrix * vec = |D E 0| |dy| = |A*dx+B*dy D*dx+E*dy G*dx+H*dy+I| = ----------- , -----------
                       |G H I| | 1|                                       G*dx+H*dy+I   G*dx+*dHy+I

        @param dx      x-axis value of vector to map
        @param dy      y-axis value of vector to map
        @param result  storage for mapped vector
    */
    void mapVector(SkScalar dx, SkScalar dy, SkVector* result) const noexcept {
        SkVector vec = {dx, dy};
        this->mapVectors(result, &vec, 1);
    }

    /** Returns vector (dx, dy) multiplied by SkMatrix, treating SkMatrix translation as zero.
        Given:

                     | A B 0 |         | dx |
            Matrix = | D E 0 |,  vec = | dy |
                     | G H I |         |  1 |

        each result vector is computed as:

                       |A B 0| |dx|                                        A*dx+B*dy     D*dx+E*dy
        Matrix * vec = |D E 0| |dy| = |A*dx+B*dy D*dx+E*dy G*dx+H*dy+I| = ----------- , -----------
                       |G H I| | 1|                                       G*dx+H*dy+I   G*dx+*dHy+I

        @param dx  x-axis value of vector to map
        @param dy  y-axis value of vector to map
        @return    mapped vector
    */
    SkVector mapVector(SkScalar dx, SkScalar dy) const noexcept {
        SkVector vec = {dx, dy};
        this->mapVectors(&vec, &vec, 1);
        return vec;
    }

    /** Sets dst to bounds of src corners mapped by SkMatrix.
        Returns true if mapped corners are dst corners.

        Returned value is the same as calling rectStaysRect().

        @param dst  storage for bounds of mapped SkPoint
        @param src  SkRect to map
        @return     true if dst is equivalent to mapped src
    */
    bool mapRect(SkRect* dst, const SkRect& src) const noexcept;

    /** Sets rect to bounds of rect corners mapped by SkMatrix.
        Returns true if mapped corners are computed rect corners.

        Returned value is the same as calling rectStaysRect().

        @param rect  rectangle to map, and storage for bounds of mapped corners
        @return      true if result is equivalent to mapped rect
    */
    bool mapRect(SkRect* rect) const noexcept { return this->mapRect(rect, *rect); }

    /** Returns bounds of src corners mapped by SkMatrix.

        @param src  rectangle to map
        @return     mapped bounds
    */
    SkRect mapRect(const SkRect& src) const noexcept {
        SkRect dst;
        (void)this->mapRect(&dst, src);
        return dst;
    }

    /** Maps four corners of rect to dst. SkPoint are mapped by multiplying each
        rect corner by SkMatrix. rect corner is processed in this order:
        (rect.fLeft, rect.fTop), (rect.fRight, rect.fTop), (rect.fRight, rect.fBottom),
        (rect.fLeft, rect.fBottom).

        rect may be empty: rect.fLeft may be greater than or equal to rect.fRight;
        rect.fTop may be greater than or equal to rect.fBottom.

        Given:

                     | A B C |        | x |
            Matrix = | D E F |,  pt = | y |
                     | G H I |        | 1 |

        where pt is initialized from each of (rect.fLeft, rect.fTop),
        (rect.fRight, rect.fTop), (rect.fRight, rect.fBottom), (rect.fLeft, rect.fBottom),
        each dst SkPoint is computed as:

                          |A B C| |x|                               Ax+By+C   Dx+Ey+F
            Matrix * pt = |D E F| |y| = |Ax+By+C Dx+Ey+F Gx+Hy+I| = ------- , -------
                          |G H I| |1|                               Gx+Hy+I   Gx+Hy+I

        @param dst   storage for mapped corner SkPoint
        @param rect  SkRect to map
    */
    void mapRectToQuad(SkPoint dst[4], const SkRect& rect) const noexcept {
        // This could potentially be faster if we only transformed each x and y of the rect once.
        rect.toQuad(dst);
        this->mapPoints(dst, 4);
    }

    /** Sets dst to bounds of src corners mapped by SkMatrix. If matrix contains
        elements other than scale or translate: asserts if SK_DEBUG is defined;
        otherwise, results are undefined.

        @param dst  storage for bounds of mapped SkPoint
        @param src  SkRect to map
    */
    void mapRectScaleTranslate(SkRect* dst, const SkRect& src) const noexcept;

    /** Returns geometric mean radius of ellipse formed by constructing circle of
        size radius, and mapping constructed circle with SkMatrix. The result squared is
        equal to the major axis length times the minor axis length.
        Result is not meaningful if SkMatrix contains perspective elements.

        @param radius  circle size to map
        @return        average mapped radius
    */
    SkScalar mapRadius(SkScalar radius) const;

    /** Returns true if a unit step on x-axis at some y-axis value mapped through SkMatrix
        can be represented by a constant vector. Returns true if getType() returns
        kIdentity_Mask, or combinations of: kTranslate_Mask, kScale_Mask, and kAffine_Mask.

        May return true if getType() returns kPerspective_Mask, but only when SkMatrix
        does not include rotation or skewing along the y-axis.

        @return  true if SkMatrix does not have complex perspective
    */
    bool isFixedStepInX() const noexcept;

    /** Returns vector representing a unit step on x-axis at y mapped through SkMatrix.
        If isFixedStepInX() is false, returned value is undefined.

        @param y  position of line parallel to x-axis
        @return   vector advance of mapped unit step on x-axis
    */
    SkVector fixedStepInX(SkScalar y) const noexcept;

    /** Returns true if SkMatrix equals m, using an efficient comparison.

        Returns false when the sign of zero values is the different; when one
        matrix has positive zero value and the other has negative zero value.

        Returns true even when both SkMatrix contain NaN.

        NaN never equals any value, including itself. To improve performance, NaN values
        are treated as bit patterns that are equal if their bit patterns are equal.

        @param m  SkMatrix to compare
        @return   true if m and SkMatrix are represented by identical bit patterns
    */
    bool cheapEqualTo(const SkMatrix& m) const noexcept {
        return 0 == memcmp(fMat, m.fMat, sizeof(fMat));
    }

    /** Compares a and b; returns true if a and b are numerically equal. Returns true
        even if sign of zero values are different. Returns false if either SkMatrix
        contains NaN, even if the other SkMatrix also contains NaN.

        @param a  SkMatrix to compare
        @param b  SkMatrix to compare
        @return   true if SkMatrix a and SkMatrix b are numerically equal
    */
    friend SK_API bool operator==(const SkMatrix& a, const SkMatrix& b) noexcept;

    /** Compares a and b; returns true if a and b are not numerically equal. Returns false
        even if sign of zero values are different. Returns true if either SkMatrix
        contains NaN, even if the other SkMatrix also contains NaN.

        @param a  SkMatrix to compare
        @param b  SkMatrix to compare
        @return   true if SkMatrix a and SkMatrix b are numerically not equal
    */
    friend SK_API bool operator!=(const SkMatrix& a, const SkMatrix& b) noexcept {
        return !(a == b);
    }

    /** Writes text representation of SkMatrix to standard output. Floating point values
        are written with limited precision; it may not be possible to reconstruct
        original SkMatrix from output.
    */
    void dump() const;

    /** Returns the minimum scaling factor of SkMatrix by decomposing the scaling and
        skewing elements.
        Returns -1 if scale factor overflows or SkMatrix contains perspective.

        @return  minimum scale factor
    */
    SkScalar getMinScale() const;

    /** Returns the maximum scaling factor of SkMatrix by decomposing the scaling and
        skewing elements.
        Returns -1 if scale factor overflows or SkMatrix contains perspective.

        @return  maximum scale factor
    */
    SkScalar getMaxScale() const;

    /** Sets scaleFactors[0] to the minimum scaling factor, and scaleFactors[1] to the
        maximum scaling factor. Scaling factors are computed by decomposing
        the SkMatrix scaling and skewing elements.

        Returns true if scaleFactors are found; otherwise, returns false and sets
        scaleFactors to undefined values.

        @param scaleFactors  storage for minimum and maximum scale factors
        @return              true if scale factors were computed correctly
    */
    bool SK_WARN_UNUSED_RESULT getMinMaxScales(SkScalar scaleFactors[2]) const;

    /** Decomposes SkMatrix into scale components and whatever remains. Returns false if
        SkMatrix could not be decomposed.

        Sets scale to portion of SkMatrix that scale axes. Sets remaining to SkMatrix
        with scaling factored out. remaining may be passed as nullptr
        to determine if SkMatrix can be decomposed without computing remainder.

        Returns true if scale components are found. scale and remaining are
        unchanged if SkMatrix contains perspective; scale factors are not finite, or
        are nearly zero.

        On success: Matrix = scale * Remaining.

        @param scale      axes scaling factors; may be nullptr
        @param remaining  SkMatrix without scaling; may be nullptr
        @return           true if scale can be computed
    */
    bool decomposeScale(SkSize* scale, SkMatrix* remaining = nullptr) const;

    /** Returns reference to const identity SkMatrix. Returned SkMatrix is set to:

            | 1 0 0 |
            | 0 1 0 |
            | 0 0 1 |

        @return  const identity SkMatrix
    */
    static const SkMatrix& I() noexcept;

    /** Returns reference to a const SkMatrix with invalid values. Returned SkMatrix is set
        to:

            | SK_ScalarMax SK_ScalarMax SK_ScalarMax |
            | SK_ScalarMax SK_ScalarMax SK_ScalarMax |
            | SK_ScalarMax SK_ScalarMax SK_ScalarMax |

        @return  const invalid SkMatrix
    */
    static const SkMatrix& InvalidMatrix() noexcept;

    /** Returns SkMatrix a multiplied by SkMatrix b.

        Given:

                | A B C |      | J K L |
            a = | D E F |, b = | M N O |
                | G H I |      | P Q R |

        sets SkMatrix to:

                    | A B C |   | J K L |   | AJ+BM+CP AK+BN+CQ AL+BO+CR |
            a * b = | D E F | * | M N O | = | DJ+EM+FP DK+EN+FQ DL+EO+FR |
                    | G H I |   | P Q R |   | GJ+HM+IP GK+HN+IQ GL+HO+IR |

        @param a  SkMatrix on left side of multiply expression
        @param b  SkMatrix on right side of multiply expression
        @return   SkMatrix computed from a times b
    */
    static SkMatrix Concat(const SkMatrix& a, const SkMatrix& b) noexcept {
        SkMatrix result;
        result.setConcat(a, b);
        return result;
    }

    /** Sets internal cache to unknown state. Use to force update after repeated
        modifications to SkMatrix element reference returned by operator[](int index).
    */
    void dirtyMatrixTypeCache() noexcept { this->setTypeMask(kUnknown_Mask); }

    /** Initializes SkMatrix with scale and translate elements.

            | sx  0 tx |
            |  0 sy ty |
            |  0  0  1 |

        @param sx  horizontal scale factor to store
        @param sy  vertical scale factor to store
        @param tx  horizontal translation to store
        @param ty  vertical translation to store
    */
    void setScaleTranslate(SkScalar sx, SkScalar sy, SkScalar tx, SkScalar ty) noexcept {
        fMat[kMScaleX] = sx;
        fMat[kMSkewX] = 0;
        fMat[kMTransX] = tx;

        fMat[kMSkewY] = 0;
        fMat[kMScaleY] = sy;
        fMat[kMTransY] = ty;

        fMat[kMPersp0] = 0;
        fMat[kMPersp1] = 0;
        fMat[kMPersp2] = 1;

        unsigned mask = 0;
        if (sx != 1 || sy != 1) {
            mask |= kScale_Mask;
        }
        if (tx || ty) {
            mask |= kTranslate_Mask;
        }
        this->setTypeMask(mask | kRectStaysRect_Mask);
    }

    /** Returns true if all elements of the matrix are finite. Returns false if any
        element is infinity, or NaN.

        @return  true if matrix has only finite elements
    */
    bool isFinite() const noexcept { return SkScalarsAreFinite(fMat, 9); }

private:
    /** Set if the matrix will map a rectangle to another rectangle. This
        can be true if the matrix is scale-only, or rotates a multiple of
        90 degrees.

        This bit will be set on identity matrices
    */
    static constexpr int kRectStaysRect_Mask = 0x10;

    /** Set if the perspective bit is valid even though the rest of
        the matrix is Unknown.
    */
    static constexpr int kOnlyPerspectiveValid_Mask = 0x40;

    static constexpr int kUnknown_Mask = 0x80;

    static constexpr int kORableMasks =
            kTranslate_Mask | kScale_Mask | kAffine_Mask | kPerspective_Mask;

    static constexpr int kAllMasks =
            kTranslate_Mask | kScale_Mask | kAffine_Mask | kPerspective_Mask | kRectStaysRect_Mask;

    SkScalar fMat[9];
    mutable uint32_t fTypeMask;

    static void ComputeInv(SkScalar dst[9], const SkScalar src[9], double invDet,
                           bool isPersp) noexcept;

    uint8_t computeTypeMask() const noexcept;
    uint8_t computePerspectiveTypeMask() const noexcept;

    void setTypeMask(int mask) noexcept {
        // allow kUnknown or a valid mask
        SkASSERT(kUnknown_Mask == mask || (mask & kAllMasks) == mask ||
                 ((kUnknown_Mask | kOnlyPerspectiveValid_Mask) & mask) ==
                         (kUnknown_Mask | kOnlyPerspectiveValid_Mask));
        fTypeMask = SkToU8(mask);
    }

    void orTypeMask(int mask) noexcept {
        SkASSERT((mask & kORableMasks) == mask);
        fTypeMask = SkToU8(fTypeMask | mask);
    }

    void clearTypeMask(int mask) noexcept {
        // only allow a valid mask
        SkASSERT((mask & kAllMasks) == mask);
        fTypeMask = fTypeMask & ~mask;
    }

    TypeMask getPerspectiveTypeMaskOnly() const noexcept {
        if ((fTypeMask & kUnknown_Mask) && !(fTypeMask & kOnlyPerspectiveValid_Mask)) {
            fTypeMask = this->computePerspectiveTypeMask();
        }
        return (TypeMask)(fTypeMask & 0xF);
    }

    /** Returns true if we already know that the matrix is identity;
        false otherwise.
    */
    bool isTriviallyIdentity() const noexcept {
        if (fTypeMask & kUnknown_Mask) {
            return false;
        }
        return ((fTypeMask & 0xF) == 0);
    }

    inline void updateTranslateMask() noexcept {
        if ((fMat[kMTransX] != 0) | (fMat[kMTransY] != 0)) {
            fTypeMask |= kTranslate_Mask;
        } else {
            fTypeMask &= ~kTranslate_Mask;
        }
    }

    typedef void (*MapXYProc)(const SkMatrix& mat, SkScalar x, SkScalar y, SkPoint* result);

    static MapXYProc GetMapXYProc(TypeMask mask) noexcept {
        SkASSERT((mask & ~kAllMasks) == 0);
        return gMapXYProcs[mask & kAllMasks];
    }

    MapXYProc getMapXYProc() const noexcept { return GetMapXYProc(this->getType()); }

    typedef void (*MapPtsProc)(const SkMatrix& mat, SkPoint dst[], const SkPoint src[], int count);

    static MapPtsProc GetMapPtsProc(TypeMask mask) noexcept {
        SkASSERT((mask & ~kAllMasks) == 0);
        return gMapPtsProcs[mask & kAllMasks];
    }

    MapPtsProc getMapPtsProc() const noexcept { return GetMapPtsProc(this->getType()); }

    bool SK_WARN_UNUSED_RESULT invertNonIdentity(SkMatrix* inverse) const noexcept;

    static bool Poly2Proc(const SkPoint[], SkMatrix*) noexcept;
    static bool Poly3Proc(const SkPoint[], SkMatrix*) noexcept;
    static bool Poly4Proc(const SkPoint[], SkMatrix*) noexcept;

    static void Identity_xy(const SkMatrix&, SkScalar, SkScalar, SkPoint*) noexcept;
    static void Trans_xy(const SkMatrix&, SkScalar, SkScalar, SkPoint*) noexcept;
    static void Scale_xy(const SkMatrix&, SkScalar, SkScalar, SkPoint*) noexcept;
    static void ScaleTrans_xy(const SkMatrix&, SkScalar, SkScalar, SkPoint*) noexcept;
    static void Rot_xy(const SkMatrix&, SkScalar, SkScalar, SkPoint*) noexcept;
    static void RotTrans_xy(const SkMatrix&, SkScalar, SkScalar, SkPoint*) noexcept;
    static void Persp_xy(const SkMatrix&, SkScalar, SkScalar, SkPoint*) noexcept;

    static const MapXYProc gMapXYProcs[];

    static void Identity_pts(const SkMatrix&, SkPoint[], const SkPoint[], int) noexcept;
    static void Trans_pts(const SkMatrix&, SkPoint dst[], const SkPoint[], int) noexcept;
    static void Scale_pts(const SkMatrix&, SkPoint dst[], const SkPoint[], int) noexcept;
    static void ScaleTrans_pts(const SkMatrix&, SkPoint dst[], const SkPoint[], int count);
    static void Persp_pts(const SkMatrix&, SkPoint dst[], const SkPoint[], int) noexcept;

    static void Affine_vpts(const SkMatrix&, SkPoint dst[], const SkPoint[], int);

    static const MapPtsProc gMapPtsProcs[];

    // return the number of bytes written, whether or not buffer is null
    size_t writeToMemory(void* buffer) const noexcept;
    /**
     * Reads data from the buffer parameter
     *
     * @param buffer Memory to read from
     * @param length Amount of memory available in the buffer
     * @return number of bytes read (must be a multiple of 4) or
     *         0 if there was not enough memory available
     */
    size_t readFromMemory(const void* buffer, size_t length) noexcept;

    friend class SkPerspIter;
    friend class SkMatrixPriv;
    friend class SkReader32;
    friend class SerializationTest;
};
SK_END_REQUIRE_DENSE

#endif
