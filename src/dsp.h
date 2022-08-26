/* =================================== PanPI ===================================
 * Digital Signal Processing module
 * (C) 2022 Ryan Suchocki
 * http://suchocki.co.uk/
 */

#pragma once

#include "common.h"
#include "config.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * API
 ******************************************************************************/

extern void dsp_init(unsigned init_fft_size);
extern void dsp_free(void);
extern void dsp_process(const complex double *samples, double *results);
