/* =================================== PanPI ===================================
 * Waterfall rendering module
 * (C) 2022 Ryan Suchocki
 * http://suchocki.co.uk/
 */

#include "waterfall.h"

#include "common.h"
#include "config.h"
#include "framebuffer.h"
#include "layout.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

#define INTENS_LEVELS (255 - 0x17)

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

static colour16_t waterfall[WFALL_HEIGHT][WFALL_WIDTH];
static unsigned waterfall_i = 0;
static colour16_t wf_map[INTENS_LEVELS];

/*******************************************************************************
 * Code
 ******************************************************************************/

void waterfall_init(void)
{
    // Pre-compute colour values for each of the waterfall intensity levels
    for (unsigned i = 0; i < INTENS_LEVELS; i++)
    {
        wf_map[i] = colour(i * 0, (uint8_t)(0x07 + i * 0x07 / 0x17), (uint8_t)(0x17 + i));
    }
}

void waterfall_update(const double *values)
{
    static unsigned phase = 0;

    if (++phase == config.wfall_zoom)
    {
        phase = 0;

        for (unsigned x = 0; x < WFALL_WIDTH; x++)
        {
            int intens = (int)(values[x] * INTENS_LEVELS);
            EQMAX(intens, 0);
            waterfall[waterfall_i][x] =
                (intens >= INTENS_LEVELS) ? WHITE : wf_map[intens];
        }

        waterfall_i = (waterfall_i + 1) % WFALL_HEIGHT;
    }
}

void render_waterfall(fb_buf_t *buf)
{
    for (unsigned x = 0; x < WFALL_WIDTH; x++)
    {
        for (unsigned y = 0; y < WFALL_HEIGHT; y++)
        {
            buf->xy[WFALL_TOP + WFALL_HEIGHT - 1 - y][x + WFALL_LEFT] =
                waterfall[(waterfall_i + y + 1) % WFALL_HEIGHT][x];
        }
    }
}
