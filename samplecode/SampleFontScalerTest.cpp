/*
 * Copyright 2011 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */
#include "Sample.h"
#include "Sk1DPathEffect.h"
#include "SkCanvas.h"
#include "SkColorFilter.h"
#include "SkColorPriv.h"
#include "SkCornerPathEffect.h"
#include "SkDither.h"
#include "SkFontStyle.h"
#include "SkPath.h"
#include "SkPathMeasure.h"
#include "SkRandom.h"
#include "SkRegion.h"
#include "SkShader.h"
#include "SkTypeface.h"
#include "SkUTF.h"

static constexpr struct {
    const char* fName;
    SkFontStyle fStyle;
} gFaces[] = {
    { nullptr, SkFontStyle::Normal() },
    { nullptr, SkFontStyle::Bold() },
    { "serif", SkFontStyle::Normal() },
    { "serif", SkFontStyle::Bold() },
    { "serif", SkFontStyle::Italic() },
    { "serif", SkFontStyle::BoldItalic() },
    { "monospace", SkFontStyle::Normal() }
};

static const int gFaceCount = SK_ARRAY_COUNT(gFaces);

class FontScalerTestView : public Sample {
    sk_sp<SkTypeface> fFaces[gFaceCount];

public:
    FontScalerTestView() {
        for (int i = 0; i < gFaceCount; i++) {
            fFaces[i] = SkTypeface::MakeFromName(gFaces[i].fName, gFaces[i].fStyle);
        }
    }

protected:
    virtual bool onQuery(Sample::Event* evt) {
        if (Sample::TitleQ(*evt)) {
            Sample::TitleR(evt, "FontScaler Test");
            return true;
        }
        return this->INHERITED::onQuery(evt);
    }

    virtual void onDrawContent(SkCanvas* canvas) {
        SkPaint paint;

        // test handling of obscene cubic values (currently broken)
        if (false) {
            SkPoint pts[4];
            pts[0].set(1.61061274e+09f, 6291456);
            pts[1].set(-7.18397061e+15f,
                       -1.53091184e+13f);
            pts[2].set(-1.30077315e+16f,
                       -2.77196141e+13f);
            pts[3].set(-1.30077315e+16f,
                       -2.77196162e+13f);

            SkPath path;
            path.moveTo(pts[0]);
            path.cubicTo(pts[1], pts[2], pts[3]);
            canvas->drawPath(path, paint);
        }

//        paint.setSubpixelText(true);
        paint.setAntiAlias(true);
        paint.setLCDRenderText(true);
        paint.setTypeface(SkTypeface::MakeFromName("Times Roman", SkFontStyle()));

//        const char* text = "abcdefghijklmnopqrstuvwxyz";
        const char* text = "Hamburgefons ooo mmm";
        const size_t textLen = strlen(text);

        for (int j = 0; j < 2; ++j) {
            for (int i = 0; i < 6; ++i) {
                SkScalar x = SkIntToScalar(10);
                SkScalar y = SkIntToScalar(20);

                SkAutoCanvasRestore acr(canvas, true);
                canvas->translate(SkIntToScalar(50 + i * 230),
                                  SkIntToScalar(20));
                canvas->rotate(SkIntToScalar(i * 5), x, y * 10);

                {
                    SkPaint p;
                    p.setAntiAlias(true);
                    SkRect r;
                    r.set(x-3, 15, x-1, 280);
                    canvas->drawRect(r, p);
                }

                int index = 0;
                for (int ps = 6; ps <= 22; ps++) {
                    paint.setTextSize(SkIntToScalar(ps));
                    canvas->drawText(text, textLen, x, y, paint);
                    y += paint.getFontMetrics(nullptr);
                    index += 1;
                }
            }
            canvas->translate(0, 400);
            paint.setSubpixelText(true);
        }
    }

private:
    typedef Sample INHERITED;
};

//////////////////////////////////////////////////////////////////////////////

DEF_SAMPLE( return new FontScalerTestView(); )
