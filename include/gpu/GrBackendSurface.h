/*
 * Copyright 2017 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef GrBackendSurface_DEFINED
#define GrBackendSurface_DEFINED

#include "include/gpu/GrBackendSurfaceMutableState.h"
#include "include/gpu/GrTypes.h"
#include "include/gpu/gl/GrGLTypes.h"
#include "include/gpu/mock/GrMockTypes.h"
#include "include/gpu/vk/GrVkTypes.h"
#include "include/private/GrGLTypesPriv.h"
#include "include/private/GrVkTypesPriv.h"

#ifdef SK_DAWN
#  include "include/gpu/dawn/GrDawnTypes.h"
#endif

class GrBackendSurfaceMutableStateImpl;
class GrVkImageLayout;
class GrGLTextureParameters;

#ifdef SK_DAWN
#  include "dawn/webgpu_cpp.h"
#endif

#ifdef SK_METAL
#  include "include/gpu/mtl/GrMtlTypes.h"
#endif

#ifdef SK_DIRECT3D
#  include "include/gpu/d3d/GrD3DTypesMinimal.h"
#  include "include/private/GrD3DTypesPriv.h"
class GrD3DResourceState;
#endif

#if defined(SK_DEBUG) || GR_TEST_UTILS
class SkString;
#endif

#if !SK_SUPPORT_GPU

// SkSurfaceCharacterization always needs a minimal version of this
class SK_API GrBackendFormat {
 public:
  bool isValid() const noexcept { return false; }
};

// SkSurface and SkImage rely on a minimal version of these always being available
class SK_API GrBackendTexture {
 public:
  constexpr GrBackendTexture() noexcept = default;

  bool isValid() const noexcept { return false; }
};

class SK_API GrBackendRenderTarget {
 public:
  constexpr GrBackendRenderTarget() noexcept = default;

  bool isValid() const noexcept { return false; }
  bool isFramebufferOnly() const noexcept { return false; }
};
#else

enum class GrGLFormat;

class SK_API GrBackendFormat {
 public:
  // Creates an invalid backend format.
  GrBackendFormat() noexcept {}
  GrBackendFormat(const GrBackendFormat&) noexcept;
  GrBackendFormat& operator=(const GrBackendFormat&) noexcept;

  static GrBackendFormat MakeGL(GrGLenum format, GrGLenum target) noexcept {
    return GrBackendFormat(format, target);
  }

  static GrBackendFormat MakeVk(VkFormat format) noexcept {
    return GrBackendFormat(format, GrVkYcbcrConversionInfo());
  }

  static GrBackendFormat MakeVk(const GrVkYcbcrConversionInfo& ycbcrInfo) noexcept;

#  ifdef SK_DAWN
  static GrBackendFormat MakeDawn(wgpu::TextureFormat format) { return GrBackendFormat(format); }
#  endif

#  ifdef SK_METAL
  static GrBackendFormat MakeMtl(GrMTLPixelFormat format) { return GrBackendFormat(format); }
#  endif

#  ifdef SK_DIRECT3D
  static GrBackendFormat MakeDxgi(DXGI_FORMAT format) { return GrBackendFormat(format); }
#  endif

  static GrBackendFormat MakeMock(
      GrColorType colorType, SkImage::CompressionType compression) noexcept;

  bool operator==(const GrBackendFormat& that) const noexcept;
  bool operator!=(const GrBackendFormat& that) const noexcept { return !(*this == that); }

  GrBackendApi backend() const noexcept { return fBackend; }
  GrTextureType textureType() const noexcept { return fTextureType; }

  /**
   * Gets the channels present in the format as a bitfield of SkColorChannelFlag values.
   * Luminance channels are reported as kGray_SkColorChannelFlag.
   */
  uint32_t channelMask() const;

  /**
   * If the backend API is GL this gets the format as a GrGLFormat. Otherwise, returns
   * GrGLFormat::kUnknown.
   */
  GrGLFormat asGLFormat() const noexcept;

  /**
   * If the backend API is Vulkan this gets the format as a VkFormat and returns true. Otherwise,
   * returns false.
   */
  bool asVkFormat(VkFormat*) const noexcept;

  const GrVkYcbcrConversionInfo* getVkYcbcrConversionInfo() const noexcept;

#  ifdef SK_DAWN
  /**
   * If the backend API is Dawn this gets the format as a wgpu::TextureFormat and returns true.
   * Otherwise, returns false.
   */
  bool asDawnFormat(wgpu::TextureFormat*) const;
#  endif

#  ifdef SK_METAL
  /**
   * If the backend API is Metal this gets the format as a GrMtlPixelFormat. Otherwise,
   * Otherwise, returns MTLPixelFormatInvalid.
   */
  GrMTLPixelFormat asMtlFormat() const;
#  endif

#  ifdef SK_DIRECT3D
  /**
   * If the backend API is Direct3D this gets the format as a DXGI_FORMAT and returns true.
   * Otherwise, returns false.
   */
  bool asDxgiFormat(DXGI_FORMAT*) const;
#  endif

  /**
   * If the backend API is not Mock these two calls will return kUnknown and kNone, respectively.
   * Otherwise, if the compression type is kNone then the GrColorType will be valid. If the
   * compression type is anything other then kNone than the GrColorType will be kUnknown.
   */
  GrColorType asMockColorType() const noexcept;
  SkImage::CompressionType asMockCompressionType() const noexcept;

  // If possible, copies the GrBackendFormat and forces the texture type to be Texture2D. If the
  // GrBackendFormat was for Vulkan and it originally had a GrVkYcbcrConversionInfo, we will
  // remove the conversion and set the format to be VK_FORMAT_R8G8B8A8_UNORM.
  GrBackendFormat makeTexture2D() const;

  // Returns true if the backend format has been initialized.
  bool isValid() const noexcept { return fValid; }

#  if defined(SK_DEBUG) || GR_TEST_UTILS
  SkString toStr() const;
#  endif

 private:
  GrBackendFormat(GrGLenum format, GrGLenum target) noexcept;

  GrBackendFormat(const VkFormat vkFormat, const GrVkYcbcrConversionInfo&) noexcept;

#  ifdef SK_DAWN
  GrBackendFormat(wgpu::TextureFormat format);
#  endif

#  ifdef SK_METAL
  GrBackendFormat(const GrMTLPixelFormat mtlFormat);
#  endif

#  ifdef SK_DIRECT3D
  GrBackendFormat(DXGI_FORMAT dxgiFormat);
#  endif

  GrBackendFormat(GrColorType, SkImage::CompressionType) noexcept;

  GrBackendApi fBackend = GrBackendApi::kMock;
  bool fValid = false;

  union {
    GrGLenum fGLFormat;  // the sized, internal format of the GL resource
    struct {
      VkFormat fFormat;
      GrVkYcbcrConversionInfo fYcbcrConversionInfo;
    } fVk;
#  ifdef SK_DAWN
    wgpu::TextureFormat fDawnFormat;
#  endif

#  ifdef SK_METAL
    GrMTLPixelFormat fMtlFormat;
#  endif

#  ifdef SK_DIRECT3D
    DXGI_FORMAT fDxgiFormat;
#  endif
    struct {
      GrColorType fColorType;
      SkImage::CompressionType fCompressionType;
    } fMock;
  };
  GrTextureType fTextureType = GrTextureType::kNone;
};

class SK_API GrBackendTexture {
 public:
  // Creates an invalid backend texture.
  GrBackendTexture() noexcept;

  // The GrGLTextureInfo must have a valid fFormat.
  GrBackendTexture(int width, int height, GrMipmapped, const GrGLTextureInfo& glInfo);

#  ifdef SK_VULKAN
  GrBackendTexture(int width, int height, const GrVkImageInfo& vkInfo);
#  endif

#  ifdef SK_METAL
  GrBackendTexture(int width, int height, GrMipmapped, const GrMtlTextureInfo& mtlInfo);
#  endif

#  ifdef SK_DIRECT3D
  GrBackendTexture(int width, int height, const GrD3DTextureResourceInfo& d3dInfo);
#  endif

#  ifdef SK_DAWN
  GrBackendTexture(int width, int height, const GrDawnTextureInfo& dawnInfo);
#  endif

  GrBackendTexture(int width, int height, GrMipmapped, const GrMockTextureInfo& mockInfo) noexcept;

  GrBackendTexture(const GrBackendTexture& that) noexcept;

  ~GrBackendTexture();

  GrBackendTexture& operator=(const GrBackendTexture& that) noexcept;

  SkISize dimensions() const noexcept { return {fWidth, fHeight}; }
  int width() const noexcept { return fWidth; }
  int height() const noexcept { return fHeight; }
  bool hasMipmaps() const noexcept { return fMipmapped == GrMipmapped::kYes; }
  /** deprecated alias of hasMipmaps(). */
  bool hasMipMaps() const noexcept { return this->hasMipmaps(); }
  GrBackendApi backend() const noexcept { return fBackend; }

  // If the backend API is GL, copies a snapshot of the GrGLTextureInfo struct into the passed in
  // pointer and returns true. Otherwise returns false if the backend API is not GL.
  bool getGLTextureInfo(GrGLTextureInfo*) const;

  // Call this to indicate that the texture parameters have been modified in the GL context
  // externally to GrContext.
  void glTextureParametersModified();

#  ifdef SK_DAWN
  // If the backend API is Dawn, copies a snapshot of the GrDawnTextureInfo struct into the passed
  // in pointer and returns true. Otherwise returns false if the backend API is not Dawn.
  bool getDawnTextureInfo(GrDawnTextureInfo*) const;
#  endif

  // If the backend API is Vulkan, copies a snapshot of the GrVkImageInfo struct into the passed
  // in pointer and returns true. This snapshot will set the fImageLayout to the current layout
  // state. Otherwise returns false if the backend API is not Vulkan.
  bool getVkImageInfo(GrVkImageInfo*) const;

  // Anytime the client changes the VkImageLayout of the VkImage captured by this
  // GrBackendTexture, they must call this function to notify Skia of the changed layout.
  void setVkImageLayout(VkImageLayout) noexcept;

#  ifdef SK_METAL
  // If the backend API is Metal, copies a snapshot of the GrMtlTextureInfo struct into the passed
  // in pointer and returns true. Otherwise returns false if the backend API is not Metal.
  bool getMtlTextureInfo(GrMtlTextureInfo*) const;
#  endif

#  ifdef SK_DIRECT3D
  // If the backend API is Direct3D, copies a snapshot of the GrD3DTextureResourceInfo struct into
  // the passed in pointer and returns true. This snapshot will set the fResourceState to the
  // current resource state. Otherwise returns false if the backend API is not D3D.
  bool getD3DTextureResourceInfo(GrD3DTextureResourceInfo*) const;

  // Anytime the client changes the D3D12_RESOURCE_STATES of the D3D12_RESOURCE captured by this
  // GrBackendTexture, they must call this function to notify Skia of the changed layout.
  void setD3DResourceState(GrD3DResourceStateEnum);
#  endif

  // Get the GrBackendFormat for this texture (or an invalid format if this is not valid).
  GrBackendFormat getBackendFormat() const;

  // If the backend API is Mock, copies a snapshot of the GrMockTextureInfo struct into the passed
  // in pointer and returns true. Otherwise returns false if the backend API is not Mock.
  bool getMockTextureInfo(GrMockTextureInfo*) const;

  // If the client changes any of the mutable backend of the GrBackendTexture they should call
  // this function to inform Skia that those values have changed. The backend API specific state
  // that can be set from this function are:
  //
  // Vulkan: VkImageLayout and QueueFamilyIndex
  void setMutableState(const GrBackendSurfaceMutableState&);

  // Returns true if we are working with protected content.
  bool isProtected() const;

  // Returns true if the backend texture has been initialized.
  bool isValid() const noexcept { return fIsValid; }

  // Returns true if both textures are valid and refer to the same API texture.
  bool isSameTexture(const GrBackendTexture&);

#  if GR_TEST_UTILS
  static bool TestingOnly_Equals(const GrBackendTexture&, const GrBackendTexture&);
#  endif

 private:
  friend class GrVkGpu;  // for getMutableState
  sk_sp<GrBackendSurfaceMutableStateImpl> getMutableState() const noexcept;

#  ifdef SK_GL
  friend class GrGLTexture;
  friend class GrGLGpu;  // for getGLTextureParams
  GrBackendTexture(
      int width, int height, GrMipmapped, const GrGLTextureInfo,
      sk_sp<GrGLTextureParameters>) noexcept;
  sk_sp<GrGLTextureParameters> getGLTextureParams() const noexcept;
#  endif

#  ifdef SK_VULKAN
  friend class GrVkTexture;
  GrBackendTexture(
      int width, int height, const GrVkImageInfo& vkInfo,
      sk_sp<GrBackendSurfaceMutableStateImpl> mutableState) noexcept;
#  endif

#  ifdef SK_DIRECT3D
  friend class GrD3DTexture;
  friend class GrD3DGpu;  // for getGrD3DResourceState
  GrBackendTexture(
      int width, int height, const GrD3DTextureResourceInfo& vkInfo,
      sk_sp<GrD3DResourceState> state);
  sk_sp<GrD3DResourceState> getGrD3DResourceState() const;
#  endif

  // Free and release and resources being held by the GrBackendTexture.
  void cleanup() noexcept;

  bool fIsValid;
  int fWidth;   //<! width in pixels
  int fHeight;  //<! height in pixels
  GrMipmapped fMipmapped;
  GrBackendApi fBackend;

  union {
#  ifdef SK_GL
    GrGLBackendTextureInfo fGLInfo;
#  endif
    GrVkBackendSurfaceInfo fVkInfo;
    GrMockTextureInfo fMockInfo;
#  ifdef SK_DIRECT3D
    GrD3DBackendSurfaceInfo fD3DInfo;
#  endif
  };
#  ifdef SK_METAL
  GrMtlTextureInfo fMtlInfo;
#  endif
#  ifdef SK_DAWN
  GrDawnTextureInfo fDawnInfo;
#  endif

  sk_sp<GrBackendSurfaceMutableStateImpl> fMutableState;
};

class SK_API GrBackendRenderTarget {
 public:
  // Creates an invalid backend texture.
  GrBackendRenderTarget();

  // The GrGLTextureInfo must have a valid fFormat.
  GrBackendRenderTarget(
      int width, int height, int sampleCnt, int stencilBits,
      const GrGLFramebufferInfo& glInfo) noexcept;

#  ifdef SK_DAWN
  GrBackendRenderTarget(
      int width, int height, int sampleCnt, int stencilBits,
      const GrDawnRenderTargetInfo& dawnInfo);
#  endif

#  ifdef SK_VULKAN
  /** Deprecated, use version that does not take stencil bits. */
  GrBackendRenderTarget(
      int width, int height, int sampleCnt, int stencilBits, const GrVkImageInfo& vkInfo);
  GrBackendRenderTarget(int width, int height, int sampleCnt, const GrVkImageInfo& vkInfo);
#  endif

#  ifdef SK_METAL
  GrBackendRenderTarget(int width, int height, int sampleCnt, const GrMtlTextureInfo& mtlInfo);
#  endif

#  ifdef SK_DIRECT3D
  GrBackendRenderTarget(
      int width, int height, int sampleCnt, const GrD3DTextureResourceInfo& d3dInfo);
#  endif

  GrBackendRenderTarget(
      int width, int height, int sampleCnt, int stencilBits,
      const GrMockRenderTargetInfo& mockInfo) noexcept;

  ~GrBackendRenderTarget();

  GrBackendRenderTarget(const GrBackendRenderTarget& that) noexcept;
  GrBackendRenderTarget& operator=(const GrBackendRenderTarget&) noexcept;

  SkISize dimensions() const noexcept { return {fWidth, fHeight}; }
  int width() const noexcept { return fWidth; }
  int height() const noexcept { return fHeight; }
  int sampleCnt() const noexcept { return fSampleCnt; }
  int stencilBits() const noexcept { return fStencilBits; }
  GrBackendApi backend() const noexcept { return fBackend; }
  bool isFramebufferOnly() const noexcept { return fFramebufferOnly; }

  // If the backend API is GL, copies a snapshot of the GrGLFramebufferInfo struct into the passed
  // in pointer and returns true. Otherwise returns false if the backend API is not GL.
  bool getGLFramebufferInfo(GrGLFramebufferInfo*) const;

#  ifdef SK_DAWN
  // If the backend API is Dawn, copies a snapshot of the GrDawnRenderTargetInfo struct into the
  // passed-in pointer and returns true. Otherwise returns false if the backend API is not Dawn.
  bool getDawnRenderTargetInfo(GrDawnRenderTargetInfo*) const;
#  endif

  // If the backend API is Vulkan, copies a snapshot of the GrVkImageInfo struct into the passed
  // in pointer and returns true. This snapshot will set the fImageLayout to the current layout
  // state. Otherwise returns false if the backend API is not Vulkan.
  bool getVkImageInfo(GrVkImageInfo*) const;

  // Anytime the client changes the VkImageLayout of the VkImage captured by this
  // GrBackendRenderTarget, they must call this function to notify Skia of the changed layout.
  void setVkImageLayout(VkImageLayout);

#  ifdef SK_METAL
  // If the backend API is Metal, copies a snapshot of the GrMtlTextureInfo struct into the passed
  // in pointer and returns true. Otherwise returns false if the backend API is not Metal.
  bool getMtlTextureInfo(GrMtlTextureInfo*) const;
#  endif

#  ifdef SK_DIRECT3D
  // If the backend API is Direct3D, copies a snapshot of the GrMtlTextureInfo struct into the
  // passed in pointer and returns true. Otherwise returns false if the backend API is not D3D.
  bool getD3DTextureResourceInfo(GrD3DTextureResourceInfo*) const;

  // Anytime the client changes the D3D12_RESOURCE_STATES of the D3D12_RESOURCE captured by this
  // GrBackendTexture, they must call this function to notify Skia of the changed layout.
  void setD3DResourceState(GrD3DResourceStateEnum);
#  endif

  // Get the GrBackendFormat for this render target (or an invalid format if this is not valid).
  GrBackendFormat getBackendFormat() const;

  // If the backend API is Mock, copies a snapshot of the GrMockTextureInfo struct into the passed
  // in pointer and returns true. Otherwise returns false if the backend API is not Mock.
  bool getMockRenderTargetInfo(GrMockRenderTargetInfo*) const noexcept;

  // If the client changes any of the mutable backend of the GrBackendTexture they should call
  // this function to inform Skia that those values have changed. The backend API specific state
  // that can be set from this function are:
  //
  // Vulkan: VkImageLayout and QueueFamilyIndex
  void setMutableState(const GrBackendSurfaceMutableState&) noexcept;

  // Returns true if we are working with protected content.
  bool isProtected() const noexcept;

  // Returns true if the backend texture has been initialized.
  bool isValid() const noexcept { return fIsValid; }

#  if GR_TEST_UTILS
  static bool TestingOnly_Equals(const GrBackendRenderTarget&, const GrBackendRenderTarget&);
#  endif

 private:
  friend class GrVkGpu;  // for getMutableState
  sk_sp<GrBackendSurfaceMutableStateImpl> getMutableState() const;

#  ifdef SK_VULKAN
  friend class GrVkRenderTarget;
  GrBackendRenderTarget(
      int width, int height, int sampleCnt, const GrVkImageInfo& vkInfo,
      sk_sp<GrBackendSurfaceMutableStateImpl> mutableState) noexcept;
#  endif

#  ifdef SK_DIRECT3D
  friend class GrD3DGpu;
  friend class GrD3DRenderTarget;
  GrBackendRenderTarget(
      int width, int height, int sampleCnt, const GrD3DTextureResourceInfo& d3dInfo,
      sk_sp<GrD3DResourceState> state);
  sk_sp<GrD3DResourceState> getGrD3DResourceState() const;
#  endif

  // Free and release and resources being held by the GrBackendTexture.
  void cleanup() noexcept;

  bool fIsValid;
  bool fFramebufferOnly = false;
  int fWidth;   //<! width in pixels
  int fHeight;  //<! height in pixels

  int fSampleCnt;
  int fStencilBits;

  GrBackendApi fBackend;

  union {
#  ifdef SK_GL
    GrGLFramebufferInfo fGLInfo;
#  endif
    GrVkBackendSurfaceInfo fVkInfo;
    GrMockRenderTargetInfo fMockInfo;
#  ifdef SK_DIRECT3D
    GrD3DBackendSurfaceInfo fD3DInfo;
#  endif
  };
#  ifdef SK_METAL
  GrMtlTextureInfo fMtlInfo;
#  endif
#  ifdef SK_DAWN
  GrDawnRenderTargetInfo fDawnInfo;
#  endif
  sk_sp<GrBackendSurfaceMutableStateImpl> fMutableState;
};

#endif

#endif
