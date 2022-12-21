/* =================================== PanPI ===================================
 * Linux framebuffer wrapper module
 * (C) 2022 Ryan Suchocki
 * http://suchocki.co.uk/
 */

#pragma once

#include "common.h"
#include "config.h"
#include "layout.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

typedef uint16_t colour16_t;

static inline colour16_t colour(uint8_t red, uint8_t green, uint8_t blue)
{
    return (colour16_t)(((red >> 3) << 11) |
                        ((green >> 2) << 5) |
                        (blue >> 3));
}

static inline uint32_t colour16_to_24(colour16_t c)
{
    return (
        ((c & 0xF800) << 8) |
        ((c & 0x7E0) << 5) |
        ((c & 0x1F) << 3));
}

typedef struct
{
    unsigned size_x;
    unsigned size_y;
    size_t buf_size;
    colour16_t buf[];
} fb_buf_t;

typedef struct
{
    fb_buf_t *buf;
    void (*open)(void);
    void (*close)(void);
    void (*draw)(void);
} fb_t;

/*******************************************************************************
 * API
 ******************************************************************************/

static inline fb_t *fb_init(bool use_x11)
{
    if (use_x11)
    {
#if COMPILE_X11
        extern fb_t fb_x;
        return &fb_x;
#else
        eprintf("PanPI was compiled without X11 support\n");
        exit(1);
#endif
    }
    else
    {
        extern fb_t fb_raw;
        return &fb_raw;
    }
}

static inline fb_buf_t *fb_buf_create(unsigned size_x, unsigned size_y)
{
    size_t buf_size = sizeof(colour16_t) * size_x * size_y;
    fb_buf_t *buf = (fb_buf_t *)malloc(sizeof(fb_buf_t) + buf_size);
    buf->buf_size = buf_size;
    buf->size_x = size_x;
    buf->size_y = size_y;

    return buf;
}

static inline colour16_t *xy(fb_buf_t *buf, unsigned y, unsigned x)
{
    // TODO assert?
    return &buf->buf[(size_t)y * buf->size_x + (size_t)x];
}
