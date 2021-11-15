#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>
#include <time.h>

#if defined(_M_IA64) || defined(_M_IX86) || defined(__ia64__) ||      \
    defined(__i386__) || defined(__amd64__) || defined(__x86_64__) || \
    defined(_M_AMD64)
#define HAS_RDTSC
#ifdef _MSC_VER
#include <intrin.h>
#else
#include <x86intrin.h>
#endif
#endif
#if defined(__aarch64__)
#define HAS_CNTVCT
#endif

static inline uint64_t get_cpu_ticks() {
#if defined(HAS_RDTSC)
    return __rdtsc();
#elif defined(HAS_CNTVCT)
    uint64_t vct;
    asm volatile("mrs %0, cntvct_el0" : "=r"(vct));
    return vct;
#else
    return 0;
#endif
}

static inline uint64_t get_timestamp_ns(clockid_t cid) {
    struct timespec ts = {};
    clock_gettime(cid, &ts);
    return static_cast<uint64_t>(ts.tv_sec * 1000000000LL + ts.tv_nsec);
}

void get_sync_times(uint64_t* boot_time,
                    uint64_t* monotonic_time,
                    uint64_t* cpu_time) {
    *boot_time = get_timestamp_ns(CLOCK_BOOTTIME);
    *cpu_time = get_cpu_ticks();
    *monotonic_time = get_timestamp_ns(CLOCK_MONOTONIC);
    // Read again to avoid cache miss overhead.
    *boot_time = get_timestamp_ns(CLOCK_BOOTTIME);
    *cpu_time = get_cpu_ticks();
    *monotonic_time = get_timestamp_ns(CLOCK_MONOTONIC);
}
