// Copyright 2019 Google LLC.
#ifndef FontCollection_DEFINED
#define FontCollection_DEFINED

#include <memory>
#include <set>
#include "include/core/SkFontMgr.h"
#include "include/core/SkRefCnt.h"
#include "include/private/SkTHash.h"
#include "modules/skparagraph/include/ParagraphCache.h"
#include "modules/skparagraph/include/TextStyle.h"

namespace skia {
namespace textlayout {

class TextStyle;
class Paragraph;
class FontCollection : public SkRefCnt {
 public:
  FontCollection() noexcept;

  size_t getFontManagersCount() const;

  void setAssetFontManager(sk_sp<SkFontMgr> fontManager) noexcept;
  void setDynamicFontManager(sk_sp<SkFontMgr> fontManager) noexcept;
  void setTestFontManager(sk_sp<SkFontMgr> fontManager) noexcept;
  void setDefaultFontManager(sk_sp<SkFontMgr> fontManager) noexcept;
  void setDefaultFontManager(sk_sp<SkFontMgr> fontManager, const char defaultFamilyName[]);

  sk_sp<SkFontMgr> getFallbackManager() const noexcept { return fDefaultFontManager; }

  std::vector<sk_sp<SkTypeface>> findTypefaces(
      const std::vector<SkString>& familyNames, SkFontStyle fontStyle);

  sk_sp<SkTypeface> defaultFallback(
      SkUnichar unicode, SkFontStyle fontStyle, const SkString& locale);
  sk_sp<SkTypeface> defaultFallback();

  void disableFontFallback() noexcept;
  void enableFontFallback() noexcept;
  bool fontFallbackEnabled() noexcept { return fEnableFontFallback; }

  ParagraphCache* getParagraphCache() noexcept { return &fParagraphCache; }

 private:
  std::vector<sk_sp<SkFontMgr>> getFontManagerOrder() const;

  sk_sp<SkTypeface> matchTypeface(const SkString& familyName, SkFontStyle fontStyle);

  struct FamilyKey {
    FamilyKey(const std::vector<SkString>& familyNames, SkFontStyle style)
        : fFamilyNames(familyNames), fFontStyle(style) {}

    FamilyKey() noexcept = default;

    std::vector<SkString> fFamilyNames;
    SkFontStyle fFontStyle;

    bool operator==(const FamilyKey& other) const noexcept;

    struct Hasher {
      size_t operator()(const FamilyKey& key) const noexcept;
    };
  };

  bool fEnableFontFallback;
  SkTHashMap<FamilyKey, std::vector<sk_sp<SkTypeface>>, FamilyKey::Hasher> fTypefaces;
  sk_sp<SkFontMgr> fDefaultFontManager;
  sk_sp<SkFontMgr> fAssetFontManager;
  sk_sp<SkFontMgr> fDynamicFontManager;
  sk_sp<SkFontMgr> fTestFontManager;

  SkString fDefaultFamilyName;
  ParagraphCache fParagraphCache;
};
}  // namespace textlayout
}  // namespace skia

#endif  // FontCollection_DEFINED
