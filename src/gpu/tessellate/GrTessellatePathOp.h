/*
 * Copyright 2019 Google LLC.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef GrTessellatePathOp_DEFINED
#define GrTessellatePathOp_DEFINED

#include "src/gpu/ops/GrDrawOp.h"

class GrAppliedHardClip;
class GrFillPathShader;
class GrStencilPathShader;

// Renders paths using the classic Red Book "stencil, then cover" method. Curves get linearized by
// GPU tessellation shaders. This Op doesn't apply analytic AA, so it requires a render target that
// supports either MSAA or mixed samples if AA is desired.
class GrTessellatePathOp : public GrDrawOp {
 public:
  enum class Flags { kNone = 0, kStencilOnly = (1 << 0), kWireframe = (1 << 1) };

 private:
  DEFINE_OP_CLASS_ID

  GrTessellatePathOp(
      const SkMatrix& viewMatrix, const SkPath& path, GrPaint&& paint, GrAAType aaType,
      Flags flags = Flags::kNone)
      : GrDrawOp(ClassID()),
        fFlags(flags),
        fViewMatrix(viewMatrix),
        fPath(path),
        fAAType(aaType),
        fColor(paint.getColor4f()),
        fProcessors(std::move(paint)) {
    SkRect devBounds;
    fViewMatrix.mapRect(&devBounds, path.getBounds());
    this->setBounds(devBounds, HasAABloat(GrAAType::kCoverage == fAAType), GrOp::IsHairline::kNo);
  }

  const char* name() const override { return "GrTessellatePathOp"; }
  void visitProxies(const VisitProxyFunc& fn) const override { fProcessors.visitProxies(fn); }
  GrProcessorSet::Analysis finalize(
      const GrCaps& caps, const GrAppliedClip* clip, bool hasMixedSampledCoverage,
      GrClampType clampType) override {
    return fProcessors.finalize(
        fColor, GrProcessorAnalysisCoverage::kNone, clip, &GrUserStencilSettings::kUnused,
        hasMixedSampledCoverage, caps, clampType, &fColor);
  }

  FixedFunctionFlags fixedFunctionFlags() const override;
  void onPrepare(GrOpFlushState* state) override;
  void onExecute(GrOpFlushState*, const SkRect& chainBounds) override;

  void drawStencilPass(
      GrOpFlushState*, const GrAppliedHardClip&, const GrPipeline::FixedDynamicState*);
  void drawCoverPass(GrOpFlushState*, GrAppliedClip&&, const GrPipeline::FixedDynamicState*);

  const Flags fFlags;
  const SkMatrix fViewMatrix;
  const SkPath fPath;
  const GrAAType fAAType;
  SkPMColor4f fColor;
  GrProcessorSet fProcessors;

  // These path shaders get created during onPrepare for drawing the below path vertex data.
  //
  // If fFillPathShader is null, then we just stencil the full path using fStencilPathShader and
  // fCubicInstanceBuffer, and then fill it using a simple bounding box.
  //
  // If fFillPathShader is not null, then we fill the path using it plus cubic hulls from
  // fCubicInstanceBuffer instead of a bounding box.
  //
  // If fFillPathShader is not null and fStencilPathShader *is* null, then the vertex data
  // contains non-overlapping path geometry that can be drawn directly to the final render target.
  // We only need to stencil curves from fCubicInstanceBuffer, and then draw the rest of the path
  // directly.
  GrStencilPathShader* fStencilPathShader = nullptr;
  GrFillPathShader* fFillPathShader = nullptr;

  // The "path vertex data" is made up of cubic wedges or inner polygon triangles (either red book
  // style or fully tessellated). The geometry is generated by
  // GrPathParser::EmitCenterWedgePatches, GrPathParser::EmitInnerPolygonTriangles,
  // or GrTessellator::PathToTriangles.
  sk_sp<const GrBuffer> fPathVertexBuffer;
  int fBasePathVertex;
  int fPathVertexCount;

  // The cubic instance buffer defines standalone cubics to tessellate into the stencil buffer, in
  // addition to the above path geometry.
  sk_sp<const GrBuffer> fCubicInstanceBuffer;
  int fBaseCubicInstance;
  int fCubicInstanceCount;

  friend class GrOpMemoryPool;  // For ctor.
};

GR_MAKE_BITFIELD_CLASS_OPS(GrTessellatePathOp::Flags);

#endif
