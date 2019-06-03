/*
 * Copyright 2008 The Android Open Source Project
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef SkReader32_DEFINED
#define SkReader32_DEFINED

#include "include/core/SkData.h"
#include "include/core/SkMatrix.h"
#include "include/core/SkPath.h"
#include "include/core/SkRRect.h"
#include "include/core/SkRegion.h"
#include "include/core/SkScalar.h"
#include "include/private/SkNoncopyable.h"

class SkString;

class SkReader32 : SkNoncopyable {
public:
    SkReader32() noexcept : fCurr(nullptr), fStop(nullptr), fBase(nullptr) {}
    SkReader32(const void* data, size_t size) noexcept { this->setMemory(data, size); }

    void setMemory(const void* data, size_t size) noexcept {
        SkASSERT(ptr_align_4(data));
        SkASSERT(SkAlign4(size) == size);

        fBase = fCurr = (const char*)data;
        fStop = (const char*)data + size;
    }

    size_t size() const noexcept { return fStop - fBase; }
    size_t offset() const noexcept { return fCurr - fBase; }
    bool eof() const noexcept { return fCurr >= fStop; }
    const void* base() const noexcept { return fBase; }
    const void* peek() const noexcept { return fCurr; }

    size_t available() const noexcept { return fStop - fCurr; }
    bool isAvailable(size_t size) const noexcept { return size <= this->available(); }

    void rewind() noexcept { fCurr = fBase; }

    void setOffset(size_t offset) noexcept {
        SkASSERT(SkAlign4(offset) == offset);
        SkASSERT(offset <= this->size());
        fCurr = fBase + offset;
    }

    bool readBool() noexcept { return this->readInt() != 0; }

    int32_t readInt() noexcept {
        SkASSERT(ptr_align_4(fCurr));
        int32_t value = *(const int32_t*)fCurr;
        fCurr += sizeof(value);
        SkASSERT(fCurr <= fStop);
        return value;
    }

    void* readPtr() noexcept {
        void* ptr;
        // we presume this "if" is resolved at compile-time
        if (4 == sizeof(void*)) {
            ptr = *(void**)fCurr;
        } else {
            memcpy(&ptr, fCurr, sizeof(void*));
        }
        fCurr += sizeof(void*);
        return ptr;
    }

    SkScalar readScalar() noexcept {
        SkASSERT(ptr_align_4(fCurr));
        SkScalar value = *(const SkScalar*)fCurr;
        fCurr += sizeof(value);
        SkASSERT(fCurr <= fStop);
        return value;
    }

    const void* skip(size_t size) noexcept {
        SkASSERT(ptr_align_4(fCurr));
        const void* addr = fCurr;
        fCurr += SkAlign4(size);
        SkASSERT(fCurr <= fStop);
        return addr;
    }

    template <typename T> const T& skipT() {
        SkASSERT(SkAlign4(sizeof(T)) == sizeof(T));
        return *(const T*)this->skip(sizeof(T));
    }

    void read(void* dst, size_t size) noexcept {
        SkASSERT(0 == size || dst != nullptr);
        SkASSERT(ptr_align_4(fCurr));
        sk_careful_memcpy(dst, fCurr, size);
        fCurr += SkAlign4(size);
        SkASSERT(fCurr <= fStop);
    }

    uint8_t readU8() noexcept { return (uint8_t)this->readInt(); }
    uint16_t readU16() noexcept { return (uint16_t)this->readInt(); }
    int32_t readS32() noexcept { return this->readInt(); }
    uint32_t readU32() noexcept { return this->readInt(); }

    bool readPath(SkPath* path) noexcept { return this->readObjectFromMemory(path); }

    bool readMatrix(SkMatrix* matrix) noexcept { return this->readObjectFromMemory(matrix); }

    bool readRRect(SkRRect* rrect) noexcept { return this->readObjectFromMemory(rrect); }

    bool readRegion(SkRegion* rgn) noexcept { return this->readObjectFromMemory(rgn); }

    /**
     *  Read the length of a string (written by SkWriter32::writeString) into
     *  len (if len is not nullptr) and return the null-ternimated address of the
     *  string within the reader's buffer.
     */
    const char* readString(size_t* len = nullptr);

    /**
     *  Read the string (written by SkWriter32::writeString) and return it in
     *  copy (if copy is not null). Return the length of the string.
     */
    size_t readIntoString(SkString* copy);

    sk_sp<SkData> readData() {
        uint32_t byteLength = this->readU32();
        if (0 == byteLength) {
            return SkData::MakeEmpty();
        }
        return SkData::MakeWithCopy(this->skip(byteLength), byteLength);
    }

private:
    template <typename T> bool readObjectFromMemory(T* obj) noexcept {
        size_t size = obj->readFromMemory(this->peek(), this->available());
        // If readFromMemory() fails (which means that available() was too small), it returns 0
        bool success = (size > 0) && (size <= this->available()) && (SkAlign4(size) == size);
        // In case of failure, we want to skip to the end
        (void)this->skip(success ? size : this->available());
        return success;
    }

    // these are always 4-byte aligned
    const char* fCurr;  // current position within buffer
    const char* fStop;  // end of buffer
    const char* fBase;  // beginning of buffer

#ifdef SK_DEBUG
    static bool ptr_align_4(const void* ptr) {
        return (((const char*)ptr - (const char*)nullptr) & 3) == 0;
    }
#endif
};

#endif
