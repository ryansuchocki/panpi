/* =================================== PanPI ===================================
 * Linux framebuffer wrapper module
 * (C) 2022 Ryan Suchocki
 * http://suchocki.co.uk/
 */

#include "common.h"
#include "framebuffer.h"
#include "layout.h"

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

static void fb_x_open(void);
static void fb_x_close(void);
static void fb_x_draw(void);

/*******************************************************************************
 * Variables
 ******************************************************************************/

static Display *display;
static Window window;
static Atom wm_delete_window;
static XImage *img;

#define FB_WIDTH 800
#define FB_HEIGHT 500

fb_t fb_x = {
    .buf = NULL,
    .open = &fb_x_open,
    .close = &fb_x_close,
    .draw = &fb_x_draw,
};

/*******************************************************************************
 * Code
 ******************************************************************************/

static void fb_x_open(void)
{
    fb_x.buf = fb_buf_create(FB_WIDTH, FB_HEIGHT);

    display = XOpenDisplay(NULL);
    if (!display)
        exit(1);

    window = XCreateWindow(
        display,
        DefaultRootWindow(display),
        0,
        0,
        FB_WIDTH,
        FB_HEIGHT,
        0,
        CopyFromParent,
        InputOutput,
        CopyFromParent,
        CWBackingStore | CWEventMask | CWBackPixel,
        &((XSetWindowAttributes){
            .backing_store = Always,
            .event_mask = StructureNotifyMask,
            .background_pixel = BlackPixel(display, DefaultScreen(display))}));

    const char *s = "PanPI " VERSION;
    XTextProperty prop;
    XStringListToTextProperty((char **)&s, 1, &prop);
    XSetWMProperties(display, window, &prop, &prop, NULL, 0, NULL, NULL, NULL);

    wm_delete_window = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, window, &wm_delete_window, 1);

    img = XCreateImage(
        display,
        DefaultVisual(display, DefaultScreen(display)),
        24,
        ZPixmap,
        0,
        malloc(FB_WIDTH * FB_HEIGHT * 4),
        FB_WIDTH,
        FB_HEIGHT,
        32,
        0);

    XMapWindow(display, window);
}

static void fb_x_close(void)
{
}

static void fb_x_draw(void)
{
    for (unsigned y = 0; y < FB_HEIGHT; y++)
        for (unsigned x = 0; x < FB_WIDTH; x++)
            XPutPixel(img, (int)x, (int)y, colour16_to_24(*xy(fb_x.buf, y, x)));

    XPutImage(display, window, DefaultGC(display, DefaultScreen(display)), img, 0, 0, 0, 0, FB_WIDTH, FB_HEIGHT);

    while (XPending(display))
    {
        XEvent e;
        XNextEvent(display, &e);

        switch (e.type)
        {
            case ClientMessage:
                if ((Atom)e.xclient.data.l[0] == wm_delete_window) exit(0);
            default:
                break;
        }
    }
}
