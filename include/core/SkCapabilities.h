/*
 * Copyright 2022 Google LLC
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef SkCapabilities_DEFINED
#define SkCapabilities_DEFINED

#include "include/core/SkRefCnt.h"

#ifdef SK_ENABLE_SKSL
#  include "include/sksl/SkSLVersion.h"
namespace SkSL {
struct ShaderCaps;
}
#endif

class SK_API SkCapabilities : public SkRefCnt {
 public:
  static sk_sp<const SkCapabilities> RasterBackend();

#ifdef SK_ENABLE_SKSL
  SkSL::Version skslVersion() const noexcept { return fSkSLVersion; }
#endif

 protected:
  constexpr SkCapabilities() noexcept = default;

#ifdef SK_ENABLE_SKSL
  void initSkCaps(const SkSL::ShaderCaps*);

  SkSL::Version fSkSLVersion = SkSL::Version::k100;
#endif
};

#endif
