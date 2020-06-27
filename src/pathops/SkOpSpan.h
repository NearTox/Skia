/*
 * Copyright 2012 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */
#ifndef SkOpSpan_DEFINED
#define SkOpSpan_DEFINED

#include "include/core/SkPoint.h"
#include "src/pathops/SkPathOpsDebug.h"
#include "src/pathops/SkPathOpsTypes.h"

class SkArenaAlloc;
class SkOpAngle;
class SkOpContour;
class SkOpGlobalState;
class SkOpSegment;
class SkOpSpanBase;
class SkOpSpan;
struct SkPathOpsBounds;

// subset of op span used by terminal span (when t is equal to one)
class SkOpPtT {
 public:
  enum { kIsAlias = 1, kIsDuplicate = 1 };

  const SkOpPtT* active() const noexcept;

  // please keep in sync with debugAddOpp()
  void addOpp(SkOpPtT* opp, SkOpPtT* oppPrev) noexcept {
    SkOpPtT* oldNext = this->fNext;
    SkASSERT(this != opp);
    this->fNext = opp;
    SkASSERT(oppPrev != oldNext);
    oppPrev->fNext = oldNext;
  }

  bool alias() const noexcept;
  bool coincident() const noexcept { return fCoincident; }
  bool contains(const SkOpPtT*) const noexcept;
  bool contains(const SkOpSegment*, const SkPoint&) const noexcept;
  bool contains(const SkOpSegment*, double t) const noexcept;
  const SkOpPtT* contains(const SkOpSegment*) const noexcept;
  SkOpContour* contour() const noexcept;

  int debugID() const noexcept { return SkDEBUGRELEASE(fID, -1); }

  void debugAddOpp(const SkOpPtT* opp, const SkOpPtT* oppPrev) const;
  const SkOpAngle* debugAngle(int id) const;
  const SkOpCoincidence* debugCoincidence() const;
  bool debugContains(const SkOpPtT*) const;
  const SkOpPtT* debugContains(const SkOpSegment* check) const;
  SkOpContour* debugContour(int id) const;
  const SkOpPtT* debugEnder(const SkOpPtT* end) const;
  int debugLoopLimit(bool report) const;
  bool debugMatchID(int id) const;
  const SkOpPtT* debugOppPrev(const SkOpPtT* opp) const;
  const SkOpPtT* debugPtT(int id) const;
  void debugResetCoinT() const;
  const SkOpSegment* debugSegment(int id) const;
  void debugSetCoinT(int) const;
  const SkOpSpanBase* debugSpan(int id) const;
  void debugValidate() const;

  bool deleted() const noexcept { return fDeleted; }

  bool duplicate() const noexcept { return fDuplicatePt; }

  void dump() const;  // available to testing only
  void dumpAll() const;
  void dumpBase() const;

  const SkOpPtT* find(const SkOpSegment*) const noexcept;
  SkOpGlobalState* globalState() const noexcept;
  void init(SkOpSpanBase*, double t, const SkPoint&, bool dup) noexcept;

  void insert(SkOpPtT* span) noexcept {
    SkASSERT(span != this);
    span->fNext = fNext;
    fNext = span;
  }

  const SkOpPtT* next() const noexcept { return fNext; }

  SkOpPtT* next() noexcept { return fNext; }

  bool onEnd() const noexcept;

  // returns nullptr if this is already in the opp ptT loop
  SkOpPtT* oppPrev(const SkOpPtT* opp) const noexcept {
    // find the fOpp ptr to opp
    SkOpPtT* oppPrev = opp->fNext;
    if (oppPrev == this) {
      return nullptr;
    }
    while (oppPrev->fNext != opp) {
      oppPrev = oppPrev->fNext;
      if (oppPrev == this) {
        return nullptr;
      }
    }
    return oppPrev;
  }

  static bool Overlaps(
      const SkOpPtT* s1, const SkOpPtT* e1, const SkOpPtT* s2, const SkOpPtT* e2,
      const SkOpPtT** sOut, const SkOpPtT** eOut) noexcept {
    const SkOpPtT* start1 = s1->fT < e1->fT ? s1 : e1;
    const SkOpPtT* start2 = s2->fT < e2->fT ? s2 : e2;
    *sOut = between(s1->fT, start2->fT, e1->fT)   ? start2
            : between(s2->fT, start1->fT, e2->fT) ? start1
                                                  : nullptr;
    const SkOpPtT* end1 = s1->fT < e1->fT ? e1 : s1;
    const SkOpPtT* end2 = s2->fT < e2->fT ? e2 : s2;
    *eOut = between(s1->fT, end2->fT, e1->fT)   ? end2
            : between(s2->fT, end1->fT, e2->fT) ? end1
                                                : nullptr;
    if (*sOut == *eOut) {
      SkOPOBJASSERT(s1, start1->fT >= end2->fT || start2->fT >= end1->fT);
      return false;
    }
    SkASSERT(!*sOut || *sOut != *eOut);
    return *sOut && *eOut;
  }

  bool ptAlreadySeen(const SkOpPtT* head) const noexcept;
  SkOpPtT* prev() noexcept;

  const SkOpSegment* segment() const noexcept;
  SkOpSegment* segment() noexcept;

  void setCoincident() const noexcept {
    SkOPASSERT(!fDeleted);
    fCoincident = true;
  }

  void setDeleted() noexcept;

  void setSpan(const SkOpSpanBase* span) noexcept { fSpan = const_cast<SkOpSpanBase*>(span); }

  const SkOpSpanBase* span() const noexcept { return fSpan; }

  SkOpSpanBase* span() noexcept { return fSpan; }

  const SkOpPtT* starter(const SkOpPtT* end) const noexcept { return fT < end->fT ? this : end; }

  double fT;
  SkPoint fPt;  // cache of point value at this t
 protected:
  SkOpSpanBase* fSpan;  // contains winding data
  SkOpPtT* fNext;       // intersection on opposite curve or alias on this curve
  bool fDeleted;        // set if removed from span list
  bool fDuplicatePt;    // set if identical pt is somewhere in the next loop
  // below mutable since referrer is otherwise always const
  mutable bool fCoincident;  // set if at some point a coincident span pointed here
  SkDEBUGCODE(int fID);
};

class SkOpSpanBase {
 public:
  enum class Collapsed {
    kNo,
    kYes,
    kError,
  };

  bool addOpp(SkOpSpanBase* opp);

  void bumpSpanAdds() noexcept { ++fSpanAdds; }

  bool chased() const noexcept { return fChased; }

  void checkForCollapsedCoincidence();

  const SkOpSpanBase* coinEnd() const noexcept { return fCoinEnd; }

  Collapsed collapsed(double s, double e) const noexcept;
  bool contains(const SkOpSpanBase*) const noexcept;
  const SkOpPtT* contains(const SkOpSegment*) const noexcept;

  bool containsCoinEnd(const SkOpSpanBase* coin) const noexcept {
    SkASSERT(this != coin);
    const SkOpSpanBase* next = this;
    while ((next = next->fCoinEnd) != this) {
      if (next == coin) {
        return true;
      }
    }
    return false;
  }

  bool containsCoinEnd(const SkOpSegment*) const noexcept;
  SkOpContour* contour() const noexcept;

  int debugBumpCount() noexcept { return SkDEBUGRELEASE(++fCount, -1); }

  int debugID() const noexcept { return SkDEBUGRELEASE(fID, -1); }

#if DEBUG_COIN
  void debugAddOpp(SkPathOpsDebug::GlitchLog*, const SkOpSpanBase* opp) const;
#endif
  bool debugAlignedEnd(double t, const SkPoint& pt) const;
  bool debugAlignedInner() const;
  const SkOpAngle* debugAngle(int id) const;
#if DEBUG_COIN
  void debugCheckForCollapsedCoincidence(SkPathOpsDebug::GlitchLog*) const;
#endif
  const SkOpCoincidence* debugCoincidence() const;
  bool debugCoinEndLoopCheck() const;
  SkOpContour* debugContour(int id) const;
#ifdef SK_DEBUG
  bool debugDeleted() const { return fDebugDeleted; }
#endif
#if DEBUG_COIN
  void debugInsertCoinEnd(SkPathOpsDebug::GlitchLog*, const SkOpSpanBase*) const;
  void debugMergeMatches(SkPathOpsDebug::GlitchLog* log, const SkOpSpanBase* opp) const;
#endif
  const SkOpPtT* debugPtT(int id) const;
  void debugResetCoinT() const;
  const SkOpSegment* debugSegment(int id) const;
  void debugSetCoinT(int) const;
#ifdef SK_DEBUG
  void debugSetDeleted() { fDebugDeleted = true; }
#endif
  const SkOpSpanBase* debugSpan(int id) const;
  const SkOpSpan* debugStarter(SkOpSpanBase const** endPtr) const;
  SkOpGlobalState* globalState() const noexcept;
  void debugValidate() const;

  bool deleted() const noexcept { return fPtT.deleted(); }

  void dump() const;  // available to testing only
  void dumpCoin() const;
  void dumpAll() const;
  void dumpBase() const;
  void dumpHead() const;

  bool final() const noexcept { return fPtT.fT == 1; }

  SkOpAngle* fromAngle() const noexcept { return fFromAngle; }

  void initBase(SkOpSegment* parent, SkOpSpan* prev, double t, const SkPoint& pt) noexcept;

  // Please keep this in sync with debugInsertCoinEnd()
  void insertCoinEnd(SkOpSpanBase* coin) {
    if (containsCoinEnd(coin)) {
      SkASSERT(coin->containsCoinEnd(this));
      return;
    }
    debugValidate();
    SkASSERT(this != coin);
    SkOpSpanBase* coinNext = coin->fCoinEnd;
    coin->fCoinEnd = this->fCoinEnd;
    this->fCoinEnd = coinNext;
    debugValidate();
  }

  void merge(SkOpSpan* span);
  bool mergeMatches(SkOpSpanBase* opp);

  const SkOpSpan* prev() const noexcept { return fPrev; }

  SkOpSpan* prev() noexcept { return fPrev; }

  const SkPoint& pt() const noexcept { return fPtT.fPt; }

  const SkOpPtT* ptT() const noexcept { return &fPtT; }

  SkOpPtT* ptT() noexcept { return &fPtT; }

  SkOpSegment* segment() const noexcept { return fSegment; }

  void setAligned() noexcept { fAligned = true; }

  void setChased(bool chased) noexcept { fChased = chased; }

  void setFromAngle(SkOpAngle* angle) noexcept { fFromAngle = angle; }

  void setPrev(SkOpSpan* prev) noexcept { fPrev = prev; }

  bool simple() const {
    fPtT.debugValidate();
    return fPtT.next()->next() == &fPtT;
  }

  int spanAddsCount() const noexcept { return fSpanAdds; }

  const SkOpSpan* starter(const SkOpSpanBase* end) const noexcept {
    const SkOpSpanBase* result = t() < end->t() ? this : end;
    return result->upCast();
  }

  SkOpSpan* starter(SkOpSpanBase* end) noexcept {
    SkASSERT(this->segment() == end->segment());
    SkOpSpanBase* result = t() < end->t() ? this : end;
    return result->upCast();
  }

  SkOpSpan* starter(SkOpSpanBase** endPtr) noexcept {
    SkOpSpanBase* end = *endPtr;
    SkASSERT(this->segment() == end->segment());
    SkOpSpanBase* result;
    if (t() < end->t()) {
      result = this;
    } else {
      result = end;
      *endPtr = this;
    }
    return result->upCast();
  }

  int step(const SkOpSpanBase* end) const noexcept { return t() < end->t() ? 1 : -1; }

  double t() const noexcept { return fPtT.fT; }

  void unaligned() noexcept { fAligned = false; }

  SkOpSpan* upCast() noexcept {
    SkASSERT(!final());
    return (SkOpSpan*)this;
  }

  const SkOpSpan* upCast() const noexcept {
    SkOPASSERT(!final());
    return (const SkOpSpan*)this;
  }

  SkOpSpan* upCastable() noexcept { return final() ? nullptr : upCast(); }

  const SkOpSpan* upCastable() const noexcept { return final() ? nullptr : upCast(); }

 private:
  void alignInner();

 protected:                // no direct access to internals to avoid treating a span base as a span
  SkOpPtT fPtT;            // list of points and t values associated with the start of this span
  SkOpSegment* fSegment;   // segment that contains this span
  SkOpSpanBase* fCoinEnd;  // linked list of coincident spans that end here (may point to itself)
  SkOpAngle* fFromAngle;   // points to next angle from span start to end
  SkOpSpan* fPrev;         // previous intersection point
  int fSpanAdds;           // number of times intersections have been added to span
  bool fAligned;
  bool fChased;             // set after span has been added to chase array
  SkDEBUGCODE(int fCount);  // number of pt/t pairs added
  SkDEBUGCODE(int fID);
  SkDEBUGCODE(bool fDebugDeleted);  // set when span was merged with another span
};

class SkOpSpan : public SkOpSpanBase {
 public:
  bool alreadyAdded() const noexcept {
    if (fAlreadyAdded) {
      return true;
    }
    return false;
  }

  bool clearCoincident() noexcept {
    SkASSERT(!final());
    if (fCoincident == this) {
      return false;
    }
    fCoincident = this;
    return true;
  }

  int computeWindSum();
  bool containsCoincidence(const SkOpSegment*) const noexcept;

  bool containsCoincidence(const SkOpSpan* coin) const noexcept {
    SkASSERT(this != coin);
    const SkOpSpan* next = this;
    while ((next = next->fCoincident) != this) {
      if (next == coin) {
        return true;
      }
    }
    return false;
  }

  bool debugCoinLoopCheck() const;
#if DEBUG_COIN
  void debugInsertCoincidence(SkPathOpsDebug::GlitchLog*, const SkOpSpan*) const;
  void debugInsertCoincidence(
      SkPathOpsDebug::GlitchLog*, const SkOpSegment*, bool flipped, bool ordered) const;
#endif
  void dumpCoin() const;
  bool dumpSpan() const;

  bool done() const noexcept {
    SkASSERT(!final());
    return fDone;
  }

  void init(SkOpSegment* parent, SkOpSpan* prev, double t, const SkPoint& pt) noexcept;
  bool insertCoincidence(const SkOpSegment*, bool flipped, bool ordered);

  // Please keep this in sync with debugInsertCoincidence()
  void insertCoincidence(SkOpSpan* coin) {
    if (containsCoincidence(coin)) {
      SkASSERT(coin->containsCoincidence(this));
      return;
    }
    debugValidate();
    SkASSERT(this != coin);
    SkOpSpan* coinNext = coin->fCoincident;
    coin->fCoincident = this->fCoincident;
    this->fCoincident = coinNext;
    debugValidate();
  }

  bool isCanceled() const noexcept {
    SkASSERT(!final());
    return fWindValue == 0 && fOppValue == 0;
  }

  bool isCoincident() const noexcept {
    SkASSERT(!final());
    return fCoincident != this;
  }

  void markAdded() noexcept { fAlreadyAdded = true; }

  SkOpSpanBase* next() const noexcept {
    SkASSERT(!final());
    return fNext;
  }

  int oppSum() const noexcept {
    SkASSERT(!final());
    return fOppSum;
  }

  int oppValue() const noexcept {
    SkASSERT(!final());
    return fOppValue;
  }

  void release(const SkOpPtT*);

  SkOpPtT* setCoinStart(SkOpSpan* oldCoinStart, SkOpSegment* oppSegment);

  void setDone(bool done) noexcept {
    SkASSERT(!final());
    fDone = done;
  }

  void setNext(SkOpSpanBase* nextT) noexcept {
    SkASSERT(!final());
    fNext = nextT;
  }

  void setOppSum(int oppSum) noexcept;

  void setOppValue(int oppValue) noexcept {
    SkASSERT(!final());
    SkASSERT(fOppSum == SK_MinS32);
    SkOPASSERT(!oppValue || !fDone);
    fOppValue = oppValue;
  }

  void setToAngle(SkOpAngle* angle) noexcept {
    SkASSERT(!final());
    fToAngle = angle;
  }

  void setWindSum(int windSum) noexcept;

  void setWindValue(int windValue) noexcept {
    SkASSERT(!final());
    SkASSERT(windValue >= 0);
    SkASSERT(fWindSum == SK_MinS32);
    SkOPASSERT(!windValue || !fDone);
    fWindValue = windValue;
  }

  bool sortableTop(SkOpContour*);

  SkOpAngle* toAngle() const noexcept {
    SkASSERT(!final());
    return fToAngle;
  }

  int windSum() const noexcept {
    SkASSERT(!final());
    return fWindSum;
  }

  int windValue() const noexcept {
    SkOPASSERT(!final());
    return fWindValue;
  }

 private:                 // no direct access to internals to avoid treating a span base as a span
  SkOpSpan* fCoincident;  // linked list of spans coincident with this one (may point to itself)
  SkOpAngle* fToAngle;    // points to next angle from span start to end
  SkOpSpanBase* fNext;    // next intersection point
  int fWindSum;           // accumulated from contours surrounding this one.
  int fOppSum;            // for binary operators: the opposite winding sum
  int fWindValue;         // 0 == canceled; 1 == normal; >1 == coincident
  int fOppValue;          // normally 0 -- when binary coincident edges combine, opp value goes here
  int fTopTTry;           // specifies direction and t value to try next
  bool fDone;             // if set, this span to next higher T has been processed
  bool fAlreadyAdded;
};

#endif
