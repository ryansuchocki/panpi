/* =================================== PanPI ===================================
 * Linux framebuffer wrapper module
 * (C) 2022 Ryan Suchocki
 * http://suchocki.co.uk/
 */

#include "common.h"
#include "framebuffer.h"
#include "layout.h"

#include <sys/ioctl.h>

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
static colour16_t *frontbuf = NULL;

fb_t fb_raw = {
    .buf = NULL,
    .open = &fb_raw_open,
    .close = &fb_raw_close,
    .draw = &fb_raw_draw,
};

/*******************************************************************************
 * Code
 ******************************************************************************/

#include <linux/fb.h>

static void fb_raw_open(void)
{
    fbfd = open(config.fb_dev, O_RDWR);
    if (fbfd == FD_OPEN_FAILED)
    {
        eprintf("Cannot open framebuffer device\n");
        exit(1);
    }

    struct fb_var_screeninfo vinfo;
    if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo) != 0)
    {
        eprintf("Failed to determine framebuffer dimensions (IOCTL)\n");
        exit(1);
    }

    printf("Framebuffer dimensions: x=%u y=%u\n", vinfo.xres, vinfo.yres);

    if (vinfo.xres == 0 || vinfo.yres == 0)
    {
        eprintf("Failed to determine framebuffer dimensions (zero size)\n");
        exit(1);
    }
    // TODO verify colour format?

    fb_raw.buf = fb_buf_create(vinfo.xres, vinfo.yres);

    frontbuf = mmap(0, fb_raw.buf->buf_size, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
    if (frontbuf == MAP_FAILED)
    {
        eprintf("Failed to map framebuffer device to memory\n");
        exit(1);
    }
}

static void fb_raw_close(void)
{
    munmap(frontbuf, fb_raw.buf->buf_size);
    close(fbfd);
    free(fb_raw.buf);
}

static void fb_raw_draw(void)
{
    memcpy(frontbuf, fb_raw.buf->buf, fb_raw.buf->buf_size);
}
