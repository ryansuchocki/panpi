/* =================================== PanPI ===================================
 * Text rendering module
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

extern void render_text(fb_buf_t *buf, const char *text, int x, int y, bool v_centre, bool h_centre, colour16_t colour);
extern int get_width_text(const char *text);
extern void render_text_small(fb_buf_t *buf, const char *text, int x, int y, bool v_centre, bool h_centre, colour16_t colour);
