#include <stdio.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "get-sync-times.h"

int main() {
    uint64_t boot_time = 0;
    uint64_t monotonic_time = 0;
    uint64_t cpu_time = 0;
    for(;;) {
        usleep(100000);
        get_sync_times(&boot_time, &monotonic_time, &cpu_time);
        fprintf(stderr, "boottime:%llu monotonic:%llu cputime:%llu\n",
                (unsigned long long)boot_time, (unsigned long long)monotonic_time, (unsigned long long)cpu_time);
    }
    return 0;
}
