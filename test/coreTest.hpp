#include "include/core/SkCanvas.h"
#include "include/core/SkFont.h"
#include "include/core/SkImage.h"
#include "include/core/SkPaint.h"
#include "include/core/SkPath.h"
#include "include/core/SkRRect.h"
#include "include/core/SkBitmap.h"
#include "include/core/SkString.h"
#include "include/core/SkString.h"
#include "src/core/SkAAClip.h"

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

static_assert(std::is_nothrow_default_constructible_v<SkAAClip>);
static_assert(std::is_nothrow_move_assignable_v<SkAAClip>);
static_assert(std::is_nothrow_move_constructible_v<SkAAClip>);

static_assert(std::is_nothrow_default_constructible_v<SkRegion>);
static_assert(std::is_nothrow_move_assignable_v<SkRegion>);
static_assert(std::is_nothrow_move_constructible_v<SkRegion>);

static_assert(std::is_nothrow_default_constructible_v<SkString>);
static_assert(std::is_nothrow_move_assignable_v<SkString>);
static_assert(std::is_nothrow_move_constructible_v<SkString>);

static_assert(std::is_nothrow_default_constructible_v<SkPaint>);
static_assert(std::is_nothrow_move_assignable_v<SkPaint>);
static_assert(std::is_nothrow_move_constructible_v<SkPaint>);

static_assert(std::is_nothrow_default_constructible_v<sk_sp<SkImage>>);
static_assert(std::is_nothrow_move_assignable_v<sk_sp<SkImage>>);
static_assert(std::is_nothrow_move_constructible_v<sk_sp<SkImage>>);
