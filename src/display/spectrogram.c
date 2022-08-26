/* =================================== PanPI ===================================
 * Spectrogram display module
 * (C) 2022 Ryan Suchocki
 * http://suchocki.co.uk/
 */

#include "spectrogram.h"

#include "common.h"
#include "config.h"
#include "framebuffer.h"
#include "layout.h"
#include "text.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

static double smoothed_values[SGAM_WIDTH];

/*******************************************************************************
 * Code
 ******************************************************************************/

void spectrogram_render_bg(fb_buf_t *bg)
{
    // Vertical centre line
    for (int x = 0; x < SGAM_WIDTH; x++)
    {
        for (int y = 0; y < SGAM_HEIGHT; y++)
        {
            bg->xy[SGAM_TOP + SGAM_HEIGHT - 1 - y][SGAM_LEFT + x] =
                (x == SGAM_WIDTH / 2) ? CLINECOL : BGCOL;
        }
    }

    // Horizontal reference lines
    for (int y = (int)(floor(config.refl + 1)); y < config.refh; y++)
    {
        int yyy = (int)((y - config.refl) * SGAM_HEIGHT / (config.refh - config.refl));
        for (int x = 8 + 8 + 8 + 1; (x + 3) < SGAM_WIDTH; x += 8)
            for (int xx = x; xx < x + 3; xx++)
                bg->xy[SGAM_TOP + SGAM_HEIGHT - 1 - yyy][SGAM_LEFT + xx] =
                    HLINECOL;

        char buf[5];
        snprintf(buf, 5, "%i", y);

        render_text(bg, buf, SGAM_LEFT + 6, SGAM_TOP + SGAM_HEIGHT - 1 - yyy, true, YELLOW);
    }
}

void spectrogram_update(const double *values)
{
    double sum = 0;
    for (unsigned i = 0; i < config.sgam_spread * 2; i++)
    {
        sum += values[i];
    }

    for (unsigned i = config.sgam_spread; i < SGAM_WIDTH - config.sgam_spread; i++)
    {
        sum += values[i + config.sgam_spread];
        double this_amplitude = sum / (config.sgam_spread * 2 + 1);
        sum -= values[i - config.sgam_spread];

        smoothed_values[i] = smoothed_values[i] * config.sgam_drag;
        smoothed_values[i] += this_amplitude * (1.0 - config.sgam_drag);
    }
}

void render_spectrogram(fb_buf_t *buf)
{
    int lastamp = 0;
    for (int x = 0; x < SGAM_WIDTH; x++)
    {
        int col = SGAM_LEFT + x;

        int amp = (int)(smoothed_values[x] * SGAM_HEIGHT);

        // amp *= 0.244140625 * SGAM_HEIGHT;
        EQMAX(amp, 0);
        EQMIN(amp, SGAM_HEIGHT);

        int start = MIN(amp, lastamp);
        int stop = MAX(amp, lastamp);
        lastamp = amp;

        for (int y = start; y <= stop; y++)
        {
            buf->xy[SGAM_TOP + SGAM_HEIGHT - y - 1][col] = YELLOW;
        }
    }
}
