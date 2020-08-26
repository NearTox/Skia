/*
 * Copyright 2019 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef SkZip_DEFINED
#define SkZip_DEFINED

#include <iterator>
#include <tuple>
#include <type_traits>
#include <utility>

#include "include/core/SkTypes.h"
#include "include/private/SkTemplates.h"
#include "include/private/SkTo.h"
#include "src/core/SkSpan.h"

// Take a list of things that can be pointers, and use them all in parallel. The iterators and
// accessor operator[] for the class produce a tuple of the items.
template <typename... Ts>
class SkZip {
  using ReturnTuple = std::tuple<Ts&...>;

  class Iterator {
   public:
    using value_type = ReturnTuple;
    using difference_type = ptrdiff_t;
    using pointer = value_type*;
    using reference = value_type;
    using iterator_category = std::input_iterator_tag;
    constexpr Iterator(const SkZip* zip, size_t index) noexcept : fZip{zip}, fIndex{index} {}
    constexpr Iterator(const Iterator& that) noexcept : Iterator{that.fZip, that.fIndex} {}
    constexpr Iterator& operator++() noexcept {
      ++fIndex;
      return *this;
    }
    constexpr Iterator operator++(int) noexcept {
      Iterator tmp(*this);
      operator++();
      return tmp;
    }
    constexpr bool operator==(const Iterator& rhs) const noexcept { return fIndex == rhs.fIndex; }
    constexpr bool operator!=(const Iterator& rhs) const noexcept { return fIndex != rhs.fIndex; }
    constexpr reference operator*() noexcept { return (*fZip)[fIndex]; }
    friend constexpr difference_type operator-(Iterator lhs, Iterator rhs) noexcept {
      return lhs.fIndex - rhs.fIndex;
    }

   private:
    const SkZip* const fZip = nullptr;
    size_t fIndex = 0;
  };

  template <typename T>
  static constexpr T* nullify = nullptr;

 public:
  constexpr SkZip() noexcept : fPointers{nullify<Ts>...}, fSize{0} {}
  constexpr SkZip(size_t) noexcept = delete;
  constexpr SkZip(size_t size, Ts*... ts) noexcept : fPointers{ts...}, fSize{size} {}
  constexpr SkZip(const SkZip& that) noexcept = default;

  // Check to see if U can be used for const T or is the same as T
  template <typename U, typename T>
  using CanConvertToConst = typename std::integral_constant<
      bool, std::is_convertible<U*, T*>::value && sizeof(U) == sizeof(T)>::type;

  // Allow SkZip<const T> to be constructed from SkZip<T>.
  template <
      typename... Us,
      typename = std::enable_if<skstd::conjunction<CanConvertToConst<Us, Ts>...>::value>>
  constexpr SkZip(const SkZip<Us...>& that) noexcept : fPointers(that.data()), fSize{that.size()} {}

  constexpr ReturnTuple operator[](size_t i) const noexcept { return this->index(i); }
  constexpr size_t size() const noexcept { return fSize; }
  constexpr bool empty() const noexcept { return this->size() == 0; }
  constexpr ReturnTuple front() const noexcept { return this->index(0); }
  constexpr ReturnTuple back() const noexcept { return this->index(this->size() - 1); }
  constexpr Iterator begin() const noexcept { return Iterator{this, 0}; }
  constexpr Iterator end() const noexcept { return Iterator{this, this->size()}; }
  template <size_t I>
  constexpr auto get() const noexcept {
    return SkMakeSpan(std::get<I>(fPointers), fSize);
  }
  constexpr std::tuple<Ts*...> data() const noexcept { return fPointers; }
  constexpr SkZip first(size_t n) const noexcept {
    SkASSERT(n <= this->size());
    if (n == 0) {
      return SkZip();
    }
    return SkZip{n, fPointers};
  }
  constexpr SkZip last(size_t n) const noexcept {
    SkASSERT(n <= this->size());
    if (n == 0) {
      return SkZip();
    }
    return SkZip{n, this->pointersAt(fSize - n)};
  }
  constexpr SkZip subspan(size_t offset, size_t count) const noexcept {
    SkASSERT(offset < this->size());
    SkASSERT(count <= this->size() - offset);
    if (count == 0) {
      return SkZip();
    }
    return SkZip(count, pointersAt(offset));
  }

 private:
  constexpr SkZip(size_t n, const std::tuple<Ts*...>& pointers) noexcept
      : fPointers{pointers}, fSize{n} {}

  constexpr ReturnTuple index(size_t i) const noexcept {
    SkASSERT(this->size() > 0);
    SkASSERT(i < this->size());
    return indexDetail(i, std::make_index_sequence<sizeof...(Ts)>{});
  }

  template <std::size_t... Is>
  constexpr ReturnTuple indexDetail(size_t i, std::index_sequence<Is...>) const noexcept {
    return ReturnTuple((std::get<Is>(fPointers))[i]...);
  }

  std::tuple<Ts*...> pointersAt(size_t i) const {
    SkASSERT(this->size() > 0);
    SkASSERT(i < this->size());
    return pointersAtDetail(i, std::make_index_sequence<sizeof...(Ts)>{});
  }

  template <std::size_t... Is>
  constexpr std::tuple<Ts*...> pointersAtDetail(
      size_t i, std::index_sequence<Is...>) const noexcept {
    return std::tuple<Ts*...>{&(std::get<Is>(fPointers))[i]...};
  }

  std::tuple<Ts*...> fPointers;
  size_t fSize;
};

class SkMakeZipDetail {
  template <typename T>
  struct DecayPointer {
    using U = typename std::remove_cv<typename std::remove_reference<T>::type>::type;
    using type = typename std::conditional<std::is_pointer<U>::value, U, T>::type;
  };
  template <typename T>
  using DecayPointerT = typename DecayPointer<T>::type;

  template <typename C>
  struct ContiguousMemory {};
  template <typename T>
  struct ContiguousMemory<T*> {
    using value_type = T;
    static constexpr value_type* Data(T* t) noexcept { return t; }
    static constexpr size_t Size(T* s) noexcept { return SIZE_MAX; }
  };
  template <typename T, size_t N>
  struct ContiguousMemory<T (&)[N]> {
    using value_type = T;
    static constexpr value_type* Data(T (&t)[N]) noexcept { return t; }
    static constexpr size_t Size(T (&)[N]) noexcept { return N; }
  };
  // In general, we don't want r-value collections, but SkSpans are ok, because they are a view
  // onto an actual container.
  template <typename T>
  struct ContiguousMemory<SkSpan<T>> {
    using value_type = T;
    static constexpr value_type* Data(SkSpan<T> s) noexcept { return s.data(); }
    static constexpr size_t Size(SkSpan<T> s) noexcept { return s.size(); }
  };
  // Only accept l-value references to collections.
  template <typename C>
  struct ContiguousMemory<C&> {
    using value_type = typename std::remove_pointer<decltype(std::declval<C>().data())>::type;
    static constexpr value_type* Data(C& c) noexcept { return c.data(); }
    static constexpr size_t Size(C& c) noexcept { return c.size(); }
  };
  template <typename C>
  using Span = ContiguousMemory<DecayPointerT<C>>;
  template <typename C>
  using ValueType = typename Span<C>::value_type;

  template <typename C, typename... Ts>
  struct PickOneSize {};
  template <typename T, typename... Ts>
  struct PickOneSize<T*, Ts...> {
    static constexpr size_t Size(T* t, Ts... ts) noexcept {
      return PickOneSize<Ts...>::Size(std::forward<Ts>(ts)...);
    }
  };
  template <typename T, typename... Ts, size_t N>
  struct PickOneSize<T (&)[N], Ts...> {
    static constexpr size_t Size(T (&)[N], Ts...) noexcept { return N; }
  };
  template <typename T, typename... Ts>
  struct PickOneSize<SkSpan<T>, Ts...> {
    static constexpr size_t Size(SkSpan<T> s, Ts...) noexcept { return s.size(); }
  };
  template <typename C, typename... Ts>
  struct PickOneSize<C&, Ts...> {
    static constexpr size_t Size(C& c, Ts...) noexcept { return c.size(); }
  };

 public:
  template <typename... Ts>
  static constexpr auto MakeZip(Ts&&... ts) noexcept {
    // Pick the first collection that has a size, and use that for the size.
    size_t size = PickOneSize<DecayPointerT<Ts>...>::Size(std::forward<Ts>(ts)...);

#ifdef SK_DEBUG
    // Check that all sizes are the same.
    size_t minSize = SIZE_MAX;
    size_t maxSize = 0;
    for (size_t s : {Span<Ts>::Size(std::forward<Ts>(ts))...}) {
      if (s != SIZE_MAX) {
        minSize = std::min(minSize, s);
        maxSize = std::max(maxSize, s);
      }
    }
    SkASSERT(minSize == maxSize);
#endif

    return SkZip<ValueType<Ts>...>{size, Span<Ts>::Data(std::forward<Ts>(ts))...};
  }
};

template <typename... Ts>
template <typename T>
constexpr T* SkZip<Ts...>::nullify;

template <typename... Ts>
inline constexpr auto SkMakeZip(Ts&&... ts) noexcept {
  return SkMakeZipDetail::MakeZip(std::forward<Ts>(ts)...);
}
#endif  // SkZip_DEFINED
