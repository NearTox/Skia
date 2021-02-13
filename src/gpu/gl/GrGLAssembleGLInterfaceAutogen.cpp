/*
 * Copyright 2019 Google LLC
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 * THIS FILE IS AUTOGENERATED
 * Make edits to tools/gpu/gl/interface/templates.go or they will
 * be overwritten.
 */

#include "include/gpu/gl/GrGLAssembleHelpers.h"
#include "include/gpu/gl/GrGLAssembleInterface.h"
#include "src/gpu/gl/GrGLUtil.h"

#define GET_PROC(F) functions->f##F = (GrGL##F##Fn*)get(ctx, "gl" #F)
#define GET_PROC_SUFFIX(F, S) functions->f##F = (GrGL##F##Fn*)get(ctx, "gl" #F #S)
#define GET_PROC_LOCAL(F) GrGL##F##Fn* F = (GrGL##F##Fn*)get(ctx, "gl" #F)

#define GET_EGL_PROC_SUFFIX(F, S) functions->fEGL##F = (GrEGL##F##Fn*)get(ctx, "egl" #F #S)

#if SK_DISABLE_GL_INTERFACE
sk_sp<const GrGLInterface> GrGLMakeAssembledGLInterface(void* ctx, GrGLGetProc get) {
  return nullptr;
}
#else
sk_sp<const GrGLInterface> GrGLMakeAssembledGLInterface(void* ctx, GrGLGetProc get) {
  GET_PROC_LOCAL(GetString);
  GET_PROC_LOCAL(GetStringi);
  GET_PROC_LOCAL(GetIntegerv);

  // GetStringi may be nullptr depending on the GL version.
  if (nullptr == GetString || nullptr == GetIntegerv) {
    return nullptr;
  }

  const char* versionString = (const char*)GetString(GR_GL_VERSION);
  GrGLVersion glVer = GrGLGetVersionFromString(versionString);

  if (glVer < GR_GL_VER(2, 0) || GR_GL_INVALID_VER == glVer) {
    // This is our minimum for non-ES GL.
    return nullptr;
  }

  GrEGLQueryStringFn* queryString;
  GrEGLDisplay display;
  GrGetEGLQueryAndDisplay(&queryString, &display, ctx, get);
  GrGLExtensions extensions;
  if (!extensions.init(
          kGL_GrGLStandard, GetString, GetStringi, GetIntegerv, queryString, display)) {
    return nullptr;
  }

  sk_sp<GrGLInterface> interface(new GrGLInterface());
  GrGLInterface::Functions* functions = &interface->fFunctions;

  // Autogenerated content follows
  GET_PROC(ActiveTexture);
  GET_PROC(AttachShader);
  GET_PROC(BindAttribLocation);
  GET_PROC(BindBuffer);
  GET_PROC(BindTexture);
  GET_PROC(BlendColor);
  GET_PROC(BlendEquation);
  GET_PROC(BlendFunc);
  GET_PROC(BufferData);
  GET_PROC(BufferSubData);
  GET_PROC(Clear);
  GET_PROC(ClearColor);
  GET_PROC(ClearStencil);
  GET_PROC(ColorMask);
  GET_PROC(CompileShader);
  GET_PROC(CompressedTexImage2D);
  GET_PROC(CompressedTexSubImage2D);
  GET_PROC(CopyTexSubImage2D);
  GET_PROC(CreateProgram);
  GET_PROC(CreateShader);
  GET_PROC(CullFace);
  GET_PROC(DeleteBuffers);
  GET_PROC(DeleteProgram);
  GET_PROC(DeleteShader);
  GET_PROC(DeleteTextures);
  GET_PROC(DepthMask);
  GET_PROC(Disable);
  GET_PROC(DisableVertexAttribArray);
  GET_PROC(DrawArrays);
  GET_PROC(DrawElements);
  GET_PROC(Enable);
  GET_PROC(EnableVertexAttribArray);
  GET_PROC(Finish);
  GET_PROC(Flush);
  GET_PROC(FrontFace);
  GET_PROC(GenBuffers);
  GET_PROC(GenTextures);
  GET_PROC(GetBufferParameteriv);
  GET_PROC(GetError);
  GET_PROC(GetIntegerv);
  GET_PROC(GetProgramInfoLog);
  GET_PROC(GetProgramiv);
  GET_PROC(GetShaderInfoLog);
  GET_PROC(GetShaderiv);
  GET_PROC(GetString);
  GET_PROC(GetUniformLocation);
  GET_PROC(IsTexture);
  GET_PROC(LineWidth);
  GET_PROC(LinkProgram);
  GET_PROC(PixelStorei);
  GET_PROC(ReadPixels);
  GET_PROC(Scissor);
  GET_PROC(ShaderSource);
  GET_PROC(StencilFunc);
  GET_PROC(StencilFuncSeparate);
  GET_PROC(StencilMask);
  GET_PROC(StencilMaskSeparate);
  GET_PROC(StencilOp);
  GET_PROC(StencilOpSeparate);
  GET_PROC(TexImage2D);
  GET_PROC(TexParameterf);
  GET_PROC(TexParameterfv);
  GET_PROC(TexParameteri);
  GET_PROC(TexParameteriv);
  GET_PROC(TexSubImage2D);
  GET_PROC(Uniform1f);
  GET_PROC(Uniform1fv);
  GET_PROC(Uniform1i);
  GET_PROC(Uniform1iv);
  GET_PROC(Uniform2f);
  GET_PROC(Uniform2fv);
  GET_PROC(Uniform2i);
  GET_PROC(Uniform2iv);
  GET_PROC(Uniform3f);
  GET_PROC(Uniform3fv);
  GET_PROC(Uniform3i);
  GET_PROC(Uniform3iv);
  GET_PROC(Uniform4f);
  GET_PROC(Uniform4fv);
  GET_PROC(Uniform4i);
  GET_PROC(Uniform4iv);
  GET_PROC(UniformMatrix2fv);
  GET_PROC(UniformMatrix3fv);
  GET_PROC(UniformMatrix4fv);
  GET_PROC(UseProgram);
  GET_PROC(VertexAttrib1f);
  GET_PROC(VertexAttrib2fv);
  GET_PROC(VertexAttrib3fv);
  GET_PROC(VertexAttrib4fv);
  GET_PROC(VertexAttribPointer);
  GET_PROC(Viewport);

  GET_PROC(DrawBuffer);
  GET_PROC(PolygonMode);

  if (glVer >= GR_GL_VER(3, 0)) {
    GET_PROC(GetStringi);
  }

  if (glVer >= GR_GL_VER(4, 2)) {
    GET_PROC(MemoryBarrier);
  }

  if (glVer >= GR_GL_VER(3, 0)) {
    GET_PROC(BindVertexArray);
    GET_PROC(DeleteVertexArrays);
    GET_PROC(GenVertexArrays);
  } else if (extensions.has("GL_ARB_vertex_array_object")) {
    GET_PROC(BindVertexArray);
    GET_PROC(DeleteVertexArrays);
    GET_PROC(GenVertexArrays);
  } else if (extensions.has("GL_APPLE_vertex_array_object")) {
    GET_PROC_SUFFIX(BindVertexArray, APPLE);
    GET_PROC_SUFFIX(DeleteVertexArrays, APPLE);
    GET_PROC_SUFFIX(GenVertexArrays, APPLE);
  }

  if (glVer >= GR_GL_VER(4, 0)) {
    GET_PROC(PatchParameteri);
  } else if (extensions.has("GL_ARB_tessellation_shader")) {
    GET_PROC(PatchParameteri);
  }

  if (glVer >= GR_GL_VER(3, 0)) {
    GET_PROC(BindFragDataLocation);
  }

  if (glVer >= GR_GL_VER(3, 3)) {
    GET_PROC(BindFragDataLocationIndexed);
  } else if (extensions.has("GL_ARB_blend_func_extended")) {
    GET_PROC(BindFragDataLocationIndexed);
  }

  if (extensions.has("GL_KHR_blend_equation_advanced")) {
    GET_PROC_SUFFIX(BlendBarrier, KHR);
  } else if (extensions.has("GL_NV_blend_equation_advanced")) {
    GET_PROC_SUFFIX(BlendBarrier, NV);
  }

  if (glVer >= GR_GL_VER(4, 4)) {
    GET_PROC(ClearTexImage);
    GET_PROC(ClearTexSubImage);
  } else if (extensions.has("GL_ARB_clear_texture")) {
    GET_PROC(ClearTexImage);
    GET_PROC(ClearTexSubImage);
  }

  if (glVer >= GR_GL_VER(3, 1)) {
    GET_PROC(DrawArraysInstanced);
    GET_PROC(DrawElementsInstanced);
  } else if (extensions.has("GL_ARB_draw_instanced")) {
    GET_PROC(DrawArraysInstanced);
    GET_PROC(DrawElementsInstanced);
  } else if (extensions.has("GL_EXT_draw_instanced")) {
    GET_PROC_SUFFIX(DrawArraysInstanced, EXT);
    GET_PROC_SUFFIX(DrawElementsInstanced, EXT);
  }

  if (glVer >= GR_GL_VER(4, 2)) {
    GET_PROC(DrawArraysInstancedBaseInstance);
    GET_PROC(DrawElementsInstancedBaseVertexBaseInstance);
  } else if (extensions.has("GL_ARB_base_instance")) {
    GET_PROC(DrawArraysInstancedBaseInstance);
    GET_PROC(DrawElementsInstancedBaseVertexBaseInstance);
  }

  GET_PROC(DrawBuffers);
  GET_PROC(ReadBuffer);

  if (glVer >= GR_GL_VER(4, 0)) {
    GET_PROC(DrawArraysIndirect);
    GET_PROC(DrawElementsIndirect);
  } else if (extensions.has("GL_ARB_draw_indirect")) {
    GET_PROC(DrawArraysIndirect);
    GET_PROC(DrawElementsIndirect);
  }

  GET_PROC(DrawRangeElements);

  if (glVer >= GR_GL_VER(3, 2)) {
    GET_PROC(GetMultisamplefv);
  } else if (extensions.has("GL_ARB_texture_multisample")) {
    GET_PROC(GetMultisamplefv);
  }

  GET_PROC(GetTexLevelParameteriv);

  if (glVer >= GR_GL_VER(4, 3)) {
    GET_PROC(MultiDrawArraysIndirect);
    GET_PROC(MultiDrawElementsIndirect);
  } else if (extensions.has("GL_ARB_multi_draw_indirect")) {
    GET_PROC(MultiDrawArraysIndirect);
    GET_PROC(MultiDrawElementsIndirect);
  }

  if (glVer >= GR_GL_VER(3, 1)) {
    GET_PROC(TexBuffer);
  }

  if (glVer >= GR_GL_VER(4, 3)) {
    GET_PROC(TexBufferRange);
  }

  if (glVer >= GR_GL_VER(4, 2)) {
    GET_PROC(TexStorage2D);
  } else if (extensions.has("GL_ARB_texture_storage")) {
    GET_PROC(TexStorage2D);
  } else if (extensions.has("GL_EXT_texture_storage")) {
    GET_PROC_SUFFIX(TexStorage2D, EXT);
  }

  if (glVer >= GR_GL_VER(4, 5)) {
    GET_PROC(TextureBarrier);
  } else if (extensions.has("GL_ARB_texture_barrier")) {
    GET_PROC(TextureBarrier);
  } else if (extensions.has("GL_NV_texture_barrier")) {
    GET_PROC_SUFFIX(TextureBarrier, NV);
  }

  if (glVer >= GR_GL_VER(3, 2)) {
    GET_PROC(VertexAttribDivisor);
  } else if (extensions.has("GL_ARB_instanced_arrays")) {
    GET_PROC(VertexAttribDivisor);
  }

  if (glVer >= GR_GL_VER(3, 0)) {
    GET_PROC(VertexAttribIPointer);
  }

  if (glVer >= GR_GL_VER(3, 0)) {
    GET_PROC(BindFramebuffer);
    GET_PROC(BindRenderbuffer);
    GET_PROC(CheckFramebufferStatus);
    GET_PROC(DeleteFramebuffers);
    GET_PROC(DeleteRenderbuffers);
    GET_PROC(FramebufferRenderbuffer);
    GET_PROC(FramebufferTexture2D);
    GET_PROC(GenFramebuffers);
    GET_PROC(GenRenderbuffers);
    GET_PROC(GenerateMipmap);
    GET_PROC(GetFramebufferAttachmentParameteriv);
    GET_PROC(GetRenderbufferParameteriv);
    GET_PROC(RenderbufferStorage);
  } else if (extensions.has("GL_ARB_framebuffer_object")) {
    GET_PROC(BindFramebuffer);
    GET_PROC(BindRenderbuffer);
    GET_PROC(CheckFramebufferStatus);
    GET_PROC(DeleteFramebuffers);
    GET_PROC(DeleteRenderbuffers);
    GET_PROC(FramebufferRenderbuffer);
    GET_PROC(FramebufferTexture2D);
    GET_PROC(GenFramebuffers);
    GET_PROC(GenRenderbuffers);
    GET_PROC(GenerateMipmap);
    GET_PROC(GetFramebufferAttachmentParameteriv);
    GET_PROC(GetRenderbufferParameteriv);
    GET_PROC(RenderbufferStorage);
  } else if (extensions.has("GL_EXT_framebuffer_object")) {
    GET_PROC_SUFFIX(BindFramebuffer, EXT);
    GET_PROC_SUFFIX(BindRenderbuffer, EXT);
    GET_PROC_SUFFIX(CheckFramebufferStatus, EXT);
    GET_PROC_SUFFIX(DeleteFramebuffers, EXT);
    GET_PROC_SUFFIX(DeleteRenderbuffers, EXT);
    GET_PROC_SUFFIX(FramebufferRenderbuffer, EXT);
    GET_PROC_SUFFIX(FramebufferTexture2D, EXT);
    GET_PROC_SUFFIX(GenFramebuffers, EXT);
    GET_PROC_SUFFIX(GenRenderbuffers, EXT);
    GET_PROC_SUFFIX(GenerateMipmap, EXT);
    GET_PROC_SUFFIX(GetFramebufferAttachmentParameteriv, EXT);
    GET_PROC_SUFFIX(GetRenderbufferParameteriv, EXT);
    GET_PROC_SUFFIX(RenderbufferStorage, EXT);
  }

  if (glVer >= GR_GL_VER(3, 0)) {
    GET_PROC(BlitFramebuffer);
  } else if (extensions.has("GL_ARB_framebuffer_object")) {
    GET_PROC(BlitFramebuffer);
  } else if (extensions.has("GL_EXT_framebuffer_blit")) {
    GET_PROC_SUFFIX(BlitFramebuffer, EXT);
  }

  if (glVer >= GR_GL_VER(3, 0)) {
    GET_PROC(RenderbufferStorageMultisample);
  } else if (extensions.has("GL_ARB_framebuffer_object")) {
    GET_PROC(RenderbufferStorageMultisample);
  } else if (extensions.has("GL_EXT_framebuffer_multisample")) {
    GET_PROC_SUFFIX(RenderbufferStorageMultisample, EXT);
  }

  GET_PROC(MapBuffer);

  GET_PROC(UnmapBuffer);

  if (glVer >= GR_GL_VER(3, 0)) {
    GET_PROC(FlushMappedBufferRange);
    GET_PROC(MapBufferRange);
  } else if (extensions.has("GL_ARB_map_buffer_range")) {
    GET_PROC(FlushMappedBufferRange);
    GET_PROC(MapBufferRange);
  }

  if (extensions.has("GL_EXT_debug_marker")) {
    GET_PROC_SUFFIX(InsertEventMarker, EXT);
    GET_PROC_SUFFIX(PopGroupMarker, EXT);
    GET_PROC_SUFFIX(PushGroupMarker, EXT);
  }

  if (glVer >= GR_GL_VER(4, 3)) {
    GET_PROC(GetProgramResourceLocation);
  } else if (extensions.has("GL_ARB_program_interface_query")) {
    GET_PROC(GetProgramResourceLocation);
  }

  if (extensions.has("GL_NV_path_rendering")) {
    GET_PROC_SUFFIX(MatrixLoadIdentity, EXT);
    GET_PROC_SUFFIX(MatrixLoadf, EXT);
  }

  if (extensions.has("GL_NV_path_rendering")) {
    GET_PROC_SUFFIX(CoverFillPath, NV);
    GET_PROC_SUFFIX(CoverFillPathInstanced, NV);
    GET_PROC_SUFFIX(CoverStrokePath, NV);
    GET_PROC_SUFFIX(CoverStrokePathInstanced, NV);
    GET_PROC_SUFFIX(DeletePaths, NV);
    GET_PROC_SUFFIX(GenPaths, NV);
    GET_PROC_SUFFIX(IsPath, NV);
    GET_PROC_SUFFIX(PathCommands, NV);
    GET_PROC_SUFFIX(PathParameterf, NV);
    GET_PROC_SUFFIX(PathParameteri, NV);
    GET_PROC_SUFFIX(PathStencilFunc, NV);
    GET_PROC_SUFFIX(ProgramPathFragmentInputGen, NV);
    GET_PROC_SUFFIX(StencilFillPath, NV);
    GET_PROC_SUFFIX(StencilFillPathInstanced, NV);
    GET_PROC_SUFFIX(StencilStrokePath, NV);
    GET_PROC_SUFFIX(StencilStrokePathInstanced, NV);
    GET_PROC_SUFFIX(StencilThenCoverFillPath, NV);
    GET_PROC_SUFFIX(StencilThenCoverFillPathInstanced, NV);
    GET_PROC_SUFFIX(StencilThenCoverStrokePath, NV);
    GET_PROC_SUFFIX(StencilThenCoverStrokePathInstanced, NV);
  }

  if (extensions.has("GL_NV_framebuffer_mixed_samples")) {
    GET_PROC_SUFFIX(CoverageModulation, NV);
  }

  if (glVer >= GR_GL_VER(4, 3)) {
    GET_PROC(DebugMessageCallback);
    GET_PROC(DebugMessageControl);
    GET_PROC(DebugMessageInsert);
    GET_PROC(GetDebugMessageLog);
    GET_PROC(ObjectLabel);
    GET_PROC(PopDebugGroup);
    GET_PROC(PushDebugGroup);
  } else if (extensions.has("GL_KHR_debug")) {
    GET_PROC(DebugMessageCallback);
    GET_PROC(DebugMessageControl);
    GET_PROC(DebugMessageInsert);
    GET_PROC(GetDebugMessageLog);
    GET_PROC(ObjectLabel);
    GET_PROC(PopDebugGroup);
    GET_PROC(PushDebugGroup);
  }

  if (extensions.has("GL_EXT_window_rectangles")) {
    GET_PROC_SUFFIX(WindowRectangles, EXT);
  }

  if (glVer >= GR_GL_VER(3, 2)) {
    GET_PROC(ClientWaitSync);
    GET_PROC(DeleteSync);
    GET_PROC(FenceSync);
    GET_PROC(IsSync);
    GET_PROC(WaitSync);
  } else if (extensions.has("GL_ARB_sync")) {
    GET_PROC(ClientWaitSync);
    GET_PROC(DeleteSync);
    GET_PROC(FenceSync);
    GET_PROC(IsSync);
    GET_PROC(WaitSync);
  }

  if (glVer >= GR_GL_VER(4, 2)) {
    GET_PROC(GetInternalformativ);
  } else if (extensions.has("GL_ARB_internalformat_query")) {
    GET_PROC(GetInternalformativ);
  }

  if (glVer >= GR_GL_VER(4, 1)) {
    GET_PROC(GetProgramBinary);
    GET_PROC(ProgramBinary);
  }

  if (glVer >= GR_GL_VER(4, 1)) {
    GET_PROC(ProgramParameteri);
  }

  if (glVer >= GR_GL_VER(3, 2)) {
    GET_PROC(BindSampler);
    GET_PROC(DeleteSamplers);
    GET_PROC(GenSamplers);
    GET_PROC(SamplerParameteri);
    GET_PROC(SamplerParameteriv);
  } else if (extensions.has("GL_ARB_sampler_objects")) {
    GET_PROC(BindSampler);
    GET_PROC(DeleteSamplers);
    GET_PROC(GenSamplers);
    GET_PROC(SamplerParameteri);
    GET_PROC(SamplerParameteriv);
  }

  GET_PROC(GetQueryObjectiv);

#  if GR_TEST_UTILS
  GET_PROC(BeginQuery);
  GET_PROC(DeleteQueries);
  GET_PROC(EndQuery);
  GET_PROC(GenQueries);
  GET_PROC(GetQueryObjectuiv);
  GET_PROC(GetQueryiv);
#  endif

  if (glVer >= GR_GL_VER(3, 3)) {
    GET_PROC(GetQueryObjecti64v);
    GET_PROC(GetQueryObjectui64v);
  } else if (extensions.has("GL_ARB_timer_query")) {
    GET_PROC(GetQueryObjecti64v);
    GET_PROC(GetQueryObjectui64v);
  } else if (extensions.has("GL_EXT_timer_query")) {
    GET_PROC_SUFFIX(GetQueryObjecti64v, EXT);
    GET_PROC_SUFFIX(GetQueryObjectui64v, EXT);
  }

  if (glVer >= GR_GL_VER(3, 3)) {
    GET_PROC(QueryCounter);
  } else if (extensions.has("GL_ARB_timer_query")) {
    GET_PROC(QueryCounter);
  }

  if (glVer >= GR_GL_VER(4, 3)) {
    GET_PROC(InvalidateBufferData);
    GET_PROC(InvalidateBufferSubData);
    GET_PROC(InvalidateTexImage);
    GET_PROC(InvalidateTexSubImage);
  } else if (extensions.has("GL_ARB_invalidate_subdata")) {
    GET_PROC(InvalidateBufferData);
    GET_PROC(InvalidateBufferSubData);
    GET_PROC(InvalidateTexImage);
    GET_PROC(InvalidateTexSubImage);
  }

  if (glVer >= GR_GL_VER(4, 3)) {
    GET_PROC(InvalidateFramebuffer);
    GET_PROC(InvalidateSubFramebuffer);
  } else if (extensions.has("GL_ARB_invalidate_subdata")) {
    GET_PROC(InvalidateFramebuffer);
    GET_PROC(InvalidateSubFramebuffer);
  }

  if (glVer >= GR_GL_VER(4, 3)) {
    GET_PROC(GetShaderPrecisionFormat);
  } else if (extensions.has("GL_ARB_ES2_compatibility")) {
    GET_PROC(GetShaderPrecisionFormat);
  }

  if (extensions.has("GL_NV_fence")) {
    GET_PROC_SUFFIX(DeleteFences, NV);
    GET_PROC_SUFFIX(FinishFence, NV);
    GET_PROC_SUFFIX(GenFences, NV);
    GET_PROC_SUFFIX(SetFence, NV);
    GET_PROC_SUFFIX(TestFence, NV);
  }

  // End autogenerated content
  interface->fStandard = kGL_GrGLStandard;
  interface->fExtensions.swap(&extensions);

  return std::move(interface);
}
#endif
