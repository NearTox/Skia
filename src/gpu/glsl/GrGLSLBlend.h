/*
 * Copyright 2015 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef GrGLBlend_DEFINED
#define GrGLBlend_DEFINED

#include "include/core/SkBlendMode.h"

class GrGLSLShaderBuilder;

namespace GrGLSLBlend {
/*
 * Returns the name of the built in blend function for a SkBlendMode.
 * When and if the SkSL compiler supports inlining it'd be simpler to just call
 * blend(mode, src, dst) where mode is a literal when the desired blend mode is
 * not variable.
 */
const char* BlendFuncName(SkBlendMode mode);

/*
 * Appends GLSL code to fsBuilder that assigns a specified blend of the srcColor and dstColor
 * variables to the outColor variable.
 */
void AppendMode(
    GrGLSLShaderBuilder* fsBuilder, const char* srcColor, const char* dstColor,
    const char* outColor, SkBlendMode mode);
}  // namespace GrGLSLBlend

#endif
