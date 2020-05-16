/*
 * Copyright 2006 The Android Open Source Project
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

// TODO: add unittests for all these operations

#ifndef SkOSFile_DEFINED
#define SkOSFile_DEFINED

#include <stdio.h>

#include "include/core/SkString.h"

enum SkFILE_Flags {
  kRead_SkFILE_Flag = 0x01,
  kWrite_SkFILE_Flag = 0x02,
  kAppend_SkFILE_Flag = 0x04
};

FILE* sk_fopen(const char path[], SkFILE_Flags);
void sk_fclose(FILE*) noexcept;

size_t sk_fgetsize(FILE*) noexcept;

size_t sk_fwrite(const void* buffer, size_t byteCount, FILE*) noexcept;

void sk_fflush(FILE*) noexcept;
void sk_fsync(FILE*) noexcept;

size_t sk_ftell(FILE*) noexcept;

/** Maps a file into memory. Returns the address and length on success, NULL otherwise.
 *  The mapping is read only.
 *  When finished with the mapping, free the returned pointer with sk_fmunmap.
 */
void* sk_fmmap(FILE* f, size_t* length) noexcept;

/** Maps a file descriptor into memory. Returns the address and length on success, NULL otherwise.
 *  The mapping is read only.
 *  When finished with the mapping, free the returned pointer with sk_fmunmap.
 */
void* sk_fdmmap(int fd, size_t* length) noexcept;

/** Unmaps a file previously mapped by sk_fmmap or sk_fdmmap.
 *  The length parameter must be the same as returned from sk_fmmap.
 */
void sk_fmunmap(const void* addr, size_t length) noexcept;

/** Returns true if the two point at the exact same filesystem object. */
bool sk_fidentical(FILE* a, FILE* b) noexcept;

/** Returns the underlying file descriptor for the given file.
 *  The return value will be < 0 on failure.
 */
int sk_fileno(FILE* f) noexcept;

/** Returns true if something (file, directory, ???) exists at this path,
 *  and has the specified access flags.
 */
bool sk_exists(const char* path, SkFILE_Flags = (SkFILE_Flags)0) noexcept;

// Returns true if a directory exists at this path.
bool sk_isdir(const char* path) noexcept;

// Like pread, but may affect the file position marker.
// Returns the number of bytes read or SIZE_MAX if failed.
size_t sk_qread(FILE*, void* buffer, size_t count, size_t offset) noexcept;

// Create a new directory at this path; returns true if successful.
// If the directory already existed, this will return true.
// Description of the error, if any, will be written to stderr.
bool sk_mkdir(const char* path) noexcept;

class SkOSFile {
 public:
  class Iter {
   public:
    Iter() noexcept;
    Iter(const char path[], const char suffix[] = nullptr) noexcept;
    ~Iter();

    void reset(const char path[], const char suffix[] = nullptr) noexcept;
    /** If getDir is true, only returns directories.
        Results are undefined if true and false calls are
        interleaved on a single iterator.
    */
    bool next(SkString* name, bool getDir = false);

    static const size_t kStorageSize = 40;

   private:
    SkAlignedSStorage<kStorageSize> fSelf;
  };
};

#endif
