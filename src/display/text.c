/* =================================== PanPI ===================================
 * Text rendering module
 * (C) 2022 Ryan Suchocki
 * http://suchocki.co.uk/
 */

#include "text.h"

#include "common.h"
#include "framebuffer.h"

#define blit_pixel colour16_t
#include "blit-fonts/blit32.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/

void render_text(fb_buf_t *buf, const char *text, int x, int y, bool v_centre, colour16_t colour)
{
    if (x < 0)
        x += buf->size_x - (blit32_ADVANCE * (int)strlen(text));

    if (y < 0)
        y += buf->size_y - blit32_HEIGHT;

    if (v_centre)
        y -= blit32_HEIGHT / 2;

    blit32_TextExplicit(
        (blit_pixel *)buf->buf,
        colour,
        1,
        buf->size_x,
        buf->size_y,
        blit_Clip,
        x,
        y,
        (char *)text);
}
