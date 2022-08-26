/* =================================== PanPI ===================================
 * Config management module
 * (C) 2022 Ryan Suchocki
 * http://suchocki.co.uk/
 */

#include "config.h"

#include "common.h"

#include <sys/inotify.h>

/*******************************************************************************
 * Definitions
 ******************************************************************************/

#define CONFIG_FILEPATH "panpi.cfg"

CONFIG_T config;

static CONFIG_T defaults = {
    .source = "soundcard",
    .device = "auto",
    .file = "",
    .sample_rate = 96000,
    .capture_gain = 1,
    .dc_alpha = 0.999,
    .x_window = false,
    .refl = 0,
    .refh = 20,
    .sgam_spread = 1,
    .sgam_drag = 0.9,
    .wfall_zoom = 5,
};

struct
{
    const char *key;
    enum
    {
        INT,
        UINT,
        DOUBLE,
        STR,
        BOOL
    } type;
    union
    {
        int *INT;
        unsigned *UINT;
        double *DOUBLE;
        struct
        {
            size_t STRLEN;
            char *STR;
        };
        bool *BOOL;
    };
} map[] = {
    {"source", STR, .STR = config.source, .STRLEN = sizeof(config.source)},
    {"device", STR, .STR = config.device, .STRLEN = sizeof(config.device)},
    {"file", STR, .STR = config.file, .STRLEN = sizeof(config.file)},
    {"sample_rate", UINT, .UINT = &config.sample_rate},
    {"capture_gain", DOUBLE, .DOUBLE = &config.capture_gain},
    {"dc_alpha", DOUBLE, .DOUBLE = &config.dc_alpha},
    {"x_window", BOOL, .BOOL = &config.x_window},
    {"refl", DOUBLE, .DOUBLE = &config.refl},
    {"refh", DOUBLE, .DOUBLE = &config.refh},
    {"sgam_spread", UINT, .UINT = &config.sgam_spread},
    {"sgam_drag", DOUBLE, .DOUBLE = &config.sgam_drag},
    {"wfall_zoom", UINT, .UINT = &config.wfall_zoom},
};

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

static void parse(void);

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/

void parse(void)
{
    config = defaults;

    FILE *f = fopen(CONFIG_FILEPATH, "r");

    char *linebuf;

    while (fscanf(f, " %m[^\n] ", &linebuf) == 1)
    {
        char *keybuf, *valbuf = NULL;

        if (sscanf(linebuf, " %m[^:\n]: %m[^\n] ", &keybuf, &valbuf) == 2)
        {
            for (unsigned i = 0; i < ARRAYLEN(map); i++)
            {
                if (!strcmp(map[i].key, keybuf))
                {
                    int parsed = 0;

                    switch (map[i].type)
                    {
                        case INT:
                            parsed = sscanf(valbuf, "%i", map[i].INT);
                            break;
                        case UINT:
                            parsed = sscanf(valbuf, "%u", map[i].UINT);
                            break;
                        case DOUBLE:
                            parsed = sscanf(valbuf, "%lf", map[i].DOUBLE);
                            break;
                        case STR:
                            parsed = strlen(valbuf) < map[i].STRLEN;
                            if (parsed)
                            {
                                strcpy(map[i].STR, valbuf);
                            }
                            break;
                        case BOOL:
                            if (!strcmp(valbuf, "false"))
                            {
                                *map[i].BOOL = false;
                                parsed = 1;
                            }
                            else if (!strcmp(valbuf, "true"))
                            {
                                *map[i].BOOL = true;
                                parsed = 1;
                            }
                            break;
                        default:
                            eprintf("Unhandled case in parse\n");
                    }

                    if (parsed != 1)
                    {
                        eprintf("Cannot parse \"%s\" (\"%s\")\n", keybuf, valbuf);
                    }

                    break;
                }
            }
        }

        if (keybuf) free(keybuf);
        if (valbuf) free(valbuf);

        free(linebuf);
    }

    fclose(f);
}

static int fd;

void config_init(void)
{
    fd = inotify_init();
    if (fd < 0)
    {
        perror("Couldn't initialize inotify");
    }

    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0) | O_NONBLOCK);

    int wd = inotify_add_watch(fd, CONFIG_FILEPATH, IN_CLOSE_WRITE);
    if (wd == -1)
    {
        eprintf("Failed to add file watch\n");
    }

    parse();
}

bool config_update(void)
{
    // NB there will be no 'name' as we're not watching a directory
    struct inotify_event ev = {0};
    ssize_t length = read(fd, &ev, sizeof(ev));
    ev.len = 0;
    if (length)
    {
        if (ev.mask & IN_CLOSE_WRITE)
        {
            parse();
            return true;
        }
    }

    return false;
}

//     /* Clean up*/
//     inotify_rm_watch( fd, wd );
//     close( fd );

//     return 0;
// }
