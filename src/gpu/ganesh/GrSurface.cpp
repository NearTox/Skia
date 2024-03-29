/*
 * Copyright 2012 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "src/core/SkCompressedDataUtils.h"
#include "src/gpu/ganesh/GrBackendUtils.h"
#include "src/gpu/ganesh/GrRenderTarget.h"
#include "src/gpu/ganesh/GrResourceProvider.h"
#include "src/gpu/ganesh/GrSurface.h"
#include "src/gpu/ganesh/GrTexture.h"

#include "src/core/SkMathPriv.h"
#include "src/gpu/ganesh/SkGr.h"

size_t GrSurface::ComputeSize(
    const GrBackendFormat& format, SkISize dimensions, int colorSamplesPerPixel,
    GrMipmapped mipmapped, bool binSize) {
  // For external formats we do not actually know the real size of the resource so we just return
  // 0 here to indicate this.
  if (format.textureType() == GrTextureType::kExternal) {
    return 0;
  }

  size_t colorSize;

  if (binSize) {
    dimensions = GrResourceProvider::MakeApprox(dimensions);
  }

  SkImage::CompressionType compressionType = GrBackendFormatToCompressionType(format);
  if (compressionType != SkImage::CompressionType::kNone) {
    colorSize =
        SkCompressedFormatDataSize(compressionType, dimensions, mipmapped == GrMipmapped::kYes);
  } else {
    colorSize =
        (size_t)dimensions.width() * dimensions.height() * GrBackendFormatBytesPerPixel(format);
  }
  SkASSERT(colorSize > 0);

  size_t finalSize = colorSamplesPerPixel * colorSize;

  if (GrMipmapped::kYes == mipmapped) {
    // We don't have to worry about the mipmaps being a different dimensions than
    // we'd expect because we never change fDesc.fWidth/fHeight.
    finalSize += colorSize / 3;
  }
  return finalSize;
}

//////////////////////////////////////////////////////////////////////////////

void GrSurface::onRelease() {
  this->invokeReleaseProc();
  this->INHERITED::onRelease();
}

void GrSurface::onAbandon() {
  this->invokeReleaseProc();
  this->INHERITED::onAbandon();
}
