/* =================================== PanPI ===================================
 * IQ sample capture module
 * (C) 2022 Ryan Suchocki
 * http://suchocki.co.uk/
 */

#pragma once

#include "common.h"
#include "config.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

typedef struct
{
    int (*open)(int sample_rate);
    int (*configure)(void);
    int (*close)(void);
    int (*get)(complex double *buffer, int n);
} capture_t;

/*******************************************************************************
 * API
 ******************************************************************************/

static inline capture_t capture_init(const char *source)
{
    if (!strcmp(source, "soundcard"))
    {
        extern capture_t capture_soundcard;
        return capture_soundcard;
    }
    else if (!strcmp(source, "file"))
    {
        extern capture_t capture_file;
        return capture_file;
    }
    else
    {
        eprintf("Unknown source type '%s'\n", source);
        exit(1);
    }
}
