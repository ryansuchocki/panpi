/* =================================== PanPI ===================================
 * Overall display rendering module
 * (C) 2022 Ryan Suchocki
 * http://suchocki.co.uk/
 */

#include "display.h"

#include "common.h"
#include "config.h"
#include "display.h"
#include "framebuffer.h"
#include "layout.h"
#include "spectrogram.h"
#include "text.h"
#include "waterfall.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

static void render_debug_line(void);

/*******************************************************************************
 * Variables
 ******************************************************************************/

static fb_t fb;
static fb_buf_t bg;

/*******************************************************************************
 * Code
 ******************************************************************************/

void display_open(unsigned sample_rate)
{
    fb = fb_init(config.x_window);

    waterfall_init();

    fb.open();
    display_update_bg(sample_rate);
    fb.draw();
}

void display_close(void)
{
    fb.close();
}

void display_update_bg(unsigned sample_rate)
{
    spectrogram_render_bg(&bg);

    // Horizontal lines
    for (int x = 3; x < FB_WIDTH - 3; x++)
        for (int y = 0; y < 3; y++)
        {
            // Head
            bg.xy[3 + y][x] = WHITE;
            // Mid
            bg.xy[SGAM_TOP + SGAM_HEIGHT + y][x] = WHITE;
            // Foot
            bg.xy[FB_HEIGHT - 4 - y][x] = WHITE;
        }

    // Vertical lines
    for (int y = 3; y < FB_HEIGHT - 4; y++)
        for (int x = 0; x < 3; x++)
        {
            // Left
            bg.xy[y][3 + x] = WHITE;
            // Right
            bg.xy[y][FB_WIDTH - 4 - x] = WHITE;
        }

    // Head & foot bottom frequency pips
    for (int x = 0; x < FB_WIDTH / 2;
         x += (PIP_INTERVAL_HZ * FB_WIDTH / (int)sample_rate))
        for (int xx = x - 1; xx < x + 2; xx++)
            for (int y = 0; y < 3; y++)
            {
                // Head left
                bg.xy[y][FB_WIDTH / 2 - xx] = WHITE;
                // Head right
                bg.xy[y][FB_WIDTH / 2 + xx] = WHITE;
                // Foot left
                bg.xy[FB_HEIGHT - y - 1][FB_WIDTH / 2 - xx] = WHITE;
                // Foot right
                bg.xy[FB_HEIGHT - y - 1][FB_WIDTH / 2 + xx] = WHITE;
            }
}

void render_debug_line(void)
{
    static time_t t_last = 0;
    static int count = 0;
    static char debug_line[20];

    count++;
    time_t t_now = time(NULL);

    if (t_now > t_last)
    {
        snprintf(debug_line, 20, "%ifps", count);
        count = 0;
        t_last = t_now;
    }

    render_text(fb.buf, debug_line, -7, -10, false, YELLOW);
}

void display_update(const double *dbm_values)
{
    spectrogram_update(dbm_values);
    waterfall_update(dbm_values);

    static int64_t next_frame_due = 0;

    int64_t nanos = get_nanos();

    if (nanos >= next_frame_due)
    {
        next_frame_due = next_frame_due ? next_frame_due : nanos;
        next_frame_due += NANOSECONDS_PER_SECOND / TARGET_FPS;

        memcpy(fb.buf, &bg, sizeof(*fb.buf));

        render_spectrogram(fb.buf);
        render_waterfall(fb.buf);
        render_debug_line();

        fb.draw();
    }
}
