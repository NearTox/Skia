/*
 * Copyright 2022 Google LLC
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef skgpu_graphite_TextureProxyView_DEFINED
#define skgpu_graphite_TextureProxyView_DEFINED

#include "include/core/SkRect.h"
#include "include/core/SkRefCnt.h"
#include "include/gpu/graphite/GraphiteTypes.h"
#include "src/gpu/Swizzle.h"
#include "src/gpu/graphite/TextureProxy.h"

namespace skgpu::graphite {

class Recorder;

class TextureProxyView {
 public:
  TextureProxyView() = default;

  TextureProxyView(sk_sp<TextureProxy> proxy, Swizzle swizzle)
      : fProxy(std::move(proxy)), fSwizzle(swizzle) {}

  // This entry point is used when we don't care about the swizzle.
  explicit TextureProxyView(sk_sp<TextureProxy> proxy) : fProxy(std::move(proxy)) {}

  TextureProxyView(TextureProxyView&& view) = default;
  TextureProxyView(const TextureProxyView&) = default;

  explicit operator bool() const { return SkToBool(fProxy.get()); }

  TextureProxyView& operator=(const TextureProxyView&) = default;
  TextureProxyView& operator=(TextureProxyView&& view) = default;

  bool operator==(const TextureProxyView& view) const {
    return fProxy == view.fProxy && fSwizzle == view.fSwizzle;
  }
  bool operator!=(const TextureProxyView& other) const { return !(*this == other); }

  int width() const { return this->proxy()->dimensions().width(); }
  int height() const { return this->proxy()->dimensions().height(); }
  SkISize dimensions() const { return this->proxy()->dimensions(); }

  Mipmapped mipmapped() const {
    if (const TextureProxy* proxy = this->proxy()) {
      return proxy->mipmapped();
    }
    return Mipmapped::kNo;
  }

  TextureProxy* proxy() const { return fProxy.get(); }
  sk_sp<TextureProxy> refProxy() const { return fProxy; }

  Swizzle swizzle() const { return fSwizzle; }

  void concatSwizzle(Swizzle swizzle) { fSwizzle = skgpu::Swizzle::Concat(fSwizzle, swizzle); }

  TextureProxyView makeSwizzle(Swizzle swizzle) const& {
    return {fProxy, Swizzle::Concat(fSwizzle, swizzle)};
  }

  TextureProxyView makeSwizzle(Swizzle swizzle) && {
    return {std::move(fProxy), Swizzle::Concat(fSwizzle, swizzle)};
  }

  void reset() { *this = {}; }

  // Helper that copies a rect of a src view's proxy and then creates a view for the copy with
  // the same swizzle as the src view.
  static TextureProxyView Copy(
      Recorder* recorder, TextureProxyView src, Mipmapped mipmapped, SkIRect srcRect,
      SkBackingFit fit, SkBudgeted budgeted) {
    // TODO
    return {};
  }

  static TextureProxyView Copy(
      Recorder* recorder, TextureProxyView src, Mipmapped mipmapped, SkBackingFit fit,
      SkBudgeted budgeted) {
    return TextureProxyView::Copy(
        recorder, src, mipmapped, SkIRect::MakeSize(src.proxy()->dimensions()), fit, budgeted);
  }

  // This does not reset the swizzle, so the View can still be used to access those
  // properties associated with the detached proxy.
  sk_sp<TextureProxy> detachProxy() { return std::move(fProxy); }

 private:
  sk_sp<TextureProxy> fProxy;
  Swizzle fSwizzle;
};

}  // namespace skgpu::graphite

#endif  // skgpu_graphite_TextureProxyView_DEFINED
