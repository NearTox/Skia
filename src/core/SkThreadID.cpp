/*
 * Copyright 2015 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "include/private/SkThreadID.h"
#include "include/private/SkLeanWindows.h"

#ifdef SK_BUILD_FOR_WIN
SkThreadID SkGetThreadID() noexcept { return GetCurrentThreadId(); }
#else
#include <pthread.h>
SkThreadID SkGetThreadID() { return (int64_t)pthread_self(); }
#endif
