/*
 * Copyright 2019 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef GrCpuBuffer_DEFINED
#define GrCpuBuffer_DEFINED

#include "src/gpu/GrBuffer.h"
#include "src/gpu/GrNonAtomicRef.h"

class GrCpuBuffer final : public GrNonAtomicRef<GrCpuBuffer>, public GrBuffer {
public:
    static sk_sp<GrCpuBuffer> Make(size_t size) {
        SkASSERT(size > 0);
        auto mem = ::operator new(sizeof(GrCpuBuffer) + size);
        return sk_sp<GrCpuBuffer>(new (mem) GrCpuBuffer((char*)mem + sizeof(GrCpuBuffer), size));
    }

    void ref() const noexcept override { GrNonAtomicRef<GrCpuBuffer>::ref(); }
    void unref() const noexcept override { GrNonAtomicRef<GrCpuBuffer>::unref(); }
    size_t size() const noexcept override { return fSize; }
    bool isCpuBuffer() const noexcept override { return true; }

    char* data() noexcept { return reinterpret_cast<char*>(fData); }
    const char* data() const noexcept { return reinterpret_cast<const char*>(fData); }

private:
    GrCpuBuffer(void* data, size_t size) noexcept : fData(data), fSize(size) {}
    void* fData;
    size_t fSize;
};

#endif
