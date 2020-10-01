/*
 * Copyright 2012 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */
#ifndef SkPathOpsTypes_DEFINED
#define SkPathOpsTypes_DEFINED

#include <float.h>  // for FLT_EPSILON

#include "include/core/SkPath.h"
#include "include/core/SkScalar.h"
#include "include/pathops/SkPathOps.h"
#include "include/private/SkFloatingPoint.h"
#include "include/private/SkSafe_math.h"
#include "src/pathops/SkPathOpsDebug.h"

enum SkPathOpsMask { kWinding_PathOpsMask = -1, kNo_PathOpsMask = 0, kEvenOdd_PathOpsMask = 1 };

class SkArenaAlloc;
class SkOpCoincidence;
class SkOpContour;
class SkOpContourHead;
class SkIntersections;
class SkIntersectionHelper;

enum class SkOpPhase : char {
  kNoChange,
  kIntersecting,
  kWalking,
  kFixWinding,
};

class SkOpGlobalState {
 public:
  SkOpGlobalState(
      SkOpContourHead* head, SkArenaAlloc* allocator SkDEBUGPARAMS(bool debugSkipAssert)
                                 SkDEBUGPARAMS(const char* testName)) noexcept;

  enum { kMaxWindingTries = 10 };

  bool allocatedOpSpan() const noexcept { return fAllocatedOpSpan; }

  SkArenaAlloc* allocator() noexcept { return fAllocator; }

  void bumpNested() noexcept { ++fNested; }

  void clearNested() noexcept { fNested = 0; }

  SkOpCoincidence* coincidence() noexcept { return fCoincidence; }

  SkOpContourHead* contourHead() noexcept { return fContourHead; }

#ifdef SK_DEBUG
  const class SkOpAngle* debugAngle(int id) const;
  const SkOpCoincidence* debugCoincidence() const;
  SkOpContour* debugContour(int id) const;
  const class SkOpPtT* debugPtT(int id) const;
#endif

  static bool DebugRunFail();

#ifdef SK_DEBUG
  const class SkOpSegment* debugSegment(int id) const;
  bool debugSkipAssert() const { return fDebugSkipAssert; }
  const class SkOpSpanBase* debugSpan(int id) const;
  const char* debugTestName() const { return fDebugTestName; }
#endif

#if DEBUG_T_SECT_LOOP_COUNT
  void debugAddLoopCount(
      SkIntersections*, const SkIntersectionHelper&, const SkIntersectionHelper&);
  void debugDoYourWorst(SkOpGlobalState*);
  void debugLoopReport();
  void debugResetLoopCounts();
#endif

#if DEBUG_COINCIDENCE
  void debugSetCheckHealth(bool check) noexcept { fDebugCheckHealth = check; }
  bool debugCheckHealth() const { return fDebugCheckHealth; }
#endif

#if DEBUG_VALIDATE || DEBUG_COIN
  void debugSetPhase(const char* funcName DEBUG_COIN_DECLARE_PARAMS()) const;
#endif

#if DEBUG_COIN
  void debugAddToCoinChangedDict();
  void debugAddToGlobalCoinDicts();
  SkPathOpsDebug::CoinDict* debugCoinChangedDict() noexcept { return &fCoinChangedDict; }
  const SkPathOpsDebug::CoinDictEntry& debugCoinDictEntry() const { return fCoinDictEntry; }

  static void DumpCoinDict();
#endif

  int nested() const noexcept { return fNested; }

#ifdef SK_DEBUG
  int nextAngleID() noexcept { return ++fAngleID; }

  int nextCoinID() noexcept { return ++fCoinID; }

  int nextContourID() noexcept { return ++fContourID; }

  int nextPtTID() noexcept { return ++fPtTID; }

  int nextSegmentID() noexcept { return ++fSegmentID; }

  int nextSpanID() noexcept { return ++fSpanID; }
#endif

  SkOpPhase phase() const noexcept { return fPhase; }

  void resetAllocatedOpSpan() noexcept { fAllocatedOpSpan = false; }

  void setAllocatedOpSpan() noexcept { fAllocatedOpSpan = true; }

  void setCoincidence(SkOpCoincidence* coincidence) noexcept { fCoincidence = coincidence; }

  void setContourHead(SkOpContourHead* contourHead) noexcept { fContourHead = contourHead; }

  void setPhase(SkOpPhase phase) noexcept {
    if (SkOpPhase::kNoChange == phase) {
      return;
    }
    SkASSERT(fPhase != phase);
    fPhase = phase;
  }

  // called in very rare cases where angles are sorted incorrectly -- signfies op will fail
  void setWindingFailed() noexcept { fWindingFailed = true; }

  bool windingFailed() const noexcept { return fWindingFailed; }

 private:
  SkArenaAlloc* fAllocator;
  SkOpCoincidence* fCoincidence;
  SkOpContourHead* fContourHead;
  int fNested;
  bool fAllocatedOpSpan;
  bool fWindingFailed;
  SkOpPhase fPhase;
#ifdef SK_DEBUG
  const char* fDebugTestName;
  void* fDebugReporter;
  int fAngleID;
  int fCoinID;
  int fContourID;
  int fPtTID;
  int fSegmentID;
  int fSpanID;
  bool fDebugSkipAssert;
#endif
#if DEBUG_T_SECT_LOOP_COUNT
  int fDebugLoopCount[3];
  SkPath::Verb fDebugWorstVerb[6];
  SkPoint fDebugWorstPts[24];
  float fDebugWorstWeight[6];
#endif
#if DEBUG_COIN
  SkPathOpsDebug::CoinDict fCoinChangedDict;
  SkPathOpsDebug::CoinDict fCoinVisitedDict;
  SkPathOpsDebug::CoinDictEntry fCoinDictEntry;
  const char* fPreviousFuncName;
#endif
#if DEBUG_COINCIDENCE
  bool fDebugCheckHealth;
#endif
};

#ifdef SK_DEBUG
#  if DEBUG_COINCIDENCE
#    define SkOPASSERT(cond)                                                                       \
      SkASSERT(                                                                                    \
          (this->globalState() &&                                                                  \
           (this->globalState()->debugCheckHealth() || this->globalState()->debugSkipAssert())) || \
          (cond))
#  else
#    define SkOPASSERT(cond) \
      SkASSERT((this->globalState() && this->globalState()->debugSkipAssert()) || (cond))
#  endif
#  define SkOPOBJASSERT(obj, cond) \
    SkASSERT((obj->globalState() && obj->globalState()->debugSkipAssert()) || (cond))
#else
#  define SkOPASSERT(cond)
#  define SkOPOBJASSERT(obj, cond)
#endif

// Use Almost Equal when comparing coordinates. Use epsilon to compare T values.
bool AlmostEqualUlps(float a, float b) noexcept;
inline bool AlmostEqualUlps(double a, double b) noexcept {
  return AlmostEqualUlps(SkDoubleToScalar(a), SkDoubleToScalar(b));
}

bool AlmostEqualUlpsNoNormalCheck(float a, float b) noexcept;
inline bool AlmostEqualUlpsNoNormalCheck(double a, double b) noexcept {
  return AlmostEqualUlpsNoNormalCheck(SkDoubleToScalar(a), SkDoubleToScalar(b));
}

bool AlmostEqualUlps_Pin(float a, float b) noexcept;
inline bool AlmostEqualUlps_Pin(double a, double b) noexcept {
  return AlmostEqualUlps_Pin(SkDoubleToScalar(a), SkDoubleToScalar(b));
}

// Use Almost Dequal when comparing should not special case denormalized values.
bool AlmostDequalUlps(float a, float b) noexcept;
bool AlmostDequalUlps(double a, double b) noexcept;

bool NotAlmostEqualUlps(float a, float b) noexcept;
inline bool NotAlmostEqualUlps(double a, double b) noexcept {
  return NotAlmostEqualUlps(SkDoubleToScalar(a), SkDoubleToScalar(b));
}

bool NotAlmostEqualUlps_Pin(float a, float b) noexcept;
inline bool NotAlmostEqualUlps_Pin(double a, double b) noexcept {
  return NotAlmostEqualUlps_Pin(SkDoubleToScalar(a), SkDoubleToScalar(b));
}

bool NotAlmostDequalUlps(float a, float b) noexcept;
inline bool NotAlmostDequalUlps(double a, double b) noexcept {
  return NotAlmostDequalUlps(SkDoubleToScalar(a), SkDoubleToScalar(b));
}

// Use Almost Bequal when comparing coordinates in conjunction with between.
bool AlmostBequalUlps(float a, float b) noexcept;
inline bool AlmostBequalUlps(double a, double b) noexcept {
  return AlmostBequalUlps(SkDoubleToScalar(a), SkDoubleToScalar(b));
}

bool AlmostPequalUlps(float a, float b) noexcept;
inline bool AlmostPequalUlps(double a, double b) noexcept {
  return AlmostPequalUlps(SkDoubleToScalar(a), SkDoubleToScalar(b));
}

bool RoughlyEqualUlps(float a, float b) noexcept;
inline bool RoughlyEqualUlps(double a, double b) noexcept {
  return RoughlyEqualUlps(SkDoubleToScalar(a), SkDoubleToScalar(b));
}

bool AlmostLessUlps(float a, float b) noexcept;
inline bool AlmostLessUlps(double a, double b) noexcept {
  return AlmostLessUlps(SkDoubleToScalar(a), SkDoubleToScalar(b));
}

bool AlmostLessOrEqualUlps(float a, float b) noexcept;
inline bool AlmostLessOrEqualUlps(double a, double b) noexcept {
  return AlmostLessOrEqualUlps(SkDoubleToScalar(a), SkDoubleToScalar(b));
}

bool AlmostBetweenUlps(float a, float b, float c) noexcept;
inline bool AlmostBetweenUlps(double a, double b, double c) noexcept {
  return AlmostBetweenUlps(SkDoubleToScalar(a), SkDoubleToScalar(b), SkDoubleToScalar(c));
}

int UlpsDistance(float a, float b) noexcept;
inline int UlpsDistance(double a, double b) noexcept {
  return UlpsDistance(SkDoubleToScalar(a), SkDoubleToScalar(b));
}

// FLT_EPSILON == 1.19209290E-07 == 1 / (2 ^ 23)
// DBL_EPSILON == 2.22045e-16
constexpr double FLT_EPSILON_CUBED = FLT_EPSILON * FLT_EPSILON * FLT_EPSILON;
constexpr double FLT_EPSILON_HALF = FLT_EPSILON / 2;
constexpr double FLT_EPSILON_DOUBLE = FLT_EPSILON * 2;
constexpr double FLT_EPSILON_ORDERABLE_ERR = FLT_EPSILON * 16;
constexpr double FLT_EPSILON_SQUARED = FLT_EPSILON * FLT_EPSILON;
// Use a compile-time constant for FLT_EPSILON_SQRT to avoid initializers.
// A 17 digit constant guarantees exact results.
constexpr double FLT_EPSILON_SQRT = 0.00034526697709225118;  // sqrt(FLT_EPSILON);
constexpr double FLT_EPSILON_INVERSE = 1 / FLT_EPSILON;
constexpr double DBL_EPSILON_ERR = DBL_EPSILON * 4;  // FIXME: tune -- allow a few bits of error
constexpr double DBL_EPSILON_SUBDIVIDE_ERR = DBL_EPSILON * 16;
constexpr double ROUGH_EPSILON = FLT_EPSILON * 64;
constexpr double MORE_ROUGH_EPSILON = FLT_EPSILON * 256;
constexpr double WAY_ROUGH_EPSILON = FLT_EPSILON * 2048;
constexpr double BUMP_EPSILON = FLT_EPSILON * 4096;

constexpr SkScalar INVERSE_NUMBER_RANGE = FLT_EPSILON_ORDERABLE_ERR;

constexpr inline bool zero_or_one(double x) noexcept { return x == 0 || x == 1; }

inline bool approximately_zero(double x) noexcept { return fabs(x) < FLT_EPSILON; }

inline bool precisely_zero(double x) noexcept { return fabs(x) < DBL_EPSILON_ERR; }

inline bool precisely_subdivide_zero(double x) noexcept {
  return fabs(x) < DBL_EPSILON_SUBDIVIDE_ERR;
}

inline bool approximately_zero(float x) noexcept { return fabs(x) < FLT_EPSILON; }

inline bool approximately_zero_cubed(double x) noexcept { return fabs(x) < FLT_EPSILON_CUBED; }

inline bool approximately_zero_half(double x) noexcept { return fabs(x) < FLT_EPSILON_HALF; }

inline bool approximately_zero_double(double x) noexcept { return fabs(x) < FLT_EPSILON_DOUBLE; }

inline bool approximately_zero_orderable(double x) noexcept {
  return fabs(x) < FLT_EPSILON_ORDERABLE_ERR;
}

inline bool approximately_zero_squared(double x) noexcept { return fabs(x) < FLT_EPSILON_SQUARED; }

inline bool approximately_zero_sqrt(double x) noexcept { return fabs(x) < FLT_EPSILON_SQRT; }

inline bool roughly_zero(double x) noexcept { return fabs(x) < ROUGH_EPSILON; }

inline bool approximately_zero_inverse(double x) noexcept { return fabs(x) > FLT_EPSILON_INVERSE; }

inline bool approximately_zero_when_compared_to(double x, double y) noexcept {
  return x == 0 || fabs(x) < fabs(y * FLT_EPSILON);
}

inline bool precisely_zero_when_compared_to(double x, double y) noexcept {
  return x == 0 || fabs(x) < fabs(y * DBL_EPSILON);
}

inline bool roughly_zero_when_compared_to(double x, double y) noexcept {
  return x == 0 || fabs(x) < fabs(y * ROUGH_EPSILON);
}

// Use this for comparing Ts in the range of 0 to 1. For general numbers (larger and smaller) use
// AlmostEqualUlps instead.
inline bool approximately_equal(double x, double y) noexcept { return approximately_zero(x - y); }

inline bool precisely_equal(double x, double y) noexcept { return precisely_zero(x - y); }

inline bool precisely_subdivide_equal(double x, double y) noexcept {
  return precisely_subdivide_zero(x - y);
}

inline bool approximately_equal_half(double x, double y) noexcept {
  return approximately_zero_half(x - y);
}

inline bool approximately_equal_double(double x, double y) noexcept {
  return approximately_zero_double(x - y);
}

inline bool approximately_equal_orderable(double x, double y) noexcept {
  return approximately_zero_orderable(x - y);
}

inline bool approximately_equal_squared(double x, double y) noexcept {
  return approximately_equal(x, y);
}

inline constexpr bool approximately_greater(double x, double y) noexcept {
  return x - FLT_EPSILON >= y;
}

inline constexpr bool approximately_greater_double(double x, double y) noexcept {
  return x - FLT_EPSILON_DOUBLE >= y;
}

inline constexpr bool approximately_greater_orderable(double x, double y) noexcept {
  return x - FLT_EPSILON_ORDERABLE_ERR >= y;
}

inline constexpr bool approximately_greater_or_equal(double x, double y) noexcept {
  return x + FLT_EPSILON > y;
}

inline constexpr bool approximately_greater_or_equal_double(double x, double y) noexcept {
  return x + FLT_EPSILON_DOUBLE > y;
}

inline constexpr bool approximately_greater_or_equal_orderable(double x, double y) noexcept {
  return x + FLT_EPSILON_ORDERABLE_ERR > y;
}

inline constexpr bool approximately_lesser(double x, double y) noexcept {
  return x + FLT_EPSILON <= y;
}

inline constexpr bool approximately_lesser_double(double x, double y) noexcept {
  return x + FLT_EPSILON_DOUBLE <= y;
}

inline constexpr bool approximately_lesser_orderable(double x, double y) noexcept {
  return x + FLT_EPSILON_ORDERABLE_ERR <= y;
}

inline constexpr bool approximately_lesser_or_equal(double x, double y) noexcept {
  return x - FLT_EPSILON < y;
}

inline constexpr bool approximately_lesser_or_equal_double(double x, double y) noexcept {
  return x - FLT_EPSILON_DOUBLE < y;
}

inline constexpr bool approximately_lesser_or_equal_orderable(double x, double y) noexcept {
  return x - FLT_EPSILON_ORDERABLE_ERR < y;
}

inline constexpr bool approximately_greater_than_one(double x) noexcept {
  return x > 1 - FLT_EPSILON;
}

inline constexpr bool precisely_greater_than_one(double x) noexcept {
  return x > 1 - DBL_EPSILON_ERR;
}

inline constexpr bool approximately_less_than_zero(double x) noexcept { return x < FLT_EPSILON; }

inline constexpr bool precisely_less_than_zero(double x) noexcept { return x < DBL_EPSILON_ERR; }

inline constexpr bool approximately_negative(double x) noexcept { return x < FLT_EPSILON; }

inline constexpr bool approximately_negative_orderable(double x) noexcept {
  return x < FLT_EPSILON_ORDERABLE_ERR;
}

inline constexpr bool precisely_negative(double x) noexcept { return x < DBL_EPSILON_ERR; }

inline constexpr bool approximately_one_or_less(double x) noexcept { return x < 1 + FLT_EPSILON; }

inline constexpr bool approximately_one_or_less_double(double x) noexcept {
  return x < 1 + FLT_EPSILON_DOUBLE;
}

inline constexpr bool approximately_positive(double x) noexcept { return x > -FLT_EPSILON; }

inline constexpr bool approximately_positive_squared(double x) noexcept {
  return x > -(FLT_EPSILON_SQUARED);
}

inline constexpr bool approximately_zero_or_more(double x) noexcept { return x > -FLT_EPSILON; }

inline constexpr bool approximately_zero_or_more_double(double x) noexcept {
  return x > -FLT_EPSILON_DOUBLE;
}

inline constexpr bool approximately_between_orderable(double a, double b, double c) noexcept {
  return a <= c
             ? approximately_negative_orderable(a - b) && approximately_negative_orderable(b - c)
             : approximately_negative_orderable(b - a) && approximately_negative_orderable(c - b);
}

inline constexpr bool approximately_between(double a, double b, double c) noexcept {
  return a <= c ? approximately_negative(a - b) && approximately_negative(b - c)
                : approximately_negative(b - a) && approximately_negative(c - b);
}

inline constexpr bool precisely_between(double a, double b, double c) noexcept {
  return a <= c ? precisely_negative(a - b) && precisely_negative(b - c)
                : precisely_negative(b - a) && precisely_negative(c - b);
}

// returns true if (a <= b <= c) || (a >= b >= c)
inline constexpr bool between(double a, double b, double c) noexcept {
  SkASSERT(
      ((a <= b && b <= c) || (a >= b && b >= c)) == ((a - b) * (c - b) <= 0) ||
      (precisely_zero(a) && precisely_zero(b) && precisely_zero(c)));
  return (a - b) * (c - b) <= 0;
}

inline bool roughly_equal(double x, double y) noexcept { return fabs(x - y) < ROUGH_EPSILON; }

inline constexpr bool roughly_negative(double x) noexcept { return x < ROUGH_EPSILON; }

inline constexpr bool roughly_between(double a, double b, double c) noexcept {
  return a <= c ? roughly_negative(a - b) && roughly_negative(b - c)
                : roughly_negative(b - a) && roughly_negative(c - b);
}

inline bool more_roughly_equal(double x, double y) noexcept {
  return fabs(x - y) < MORE_ROUGH_EPSILON;
}

struct SkDPoint;
struct SkDVector;
struct SkDLine;
struct SkDQuad;
struct SkDConic;
struct SkDCubic;
struct SkDRect;

inline constexpr SkPath::Verb SkPathOpsPointsToVerb(int points) noexcept {
  int verb = (1 << points) >> 1;
#ifdef SK_DEBUG
  switch (points) {
    case 0: SkASSERT(SkPath::kMove_Verb == verb); break;
    case 1: SkASSERT(SkPath::kLine_Verb == verb); break;
    case 2: SkASSERT(SkPath::kQuad_Verb == verb); break;
    case 3: SkASSERT(SkPath::kCubic_Verb == verb); break;
    default: SkDEBUGFAIL("should not be here");
  }
#endif
  return (SkPath::Verb)verb;
}

inline constexpr int SkPathOpsVerbToPoints(SkPath::Verb verb) noexcept {
  int points = (int)verb - (((int)verb + 1) >> 2);
#ifdef SK_DEBUG
  switch (verb) {
    case SkPath::kLine_Verb: SkASSERT(1 == points); break;
    case SkPath::kQuad_Verb: SkASSERT(2 == points); break;
    case SkPath::kConic_Verb: SkASSERT(2 == points); break;
    case SkPath::kCubic_Verb: SkASSERT(3 == points); break;
    default: SkDEBUGFAIL("should not get here");
  }
#endif
  return points;
}

inline constexpr double SkDInterp(double A, double B, double t) noexcept { return A + (B - A) * t; }

double SkDCubeRoot(double x) noexcept;

/* Returns -1 if negative, 0 if zero, 1 if positive
 */
inline constexpr int SkDSign(double x) noexcept { return (x > 0) - (x < 0); }

/* Returns 0 if negative, 1 if zero, 2 if positive
 */
inline constexpr int SKDSide(double x) noexcept { return (x > 0) + (x >= 0); }

/* Returns 1 if negative, 2 if zero, 4 if positive
 */
inline constexpr int SkDSideBit(double x) noexcept { return 1 << SKDSide(x); }

inline constexpr double SkPinT(double t) noexcept {
  return precisely_less_than_zero(t) ? 0 : precisely_greater_than_one(t) ? 1 : t;
}

#endif
