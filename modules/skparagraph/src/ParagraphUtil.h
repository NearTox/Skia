// Copyright 2020 Google LLC.
#ifndef ParagraphUtil_DEFINED
#define ParagraphUtil_DEFINED

#include "include/core/SkString.h"
#include <string>

namespace skia {
namespace textlayout {
SkString SkStringFromU16String(const std::u16string& utf16text);
SkUnichar nextUtf8Unit(const char** ptr, const char* end) noexcept;
bool isControl(SkUnichar utf8) noexcept;
bool isWhitespace(SkUnichar utf8) noexcept;
}  // namespace textlayout
}  // namespace skia

#endif
