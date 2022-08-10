/* =================================== PanPI ===================================
 * Digital Signal Processing module
 * (C) 2022 Ryan Suchocki
 * http://suchocki.co.uk/
 */

#include "dsp.h"

#include "common.h"
#include "config.h"

#include <fftw3.h>

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

static complex double *fft_in, *fft_out;
static fftw_plan fft_plan;

static complex double dc = 0;
static unsigned fft_size = 0;
static double normalisation_db = 0;

/*******************************************************************************
 * Code
 ******************************************************************************/

void dsp_init(unsigned init_fft_size, unsigned input_scale)
{
    fft_size = init_fft_size;
    normalisation_db = 20 * log10(input_scale * fft_size);

    fft_in = fftw_alloc_complex(fft_size);
    fft_out = fftw_alloc_complex(fft_size);

    fft_plan =
        fftw_plan_dft_1d((int)fft_size, fft_in, fft_out, FFTW_FORWARD, FFTW_MEASURE);
}

void dsp_free(void)
{
    fftw_destroy_plan(fft_plan);
    fftw_free(fft_in);
    fftw_free(fft_out);
}

void dsp_process(const complex double *iq_inputs, double *dbm_results)
{
    // Prepare the samples for processing by FFTW:
    for (unsigned i = 0; i < fft_size; i++)
    {
        complex double sample = iq_inputs[i];

        // Apply any configured input gain:
        sample *= config.capture_gain;

        // Calculate DC offset using a trivial IIR filter:
        dc = (dc * config.dc_alpha) + (sample * (1 - config.dc_alpha));

        // Store the sample with DC offset removed:
        fft_in[i] = sample - dc;
    }

    fftw_execute(fft_plan);

    // Prepare the FFT results for processing by the rest of panpi:
    for (unsigned i = 0; i < fft_size; i++)
    {
        // Use an offset index so that 0Hz is shifted to the centre of the array:
        unsigned idx = ((fft_size / 2) + i) % fft_size;

        // Calculate the magnitude squared of the complex frequency bin
        // There's no point performing an expensive square root as it's
        // just a factor of two after the upcoming log().
        double mag_sq = SQUARED(creal(fft_out[idx])) + SQUARED(cimag(fft_out[idx]));

        // Convert to dB and compensate for the missing square root
        double db = (20 / 2) * log10(mag_sq);

        // Normalize for the fft size and the scale of the original integer samples
        double db_fs = db - normalisation_db; // 20 * log10(input_scale * fft_size);

        // Convert to dBm using the configured offset:
        dbm_results[i] = db_fs + config.dbm_cal;
    }
}
