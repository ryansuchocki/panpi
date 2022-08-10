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

extern void dsp_init(unsigned init_fft_size, unsigned input_scale);
extern void dsp_free(void);
extern void dsp_process(const complex double *iq_inputs, double *dbm_results);
