/*
 * Copyright 2017 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef SKSL_STRING
#define SKSL_STRING

#include <cstring>

#define SKSL_USE_STD_STRING

#include <stdarg.h>

#ifdef SKSL_USE_STD_STRING
#define SKSL_STRING_BASE std::string
#include <string>
#else
#define SKSL_STRING_BASE SkString
#include "include/core/SkString.h"
#endif

namespace SkSL {

// Represents a (not necessarily null-terminated) slice of a string.
struct StringFragment {
  StringFragment() noexcept : fChars(""), fLength(0) {}

  StringFragment(const char* chars) noexcept : fChars(chars), fLength(strlen(chars)) {}

  StringFragment(const char* chars, size_t length) noexcept : fChars(chars), fLength(length) {}

  char operator[](size_t idx) const noexcept { return fChars[idx]; }

  bool operator==(const char* s) const noexcept;
  bool operator!=(const char* s) const noexcept;
  bool operator==(StringFragment s) const noexcept;
  bool operator!=(StringFragment s) const noexcept;
  bool operator<(StringFragment s) const noexcept;

  const char* fChars;
  size_t fLength;
};

bool operator==(const char* s1, StringFragment s2) noexcept;

bool operator!=(const char* s1, StringFragment s2) noexcept;

class String : public SKSL_STRING_BASE {
 public:
  String() noexcept(std::is_nothrow_default_constructible_v<INHERITED>) = default;
  String(const String&) = default;
  String(String&&) noexcept(std::is_nothrow_move_constructible_v<INHERITED>) = default;
  String& operator=(const String&) = default;
  String& operator=(String&&) noexcept(std::is_nothrow_move_assignable_v<INHERITED>) = default;

#ifndef SKSL_USE_STD_STRING
  String(const SkString& s) : INHERITED(s) {}
#endif

  String(const char* s) : INHERITED(s) {}

  String(const char* s, size_t size) : INHERITED(s, size) {}

  String(StringFragment s) : INHERITED(s.fChars, s.fLength) {}

  static String printf(const char* fmt, ...);

#ifdef SKSL_USE_STD_STRING
  void appendf(const char* fmt, ...);
  // For API compatibility with SkString's reset (vs. std:string's clear)
  void reset() noexcept;
  // For API compatibility with SkString's findLastOf(vs. find_last_of -> size_t)
  int findLastOf(const char c) const noexcept;
#endif
  void vappendf(const char* fmt, va_list va);

  bool startsWith(const char* s) const noexcept;
  bool endsWith(const char* s) const noexcept;

  int find(const char* substring, int fromPos = 0) const noexcept;
  int find(const String& substring, int fromPos = 0) const noexcept;

  String operator+(const char* s) const;
  String operator+(const String& s) const;
  String operator+(StringFragment s) const;
  String& operator+=(char c);
  String& operator+=(const char* s);
  String& operator+=(const String& s);
  String& operator+=(StringFragment s);
  bool operator==(const char* s) const noexcept;
  bool operator!=(const char* s) const noexcept;
  bool operator==(const String& s) const noexcept;
  bool operator!=(const String& s) const noexcept;
  friend String operator+(const char* s1, const String& s2);
  friend bool operator==(const char* s1, const String& s2) noexcept;
  friend bool operator!=(const char* s1, const String& s2) noexcept;

 private:
  typedef SKSL_STRING_BASE INHERITED;
};

String operator+(const char* s1, const String& s2);
bool operator!=(const char* s1, const String& s2) noexcept;

String to_string(double value);

String to_string(int32_t value);

String to_string(uint32_t value);

String to_string(int64_t value);

String to_string(uint64_t value);

int stoi(const String& s) noexcept;

double stod(const String& s);

long stol(const String& s) noexcept;

}  // namespace SkSL

namespace std {
template <>
struct hash<SkSL::StringFragment> {
  size_t operator()(const SkSL::StringFragment& s) const noexcept {
    size_t result = 0;
    for (size_t i = 0; i < s.fLength; ++i) {
      result = result * 101 + s.fChars[i];
    }
    return result;
  }
};
}  // namespace std

#ifdef SKSL_USE_STD_STRING
namespace std {
template <>
struct hash<SkSL::String> {
  size_t operator()(const SkSL::String& s) const noexcept { return hash<std::string>{}(s); }
};
}  // namespace std
#else
#include "src/core/SkOpts.h"
namespace std {
template <>
struct hash<SkSL::String> {
  size_t operator()(const SkSL::String& s) const { return SkOpts::hash_fn(s.c_str(), s.size(), 0); }
};
}  // namespace std
#endif  // SKIA_STANDALONE

#endif
