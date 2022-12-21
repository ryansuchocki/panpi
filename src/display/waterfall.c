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

static unsigned wfall_width;
static unsigned wfall_height;
static unsigned wfall_left;
static unsigned wfall_top;

static colour16_t *waterfall;
static unsigned waterfall_i = 0;
static colour16_t wf_map[INTENS_LEVELS];

/*******************************************************************************
 * Code
 ******************************************************************************/

void waterfall_init(unsigned width, unsigned height, unsigned left, unsigned top)
{
    wfall_width = width;
    wfall_height = height;
    wfall_left = left;
    wfall_top = top;

    waterfall = (colour16_t *)malloc(sizeof(colour16_t) * wfall_width * wfall_height);

    // Pre-compute colour values for each of the waterfall intensity levels
    for (unsigned i = 0; i < INTENS_LEVELS; i++)
    {
        wf_map[i] = colour(i * 0, (uint8_t)(0x07 + i * 0x07 / 0x17), (uint8_t)(0x17 + i));
    }
}

void waterfall_update(const double *dbm_values)
{
    static unsigned phase = 0;

    if (++phase == config.wfall_zoom)
    {
        phase = 0;

        for (unsigned x = 0; x < wfall_width; x++)
        {
            double dbm_display = (dbm_values[x] - config.refl) / (config.refh - config.refl);
            int intens = (int)(dbm_display * INTENS_LEVELS);
            EQMAX(intens, 0);
            waterfall[waterfall_i * wfall_width + x] =
                (intens >= INTENS_LEVELS) ? WHITE : wf_map[intens];
        }

        waterfall_i = (waterfall_i + 1) % wfall_height;
    }
}

void render_waterfall(fb_buf_t *buf)
{
    for (unsigned x = 0; x < wfall_width; x++)
    {
        for (unsigned y = 0; y < wfall_height; y++)
        {
            *xy(buf, wfall_top + wfall_height - 1 - y, x + wfall_left) =
                waterfall[((waterfall_i + y + 1) % wfall_height) * wfall_width + x];
        }
    }
}
