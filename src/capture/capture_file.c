/* =================================== PanPI ===================================
 * IQ sample capture module
 * (C) 2022 Ryan Suchocki
 * http://suchocki.co.uk/
 */

#include "capture.h"

#include "common.h"
#include "config.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

static int capture_file_open(unsigned sample_rate);
static int capture_file_close(void);
static int capture_file_get(complex double *buffer, unsigned n);
static inline char getc_wrapped(FILE *fp);

/*******************************************************************************
 * Variables
 ******************************************************************************/

static FILE *capture_fp;
static unsigned capture_rate;
static int64_t capture_time;

capture_t capture_file = {
    .open = &capture_file_open,
    .close = &capture_file_close,
    .get = &capture_file_get,
};

/*******************************************************************************
 * Code
 ******************************************************************************/

static int capture_file_open(unsigned sample_rate)
{
    if (!strlen(config.file))
    {
        eprintf("No source file configured\n");
        exit(-1);
    }
    capture_fp = fopen(config.file, "r");
    if (!capture_fp)
    {
        eprintf("Failed to open file '%s'\n", config.file);
        exit(-1);
    }
    capture_rate = sample_rate;
    capture_time = get_nanos();

    return 0;
}

static int capture_file_close(void)
{
    fclose(capture_fp);
    return 0;
}

static int capture_file_get(complex double *buffer, unsigned n)
{
    // Simulate blocking until the requested number of samples could
    // have been received at the configured sample rate:
    capture_time += (NANOSECONDS_PER_SECOND * n / capture_rate);
    int64_t block = capture_time - (int64_t)get_nanos();
    if (block > 0)
    {
        nanosleep(&((struct timespec){
            .tv_sec = block / NANOSECONDS_PER_SECOND,
            .tv_nsec = block % NANOSECONDS_PER_SECOND
        }), NULL);
    }

    while (n--)
    {
        int16_t i, q;

        ((char *)&q)[0] = getc_wrapped(capture_fp);
        ((char *)&q)[1] = getc_wrapped(capture_fp);
        ((char *)&i)[0] = getc_wrapped(capture_fp);
        ((char *)&i)[1] = getc_wrapped(capture_fp);

        *buffer++ = CMPLX(i, q);
    }

    return 0;
}

static inline char getc_wrapped(FILE *fp)
{
    int c = getc(fp);

    if (c == EOF)
    {
        rewind(fp);

        c = getc(fp);
        if (c == EOF)
        {
            eprintf("EOF despite rewinding file\n");
            exit(1);
        }
    }

    return (char)c;
}
