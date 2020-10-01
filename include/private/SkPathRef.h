/*
 * Copyright 2012 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef SkPathRef_DEFINED
#define SkPathRef_DEFINED

#include "include/core/SkMatrix.h"
#include "include/core/SkPathTypes.h"
#include "include/core/SkPoint.h"
#include "include/core/SkRRect.h"
#include "include/core/SkRect.h"
#include "include/core/SkRefCnt.h"
#include "include/private/SkIDChangeListener.h"
#include "include/private/SkMutex.h"
#include "include/private/SkTDArray.h"
#include "include/private/SkTemplates.h"
#include "include/private/SkTo.h"

#include <atomic>
#include <limits>
#include <tuple>

struct SkPathView;
class SkRBuffer;
class SkWBuffer;

/**
 * Holds the path verbs and points. It is versioned by a generation ID. None of its public methods
 * modify the contents. To modify or append to the verbs/points wrap the SkPathRef in an
 * SkPathRef::Editor object. Installing the editor resets the generation ID. It also performs
 * copy-on-write if the SkPathRef is shared by multiple SkPaths. The caller passes the Editor's
 * constructor a pointer to a sk_sp<SkPathRef>, which may be updated to point to a new SkPathRef
 * after the editor's constructor returns.
 *
 * The points and verbs are stored in a single allocation. The points are at the begining of the
 * allocation while the verbs are stored at end of the allocation, in reverse order. Thus the points
 * and verbs both grow into the middle of the allocation until the meet. To access verb i in the
 * verb array use ref.verbs()[~i] (because verbs() returns a pointer just beyond the first
 * logical verb or the last verb in memory).
 */

class SK_API SkPathRef final : public SkNVRefCnt<SkPathRef> {
 public:
  SkPathRef(
      SkTDArray<SkPoint> points, SkTDArray<uint8_t> verbs, SkTDArray<SkScalar> weights,
      unsigned segmentMask) noexcept
      : fPoints(std::move(points)), fVerbs(std::move(verbs)), fConicWeights(std::move(weights)) {
    fBoundsIsDirty = true;  // this also invalidates fIsFinite
    fGenerationID = 0;      // recompute
    fSegmentMask = segmentMask;
    fIsOval = false;
    fIsRRect = false;
    // The next two values don't matter unless fIsOval or fIsRRect are true.
    fRRectOrOvalIsCCW = false;
    fRRectOrOvalStartIdx = 0xAC;
    SkDEBUGCODE(fEditorsAttached.store(0));
    SkDEBUGCODE(this->validate());
  }

  class Editor {
   public:
    Editor(sk_sp<SkPathRef>* pathRef, int incReserveVerbs = 0, int incReservePoints = 0);

    ~Editor() { SkDEBUGCODE(fPathRef->fEditorsAttached--); }

    /**
     * Returns the array of points.
     */
    SkPoint* writablePoints() noexcept { return fPathRef->getWritablePoints(); }
    const SkPoint* points() const noexcept { return fPathRef->points(); }

    /**
     * Gets the ith point. Shortcut for this->points() + i
     */
    SkPoint* atPoint(int i) noexcept { return fPathRef->getWritablePoints() + i; }
    const SkPoint* atPoint(int i) const noexcept { return &fPathRef->fPoints[i]; }

    /**
     * Adds the verb and allocates space for the number of points indicated by the verb. The
     * return value is a pointer to where the points for the verb should be written.
     * 'weight' is only used if 'verb' is kConic_Verb
     */
    SkPoint* growForVerb(int /*SkPath::Verb*/ verb, SkScalar weight = 0) noexcept {
      SkDEBUGCODE(fPathRef->validate());
      return fPathRef->growForVerb(verb, weight);
    }

    /**
     * Allocates space for multiple instances of a particular verb and the
     * requisite points & weights.
     * The return pointer points at the first new point (indexed normally [<i>]).
     * If 'verb' is kConic_Verb, 'weights' will return a pointer to the
     * space for the conic weights (indexed normally).
     */
    SkPoint* growForRepeatedVerb(
        int /*SkPath::Verb*/ verb, int numVbs, SkScalar** weights = nullptr) noexcept {
      return fPathRef->growForRepeatedVerb(verb, numVbs, weights);
    }

    /**
     * Concatenates all verbs from 'path' onto the pathRef's verbs array. Increases the point
     * count by the number of points in 'path', and the conic weight count by the number of
     * conics in 'path'.
     *
     * Returns pointers to the uninitialized points and conic weights data.
     */
    std::tuple<SkPoint*, SkScalar*> growForVerbsInPath(const SkPathRef& path) noexcept {
      return fPathRef->growForVerbsInPath(path);
    }

    /**
     * Resets the path ref to a new verb and point count. The new verbs and points are
     * uninitialized.
     */
    void resetToSize(int newVerbCnt, int newPointCnt, int newConicCount) {
      fPathRef->resetToSize(newVerbCnt, newPointCnt, newConicCount);
    }

    /**
     * Gets the path ref that is wrapped in the Editor.
     */
    SkPathRef* pathRef() noexcept { return fPathRef; }

    void setIsOval(bool isOval, bool isCCW, unsigned start) noexcept {
      fPathRef->setIsOval(isOval, isCCW, start);
    }

    void setIsRRect(bool isRRect, bool isCCW, unsigned start) noexcept {
      fPathRef->setIsRRect(isRRect, isCCW, start);
    }

    void setBounds(const SkRect& rect) noexcept { fPathRef->setBounds(rect); }

   private:
    SkPathRef* fPathRef;
  };

  class SK_API Iter {
   public:
    Iter() noexcept;
    Iter(const SkPathRef&) noexcept;

    void setPathRef(const SkPathRef&) noexcept;

    /** Return the next verb in this iteration of the path. When all
        segments have been visited, return kDone_Verb.

        If any point in the path is non-finite, return kDone_Verb immediately.

        @param  pts The points representing the current verb and/or segment
                    This must not be NULL.
        @return The verb for the current segment
    */
    uint8_t next(SkPoint pts[4]) noexcept;
    uint8_t peek() const noexcept;

    SkScalar conicWeight() const noexcept { return *fConicWeights; }

   private:
    const SkPoint* fPts;
    const uint8_t* fVerbs;
    const uint8_t* fVerbStop;
    const SkScalar* fConicWeights;
  };

 public:
  /**
   * Gets a path ref with no verbs or points.
   */
  static SkPathRef* CreateEmpty() noexcept;

  /**
   *  Returns true if all of the points in this path are finite, meaning there
   *  are no infinities and no NaNs.
   */
  bool isFinite() const noexcept {
    if (fBoundsIsDirty) {
      this->computeBounds();
    }
    return SkToBool(fIsFinite);
  }

  /**
   *  Returns a mask, where each bit corresponding to a SegmentMask is
   *  set if the path contains 1 or more segments of that type.
   *  Returns 0 for an empty path (no segments).
   */
  uint32_t getSegmentMasks() const noexcept { return fSegmentMask; }

  /** Returns true if the path is an oval.
   *
   * @param rect      returns the bounding rect of this oval. It's a circle
   *                  if the height and width are the same.
   * @param isCCW     is the oval CCW (or CW if false).
   * @param start     indicates where the contour starts on the oval (see
   *                  SkPath::addOval for intepretation of the index).
   *
   * @return true if this path is an oval.
   *              Tracking whether a path is an oval is considered an
   *              optimization for performance and so some paths that are in
   *              fact ovals can report false.
   */
  bool isOval(SkRect* rect, bool* isCCW, unsigned* start) const noexcept {
    if (fIsOval) {
      if (rect) {
        *rect = this->getBounds();
      }
      if (isCCW) {
        *isCCW = SkToBool(fRRectOrOvalIsCCW);
      }
      if (start) {
        *start = fRRectOrOvalStartIdx;
      }
    }

    return SkToBool(fIsOval);
  }

  bool isRRect(SkRRect* rrect, bool* isCCW, unsigned* start) const noexcept {
    if (fIsRRect) {
      if (rrect) {
        *rrect = this->getRRect();
      }
      if (isCCW) {
        *isCCW = SkToBool(fRRectOrOvalIsCCW);
      }
      if (start) {
        *start = fRRectOrOvalStartIdx;
      }
    }
    return SkToBool(fIsRRect);
  }

  bool hasComputedBounds() const noexcept { return !fBoundsIsDirty; }

  /** Returns the bounds of the path's points. If the path contains 0 or 1
      points, the bounds is set to (0,0,0,0), and isEmpty() will return true.
      Note: this bounds may be larger than the actual shape, since curves
      do not extend as far as their control points.
  */
  const SkRect& getBounds() const noexcept {
    if (fBoundsIsDirty) {
      this->computeBounds();
    }
    return fBounds;
  }

  SkRRect getRRect() const noexcept;

  /**
   * Transforms a path ref by a matrix, allocating a new one only if necessary.
   */
  static void CreateTransformedCopy(
      sk_sp<SkPathRef>* dst, const SkPathRef& src, const SkMatrix& matrix);

  //  static SkPathRef* CreateFromBuffer(SkRBuffer* buffer);

  /**
   * Rollsback a path ref to zero verbs and points with the assumption that the path ref will be
   * repopulated with approximately the same number of verbs and points. A new path ref is created
   * only if necessary.
   */
  static void Rewind(sk_sp<SkPathRef>* pathRef);

  ~SkPathRef();
  int countPoints() const noexcept { return fPoints.count(); }
  int countVerbs() const noexcept { return fVerbs.count(); }
  int countWeights() const noexcept { return fConicWeights.count(); }

  /**
   * Returns a pointer one beyond the first logical verb (last verb in memory order).
   */
  const uint8_t* verbsBegin() const noexcept { return fVerbs.begin(); }

  /**
   * Returns a const pointer to the first verb in memory (which is the last logical verb).
   */
  const uint8_t* verbsEnd() const noexcept { return fVerbs.end(); }

  /**
   * Returns a const pointer to the first point.
   */
  const SkPoint* points() const noexcept { return fPoints.begin(); }

  /**
   * Shortcut for this->points() + this->countPoints()
   */
  const SkPoint* pointsEnd() const noexcept { return this->points() + this->countPoints(); }

  const SkScalar* conicWeights() const noexcept { return fConicWeights.begin(); }
  const SkScalar* conicWeightsEnd() const noexcept { return fConicWeights.end(); }

  /**
   * Convenience methods for getting to a verb or point by index.
   */
  uint8_t atVerb(int index) const noexcept { return fVerbs[index]; }
  const SkPoint& atPoint(int index) const noexcept { return fPoints[index]; }

  bool operator==(const SkPathRef& ref) const noexcept;

  /**
   * Writes the path points and verbs to a buffer.
   */
  void writeToBuffer(SkWBuffer* buffer) const noexcept;

  /**
   * Gets the number of bytes that would be written in writeBuffer()
   */
  uint32_t writeSize() const noexcept;

  void interpolate(const SkPathRef& ending, SkScalar weight, SkPathRef* out) const noexcept;

  /**
   * Gets an ID that uniquely identifies the contents of the path ref. If two path refs have the
   * same ID then they have the same verbs and points. However, two path refs may have the same
   * contents but different genIDs.
   */
  uint32_t genID() const noexcept;

  void addGenIDChangeListener(sk_sp<SkIDChangeListener>);  // Threadsafe.
  int genIDChangeListenerCount();                          // Threadsafe

  bool isValid() const noexcept;
  SkDEBUGCODE(void validate() const noexcept { SkASSERT(this->isValid()); });

  SkPathView view(SkPathFillType, SkPathConvexityType) const noexcept;

 private:
  enum SerializationOffsets {
    kLegacyRRectOrOvalStartIdx_SerializationShift = 28,  // requires 3 bits, ignored.
    kLegacyRRectOrOvalIsCCW_SerializationShift = 27,     // requires 1 bit, ignored.
    kLegacyIsRRect_SerializationShift = 26,              // requires 1 bit, ignored.
    kIsFinite_SerializationShift = 25,                   // requires 1 bit
    kLegacyIsOval_SerializationShift = 24,               // requires 1 bit, ignored.
    kSegmentMask_SerializationShift = 0                  // requires 4 bits (deprecated)
  };

  SkPathRef() noexcept {
    fBoundsIsDirty = true;  // this also invalidates fIsFinite
    fGenerationID = kEmptyGenID;
    fSegmentMask = 0;
    fIsOval = false;
    fIsRRect = false;
    // The next two values don't matter unless fIsOval or fIsRRect are true.
    fRRectOrOvalIsCCW = false;
    fRRectOrOvalStartIdx = 0xAC;
    SkDEBUGCODE(fEditorsAttached.store(0));
    SkDEBUGCODE(this->validate());
  }

  void copy(const SkPathRef& ref, int additionalReserveVerbs, int additionalReservePoints);

  // Doesn't read fSegmentMask, but (re)computes it from the verbs array
  unsigned computeSegmentMask() const noexcept;

  // Return true if the computed bounds are finite.
  static bool ComputePtBounds(SkRect* bounds, const SkPathRef& ref) noexcept {
    return bounds->setBoundsCheck(ref.points(), ref.countPoints());
  }

  // called, if dirty, by getBounds()
  void computeBounds() const noexcept {
    SkDEBUGCODE(this->validate());
    // TODO(mtklein): remove fBoundsIsDirty and fIsFinite,
    // using an inverted rect instead of fBoundsIsDirty and always recalculating fIsFinite.
    SkASSERT(fBoundsIsDirty);

    fIsFinite = ComputePtBounds(&fBounds, *this);
    fBoundsIsDirty = false;
  }

  void setBounds(const SkRect& rect) noexcept {
    SkASSERT(rect.fLeft <= rect.fRight && rect.fTop <= rect.fBottom);
    fBounds = rect;
    fBoundsIsDirty = false;
    fIsFinite = fBounds.isFinite();
  }

  /** Makes additional room but does not change the counts or change the genID */
  void incReserve(int additionalVerbs, int additionalPoints) noexcept {
    SkDEBUGCODE(this->validate());
    fPoints.setReserve(fPoints.count() + additionalPoints);
    fVerbs.setReserve(fVerbs.count() + additionalVerbs);
    SkDEBUGCODE(this->validate());
  }

  /** Resets the path ref with verbCount verbs and pointCount points, all uninitialized. Also
   *  allocates space for reserveVerb additional verbs and reservePoints additional points.*/
  void resetToSize(
      int verbCount, int pointCount, int conicCount, int reserveVerbs = 0, int reservePoints = 0) {
    SkDEBUGCODE(this->validate());
    this->callGenIDChangeListeners();
    fBoundsIsDirty = true;  // this also invalidates fIsFinite
    fGenerationID = 0;

    fSegmentMask = 0;
    fIsOval = false;
    fIsRRect = false;

    fPoints.setReserve(pointCount + reservePoints);
    fPoints.setCount(pointCount);
    fVerbs.setReserve(verbCount + reserveVerbs);
    fVerbs.setCount(verbCount);
    fConicWeights.setCount(conicCount);
    SkDEBUGCODE(this->validate());
  }

  /**
   * Increases the verb count by numVbs and point count by the required amount.
   * The new points are uninitialized. All the new verbs are set to the specified
   * verb. If 'verb' is kConic_Verb, 'weights' will return a pointer to the
   * uninitialized conic weights.
   */
  SkPoint* growForRepeatedVerb(int /*SkPath::Verb*/ verb, int numVbs, SkScalar** weights) noexcept;

  /**
   * Increases the verb count 1, records the new verb, and creates room for the requisite number
   * of additional points. A pointer to the first point is returned. Any new points are
   * uninitialized.
   */
  SkPoint* growForVerb(int /*SkPath::Verb*/ verb, SkScalar weight) noexcept;

  /**
   * Concatenates all verbs from 'path' onto our own verbs array. Increases the point count by the
   * number of points in 'path', and the conic weight count by the number of conics in 'path'.
   *
   * Returns pointers to the uninitialized points and conic weights data.
   */
  std::tuple<SkPoint*, SkScalar*> growForVerbsInPath(const SkPathRef& path) noexcept;

  /**
   * Private, non-const-ptr version of the public function verbsMemBegin().
   */
  uint8_t* verbsBeginWritable() noexcept { return fVerbs.begin(); }

  /**
   * Called the first time someone calls CreateEmpty to actually create the singleton.
   */
  friend SkPathRef* sk_create_empty_pathref();

  void setIsOval(bool isOval, bool isCCW, unsigned start) noexcept {
    fIsOval = isOval;
    fRRectOrOvalIsCCW = isCCW;
    fRRectOrOvalStartIdx = SkToU8(start);
  }

  void setIsRRect(bool isRRect, bool isCCW, unsigned start) noexcept {
    fIsRRect = isRRect;
    fRRectOrOvalIsCCW = isCCW;
    fRRectOrOvalStartIdx = SkToU8(start);
  }

  // called only by the editor. Note that this is not a const function.
  SkPoint* getWritablePoints() noexcept {
    SkDEBUGCODE(this->validate());
    fIsOval = false;
    fIsRRect = false;
    return fPoints.begin();
  }

  const SkPoint* getPoints() const noexcept {
    SkDEBUGCODE(this->validate());
    return fPoints.begin();
  }

  void callGenIDChangeListeners();

  enum {
    kMinSize = 256,
  };

  mutable SkRect fBounds;

  SkTDArray<SkPoint> fPoints;
  SkTDArray<uint8_t> fVerbs;
  SkTDArray<SkScalar> fConicWeights;

  enum {
    kEmptyGenID = 1,  // GenID reserved for path ref with zero points and zero verbs.
  };
  mutable uint32_t fGenerationID;
  SkDEBUGCODE(std::atomic<int> fEditorsAttached);  // assert only one editor in use at any time.

  SkIDChangeListener::List fGenIDChangeListeners;

  mutable uint8_t fBoundsIsDirty;
  mutable bool fIsFinite;  // only meaningful if bounds are valid

  bool fIsOval;
  bool fIsRRect;
  // Both the circle and rrect special cases have a notion of direction and starting point
  // The next two variables store that information for either.
  bool fRRectOrOvalIsCCW;
  uint8_t fRRectOrOvalStartIdx;
  uint8_t fSegmentMask;

  friend class PathRefTest_Private;
  friend class ForceIsRRect_Private;  // unit test isRRect
  friend class SkPath;
  friend class SkPathBuilder;
  friend class SkPathPriv;
};

#endif
