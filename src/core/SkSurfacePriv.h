/*
 * Copyright 2014 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef SkSurfacePriv_DEFINED
#define SkSurfacePriv_DEFINED

#include "include/core/SkSurfaceProps.h"

struct SkImageInfo;

static inline SkSurfaceProps SkSurfacePropsCopyOrDefault(const SkSurfaceProps* props) noexcept {
    if (props) {
        return *props;
    } else {
        return SkSurfaceProps(SkSurfaceProps::kLegacyFontHost_InitType);
    }
}

constexpr size_t kIgnoreRowBytesValue = static_cast<size_t>(~0);

bool SkSurfaceValidateRasterInfo(const SkImageInfo&, size_t rb = kIgnoreRowBytesValue);

#endif
