

/**************************************************************************************************
 *** This file was autogenerated from GrNestedChildProcessors.fp; do not modify.
 **************************************************************************************************/
#ifndef GrNestedChildProcessors_DEFINED
#define GrNestedChildProcessors_DEFINED

#include "include/core/SkM44.h"
#include "include/core/SkTypes.h"

#include "src/gpu/GrFragmentProcessor.h"

class GrNestedChildProcessors : public GrFragmentProcessor {
 public:
  static std::unique_ptr<GrFragmentProcessor> Make(
      std::unique_ptr<GrFragmentProcessor> child1, std::unique_ptr<GrFragmentProcessor> child2) {
    return std::unique_ptr<GrFragmentProcessor>(
        new GrNestedChildProcessors(std::move(child1), std::move(child2)));
  }
  GrNestedChildProcessors(const GrNestedChildProcessors& src);
  std::unique_ptr<GrFragmentProcessor> clone() const override;
  const char* name() const override { return "NestedChildProcessors"; }

 private:
  GrNestedChildProcessors(
      std::unique_ptr<GrFragmentProcessor> child1, std::unique_ptr<GrFragmentProcessor> child2)
      : INHERITED(kGrNestedChildProcessors_ClassID, kNone_OptimizationFlags) {
    this->registerChild(std::move(child1), SkSL::SampleUsage::PassThrough());
    this->registerChild(std::move(child2), SkSL::SampleUsage::PassThrough());
  }
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
