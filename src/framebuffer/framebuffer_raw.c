/* =================================== PanPI ===================================
 * Linux framebuffer wrapper module
 * (C) 2022 Ryan Suchocki
 * http://suchocki.co.uk/
 */

#include "common.h"
#include "framebuffer.h"
#include "layout.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

#define FD_OPEN_FAILED (-1)

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

static void fb_raw_open(void);
static void fb_raw_close(void);
static void fb_raw_draw(void);

/*******************************************************************************
 * Variables
 ******************************************************************************/

static int fbfd = FD_OPEN_FAILED;
static fb_buf_t *frontbuf = NULL;
static fb_buf_t raw_backbuf;

fb_t fb_raw = {
    .buf = &raw_backbuf,
    .open = &fb_raw_open,
    .close = &fb_raw_close,
    .draw = &fb_raw_draw,
};

/*******************************************************************************
 * Code
 ******************************************************************************/

static void fb_raw_open(void)
{
    fbfd = open("/dev/fb0", O_RDWR);
    if (fbfd == FD_OPEN_FAILED)
    {
        eprintf("Cannot open framebuffer device\n");
        exit(1);
    }

    frontbuf = mmap(0, sizeof(fb_buf_t), PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
    if (frontbuf == MAP_FAILED)
    {
        eprintf("Failed to map framebuffer device to memory\n");
        exit(1);
    }
}

static void fb_raw_close(void)
{
    munmap(frontbuf, sizeof(fb_buf_t));
    close(fbfd);
}

static void fb_raw_draw(void)
{
    memcpy(frontbuf, &raw_backbuf, sizeof(*frontbuf));
}
