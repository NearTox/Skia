/*
 * Copyright 2014 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "src/gpu/gl/builders/GrGLProgramBuilder.h"

#include "include/gpu/GrDirectContext.h"
#include "src/core/SkATrace.h"
#include "src/core/SkAutoMalloc.h"
#include "src/core/SkReadBuffer.h"
#include "src/core/SkTraceEvent.h"
#include "src/core/SkWriteBuffer.h"
#include "src/gpu/GrAutoLocaleSetter.h"
#include "src/gpu/GrDirectContextPriv.h"
#include "src/gpu/GrPersistentCacheUtils.h"
#include "src/gpu/GrProgramDesc.h"
#include "src/gpu/GrShaderCaps.h"
#include "src/gpu/GrShaderUtils.h"
#include "src/gpu/GrSwizzle.h"
#include "src/gpu/gl/GrGLGpu.h"
#include "src/gpu/gl/GrGLProgram.h"
#include "src/gpu/gl/builders/GrGLProgramBuilder.h"

#include <memory>
#include "src/gpu/gl/builders/GrGLShaderStringBuilder.h"
#include "src/gpu/glsl/GrGLSLFragmentProcessor.h"
#include "src/gpu/glsl/GrGLSLGeometryProcessor.h"
#include "src/gpu/glsl/GrGLSLProgramDataManager.h"
#include "src/gpu/glsl/GrGLSLXferProcessor.h"

#define GL_CALL(X) GR_GL_CALL(this->gpu()->glInterface(), X)
#define GL_CALL_RET(R, X) GR_GL_CALL_RET(this->gpu()->glInterface(), R, X)

static void cleanup_shaders(GrGLGpu* gpu, const SkTDArray<GrGLuint>& shaderIDs) {
  for (int i = 0; i < shaderIDs.count(); ++i) {
    GR_GL_CALL(gpu->glInterface(), DeleteShader(shaderIDs[i]));
  }
}

static void cleanup_program(
    GrGLGpu* gpu, GrGLuint programID, const SkTDArray<GrGLuint>& shaderIDs) {
  GR_GL_CALL(gpu->glInterface(), DeleteProgram(programID));
  cleanup_shaders(gpu, shaderIDs);
}

sk_sp<GrGLProgram> GrGLProgramBuilder::CreateProgram(
    GrGLGpu* gpu, GrRenderTarget* renderTarget, const GrProgramDesc& desc,
    const GrProgramInfo& programInfo, const GrGLPrecompiledProgram* precompiledProgram) {
  ATRACE_ANDROID_FRAMEWORK_ALWAYS("shader_compile");
  GrAutoLocaleSetter als("C");

  // create a builder.  This will be handed off to effects so they can use it to add
  // uniforms, varyings, textures, etc
  GrGLProgramBuilder builder(gpu, renderTarget, desc, programInfo);

  auto persistentCache = gpu->getContext()->priv().getPersistentCache();
  if (persistentCache && !precompiledProgram) {
    sk_sp<SkData> key = SkData::MakeWithoutCopy(desc.asKey(), desc.keyLength());
    builder.fCached = persistentCache->load(*key);
    // the eventual end goal is to completely skip emitAndInstallProcs on a cache hit, but it's
    // doing necessary setup in addition to generating the SkSL code. Currently we are only able
    // to skip the SkSL->GLSL step on a cache hit.
  }
  if (!builder.emitAndInstallProcs()) {
    return nullptr;
  }
  return builder.finalize(precompiledProgram);
}

/////////////////////////////////////////////////////////////////////////////

GrGLProgramBuilder::GrGLProgramBuilder(
    GrGLGpu* gpu, GrRenderTarget* renderTarget, const GrProgramDesc& desc,
    const GrProgramInfo& programInfo)
    : INHERITED(renderTarget, desc, programInfo),
      fGpu(gpu),
      fVaryingHandler(this),
      fUniformHandler(this),
      fVertexAttributeCnt(0),
      fInstanceAttributeCnt(0),
      fVertexStride(0),
      fInstanceStride(0) {}

const GrCaps* GrGLProgramBuilder::caps() const { return fGpu->caps(); }

bool GrGLProgramBuilder::compileAndAttachShaders(
    const SkSL::String& glsl, GrGLuint programId, GrGLenum type, SkTDArray<GrGLuint>* shaderIds,
    GrContextOptions::ShaderErrorHandler* errHandler) {
  GrGLGpu* gpu = this->gpu();
  GrGLuint shaderId =
      GrGLCompileAndAttachShader(gpu->glContext(), programId, type, glsl, gpu->stats(), errHandler);
  if (!shaderId) {
    return false;
  }

  *shaderIds->append() = shaderId;
  return true;
}

void GrGLProgramBuilder::computeCountsAndStrides(
    GrGLuint programID, const GrPrimitiveProcessor& primProc, bool bindAttribLocations) {
  fVertexAttributeCnt = primProc.numVertexAttributes();
  fInstanceAttributeCnt = primProc.numInstanceAttributes();
  fAttributes =
      std::make_unique<GrGLProgram::Attribute[]>(fVertexAttributeCnt + fInstanceAttributeCnt);
  auto addAttr = [&](int i, const auto& a, size_t* stride) {
    fAttributes[i].fCPUType = a.cpuType();
    fAttributes[i].fGPUType = a.gpuType();
    fAttributes[i].fOffset = *stride;
    *stride += a.sizeAlign4();
    fAttributes[i].fLocation = i;
    if (bindAttribLocations) {
      GL_CALL(BindAttribLocation(programID, i, a.name()));
    }
  };
  fVertexStride = 0;
  int i = 0;
  for (const auto& attr : primProc.vertexAttributes()) {
    addAttr(i++, attr, &fVertexStride);
  }
  SkASSERT(fVertexStride == primProc.vertexStride());
  fInstanceStride = 0;
  for (const auto& attr : primProc.instanceAttributes()) {
    addAttr(i++, attr, &fInstanceStride);
  }
  SkASSERT(fInstanceStride == primProc.instanceStride());
}

void GrGLProgramBuilder::addInputVars(const SkSL::Program::Inputs& inputs) {
  if (inputs.fRTWidth) {
    this->addRTWidthUniform(SKSL_RTWIDTH_NAME);
  }
  if (inputs.fRTHeight) {
    this->addRTHeightUniform(SKSL_RTHEIGHT_NAME);
  }
}

static constexpr SkFourByteTag kSKSL_Tag = SkSetFourByteTag('S', 'K', 'S', 'L');
static constexpr SkFourByteTag kGLSL_Tag = SkSetFourByteTag('G', 'L', 'S', 'L');
static constexpr SkFourByteTag kGLPB_Tag = SkSetFourByteTag('G', 'L', 'P', 'B');

void GrGLProgramBuilder::storeShaderInCache(
    const SkSL::Program::Inputs& inputs, GrGLuint programID, const SkSL::String shaders[],
    bool isSkSL, SkSL::Program::Settings* settings) {
  if (!this->gpu()->getContext()->priv().getPersistentCache()) {
    return;
  }
  sk_sp<SkData> key = SkData::MakeWithoutCopy(this->desc().asKey(), this->desc().keyLength());
  if (fGpu->glCaps().programBinarySupport()) {
    // binary cache
    GrGLsizei length = 0;
    GL_CALL(GetProgramiv(programID, GL_PROGRAM_BINARY_LENGTH, &length));
    if (length > 0) {
      SkBinaryWriteBuffer writer;
      writer.writeInt(GrPersistentCacheUtils::kCurrentVersion);
      writer.writeUInt(kGLPB_Tag);

      writer.writePad32(&inputs, sizeof(inputs));

      SkAutoSMalloc<2048> binary(length);
      GrGLenum binaryFormat;
      GL_CALL(GetProgramBinary(programID, length, &length, &binaryFormat, binary.get()));

      writer.writeUInt(binaryFormat);
      writer.writeInt(length);
      writer.writePad32(binary.get(), length);

      auto data = writer.snapshotAsData();
      this->gpu()->getContext()->priv().getPersistentCache()->store(*key, *data);
    }
  } else {
    // source cache, plus metadata to allow for a complete precompile
    GrPersistentCacheUtils::ShaderMetadata meta;
    meta.fSettings = settings;
    meta.fHasCustomColorOutput = fFS.hasCustomColorOutput();
    meta.fHasSecondaryColorOutput = fFS.hasSecondaryOutput();
    for (const auto& attr : this->primitiveProcessor().vertexAttributes()) {
      meta.fAttributeNames.emplace_back(attr.name());
    }
    for (const auto& attr : this->primitiveProcessor().instanceAttributes()) {
      meta.fAttributeNames.emplace_back(attr.name());
    }

    auto data = GrPersistentCacheUtils::PackCachedShaders(
        isSkSL ? kSKSL_Tag : kGLSL_Tag, shaders, &inputs, 1, &meta);
    this->gpu()->getContext()->priv().getPersistentCache()->store(*key, *data);
  }
}

sk_sp<GrGLProgram> GrGLProgramBuilder::finalize(const GrGLPrecompiledProgram* precompiledProgram) {
  TRACE_EVENT0("skia.gpu", TRACE_FUNC);

  // verify we can get a program id
  GrGLuint programID;
  if (precompiledProgram) {
    programID = precompiledProgram->fProgramID;
  } else {
    GL_CALL_RET(programID, CreateProgram());
  }
  if (0 == programID) {
    return nullptr;
  }

  if (this->gpu()->glCaps().programBinarySupport() &&
      this->gpu()->glCaps().programParameterSupport() &&
      this->gpu()->getContext()->priv().getPersistentCache() && !precompiledProgram) {
    GL_CALL(ProgramParameteri(programID, GR_GL_PROGRAM_BINARY_RETRIEVABLE_HINT, GR_GL_TRUE));
  }

  this->finalizeShaders();

  // compile shaders and bind attributes / uniforms
  auto errorHandler = this->gpu()->getContext()->priv().getShaderErrorHandler();
  const GrPrimitiveProcessor& primProc = this->primitiveProcessor();
  SkSL::Program::Settings settings;
  settings.fFlipY = this->origin() != kTopLeft_GrSurfaceOrigin;
  settings.fSharpenTextures = this->gpu()->getContext()->priv().options().fSharpenMipmappedTextures;
  settings.fFragColorIsInOut = this->fragColorIsInOut();

  SkSL::Program::Inputs inputs;
  SkTDArray<GrGLuint> shadersToDelete;

  bool checkLinked = !fGpu->glCaps().skipErrorChecks();

  bool cached = fCached.get() != nullptr;
  bool usedProgramBinaries = false;
  SkSL::String glsl[kGrShaderTypeCount];
  SkSL::String* sksl[kGrShaderTypeCount] = {
      &fVS.fCompilerString,
      &fGS.fCompilerString,
      &fFS.fCompilerString,
  };
  SkSL::String cached_sksl[kGrShaderTypeCount];
  if (precompiledProgram) {
    // This is very similar to when we get program binaries. We even set that flag, as it's
    // used to prevent other compile work later, and to force re-querying uniform locations.
    this->addInputVars(precompiledProgram->fInputs);
    this->computeCountsAndStrides(programID, primProc, false);
    usedProgramBinaries = true;
  } else if (cached) {
    ATRACE_ANDROID_FRAMEWORK_ALWAYS("cache_hit");
    SkReadBuffer reader(fCached->data(), fCached->size());
    SkFourByteTag shaderType = GrPersistentCacheUtils::GetType(&reader);

    switch (shaderType) {
      case kGLPB_Tag: {
        // Program binary cache hit. We may opt not to use this if we don't trust program
        // binaries on this driver
        if (!fGpu->glCaps().programBinarySupport()) {
          cached = false;
          break;
        }
        reader.readPad32(&inputs, sizeof(inputs));
        GrGLenum binaryFormat = reader.readUInt();
        GrGLsizei length = reader.readInt();
        const void* binary = reader.skip(length);
        if (!reader.isValid()) {
          break;
        }
        this->gpu()->clearErrorsAndCheckForOOM();
        GR_GL_CALL_NOERRCHECK(
            this->gpu()->glInterface(),
            ProgramBinary(programID, binaryFormat, const_cast<void*>(binary), length));
        if (this->gpu()->getErrorAndCheckForOOM() == GR_GL_NO_ERROR) {
          if (checkLinked) {
            cached = this->checkLinkStatus(programID, errorHandler, nullptr, nullptr);
          }
          if (cached) {
            this->addInputVars(inputs);
            this->computeCountsAndStrides(programID, primProc, false);
          }
        } else {
          cached = false;
        }
        usedProgramBinaries = cached;
        break;
      }

      case kGLSL_Tag:
        // Source cache hit, we don't need to compile the SkSL->GLSL
        GrPersistentCacheUtils::UnpackCachedShaders(&reader, glsl, &inputs, 1);
        break;

      case kSKSL_Tag:
        // SkSL cache hit, this should only happen in tools overriding the generated SkSL
        if (GrPersistentCacheUtils::UnpackCachedShaders(&reader, cached_sksl, &inputs, 1)) {
          for (int i = 0; i < kGrShaderTypeCount; ++i) {
            sksl[i] = &cached_sksl[i];
          }
        }
        break;

      default:
        // We got something invalid, so pretend it wasn't there
        reader.validate(false);
        break;
    }
    if (!reader.isValid()) {
      cached = false;
    }
  }
  if (!usedProgramBinaries) {
    ATRACE_ANDROID_FRAMEWORK_ALWAYS("cache_miss");
    // Either a cache miss, or we got something other than binaries from the cache

    /*
       Fragment Shader
    */
    if (glsl[kFragment_GrShaderType].empty()) {
      // Don't have cached GLSL, need to compile SkSL->GLSL
      if (fFS.fForceHighPrecision) {
        settings.fForceHighPrecision = true;
      }
      std::unique_ptr<SkSL::Program> fs = GrSkSLtoGLSL(
          gpu()->glContext(), SkSL::Program::kFragment_Kind, *sksl[kFragment_GrShaderType],
          settings, &glsl[kFragment_GrShaderType], errorHandler);
      if (!fs) {
        cleanup_program(fGpu, programID, shadersToDelete);
        return nullptr;
      }
      inputs = fs->fInputs;
    }

    this->addInputVars(inputs);
    if (!this->compileAndAttachShaders(
            glsl[kFragment_GrShaderType], programID, GR_GL_FRAGMENT_SHADER, &shadersToDelete,
            errorHandler)) {
      cleanup_program(fGpu, programID, shadersToDelete);
      return nullptr;
    }

    /*
       Vertex Shader
    */
    if (glsl[kVertex_GrShaderType].empty()) {
      // Don't have cached GLSL, need to compile SkSL->GLSL
      std::unique_ptr<SkSL::Program> vs = GrSkSLtoGLSL(
          gpu()->glContext(), SkSL::Program::kVertex_Kind, *sksl[kVertex_GrShaderType], settings,
          &glsl[kVertex_GrShaderType], errorHandler);
      if (!vs) {
        cleanup_program(fGpu, programID, shadersToDelete);
        return nullptr;
      }
    }
    if (!this->compileAndAttachShaders(
            glsl[kVertex_GrShaderType], programID, GR_GL_VERTEX_SHADER, &shadersToDelete,
            errorHandler)) {
      cleanup_program(fGpu, programID, shadersToDelete);
      return nullptr;
    }

    // This also binds vertex attribute locations. NVPR doesn't really use vertices,
    // even though it requires a vertex shader in the program.
    if (!primProc.isPathRendering()) {
      this->computeCountsAndStrides(programID, primProc, true);
    }

    /*
       Tessellation Shaders
    */
    if (fProgramInfo.primProc().willUseTessellationShaders()) {
      // Tessellation shaders are not currently supported by SkSL. So here, we temporarily
      // generate GLSL strings directly using back door methods on GrPrimitiveProcessor, and
      // pass those raw strings on to the driver.
      SkString versionAndExtensionDecls;
      versionAndExtensionDecls.appendf("%s\n", this->shaderCaps()->versionDeclString());
      if (const char* extensionString = this->shaderCaps()->tessellationExtensionString()) {
        versionAndExtensionDecls.appendf("#extension %s : require\n", extensionString);
      }

      SkString tessControlShader = primProc.getTessControlShaderGLSL(
          fGeometryProcessor.get(), versionAndExtensionDecls.c_str(), fUniformHandler,
          *this->shaderCaps());
      if (!this->compileAndAttachShaders(
              tessControlShader.c_str(), programID, GR_GL_TESS_CONTROL_SHADER, &shadersToDelete,
              errorHandler)) {
        cleanup_program(fGpu, programID, shadersToDelete);
        return nullptr;
      }

      SkString tessEvaluationShader = primProc.getTessEvaluationShaderGLSL(
          fGeometryProcessor.get(), versionAndExtensionDecls.c_str(), fUniformHandler,
          *this->shaderCaps());
      if (!this->compileAndAttachShaders(
              tessEvaluationShader.c_str(), programID, GR_GL_TESS_EVALUATION_SHADER,
              &shadersToDelete, errorHandler)) {
        cleanup_program(fGpu, programID, shadersToDelete);
        return nullptr;
      }
    }

    /*
       Geometry Shader
    */
    if (primProc.willUseGeoShader()) {
      if (glsl[kGeometry_GrShaderType].empty()) {
        // Don't have cached GLSL, need to compile SkSL->GLSL
        std::unique_ptr<SkSL::Program> gs;
        gs = GrSkSLtoGLSL(
            gpu()->glContext(), SkSL::Program::kGeometry_Kind, *sksl[kGeometry_GrShaderType],
            settings, &glsl[kGeometry_GrShaderType], errorHandler);
        if (!gs) {
          cleanup_program(fGpu, programID, shadersToDelete);
          return nullptr;
        }
      }
      if (!this->compileAndAttachShaders(
              glsl[kGeometry_GrShaderType], programID, GR_GL_GEOMETRY_SHADER, &shadersToDelete,
              errorHandler)) {
        cleanup_program(fGpu, programID, shadersToDelete);
        return nullptr;
      }
    }
    this->bindProgramResourceLocations(programID);

    GL_CALL(LinkProgram(programID));
    if (checkLinked) {
      if (!this->checkLinkStatus(programID, errorHandler, sksl, glsl)) {
        cleanup_program(fGpu, programID, shadersToDelete);
        return nullptr;
      }
    }
  }
  this->resolveProgramResourceLocations(programID, usedProgramBinaries);

  cleanup_shaders(fGpu, shadersToDelete);

  // We temporarily can't cache tessellation shaders while using back door GLSL.
  //
  // We also can't cache SkSL or GLSL if we were given a precompiled program, but there's not
  // much point in doing so.
  if (!cached && !primProc.willUseTessellationShaders() && !precompiledProgram) {
    // FIXME: Remove the check for tessellation shaders in the above 'if' once the back door
    // GLSL mechanism is removed.
    (void)&GrPrimitiveProcessor::getTessControlShaderGLSL;
    bool isSkSL = false;
    if (fGpu->getContext()->priv().options().fShaderCacheStrategy ==
        GrContextOptions::ShaderCacheStrategy::kSkSL) {
      for (int i = 0; i < kGrShaderTypeCount; ++i) {
        glsl[i] = GrShaderUtils::PrettyPrint(*sksl[i]);
      }
      isSkSL = true;
    }
    this->storeShaderInCache(inputs, programID, glsl, isSkSL, &settings);
  }
  return this->createProgram(programID);
}

void GrGLProgramBuilder::bindProgramResourceLocations(GrGLuint programID) {
  fUniformHandler.bindUniformLocations(programID, fGpu->glCaps());

  const GrGLCaps& caps = this->gpu()->glCaps();
  if (fFS.hasCustomColorOutput() && caps.bindFragDataLocationSupport()) {
    GL_CALL(
        BindFragDataLocation(programID, 0, GrGLSLFragmentShaderBuilder::DeclaredColorOutputName()));
  }
  if (fFS.hasSecondaryOutput() && caps.shaderCaps()->mustDeclareFragmentShaderOutput()) {
    GL_CALL(BindFragDataLocationIndexed(
        programID, 0, 1, GrGLSLFragmentShaderBuilder::DeclaredSecondaryColorOutputName()));
  }

  // handle NVPR separable varyings
  if (!fGpu->glCaps().shaderCaps()->pathRenderingSupport() ||
      !fGpu->glPathRendering()->shouldBindFragmentInputs()) {
    return;
  }
  int i = 0;
  for (auto& varying : fVaryingHandler.fPathProcVaryingInfos.items()) {
    GL_CALL(BindFragmentInputLocation(programID, i, varying.fVariable.c_str()));
    varying.fLocation = i;
    ++i;
  }
}

bool GrGLProgramBuilder::checkLinkStatus(
    GrGLuint programID, GrContextOptions::ShaderErrorHandler* errorHandler, SkSL::String* sksl[],
    const SkSL::String glsl[]) {
  GrGLint linked = GR_GL_INIT_ZERO;
  GL_CALL(GetProgramiv(programID, GR_GL_LINK_STATUS, &linked));
  if (!linked) {
    SkSL::String allShaders;
    if (sksl) {
      allShaders.appendf("// Vertex SKSL\n%s\n", sksl[kVertex_GrShaderType]->c_str());
      if (!sksl[kGeometry_GrShaderType]->empty()) {
        allShaders.appendf("// Geometry SKSL\n%s\n", sksl[kGeometry_GrShaderType]->c_str());
      }
      allShaders.appendf("// Fragment SKSL\n%s\n", sksl[kFragment_GrShaderType]->c_str());
    }
    if (glsl) {
      allShaders.appendf("// Vertex GLSL\n%s\n", glsl[kVertex_GrShaderType].c_str());
      if (!glsl[kGeometry_GrShaderType].empty()) {
        allShaders.appendf("// Geometry GLSL\n%s\n", glsl[kGeometry_GrShaderType].c_str());
      }
      allShaders.appendf("// Fragment GLSL\n%s\n", glsl[kFragment_GrShaderType].c_str());
    }
    GrGLint infoLen = GR_GL_INIT_ZERO;
    GL_CALL(GetProgramiv(programID, GR_GL_INFO_LOG_LENGTH, &infoLen));
    SkAutoMalloc log(sizeof(char) * (infoLen + 1));  // outside if for debugger
    if (infoLen > 0) {
      // retrieve length even though we don't need it to workaround
      // bug in chrome cmd buffer param validation.
      GrGLsizei length = GR_GL_INIT_ZERO;
      GL_CALL(GetProgramInfoLog(programID, infoLen + 1, &length, (char*)log.get()));
    }
    errorHandler->compileError(allShaders.c_str(), infoLen > 0 ? (const char*)log.get() : "");
  }
  return SkToBool(linked);
}

void GrGLProgramBuilder::resolveProgramResourceLocations(GrGLuint programID, bool force) {
  fUniformHandler.getUniformLocations(programID, fGpu->glCaps(), force);

  // handle NVPR separable varyings
  if (!fGpu->glCaps().shaderCaps()->pathRenderingSupport() ||
      fGpu->glPathRendering()->shouldBindFragmentInputs()) {
    return;
  }
  for (auto& varying : fVaryingHandler.fPathProcVaryingInfos.items()) {
    GrGLint location;
    GL_CALL_RET(
        location,
        GetProgramResourceLocation(programID, GR_GL_FRAGMENT_INPUT, varying.fVariable.c_str()));
    varying.fLocation = location;
  }
}

sk_sp<GrGLProgram> GrGLProgramBuilder::createProgram(GrGLuint programID) {
  return GrGLProgram::Make(
      fGpu, fUniformHandles, programID, fUniformHandler.fUniforms, fUniformHandler.fSamplers,
      fVaryingHandler.fPathProcVaryingInfos, std::move(fGeometryProcessor),
      std::move(fXferProcessor), std::move(fFragmentProcessors), std::move(fAttributes),
      fVertexAttributeCnt, fInstanceAttributeCnt, fVertexStride, fInstanceStride);
}

bool GrGLProgramBuilder::PrecompileProgram(
    GrGLPrecompiledProgram* precompiledProgram, GrGLGpu* gpu, const SkData& cachedData) {
  SkReadBuffer reader(cachedData.data(), cachedData.size());
  SkFourByteTag shaderType = GrPersistentCacheUtils::GetType(&reader);
  if (shaderType != kSKSL_Tag) {
    // TODO: Support GLSL, and maybe even program binaries, too?
    return false;
  }

  const GrGLInterface* gl = gpu->glInterface();
  auto errorHandler = gpu->getContext()->priv().getShaderErrorHandler();

  SkSL::Program::Settings settings;
  settings.fSharpenTextures = gpu->getContext()->priv().options().fSharpenMipmappedTextures;
  GrPersistentCacheUtils::ShaderMetadata meta;
  meta.fSettings = &settings;

  SkSL::String shaders[kGrShaderTypeCount];
  SkSL::Program::Inputs inputs;
  if (!GrPersistentCacheUtils::UnpackCachedShaders(&reader, shaders, &inputs, 1, &meta)) {
    return false;
  }

  GrGLuint programID;
  GR_GL_CALL_RET(gl, programID, CreateProgram());
  if (0 == programID) {
    return false;
  }

  SkTDArray<GrGLuint> shadersToDelete;

  auto compileShader = [&](SkSL::Program::Kind kind, const SkSL::String& sksl, GrGLenum type) {
    SkSL::String glsl;
    auto program = GrSkSLtoGLSL(gpu->glContext(), kind, sksl, settings, &glsl, errorHandler);
    if (!program) {
      return false;
    }

    if (GrGLuint shaderID = GrGLCompileAndAttachShader(
            gpu->glContext(), programID, type, glsl, gpu->stats(), errorHandler)) {
      shadersToDelete.push_back(shaderID);
      return true;
    } else {
      return false;
    }
  };

  if (!compileShader(
          SkSL::Program::kFragment_Kind, shaders[kFragment_GrShaderType], GR_GL_FRAGMENT_SHADER) ||
      !compileShader(
          SkSL::Program::kVertex_Kind, shaders[kVertex_GrShaderType], GR_GL_VERTEX_SHADER) ||
      (!shaders[kGeometry_GrShaderType].empty() &&
       !compileShader(
           SkSL::Program::kGeometry_Kind, shaders[kGeometry_GrShaderType],
           GR_GL_GEOMETRY_SHADER))) {
    cleanup_program(gpu, programID, shadersToDelete);
    return false;
  }

  for (int i = 0; i < meta.fAttributeNames.count(); ++i) {
    GR_GL_CALL(
        gpu->glInterface(), BindAttribLocation(programID, i, meta.fAttributeNames[i].c_str()));
  }

  const GrGLCaps& caps = gpu->glCaps();
  if (meta.fHasCustomColorOutput && caps.bindFragDataLocationSupport()) {
    GR_GL_CALL(
        gpu->glInterface(),
        BindFragDataLocation(programID, 0, GrGLSLFragmentShaderBuilder::DeclaredColorOutputName()));
  }
  if (meta.fHasSecondaryColorOutput && caps.shaderCaps()->mustDeclareFragmentShaderOutput()) {
    GR_GL_CALL(
        gpu->glInterface(),
        BindFragDataLocationIndexed(
            programID, 0, 1, GrGLSLFragmentShaderBuilder::DeclaredSecondaryColorOutputName()));
  }

  GR_GL_CALL(gpu->glInterface(), LinkProgram(programID));
  GrGLint linked = GR_GL_INIT_ZERO;
  GR_GL_CALL(gpu->glInterface(), GetProgramiv(programID, GR_GL_LINK_STATUS, &linked));
  if (!linked) {
    cleanup_program(gpu, programID, shadersToDelete);
    return false;
  }

  cleanup_shaders(gpu, shadersToDelete);

  precompiledProgram->fProgramID = programID;
  precompiledProgram->fInputs = inputs;
  return true;
}
