/* =================================== PanPI ===================================
 * Config management module
 * (C) 2022 Ryan Suchocki
 * http://suchocki.co.uk/
 */

#pragma once

#include "common.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

#define CONFIG_STR_LEN 32

typedef struct
{
    char source[CONFIG_STR_LEN];
    char device[CONFIG_STR_LEN];
    char file[CONFIG_STR_LEN];
    unsigned sample_rate;
    double capture_gain;
    double dc_alpha;
    bool x_window;
    double refl;
    double refh;
    unsigned sgam_spread;
    double sgam_drag;
    unsigned wfall_zoom;
} CONFIG_T;

/*******************************************************************************
 * API
 ******************************************************************************/

extern CONFIG_T config;

extern void config_init(void);
extern bool config_update(void);
