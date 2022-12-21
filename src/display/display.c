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

static fb_t *fb;
static fb_buf_t *bg;

static unsigned sgam_height;
static unsigned sgam_top;

static unsigned sample_rate;

/*******************************************************************************
 * Code
 ******************************************************************************/

unsigned display_open(unsigned open_sample_rate)
{
    fb = fb_init(config.x_window);

    fb->open();

    bg = (fb_buf_t *)malloc(sizeof(fb_buf_t) + fb->buf->buf_size);

    bg->size_x = fb->buf->size_x;
    bg->size_y = fb->buf->size_y;
    bg->buf_size = fb->buf->buf_size;

    unsigned sgam_width = (fb->buf->size_x - MARGIN * 2 - FRAMETHICKNESS * 2);
    sgam_height = (fb->buf->size_y - MARGIN * 2 - FRAMETHICKNESS * 2) / 2;
    unsigned sgam_left = MARGIN + FRAMETHICKNESS;
    sgam_top = MARGIN + FRAMETHICKNESS;

    unsigned wfall_top = sgam_top + ((fb->buf->size_y - MARGIN * 2 - FRAMETHICKNESS * 2) / 2) + FRAMETHICKNESS;
    unsigned wfall_height = fb->buf->size_y - wfall_top - MARGIN - FRAMETHICKNESS;

    spectrogram_init(sgam_width, sgam_height, sgam_left, sgam_top);
    waterfall_init(sgam_width, wfall_height, sgam_left, wfall_top);

    display_configure(open_sample_rate);
    fb->draw();

    return sgam_width;
}

void display_close(void)
{
    fb->close();
}

void display_configure(unsigned configure_sample_rate)
{
    sample_rate = configure_sample_rate;

    // Background
    for (unsigned x = 0; x < fb->buf->size_x; x++)
        for (unsigned y = 0; y < fb->buf->size_y; y++)
            *xy(bg, y, x) = BGCOL;

    // Pane background(s)
    spectrogram_render_bg(bg);

    // Horizontal lines
    for (unsigned x = MARGIN; x < fb->buf->size_x - MARGIN; x++)
        for (unsigned y = 0; y < FRAMETHICKNESS; y++)
        {
            // Head
            *xy(bg, MARGIN + y,x) = WHITE;
            // Mid
            *xy(bg, sgam_top + sgam_height + y, x) = WHITE;
            // Foot
            *xy(bg, fb->buf->size_y - 4 - y, x) = WHITE;
        }

    // Vertical lines
    for (unsigned y = MARGIN; y < fb->buf->size_y - 4; y++)
        for (unsigned x = 0; x < FRAMETHICKNESS; x++)
        {
            // Left
            *xy(bg, y, MARGIN + x) = WHITE;
            // Right
            *xy(bg, y, fb->buf->size_x - 4 - x) = WHITE;
        }

    // Head & foot bottom frequency pips
    for (unsigned x = 0; x < fb->buf->size_x / 2;
         x += (PIP_INTERVAL_HZ * fb->buf->size_x / sample_rate))
        for (unsigned xx = x; xx <= x + 1; xx++)
            for (unsigned y = 0; y < MARGIN; y++)
            {
                // Head left
                *xy(bg, y, fb->buf->size_x / 2 - xx) = WHITE;
                // Head right
                *xy(bg, y, fb->buf->size_x / 2 + xx) = WHITE;
                // Foot left
                *xy(bg, fb->buf->size_y - y - 1, fb->buf->size_x / 2 - xx) = WHITE;
                // Foot right
                *xy(bg, fb->buf->size_y - y - 1, fb->buf->size_x / 2 + xx) = WHITE;
            }
}

void render_debug_line(void)
{
    static time_t t_last = 0;
    static int count = 0;
    static char debug_line[100];

    count++;
    time_t t_now = time(NULL);

    if (t_now > t_last)
    {
        snprintf(debug_line, 100, "%ifps %ukHz", count, sample_rate / 1000);
        count = 0;
        t_last = t_now;
    }

    render_text(fb->buf, debug_line, -7, -10, false, YELLOW);
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

        memcpy(fb->buf->buf, bg->buf, fb->buf->buf_size);

        render_spectrogram(fb->buf);
        render_waterfall(fb->buf);
        render_debug_line();

        fb->draw();
    }
}
