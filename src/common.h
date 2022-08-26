/* =================================== PanPI ===================================
 * Project-wide dependencies and definitions
 * (C) 2022 Ryan Suchocki
 * http://suchocki.co.uk/
 */

#pragma once

#include <assert.h>
#include <complex.h>
#include <fcntl.h>
#include <math.h>
#include <signal.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <time.h>
#include <unistd.h>

/*******************************************************************************
 * Definitions (Generic)
 ******************************************************************************/

#define STRINGIZE(x) #x

#define ARRAYLEN(x) (sizeof(x) / sizeof(x[0]))

#define SQUARED(x) ((x) * (x))

#define MIN(x, y) ((y < x) ? (y) : (x))
#define MAX(x, y) ((y > x) ? (y) : (x))
#define EQMIN(x, y) x = MIN(x, y);
#define EQMAX(x, y) x = MAX(x, y);

#define UNUSED(x) (void)(x)

#define NANOSECONDS_PER_SECOND (1000000000L)

#define eprintf(args...) fprintf(stderr, "ERROR: " args)

/*******************************************************************************
 * API
 ******************************************************************************/

static inline int64_t get_nanos(void)
{
    // NB: will roll over after ~250 years...
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC_RAW, &t);
    return (int64_t)t.tv_sec * NANOSECONDS_PER_SECOND + (int64_t)t.tv_nsec;
}
