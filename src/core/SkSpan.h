/*
 * Copyright 2018 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef SkSpan_DEFINED
#define SkSpan_DEFINED

#include <cstddef>
#include "include/private/SkTo.h"

template <typename T>
class SkSpan {
 public:
  constexpr SkSpan() noexcept : fPtr{nullptr}, fSize{0} {}
  constexpr SkSpan(T* ptr, size_t size) noexcept : fPtr{ptr}, fSize{size} {}
  template <typename U, typename = typename std::enable_if<std::is_same<const U, T>::value>::type>
  constexpr SkSpan(const SkSpan<U>& that) noexcept : fPtr(that.data()), fSize{that.size()} {}
  constexpr SkSpan(const SkSpan& o) noexcept = default;
  constexpr SkSpan& operator=(const SkSpan& that) noexcept {
    fPtr = that.fPtr;
    fSize = that.fSize;
    return *this;
  }
  constexpr T& operator[](size_t i) const noexcept {
    SkASSERT(i < this->size());
    return fPtr[i];
  }
  constexpr T& front() const noexcept { return fPtr[0]; }
  constexpr T& back() const noexcept { return fPtr[fSize - 1]; }
  constexpr T* begin() const noexcept { return fPtr; }
  constexpr T* end() const noexcept { return fPtr + fSize; }
  constexpr const T* cbegin() const noexcept { return this->begin(); }
  constexpr const T* cend() const noexcept { return this->end(); }
  constexpr auto rbegin() const noexcept { return std::make_reverse_iterator(this->end()); }
  constexpr auto rend() const noexcept { return std::make_reverse_iterator(this->begin()); }
  constexpr auto crbegin() const noexcept { return std::make_reverse_iterator(this->cend()); }
  constexpr auto crend() const noexcept { return std::make_reverse_iterator(this->cbegin()); }
  constexpr T* data() const noexcept { return fPtr; }
  constexpr int count() const noexcept { return SkTo<int>(fSize); }
  constexpr size_t size() const noexcept { return fSize; }
  constexpr bool empty() const noexcept { return fSize == 0; }
  constexpr size_t size_bytes() const noexcept { return fSize * sizeof(T); }
  constexpr SkSpan<T> first(size_t prefixLen) const noexcept {
    SkASSERT(prefixLen <= this->size());
    return SkSpan{fPtr, prefixLen};
  }
  constexpr SkSpan<T> last(size_t postfixLen) const noexcept {
    SkASSERT(postfixLen <= this->size());
    return SkSpan{fPtr + (this->size() - postfixLen), postfixLen};
  }
  constexpr SkSpan<T> subspan(size_t offset, size_t count) const noexcept {
    SkASSERT(offset <= this->size());
    SkASSERT(count <= this->size() - offset);
    return SkSpan{fPtr + offset, count};
  }

 private:
  T* fPtr;
  size_t fSize;
};

template <typename T, typename S>
inline constexpr SkSpan<T> SkMakeSpan(T* p, S s) noexcept {
  return SkSpan<T>{p, SkTo<size_t>(s)};
}

template <size_t N, typename T>
inline constexpr SkSpan<T> SkMakeSpan(T (&a)[N]) noexcept {
  return SkSpan<T>{a, N};
}

template <typename Container>
inline auto SkMakeSpan(Container& c)
    -> SkSpan<typename std::remove_reference<decltype(*(c.data()))>::type> {
  return {c.data(), c.size()};
}
#endif  // SkSpan_DEFINED
