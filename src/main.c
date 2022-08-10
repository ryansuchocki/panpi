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

#define FFT_SIZE SGAM_WIDTH // NB small prime factors

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
    dsp_init(FFT_SIZE, INT16_MAX);

    display_open(sample_rate);

    capture_t capture = capture_init(config.source);

    capture.open(sample_rate);

    while (should_run)
    {
        static complex double iq_samples[FFT_SIZE];
        capture.get(iq_samples, FFT_SIZE);

        static double dbm_values[FFT_SIZE];
        dsp_process(iq_samples, dbm_values);

        display_update(dbm_values);

        if (config_update())
        {
            display_update_bg(sample_rate);
            printf("BG\n");
        }
    }

    printf("Goodbye\n");

    capture.close();
    display_close();

    dsp_free();

    return 0;
}
