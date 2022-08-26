/* =================================== PanPI ===================================
 * Display layout/geometry definitions
 * (C) 2022 Ryan Suchocki
 * http://suchocki.co.uk/
 */

#pragma once

#include "common.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

#define TARGET_FPS 60

#define FB_HEIGHT 320
#define FB_WIDTH 480

#define SGAM_TOP 6
#define SGAM_HEIGHT 152
#define SGAM_LEFT 6
#define SGAM_WIDTH 468

#define WFALL_TOP (SGAM_TOP + SGAM_HEIGHT + 3)
#define WFALL_HEIGHT (FB_HEIGHT - WFALL_TOP - 6)
#define WFALL_LEFT SGAM_LEFT
#define WFALL_WIDTH SGAM_WIDTH

#define PIP_INTERVAL_HZ 10000

#define WHITE colour(255, 255, 255)
#define YELLOW colour(255, 255, 0)
#define BGCOL colour(0x00, 0x07, 0x17)
#define CLINECOL colour(0x50, 0x50, 0x50)
#define HLINECOL colour(0x50, 0x50, 0x50)

/*******************************************************************************
 * API
 ******************************************************************************/
