/*
 * Copyright 2013 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */
#ifndef SkOpContour_DEFINED
#define SkOpContour_DEFINED

#include "include/private/SkTDArray.h"
#include "src/core/SkTSort.h"
#include "src/pathops/SkOpSegment.h"

enum class SkOpRayDir;
struct SkOpRayHit;
class SkPathWriter;

class SkOpContour {
 public:
  SkOpContour() { reset(); }

  bool operator<(const SkOpContour& rh) const noexcept {
    return fBounds.fTop == rh.fBounds.fTop ? fBounds.fLeft < rh.fBounds.fLeft
                                           : fBounds.fTop < rh.fBounds.fTop;
  }

  void addConic(SkPoint pts[3], SkScalar weight) { appendSegment().addConic(pts, weight, this); }

  void addCubic(SkPoint pts[4]) { appendSegment().addCubic(pts, this); }

  SkOpSegment* addLine(SkPoint pts[2]) {
    SkASSERT(pts[0] != pts[1]);
    return appendSegment().addLine(pts, this);
  }

  void addQuad(SkPoint pts[3]) { appendSegment().addQuad(pts, this); }

  SkOpSegment& appendSegment() {
    SkOpSegment* result = fCount++ ? this->globalState()->allocator()->make<SkOpSegment>() : &fHead;
    result->setPrev(fTail);
    if (fTail) {
      fTail->setNext(result);
    }
    fTail = result;
    return *result;
  }

  const SkPathOpsBounds& bounds() const noexcept { return fBounds; }

  void calcAngles() {
    SkASSERT(fCount > 0);
    SkOpSegment* segment = &fHead;
    do {
      segment->calcAngles();
    } while ((segment = segment->next()));
  }

  void complete() noexcept { setBounds(); }

  int count() const noexcept { return fCount; }

  int debugID() const noexcept { return SkDEBUGRELEASE(fID, -1); }

  int debugIndent() const noexcept { return SkDEBUGRELEASE(fDebugIndent, 0); }

  const SkOpAngle* debugAngle(int id) const noexcept {
    return SkDEBUGRELEASE(this->globalState()->debugAngle(id), nullptr);
  }

  const SkOpCoincidence* debugCoincidence() const noexcept {
    return this->globalState()->coincidence();
  }

#if DEBUG_COIN
  void debugCheckHealth(SkPathOpsDebug::GlitchLog*) const;
#endif

  SkOpContour* debugContour(int id) const noexcept {
    return SkDEBUGRELEASE(this->globalState()->debugContour(id), nullptr);
  }

#if DEBUG_COIN
  void debugMissingCoincidence(SkPathOpsDebug::GlitchLog* log) const;
  void debugMoveMultiples(SkPathOpsDebug::GlitchLog*) const;
  void debugMoveNearby(SkPathOpsDebug::GlitchLog* log) const;
#endif

  const SkOpPtT* debugPtT(int id) const noexcept {
    return SkDEBUGRELEASE(this->globalState()->debugPtT(id), nullptr);
  }

  const SkOpSegment* debugSegment(int id) const noexcept {
    return SkDEBUGRELEASE(this->globalState()->debugSegment(id), nullptr);
  }

#if DEBUG_ACTIVE_SPANS
  void debugShowActiveSpans(SkString* str) {
    SkOpSegment* segment = &fHead;
    do {
      segment->debugShowActiveSpans(str);
    } while ((segment = segment->next()));
  }
#endif

  const SkOpSpanBase* debugSpan(int id) const noexcept {
    return SkDEBUGRELEASE(this->globalState()->debugSpan(id), nullptr);
  }

  SkOpGlobalState* globalState() const noexcept { return fState; }

  void debugValidate() const noexcept {
#if DEBUG_VALIDATE
    const SkOpSegment* segment = &fHead;
    const SkOpSegment* prior = nullptr;
    do {
      segment->debugValidate();
      SkASSERT(segment->prev() == prior);
      prior = segment;
    } while ((segment = segment->next()));
    SkASSERT(prior == fTail);
#endif
  }

  bool done() const noexcept { return fDone; }

  void dump() const;
  void dumpAll() const;
  void dumpAngles() const;
  void dumpContours() const;
  void dumpContoursAll() const;
  void dumpContoursAngles() const;
  void dumpContoursPts() const;
  void dumpContoursPt(int segmentID) const;
  void dumpContoursSegment(int segmentID) const;
  void dumpContoursSpan(int segmentID) const;
  void dumpContoursSpans() const;
  void dumpPt(int) const;
  void dumpPts(const char* prefix = "seg") const;
  void dumpPtsX(const char* prefix) const;
  void dumpSegment(int) const;
  void dumpSegments(const char* prefix = "seg", SkPathOp op = (SkPathOp)-1) const;
  void dumpSpan(int) const;
  void dumpSpans() const;

  const SkPoint& end() const noexcept { return fTail->pts()[SkPathOpsVerbToPoints(fTail->verb())]; }

  SkOpSpan* findSortableTop(SkOpContour*);

  SkOpSegment* first() noexcept {
    SkASSERT(fCount > 0);
    return &fHead;
  }

  const SkOpSegment* first() const noexcept {
    SkASSERT(fCount > 0);
    return &fHead;
  }

  void indentDump() const noexcept { SkDEBUGCODE(fDebugIndent += 2); }

  void init(SkOpGlobalState* globalState, bool operand, bool isXor) noexcept {
    fState = globalState;
    fOperand = operand;
    fXor = isXor;
    SkDEBUGCODE(fID = globalState->nextContourID());
  }

  int isCcw() const noexcept { return fCcw; }

  bool isXor() const noexcept { return fXor; }

  void joinSegments() noexcept {
    SkOpSegment* segment = &fHead;
    SkOpSegment* next;
    do {
      next = segment->next();
      segment->joinEnds(next ? next : &fHead);
    } while ((segment = next));
  }

  void markAllDone() {
    SkOpSegment* segment = &fHead;
    do {
      segment->markAllDone();
    } while ((segment = segment->next()));
  }

  // Please keep this aligned with debugMissingCoincidence()
  bool missingCoincidence() {
    SkASSERT(fCount > 0);
    SkOpSegment* segment = &fHead;
    bool result = false;
    do {
      if (segment->missingCoincidence()) {
        result = true;
      }
      segment = segment->next();
    } while (segment);
    return result;
  }

  bool moveMultiples() {
    SkASSERT(fCount > 0);
    SkOpSegment* segment = &fHead;
    do {
      if (!segment->moveMultiples()) {
        return false;
      }
    } while ((segment = segment->next()));
    return true;
  }

  bool moveNearby() {
    SkASSERT(fCount > 0);
    SkOpSegment* segment = &fHead;
    do {
      if (!segment->moveNearby()) {
        return false;
      }
    } while ((segment = segment->next()));
    return true;
  }

  SkOpContour* next() noexcept { return fNext; }

  const SkOpContour* next() const noexcept { return fNext; }

  bool operand() const noexcept { return fOperand; }

  bool oppXor() const noexcept { return fOppXor; }

  void outdentDump() const noexcept { SkDEBUGCODE(fDebugIndent -= 2); }

  void rayCheck(const SkOpRayHit& base, SkOpRayDir dir, SkOpRayHit** hits, SkArenaAlloc*);

  void reset() noexcept {
    fTail = nullptr;
    fNext = nullptr;
    fCount = 0;
    fDone = false;
    SkDEBUGCODE(fBounds.set(SK_ScalarMax, SK_ScalarMax, SK_ScalarMin, SK_ScalarMin));
    SkDEBUGCODE(fFirstSorted = -1);
    SkDEBUGCODE(fDebugIndent = 0);
  }

  void resetReverse() noexcept {
    SkOpContour* next = this;
    do {
      if (!next->count()) {
        continue;
      }
      next->fCcw = -1;
      next->fReverse = false;
    } while ((next = next->next()));
  }

  bool reversed() const noexcept { return fReverse; }

  void setBounds() noexcept {
    SkASSERT(fCount > 0);
    const SkOpSegment* segment = &fHead;
    fBounds = segment->bounds();
    while ((segment = segment->next())) {
      fBounds.add(segment->bounds());
    }
  }

  void setCcw(int ccw) noexcept { fCcw = ccw; }

  void setGlobalState(SkOpGlobalState* state) noexcept { fState = state; }

  void setNext(SkOpContour* contour) noexcept {
    //        SkASSERT(!fNext == !!contour);
    fNext = contour;
  }

  void setOperand(bool isOp) noexcept { fOperand = isOp; }

  void setOppXor(bool isOppXor) noexcept { fOppXor = isOppXor; }

  void setReverse() noexcept { fReverse = true; }

  void setXor(bool isXor) noexcept { fXor = isXor; }

  bool sortAngles() {
    SkASSERT(fCount > 0);
    SkOpSegment* segment = &fHead;
    do {
      FAIL_IF(!segment->sortAngles());
    } while ((segment = segment->next()));
    return true;
  }

  const SkPoint& start() const noexcept { return fHead.pts()[0]; }

  void toPartialBackward(SkPathWriter* path) const {
    const SkOpSegment* segment = fTail;
    do {
      SkAssertResult(segment->addCurveTo(segment->tail(), segment->head(), path));
    } while ((segment = segment->prev()));
  }

  void toPartialForward(SkPathWriter* path) const {
    const SkOpSegment* segment = &fHead;
    do {
      SkAssertResult(segment->addCurveTo(segment->head(), segment->tail(), path));
    } while ((segment = segment->next()));
  }

  void toReversePath(SkPathWriter* path) const;
  void toPath(SkPathWriter* path) const;
  SkOpSpan* undoneSpan();

 protected:
  SkOpGlobalState* fState;
  SkOpSegment fHead;
  SkOpSegment* fTail;
  SkOpContour* fNext;
  SkPathOpsBounds fBounds;
  int fCcw;
  int fCount;
  int fFirstSorted;
  bool fDone;     // set by find top segment
  bool fOperand;  // true for the second argument to a binary operator
  bool fReverse;  // true if contour should be reverse written to path (used only by fix winding)
  bool fXor;      // set if original path had even-odd fill
  bool fOppXor;   // set if opposite path had even-odd fill
  SkDEBUGCODE(int fID);
  SkDEBUGCODE(mutable int fDebugIndent);
};

class SkOpContourHead : public SkOpContour {
 public:
  SkOpContour* appendContour() {
    SkOpContour* contour = this->globalState()->allocator()->make<SkOpContour>();
    contour->setNext(nullptr);
    SkOpContour* prev = this;
    SkOpContour* next;
    while ((next = prev->next())) {
      prev = next;
    }
    prev->setNext(contour);
    return contour;
  }

  void joinAllSegments() noexcept {
    SkOpContour* next = this;
    do {
      if (!next->count()) {
        continue;
      }
      next->joinSegments();
    } while ((next = next->next()));
  }

  void remove(SkOpContour* contour) noexcept {
    if (contour == this) {
      SkASSERT(this->count() == 0);
      return;
    }
    SkASSERT(contour->next() == nullptr);
    SkOpContour* prev = this;
    SkOpContour* next;
    while ((next = prev->next()) != contour) {
      SkASSERT(next);
      prev = next;
    }
    SkASSERT(prev);
    prev->setNext(nullptr);
  }
};

class SkOpContourBuilder {
 public:
  SkOpContourBuilder(SkOpContour* contour) noexcept : fContour(contour), fLastIsLine(false) {}

  void addConic(SkPoint pts[3], SkScalar weight);
  void addCubic(SkPoint pts[4]);
  void addCurve(SkPath::Verb verb, const SkPoint pts[4], SkScalar weight = 1);
  void addLine(const SkPoint pts[2]);
  void addQuad(SkPoint pts[3]);
  void flush();
  SkOpContour* contour() noexcept { return fContour; }
  void setContour(SkOpContour* contour) {
    flush();
    fContour = contour;
  }

 protected:
  SkOpContour* fContour;
  SkPoint fLastLine[2];
  bool fLastIsLine;
};

#endif
