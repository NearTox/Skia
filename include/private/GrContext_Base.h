/*
 * Copyright 2019 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef GrContext_Base_DEFINED
#define GrContext_Base_DEFINED

#include "include/core/SkRefCnt.h"
#include "include/gpu/GrBackendSurface.h"
#include "include/gpu/GrContextOptions.h"
#include "include/gpu/GrTypes.h"

class GrBaseContextPriv;
class GrCaps;
class GrContext;
class GrContextThreadSafeProxy;
class GrImageContext;
class GrRecordingContext;

class GrContext_Base : public SkRefCnt {
 public:
  virtual ~GrContext_Base();

  /*
   * The 3D API backing this context
   */
  SK_API GrBackendApi backend() const noexcept;

  /*
   * Retrieve the default GrBackendFormat for a given SkColorType and renderability.
   * It is guaranteed that this backend format will be the one used by the GrContext
   * SkColorType and SkSurfaceCharacterization-based createBackendTexture methods.
   *
   * The caller should check that the returned format is valid.
   */
  SK_API GrBackendFormat defaultBackendFormat(SkColorType, GrRenderable) const;

  SK_API GrBackendFormat compressedBackendFormat(SkImage::CompressionType) const;

  // TODO: When the public version is gone, rename to refThreadSafeProxy and add raw ptr ver.
  sk_sp<GrContextThreadSafeProxy> threadSafeProxy() noexcept;

  // Provides access to functions that aren't part of the public API.
  GrBaseContextPriv priv() noexcept;
  const GrBaseContextPriv priv() const noexcept;

 protected:
  friend class GrBaseContextPriv;  // for hidden functions

  GrContext_Base(sk_sp<GrContextThreadSafeProxy>) noexcept;

  virtual bool init();

  /**
   * An identifier for this context. The id is used by all compatible contexts. For example,
   * if SkImages are created on one thread using an image creation context, then fed into a
   * DDL Recorder on second thread (which has a recording context) and finally replayed on
   * a third thread with a direct context, then all three contexts will report the same id.
   * It is an error for an image to be used with contexts that report different ids.
   */
  uint32_t contextID() const noexcept;

  bool matches(GrContext_Base* candidate) const noexcept {
    return candidate->contextID() == this->contextID();
  }

  /*
   * The options in effect for this context
   */
  const GrContextOptions& options() const noexcept;

  const GrCaps* caps() const noexcept;
  sk_sp<const GrCaps> refCaps() const noexcept;

  virtual GrImageContext* asImageContext() noexcept { return nullptr; }
  virtual GrRecordingContext* asRecordingContext() noexcept { return nullptr; }
  virtual GrContext* asDirectContext() noexcept { return nullptr; }

  sk_sp<GrContextThreadSafeProxy> fThreadSafeProxy;

 private:
  typedef SkRefCnt INHERITED;
};

#endif
