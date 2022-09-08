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
static inline uint8_t getc_wrapped(FILE *fp);

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
    capture_time += (NANOSECONDS_PER_SECOND * (int64_t)n / capture_rate);
    int64_t block = capture_time - get_nanos();
    if (block > 0)
    {
        nanosleep(&((struct timespec){
            .tv_sec = (time_t)(block / NANOSECONDS_PER_SECOND),
            .tv_nsec = (long)(block % NANOSECONDS_PER_SECOND)
        }), NULL);
    }

    while (n--)
    {
        int16_t i, q;

        // Assume file is 16-bit LE, IQ
        i = getc_wrapped(capture_fp);
        i |= getc_wrapped(capture_fp) << 8;
        q = getc_wrapped(capture_fp);
        q |= getc_wrapped(capture_fp) << 8;

        *buffer++ = CMPLX(i, q);
    }

    return 0;
}

static inline uint8_t getc_wrapped(FILE *fp)
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

    return (uint8_t)c;
}
