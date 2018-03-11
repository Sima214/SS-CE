#include "Clock.h"

#include <float.h>
#include "stdint.h"
#include <time.h>
#include <unistd.h>

/*
 * Clock implementation for linux systems.
 */
const int SEC2NANO = 1000000000;
const int MSEC2NANO = 1000000;
const int MILLI2MICRO = 1000;
void ssce_msleep(int64_t msecs) {
    usleep(msecs*MILLI2MICRO);
}
void ssce_start(PerfClock* pc) {
    struct timespec tp;
    clock_gettime(CLOCK_MONOTONIC_RAW, &tp);
    pc->last_query = tp.tv_nsec;
    pc->last_query_extra = tp.tv_sec;
}
void ssce_stop(PerfClock* pc) {
    struct timespec tp;
    clock_gettime(CLOCK_MONOTONIC_RAW, &tp);
    int64_t nano_delta = (tp.tv_nsec - pc->last_query) + SEC2NANO*(tp.tv_sec - pc->last_query_extra);
    double delta = ((double)nano_delta)/MSEC2NANO;
    pc->count_query++;
    pc->delta_sum += delta;
    pc->avg = pc->delta_sum/pc->count_query;
    pc->delta = delta;
    if(delta > pc->max){
        pc->max = delta;
    }
    if(delta < pc->min){
        pc->min = delta;
    }
}
void ssce_reset(PerfClock* pc) {
    pc->count_query = 0;
    pc->delta_sum = 0;
    pc->max = FLT_MIN;
    pc->min = FLT_MAX;
}