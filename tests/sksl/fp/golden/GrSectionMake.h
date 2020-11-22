

/**************************************************************************************************
 *** This file was autogenerated from GrSectionMake.fp; do not modify.
 **************************************************************************************************/
#ifndef GrSectionMake_DEFINED
#define GrSectionMake_DEFINED

#include "include/core/SkM44.h"
#include "include/core/SkTypes.h"

#include "src/gpu/GrFragmentProcessor.h"

class GrSectionMake : public GrFragmentProcessor {
 public:
  make section GrSectionMake(const GrSectionMake& src);
  std::unique_ptr<GrFragmentProcessor> clone() const override;
  const char* name() const override { return "SectionMake"; }
  bool usesExplicitReturn() const override;

 private:
  GrSectionMake() : INHERITED(kGrSectionMake_ClassID, kNone_OptimizationFlags) {}
  GrGLSLFragmentProcessor* onCreateGLSLInstance() const override;
  void onGetGLSLProcessorKey(const GrShaderCaps&, GrProcessorKeyBuilder*) const override;
  bool onIsEqual(const GrFragmentProcessor&) const override;
#if GR_TEST_UTILS
  SkString onDumpInfo() const override;
#endif
  GR_DECLARE_FRAGMENT_PROCESSOR_TEST
  using INHERITED = GrFragmentProcessor;
};
#endif
