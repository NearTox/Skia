#include "include/core/SkCanvas.h"
#include "include/core/SkFont.h"
#include "include/core/SkImage.h"
#include "include/core/SkPaint.h"
#include "include/core/SkPath.h"
#include "include/core/SkRRect.h"

static_assert(std::is_nothrow_default_constructible_v<SkFont> == true, "");
static_assert(std::is_nothrow_move_assignable_v<SkFont> == true, "");
static_assert(std::is_nothrow_move_constructible_v<SkFont> == true, "");
static_assert(std::is_nothrow_copy_assignable_v<SkFont> == true, "");
static_assert(std::is_nothrow_copy_constructible_v<SkFont> == true, "");

static_assert(std::is_nothrow_default_constructible_v<SkPath> == true, "");
static_assert(std::is_nothrow_move_assignable_v<SkPath> == true, "");
static_assert(std::is_nothrow_move_constructible_v<SkPath> == true, "");
static_assert(std::is_nothrow_copy_assignable_v<SkPath> == true, "");
static_assert(std::is_nothrow_copy_constructible_v<SkPath> == true, "");

static_assert(std::is_nothrow_default_constructible_v<SkRect> == true, "");
static_assert(std::is_nothrow_move_assignable_v<SkRect> == true, "");
static_assert(std::is_nothrow_move_constructible_v<SkRect> == true, "");
static_assert(std::is_nothrow_copy_assignable_v<SkRect> == true, "");
static_assert(std::is_nothrow_copy_constructible_v<SkRect> == true, "");

static_assert(std::is_nothrow_default_constructible_v<SkRRect> == true, "");
static_assert(std::is_nothrow_move_assignable_v<SkRRect> == true, "");
static_assert(std::is_nothrow_move_constructible_v<SkRRect> == true, "");
static_assert(std::is_nothrow_copy_assignable_v<SkRRect> == true, "");
static_assert(std::is_nothrow_copy_constructible_v<SkRRect> == true, "");

static_assert(std::is_nothrow_default_constructible_v<SkMatrix> == true, "");
static_assert(std::is_nothrow_move_assignable_v<SkMatrix> == true, "");
static_assert(std::is_nothrow_move_constructible_v<SkMatrix> == true, "");
static_assert(std::is_nothrow_copy_assignable_v<SkMatrix> == true, "");
static_assert(std::is_nothrow_copy_constructible_v<SkMatrix> == true, "");

static_assert(std::is_nothrow_default_constructible_v<SkBitmap> == true, "");
static_assert(std::is_nothrow_move_assignable_v<SkBitmap> == true, "");
static_assert(std::is_nothrow_move_constructible_v<SkBitmap> == true, "");
static_assert(std::is_nothrow_copy_assignable_v<SkBitmap> == true, "");
static_assert(std::is_nothrow_copy_constructible_v<SkBitmap> == true, "");

static_assert(std::is_nothrow_default_constructible_v<sk_sp<SkImage>> == true, "");
static_assert(std::is_nothrow_move_assignable_v<sk_sp<SkImage>> == true, "");
static_assert(std::is_nothrow_move_constructible_v<sk_sp<SkImage>> == true, "");
static_assert(std::is_nothrow_copy_assignable_v<sk_sp<SkImage>> == true, "");
static_assert(std::is_nothrow_copy_constructible_v<sk_sp<SkImage>> == true, "");

static_assert(std::is_nothrow_default_constructible_v<SkPaint> == true, "");
static_assert(std::is_nothrow_move_assignable_v<SkPaint> == true, "");
static_assert(std::is_nothrow_move_constructible_v<SkPaint> == true, "");
static_assert(std::is_nothrow_copy_assignable_v<SkPaint> == true, "");
static_assert(std::is_nothrow_copy_constructible_v<SkPaint> == true, "");