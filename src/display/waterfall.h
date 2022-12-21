/* =================================== PanPI ===================================
 * Waterfall rendering module
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

extern void waterfall_init(unsigned width, unsigned height, unsigned left, unsigned top);
extern void waterfall_update(const double *dbm_values);
extern void render_waterfall(fb_buf_t *buf);
