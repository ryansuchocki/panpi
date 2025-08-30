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

static int sgam_width;
static int sgam_height;
static int sgam_left;
static int sgam_top;
static double *smoothed_values;

/*******************************************************************************
 * Code
 ******************************************************************************/

void spectrogram_init(int width, int height, int left, int top)
{
    sgam_width = width;
    sgam_height = height;
    sgam_left = left;
    sgam_top = top;
    smoothed_values = (double *)malloc(sizeof(double) * (size_t)width);
    memset(smoothed_values, 0, sizeof(double) * (size_t)width);
}

void spectrogram_render_bg(fb_buf_t *fb)
{
    // Background & Vertical centre line
    for (int x = 0; x < sgam_width; x++)
    {
        for (int y = 0; y < sgam_height; y++)
        {
            *xy(fb, sgam_left + x, sgam_top + sgam_height - 1 - y) =
                (x == sgam_width / 2) ? CLINECOL : BGCOL;
        }
    }

    // Horizontal reference lines
    int first_db_val = (int)(floor(config.refl / config.ref_interval + 1)) * (int)config.ref_interval;
    int last_db_val = (int)ceil(config.refh);

    for (int db_val = first_db_val; db_val < last_db_val; db_val += (int)config.ref_interval)
    {
        int y = (int)((db_val - config.refl) * sgam_height / (config.refh - config.refl));
        for (int x = 8 + 8 + 8 + 1; (x + 3) < sgam_width; x += 8)
            for (int xx = x; xx < x + 3; xx++)
                *xy(fb, sgam_left + xx, (sgam_top + sgam_height - 1) - y) = HLINECOL;

        char buf[13];
        snprintf(buf, 13, "%i", db_val);

        render_text(fb, buf, sgam_left + 6, sgam_top + sgam_height - 1 - y, true, YELLOW);
    }
}

void spectrogram_update(const double *dbm_values)
{
    double sum = 0;
    int denom = config.sgam_spread;

    for (int i = 0; i < config.sgam_spread; i++)
    {
        sum += dbm_values[i];
    }

    for (int i = 0; i < sgam_width; i++)
    {
        if (i < sgam_width - config.sgam_spread)
            sum += dbm_values[i + config.sgam_spread];
        else
            denom--;

        if (i > config.sgam_spread)
            sum -= dbm_values[i - config.sgam_spread - 1];
        else
            denom++;

        double this_amplitude = sum / denom;

        // IIR/EWMA
        smoothed_values[i] += (1 - config.sgam_drag) * (this_amplitude - smoothed_values[i]);
    }
}

void render_spectrogram(fb_buf_t *buf)
{
    int lastamp = 0;
    for (int x = 0; x < sgam_width; x++)
    {
        int col = sgam_left + x;

        double dbm_display = (smoothed_values[x] - config.refl) / (config.refh - config.refl);
        int amp = (int)(dbm_display * sgam_height);

        EQMAX(amp, 0);
        EQMIN(amp, sgam_height);

        int start = MIN(amp, lastamp);
        int stop = MAX(amp, lastamp);
        lastamp = amp;

        for (int y = start; y <= stop; y++)
        {
            *xy(buf, col, sgam_top + sgam_height - y - 1) = YELLOW;
        }
    }
}
