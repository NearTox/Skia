/*
 * Copyright 2014 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "include/utils/SkEventTracer.h"
#include <atomic>
#include "include/private/SkOnce.h"

#include <stdlib.h>

class SkDefaultEventTracer : public SkEventTracer {
  SkEventTracer::Handle addTraceEvent(
      char phase, const uint8_t* categoryEnabledFlag, const char* name, uint64_t id, int numArgs,
      const char** argNames, const uint8_t* argTypes, const uint64_t* argValues,
      uint8_t flags) noexcept override {
    return 0;
  }

  void updateTraceEventDuration(
      const uint8_t* categoryEnabledFlag, const char* name,
      SkEventTracer::Handle handle) noexcept override {}

  const uint8_t* getCategoryGroupEnabled(const char* name) noexcept override {
    static uint8_t no = 0;
    return &no;
  }
  const char* getCategoryGroupName(const uint8_t* categoryEnabledFlag) noexcept override {
    static const char* dummy = "dummy";
    return dummy;
  }
};

// We prefer gUserTracer if it's been set, otherwise we fall back on a default tracer;
static std::atomic<SkEventTracer*> gUserTracer{nullptr};

bool SkEventTracer::SetInstance(SkEventTracer* tracer) noexcept {
  SkEventTracer* expected = nullptr;
  if (!gUserTracer.compare_exchange_strong(expected, tracer)) {
    delete tracer;
    return false;
  }
  atexit([]() noexcept { delete gUserTracer.load(); });
  return true;
}

SkEventTracer* SkEventTracer::GetInstance() noexcept {
  if (auto tracer = gUserTracer.load(std::memory_order_acquire)) {
    return tracer;
  }
  static SkOnce once;
  static SkDefaultEventTracer* defaultTracer;
  once([]() noexcept { defaultTracer = new SkDefaultEventTracer; });
  return defaultTracer;
}
