// Copyright 2021 The Android Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#include <string>
#include <thread>
#include <sys/types.h>
#include <time.h>

#include "get-sync-times.h"
#include "perfetto.h"

PERFETTO_DEFINE_CATEGORIES(
    perfetto::Category("cros")
        .SetDescription("Chrome OS time sync events"));

PERFETTO_TRACK_EVENT_STATIC_STORAGE();

static std::once_flag perfetto_once;
static std::thread* bg_thread = nullptr;

static void perfetto_annotate_time_sync(const perfetto::EventContext& perfetto) {
    uint64_t boot_time;
    uint64_t monotonic_time;
    uint64_t cpu_time;
    get_sync_times(&boot_time, &monotonic_time, &cpu_time);

    auto* dbg = perfetto.event()->add_debug_annotations();
    dbg->set_name("clock_sync_boottime");
    dbg->set_uint_value(boot_time);
    dbg = perfetto.event()->add_debug_annotations();
    dbg->set_name("clock_sync_monotonic");
    dbg->set_uint_value(monotonic_time);
    dbg = perfetto.event()->add_debug_annotations();
    dbg->set_name("clock_sync_cputime");
    dbg->set_uint_value(cpu_time);
}

static void tick_forever() {
    for(;;) {
        usleep(100000);
        TRACE_EVENT(
                "cros", "clock_sync",
                [&](perfetto::EventContext p) { perfetto_annotate_time_sync(p); });
    }
}

static bool init_perfetto() {
    bool was_init = false;
    std::call_once(perfetto_once, [&was_init](){
        perfetto::TracingInitArgs args;
        args.backends |= perfetto::kSystemBackend;
        perfetto::Tracing::Initialize(args);
        perfetto::TrackEvent::Register();
        was_init = true;
    });
    return was_init;
}

void trace_time_async() {
    if (init_perfetto())
        bg_thread = new std::thread(tick_forever);
}

void trace_time_blocking() {
    init_perfetto();
    tick_forever();
}
