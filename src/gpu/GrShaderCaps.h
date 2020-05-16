/*
 * Copyright 2012 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef GrShaderCaps_DEFINED
#define GrShaderCaps_DEFINED

#include "include/core/SkRefCnt.h"
#include "include/private/GrTypesPriv.h"
#include "src/gpu/GrSwizzle.h"
#include "src/gpu/glsl/GrGLSL.h"

namespace SkSL {
class ShaderCapsFactory;
}

struct GrContextOptions;
class SkJSONWriter;

class GrShaderCaps : public SkRefCnt {
 public:
  /**
   * Indicates how GLSL must interact with advanced blend equations. The KHR extension requires
   * special layout qualifiers in the fragment shader.
   */
  enum AdvBlendEqInteraction {
    kNotSupported_AdvBlendEqInteraction,     //<! No _blend_equation_advanced extension
    kAutomatic_AdvBlendEqInteraction,        //<! No interaction required
    kGeneralEnable_AdvBlendEqInteraction,    //<! layout(blend_support_all_equations) out
    kSpecificEnables_AdvBlendEqInteraction,  //<! Specific layout qualifiers per equation

    kLast_AdvBlendEqInteraction = kSpecificEnables_AdvBlendEqInteraction
  };

  GrShaderCaps(const GrContextOptions&);

  void dumpJSON(SkJSONWriter*) const;

  bool supportsDistanceFieldText() const noexcept { return fShaderDerivativeSupport; }

  bool shaderDerivativeSupport() const noexcept { return fShaderDerivativeSupport; }
  bool geometryShaderSupport() const noexcept { return fGeometryShaderSupport; }
  bool gsInvocationsSupport() const noexcept { return fGSInvocationsSupport; }
  bool pathRenderingSupport() const noexcept { return fPathRenderingSupport; }
  bool dstReadInShaderSupport() const noexcept { return fDstReadInShaderSupport; }
  bool dualSourceBlendingSupport() const noexcept { return fDualSourceBlendingSupport; }
  bool integerSupport() const noexcept { return fIntegerSupport; }

  /**
   * Some helper functions for encapsulating various extensions to read FB Buffer on openglES
   *
   * TODO(joshualitt) On desktop opengl 4.2+ we can achieve something similar to this effect
   */
  bool fbFetchSupport() const noexcept { return fFBFetchSupport; }

  bool fbFetchNeedsCustomOutput() const noexcept { return fFBFetchNeedsCustomOutput; }

  const char* versionDeclString() const noexcept { return fVersionDeclString; }

  const char* fbFetchColorName() const noexcept { return fFBFetchColorName; }

  const char* fbFetchExtensionString() const noexcept { return fFBFetchExtensionString; }

  bool flatInterpolationSupport() const noexcept { return fFlatInterpolationSupport; }

  bool preferFlatInterpolation() const noexcept { return fPreferFlatInterpolation; }

  bool noperspectiveInterpolationSupport() const noexcept {
    return fNoPerspectiveInterpolationSupport;
  }

  bool sampleMaskSupport() const noexcept { return fSampleMaskSupport; }

  bool tessellationSupport() const noexcept { return fTessellationSupport; }

  bool externalTextureSupport() const noexcept { return fExternalTextureSupport; }

  bool vertexIDSupport() const noexcept { return fVertexIDSupport; }

  // frexp, ldexp, etc.
  bool fpManipulationSupport() const noexcept { return fFPManipulationSupport; }

  bool floatIs32Bits() const noexcept { return fFloatIs32Bits; }

  bool halfIs32Bits() const noexcept { return fHalfIs32Bits; }

  bool hasLowFragmentPrecision() const noexcept { return fHasLowFragmentPrecision; }

  // SkSL only.
  bool builtinFMASupport() const noexcept { return fBuiltinFMASupport; }

  AdvBlendEqInteraction advBlendEqInteraction() const noexcept { return fAdvBlendEqInteraction; }

  bool mustEnableAdvBlendEqs() const noexcept {
    return fAdvBlendEqInteraction >= kGeneralEnable_AdvBlendEqInteraction;
  }

  bool mustEnableSpecificAdvBlendEqs() const noexcept {
    return fAdvBlendEqInteraction == kSpecificEnables_AdvBlendEqInteraction;
  }

  bool mustDeclareFragmentShaderOutput() const noexcept {
    return fGLSLGeneration > k110_GrGLSLGeneration;
  }

  bool usesPrecisionModifiers() const noexcept { return fUsesPrecisionModifiers; }

  // Returns whether we can use the glsl function any() in our shader code.
  bool canUseAnyFunctionInShader() const noexcept { return fCanUseAnyFunctionInShader; }

  bool canUseMinAndAbsTogether() const noexcept { return fCanUseMinAndAbsTogether; }

  bool canUseFractForNegativeValues() const noexcept { return fCanUseFractForNegativeValues; }

  bool mustForceNegatedAtanParamToFloat() const noexcept {
    return fMustForceNegatedAtanParamToFloat;
  }

  // Returns whether a device incorrectly implements atan(y,x) as atan(y/x)
  bool atan2ImplementedAsAtanYOverX() const noexcept { return fAtan2ImplementedAsAtanYOverX; }

  // If this returns true some operation (could be a no op) must be called between floor and abs
  // to make sure the driver compiler doesn't inline them together which can cause a driver bug in
  // the shader.
  bool mustDoOpBetweenFloorAndAbs() const noexcept { return fMustDoOpBetweenFloorAndAbs; }

  // If false, SkSL uses a workaround so that sk_FragCoord doesn't actually query gl_FragCoord
  bool canUseFragCoord() const noexcept { return fCanUseFragCoord; }

  // If true, short ints can't represent every integer in the 16-bit two's complement range as
  // required by the spec. SKSL will always emit full ints.
  bool incompleteShortIntPrecision() const noexcept { return fIncompleteShortIntPrecision; }

  bool colorSpaceMathNeedsFloat() const noexcept { return fColorSpaceMathNeedsFloat; }

  // If true, then conditions in for loops need "&& true" to work around driver bugs.
  bool addAndTrueToLoopCondition() const noexcept { return fAddAndTrueToLoopCondition; }

  // If true, then expressions such as "x && y" or "x || y" are rewritten as
  // ternary to work around driver bugs.
  bool unfoldShortCircuitAsTernary() const noexcept { return fUnfoldShortCircuitAsTernary; }

  bool emulateAbsIntFunction() const noexcept { return fEmulateAbsIntFunction; }

  bool rewriteDoWhileLoops() const noexcept { return fRewriteDoWhileLoops; }

  bool removePowWithConstantExponent() const noexcept { return fRemovePowWithConstantExponent; }

  bool requiresLocalOutputColorForFBFetch() const noexcept {
    return fRequiresLocalOutputColorForFBFetch;
  }

  bool mustObfuscateUniformColor() const noexcept { return fMustObfuscateUniformColor; }

  // The D3D shader compiler, when targeting PS 3.0 (ie within ANGLE) fails to compile certain
  // constructs. See detailed comments in GrGLCaps.cpp.
  bool mustGuardDivisionEvenAfterExplicitZeroCheck() const noexcept {
    return fMustGuardDivisionEvenAfterExplicitZeroCheck;
  }

  // On Pixel 3, 3a, and 4 devices we've noticed that the simple function:
  // half4 blend(half4 a, half4 b) { return a.a * b; }
  // may return (0, 0, 0, 1) when b is (0, 0, 0, 0).
  bool inBlendModesFailRandomlyForAllZeroVec() const noexcept {
    return fInBlendModesFailRandomlyForAllZeroVec;
  }

  // On Nexus 6, the GL context can get lost if a shader does not write a value to gl_FragColor.
  // https://bugs.chromium.org/p/chromium/issues/detail?id=445377
  bool mustWriteToFragColor() const noexcept { return fMustWriteToFragColor; }

  // The Android emulator claims samplerExternalOES is an unknown type if a default precision
  // statement is made for the type.
  bool noDefaultPrecisionForExternalSamplers() const noexcept {
    return fNoDefaultPrecisionForExternalSamplers;
  }

  // The sample mask round rect op draws nothing on several Adreno and Radeon bots. Other ops that
  // use sample mask while rendering to stencil seem to work fine.
  // http://skbug.com/8921
  bool canOnlyUseSampleMaskWithStencil() const noexcept { return fCanOnlyUseSampleMaskWithStencil; }

  // Returns the string of an extension that must be enabled in the shader to support
  // derivatives. If nullptr is returned then no extension needs to be enabled. Before calling
  // this function, the caller should check that shaderDerivativeSupport exists.
  const char* shaderDerivativeExtensionString() const noexcept {
    SkASSERT(this->shaderDerivativeSupport());
    return fShaderDerivativeExtensionString;
  }

  // Returns the string of an extension that must be enabled in the shader to support geometry
  // shaders. If nullptr is returned then no extension needs to be enabled. Before calling this
  // function, the caller must verify that geometryShaderSupport exists.
  const char* geometryShaderExtensionString() const noexcept {
    SkASSERT(this->geometryShaderSupport());
    return fGeometryShaderExtensionString;
  }

  // Returns the string of an extension that must be enabled in the shader to support
  // geometry shader invocations. If nullptr is returned then no extension needs to be enabled.
  // Before calling this function, the caller must verify that gsInvocationsSupport exists.
  const char* gsInvocationsExtensionString() const noexcept {
    SkASSERT(this->gsInvocationsSupport());
    return fGSInvocationsExtensionString;
  }

  // Returns the string of an extension that will do all necessary coord transfomations needed
  // when reading the fragment position. If such an extension does not exisits, this function
  // returns a nullptr, and all transforms of the frag position must be done manually in the
  // shader.
  const char* fragCoordConventionsExtensionString() const noexcept {
    return fFragCoordConventionsExtensionString;
  }

  // This returns the name of an extension that must be enabled in the shader, if such a thing is
  // required in order to use a secondary output in the shader. This returns a nullptr if no such
  // extension is required. However, the return value of this function does not say whether dual
  // source blending is supported.
  const char* secondaryOutputExtensionString() const noexcept {
    return fSecondaryOutputExtensionString;
  }

  // This returns the name of an extension that must be enabled in the shader to support external
  // textures. In some cases, two extensions must be enabled - the second extension is returned
  // by secondExternalTextureExtensionString(). If that function returns nullptr, then only one
  // extension is required.
  const char* externalTextureExtensionString() const noexcept {
    SkASSERT(this->externalTextureSupport());
    return fExternalTextureExtensionString;
  }

  const char* secondExternalTextureExtensionString() const noexcept {
    SkASSERT(this->externalTextureSupport());
    return fSecondExternalTextureExtensionString;
  }

  const char* noperspectiveInterpolationExtensionString() const noexcept {
    SkASSERT(this->noperspectiveInterpolationSupport());
    return fNoPerspectiveInterpolationExtensionString;
  }

  const char* sampleVariablesExtensionString() const noexcept {
    SkASSERT(this->sampleMaskSupport());
    return fSampleVariablesExtensionString;
  }

  const char* tessellationExtensionString() const noexcept {
    SkASSERT(this->tessellationSupport());
    return fTessellationExtensionString;
  }

  int maxFragmentSamplers() const noexcept { return fMaxFragmentSamplers; }

  bool textureSwizzleAppliedInShader() const noexcept { return fTextureSwizzleAppliedInShader; }

  GrGLSLGeneration generation() const noexcept { return fGLSLGeneration; }

 private:
  void applyOptionsOverrides(const GrContextOptions& options);

  GrGLSLGeneration fGLSLGeneration;

  bool fShaderDerivativeSupport : 1;
  bool fGeometryShaderSupport : 1;
  bool fGSInvocationsSupport : 1;
  bool fPathRenderingSupport : 1;
  bool fDstReadInShaderSupport : 1;
  bool fDualSourceBlendingSupport : 1;
  bool fIntegerSupport : 1;
  bool fFBFetchSupport : 1;
  bool fFBFetchNeedsCustomOutput : 1;
  bool fUsesPrecisionModifiers : 1;
  bool fFlatInterpolationSupport : 1;
  bool fPreferFlatInterpolation : 1;
  bool fNoPerspectiveInterpolationSupport : 1;
  bool fSampleMaskSupport : 1;
  bool fTessellationSupport : 1;
  bool fExternalTextureSupport : 1;
  bool fVertexIDSupport : 1;
  bool fFPManipulationSupport : 1;
  bool fFloatIs32Bits : 1;
  bool fHalfIs32Bits : 1;
  bool fHasLowFragmentPrecision : 1;
  bool fTextureSwizzleAppliedInShader : 1;

  // Used by SkSL to know when to generate polyfills.
  bool fBuiltinFMASupport : 1;

  // Used for specific driver bug work arounds
  bool fCanUseAnyFunctionInShader : 1;
  bool fCanUseMinAndAbsTogether : 1;
  bool fCanUseFractForNegativeValues : 1;
  bool fMustForceNegatedAtanParamToFloat : 1;
  bool fAtan2ImplementedAsAtanYOverX : 1;
  bool fMustDoOpBetweenFloorAndAbs : 1;
  bool fRequiresLocalOutputColorForFBFetch : 1;
  bool fMustObfuscateUniformColor : 1;
  bool fMustGuardDivisionEvenAfterExplicitZeroCheck : 1;
  bool fInBlendModesFailRandomlyForAllZeroVec : 1;
  bool fCanUseFragCoord : 1;
  bool fIncompleteShortIntPrecision : 1;
  bool fAddAndTrueToLoopCondition : 1;
  bool fUnfoldShortCircuitAsTernary : 1;
  bool fEmulateAbsIntFunction : 1;
  bool fRewriteDoWhileLoops : 1;
  bool fRemovePowWithConstantExponent : 1;
  bool fMustWriteToFragColor : 1;
  bool fNoDefaultPrecisionForExternalSamplers : 1;
  bool fCanOnlyUseSampleMaskWithStencil : 1;
  bool fColorSpaceMathNeedsFloat : 1;

  const char* fVersionDeclString;

  const char* fShaderDerivativeExtensionString;
  const char* fGeometryShaderExtensionString;
  const char* fGSInvocationsExtensionString;
  const char* fFragCoordConventionsExtensionString;
  const char* fSecondaryOutputExtensionString;
  const char* fExternalTextureExtensionString;
  const char* fSecondExternalTextureExtensionString;
  const char* fNoPerspectiveInterpolationExtensionString;
  const char* fSampleVariablesExtensionString;
  const char* fTessellationExtensionString;

  const char* fFBFetchColorName;
  const char* fFBFetchExtensionString;

  int fMaxFragmentSamplers;

  AdvBlendEqInteraction fAdvBlendEqInteraction;

  friend class GrCaps;  // For initialization.
  friend class GrDawnCaps;
  friend class GrD3DCaps;
  friend class GrGLCaps;
  friend class GrMockCaps;
  friend class GrMtlCaps;
  friend class GrVkCaps;
  friend class SkSL::ShaderCapsFactory;
};

#endif
