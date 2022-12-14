/* =================================== PanPI ===================================
 * Spectrogram display module
 * (C) 2022 Ryan Suchocki
 * http://suchocki.co.uk/
 */

#pragma once

#include "common.h"
#include "framebuffer.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * API
 ******************************************************************************/

extern void spectrogram_init(unsigned width, unsigned height, unsigned left, unsigned top);
extern void spectrogram_render_bg(fb_buf_t *fb);
extern void spectrogram_update(const double *dbm_values);
extern void render_spectrogram(fb_buf_t *buf);
