/* =================================== PanPI ===================================
 * Overall display rendering module
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

int display_open(int open_sample_rate);
void display_close(void);
void display_configure(int configure_sample_rate);
void display_update(const double *dbm_values);
