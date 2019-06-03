/*
 * Copyright 2014 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef GrGpuResourceCacheAccess_DEFINED
#define GrGpuResourceCacheAccess_DEFINED

#include "include/gpu/GrGpuResource.h"
#include "src/gpu/GrGpuResourcePriv.h"

namespace skiatest {
class Reporter;
}

/**
 * This class allows GrResourceCache increased privileged access to GrGpuResource objects.
 */
class GrGpuResource::CacheAccess {
private:
    /** The cache is allowed to go from no refs to 1 ref. */
    void ref() noexcept { fResource->addInitialRef(); }

    /**
     * Is the resource currently cached as scratch? This means it is cached, has a valid scratch
     * key, and does not have a unique key.
     */
    bool isScratch() const noexcept {
        return !fResource->getUniqueKey().isValid() && fResource->fScratchKey.isValid() &&
               GrBudgetedType::kBudgeted == fResource->resourcePriv().budgetedType();
    }

    /**
     * Called by the cache to delete the resource under normal circumstances.
     */
    void release() {
        fResource->release();
        if (!fResource->hasRefOrPendingIO()) {
            delete fResource;
        }
    }

    /**
     * Called by the cache to delete the resource when the backend 3D context is no longer valid.
     */
    void abandon() {
        fResource->abandon();
        if (!fResource->hasRefOrPendingIO()) {
            delete fResource;
        }
    }

    /** Called by the cache to assign a new unique key. */
    void setUniqueKey(const GrUniqueKey& key) noexcept { fResource->fUniqueKey = key; }

    /** Is the resource ref'ed (not counting pending IOs). */
    bool hasRef() const noexcept { return fResource->hasRef(); }

    /** Called by the cache to make the unique key invalid. */
    void removeUniqueKey() noexcept { fResource->fUniqueKey.reset(); }

    uint32_t timestamp() const noexcept { return fResource->fTimestamp; }
    void setTimestamp(uint32_t ts) noexcept { fResource->fTimestamp = ts; }

    void setTimeWhenResourceBecomePurgeable() noexcept {
        SkASSERT(fResource->isPurgeable());
        fResource->fTimeWhenBecamePurgeable = GrStdSteadyClock::now();
    }
    /**
     * Called by the cache to determine whether this resource should be purged based on the length
     * of time it has been available for purging.
     */
    GrStdSteadyClock::time_point timeWhenResourceBecamePurgeable() noexcept {
        SkASSERT(fResource->isPurgeable());
        return fResource->fTimeWhenBecamePurgeable;
    }

    int* accessCacheIndex() const noexcept { return &fResource->fCacheArrayIndex; }

    CacheAccess(GrGpuResource* resource) noexcept : fResource(resource) {}
    CacheAccess(const CacheAccess& that) noexcept : fResource(that.fResource) {}
    CacheAccess& operator=(const CacheAccess&);  // unimpl

    // No taking addresses of this type.
    const CacheAccess* operator&() const = delete;
    CacheAccess* operator&() = delete;

    GrGpuResource* fResource;

    friend class GrGpuResource;    // to construct/copy this type.
    friend class GrResourceCache;  // to use this type
    friend void test_unbudgeted_to_scratch(skiatest::Reporter* reporter);  // for unit testing
};

inline GrGpuResource::CacheAccess GrGpuResource::cacheAccess() noexcept {
    return CacheAccess(this);
}

inline const GrGpuResource::CacheAccess GrGpuResource::cacheAccess() const noexcept {
    return CacheAccess(const_cast<GrGpuResource*>(this));
}

#endif
