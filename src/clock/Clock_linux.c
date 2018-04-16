#include "Clock.h"

#include <float.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>

/*
 * Clock implementation for linux systems.
 */
static const int MILLI2MICRO = 1000;
static const int MICRO2NANO = 1000;
static const int MSEC2NANO = 1000000;
static const int SEC2NANO = 1000000000;

void ssce_delay_hint(){/*NO-OP for linux*/}

void ssce_delay(int64_t usecs) {
    if(usecs<=0 || usecs>=1000000) {
        return;
    }
    //Valid input - continue.
    struct timespec towait = {0, usecs*MICRO2NANO};
    struct timespec remain = {0, 0};
    while(nanosleep(&towait, &remain) == -1) {
        towait = remain;
    }
}

void ssce_msleep(int64_t msecs) {
    usleep(msecs*MILLI2MICRO);
}

void ssce_start(PerfClock* pc) {
    struct timespec tp;
    clock_gettime(CLOCK_MONOTONIC_RAW, &tp);
    pc->intcnt[0] = tp.tv_nsec;
    pc->intcnt[1] = tp.tv_sec;
}

void ssce_stop(PerfClock* pc) {
    struct timespec tp;
    clock_gettime(CLOCK_MONOTONIC_RAW, &tp);
    int64_t nano_delta = (tp.tv_nsec - pc->intcnt[0]) + SEC2NANO*(tp.tv_sec - pc->intcnt[1]);
    double delta = ((double)nano_delta)/MSEC2NANO;
    pc->count_query++;
    pc->delta_sum += delta;
    pc->avg = pc->delta_sum / pc->count_query;
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