/* =================================== PanPI ===================================
 * Main entry point
 * (C) 2022 Ryan Suchocki
 * http://suchocki.co.uk/
 */

#include "capture.h"
#include "common.h"
#include "config.h"
#include "display.h"
#include "dsp.h"
#include "layout.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

static volatile bool should_run = true;

unsigned sample_rate = 0;

/*******************************************************************************
 * Code
 ******************************************************************************/

static void sigint_handler(int signum)
{
    UNUSED(signum);

    should_run = false;
}

int main(int argc, const char *argv[])
{
    UNUSED(argc);
    UNUSED(argv);

    signal(SIGINT, sigint_handler);

    printf("PanPI " VERSION "\n");

    config_init();
    sample_rate = config.sample_rate;

    unsigned sgam_width = display_open(sample_rate);
    unsigned fft_size = sgam_width;

    complex double *iq_samples = malloc(sizeof(complex double) * fft_size);
    double *dbm_values = malloc(sizeof(double) * fft_size);

    dsp_init(fft_size, INT16_MAX);

    capture_t capture = capture_init(config.source);

    capture.open(sample_rate);

    while (should_run)
    {
        capture.get(iq_samples, fft_size);

        dsp_process(iq_samples, dbm_values);

        display_update(dbm_values);

        if (config_update())
        {
            display_configure(sample_rate);
            printf("BG\n");
        }
    }

    printf("Goodbye\n");

    capture.close();
    display_close();

    free(iq_samples);
    free(dbm_values);

    dsp_free();

    return 0;
}
