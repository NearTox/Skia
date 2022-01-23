#include "include/core/SkCanvas.h"
#include "include/core/SkFont.h"
#include "include/core/SkImage.h"
#include "include/core/SkPaint.h"
#include "include/core/SkPath.h"
#include "include/core/SkRRect.h"
#include "include/core/SkRegion.h"

static_assert(std::is_nothrow_default_constructible_v<SkFont>);
static_assert(std::is_nothrow_move_assignable_v<SkFont>);
static_assert(std::is_nothrow_move_constructible_v<SkFont>);

static_assert(std::is_nothrow_default_constructible_v<SkPath>);
static_assert(std::is_nothrow_move_assignable_v<SkPath>);
static_assert(std::is_nothrow_move_constructible_v<SkPath>);

static_assert(std::is_nothrow_default_constructible_v<SkBitmap>);
static_assert(std::is_nothrow_move_assignable_v<SkBitmap>);
static_assert(std::is_nothrow_move_constructible_v<SkBitmap>);

static_assert(std::is_nothrow_default_constructible_v<SkRect>);
static_assert(std::is_nothrow_move_assignable_v<SkRect>);
static_assert(std::is_nothrow_move_constructible_v<SkRect>);

static_assert(std::is_nothrow_default_constructible_v<SkRRect>);
static_assert(std::is_nothrow_move_assignable_v<SkRRect>);
static_assert(std::is_nothrow_move_constructible_v<SkRRect>);

static_assert(std::is_nothrow_default_constructible_v<SkMatrix>);
static_assert(std::is_nothrow_move_assignable_v<SkMatrix>);
static_assert(std::is_nothrow_move_constructible_v<SkMatrix>);

static_assert(std::is_nothrow_default_constructible_v<SkRegion>);
static_assert(std::is_nothrow_move_assignable_v<SkRegion>);
static_assert(std::is_nothrow_move_constructible_v<SkRegion>);

static_assert(std::is_nothrow_default_constructible_v<SkString>);
static_assert(std::is_nothrow_move_assignable_v<SkString>);
static_assert(std::is_nothrow_move_constructible_v<SkString>);

static_assert(std::is_nothrow_default_constructible_v<SkCanvas>);
static_assert(std::is_nothrow_move_assignable_v<SkBitmap>);
static_assert(std::is_nothrow_move_constructible_v<SkBitmap>);

static_assert(std::is_nothrow_default_constructible_v<sk_sp<SkImage>>);
static_assert(std::is_nothrow_move_assignable_v<sk_sp<SkImage>>);
static_assert(std::is_nothrow_move_constructible_v<sk_sp<SkImage>>);
