/*
 * Copyright 2017 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef GrProcessorSet_DEFINED
#define GrProcessorSet_DEFINED

#include "include/private/SkTemplates.h"
#include "src/gpu/GrFragmentProcessor.h"
#include "src/gpu/GrPaint.h"
#include "src/gpu/GrProcessorAnalysis.h"
#include "src/gpu/GrXferProcessor.h"

struct GrUserStencilSettings;
class GrAppliedClip;
class GrXPFactory;

class GrProcessorSet {
 private:
  // Arbitrary constructor arg for empty set and analysis
  enum class Empty { kEmpty };

 public:
  GrProcessorSet(GrPaint&&) noexcept;
  GrProcessorSet(SkBlendMode) noexcept;
  GrProcessorSet(std::unique_ptr<GrFragmentProcessor> colorFP) noexcept;
  GrProcessorSet(GrProcessorSet&&) noexcept;
  GrProcessorSet(const GrProcessorSet&) = delete;
  GrProcessorSet& operator=(const GrProcessorSet&) = delete;

  ~GrProcessorSet();

  int numColorFragmentProcessors() const noexcept { return fColorFragmentProcessorCnt; }
  int numCoverageFragmentProcessors() const noexcept {
    return this->numFragmentProcessors() - fColorFragmentProcessorCnt;
  }

  const GrFragmentProcessor* colorFragmentProcessor(int idx) const noexcept {
    SkASSERT(idx < fColorFragmentProcessorCnt);
    return fFragmentProcessors[idx + fFragmentProcessorOffset].get();
  }
  const GrFragmentProcessor* coverageFragmentProcessor(int idx) const noexcept {
    return fFragmentProcessors[idx + fColorFragmentProcessorCnt + fFragmentProcessorOffset].get();
  }

  const GrXferProcessor* xferProcessor() const noexcept {
    SkASSERT(this->isFinalized());
    return fXP.fProcessor;
  }
  sk_sp<const GrXferProcessor> refXferProcessor() const noexcept {
    SkASSERT(this->isFinalized());
    return sk_ref_sp(fXP.fProcessor);
  }

  std::unique_ptr<const GrFragmentProcessor> detachColorFragmentProcessor(int idx) noexcept {
    SkASSERT(idx < fColorFragmentProcessorCnt);
    return std::move(fFragmentProcessors[idx + fFragmentProcessorOffset]);
  }

  std::unique_ptr<const GrFragmentProcessor> detachCoverageFragmentProcessor(int idx) noexcept {
    return std::move(
        fFragmentProcessors[idx + fFragmentProcessorOffset + fColorFragmentProcessorCnt]);
  }

  /** Comparisons are only legal on finalized processor sets. */
  bool operator==(const GrProcessorSet& that) const;
  bool operator!=(const GrProcessorSet& that) const { return !(*this == that); }

  /**
   * This is used to report results of processor analysis when a processor set is finalized (see
   * below).
   */
  class Analysis {
   public:
    Analysis(const Analysis&) = default;
    Analysis() { *reinterpret_cast<uint32_t*>(this) = 0; }

    bool isInitialized() const noexcept { return fIsInitialized; }
    bool usesLocalCoords() const noexcept { return fUsesLocalCoords; }
    bool requiresDstTexture() const noexcept { return fRequiresDstTexture; }
    bool requiresNonOverlappingDraws() const noexcept { return fRequiresNonOverlappingDraws; }
    bool isCompatibleWithCoverageAsAlpha() const noexcept { return fCompatibleWithCoverageAsAlpha; }
    // Indicates whether all color fragment processors were eliminated in the analysis.
    bool hasColorFragmentProcessor() const noexcept { return fHasColorFragmentProcessor; }

    bool inputColorIsIgnored() const noexcept { return fInputColorType == kIgnored_InputColorType; }
    bool inputColorIsOverridden() const noexcept {
      return fInputColorType == kOverridden_InputColorType;
    }

   private:
    constexpr Analysis(Empty) noexcept
        : fUsesLocalCoords(false),
          fCompatibleWithCoverageAsAlpha(true),
          fRequiresDstTexture(false),
          fRequiresNonOverlappingDraws(false),
          fHasColorFragmentProcessor(false),
          fIsInitialized(true),
          fInputColorType(kOriginal_InputColorType) {}
    enum InputColorType : uint32_t {
      kOriginal_InputColorType,
      kOverridden_InputColorType,
      kIgnored_InputColorType
    };

    // MSVS 2015 won't pack different underlying types
    using PackedBool = uint32_t;
    using PackedInputColorType = uint32_t;

    PackedBool fUsesLocalCoords : 1;
    PackedBool fCompatibleWithCoverageAsAlpha : 1;
    PackedBool fRequiresDstTexture : 1;
    PackedBool fRequiresNonOverlappingDraws : 1;
    PackedBool fHasColorFragmentProcessor : 1;
    PackedBool fIsInitialized : 1;
    PackedInputColorType fInputColorType : 2;

    friend class GrProcessorSet;
  };
  static_assert(sizeof(Analysis) <= sizeof(uint32_t));

  /**
   * This analyzes the processors given an op's input color and coverage as well as a clip. The
   * state of the processor set may change to an equivalent but more optimal set of processors.
   * This new state requires that the caller respect the returned 'inputColorOverride'. This is
   * indicated by the returned Analysis's inputColorIsOverridden(). 'inputColorOverride' will not
   * be written if the analysis does not override the input color.
   *
   * This must be called before the processor set is used to construct a GrPipeline and may only
   * be called once.
   *
   * This also puts the processors in "pending execution" state and must be called when an op
   * that owns a processor set is recorded to ensure pending and writes are propagated to
   * resources referred to by the processors. Otherwise, data hazards may occur.
   */
  Analysis finalize(
      const GrProcessorAnalysisColor&, const GrProcessorAnalysisCoverage, const GrAppliedClip*,
      const GrUserStencilSettings*, bool hasMixedSampledCoverage, const GrCaps&, GrClampType,
      SkPMColor4f* inputColorOverride);

  bool isFinalized() const noexcept { return SkToBool(kFinalized_Flag & fFlags); }

  /** These are valid only for non-LCD coverage. */
  static const GrProcessorSet& EmptySet() noexcept;
  static GrProcessorSet MakeEmptySet() noexcept;
  static constexpr const Analysis EmptySetAnalysis() { return Analysis(Empty::kEmpty); }

#ifdef SK_DEBUG
  SkString dumpProcessors() const;
#endif

  void visitProxies(const GrOp::VisitProxyFunc& func) const;

 private:
  GrProcessorSet(Empty) noexcept : fXP((const GrXferProcessor*)nullptr), fFlags(kFinalized_Flag) {}

  int numFragmentProcessors() const noexcept {
    return fFragmentProcessors.count() - fFragmentProcessorOffset;
  }

  const GrFragmentProcessor* fragmentProcessor(int idx) const noexcept {
    return fFragmentProcessors[idx + fFragmentProcessorOffset].get();
  }

  // This absurdly large limit allows Analysis and this to pack fields together.
  static constexpr int kMaxColorProcessors = UINT8_MAX;

  enum Flags : uint16_t { kFinalized_Flag = 0x1 };

  union XP {
    XP(const GrXPFactory* factory) noexcept : fFactory(factory) {}
    XP(const GrXferProcessor* processor) noexcept : fProcessor(processor) {}
    explicit XP(XP&& that) noexcept : fProcessor(that.fProcessor) {
      SkASSERT(fProcessor == that.fProcessor);
      that.fProcessor = nullptr;
    }
    const GrXPFactory* fFactory;
    const GrXferProcessor* fProcessor;
  };

  const GrXPFactory* xpFactory() const noexcept {
    SkASSERT(!this->isFinalized());
    return fXP.fFactory;
  }

  SkAutoSTArray<4, std::unique_ptr<GrFragmentProcessor>> fFragmentProcessors;
  XP fXP;
  uint8_t fColorFragmentProcessorCnt = 0;
  uint8_t fFragmentProcessorOffset = 0;
  uint8_t fFlags;
};

#endif
