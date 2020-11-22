

/**************************************************************************************************
 *** This file was autogenerated from GrChildProcessorSampleMatrixSingleInUniform.fp; do not modify.
 **************************************************************************************************/
#ifndef GrChildProcessorSampleMatrixSingleInUniform_DEFINED
#define GrChildProcessorSampleMatrixSingleInUniform_DEFINED

#include "include/core/SkM44.h"
#include "include/core/SkTypes.h"

#include "src/gpu/GrFragmentProcessor.h"

class GrChildProcessorSampleMatrixSingleInUniform : public GrFragmentProcessor {
 public:
  static std::unique_ptr<GrFragmentProcessor> Make(
      std::unique_ptr<GrFragmentProcessor> child, SkMatrix matrix) {
    return std::unique_ptr<GrFragmentProcessor>(
        new GrChildProcessorSampleMatrixSingleInUniform(std::move(child), matrix));
  }
  GrChildProcessorSampleMatrixSingleInUniform(
      const GrChildProcessorSampleMatrixSingleInUniform& src);
  std::unique_ptr<GrFragmentProcessor> clone() const override;
  const char* name() const override { return "ChildProcessorSampleMatrixSingleInUniform"; }
  bool usesExplicitReturn() const override;
  SkMatrix matrix;

 private:
  GrChildProcessorSampleMatrixSingleInUniform(
      std::unique_ptr<GrFragmentProcessor> child, SkMatrix matrix)
      : INHERITED(kGrChildProcessorSampleMatrixSingleInUniform_ClassID, kNone_OptimizationFlags),
        matrix(matrix) {
    this->registerChild(
        std::move(child), SkSL::SampleUsage::UniformMatrix("matrix", matrix.hasPerspective()));
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