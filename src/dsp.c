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

/*******************************************************************************
 * Code
 ******************************************************************************/

void dsp_init(unsigned init_fft_size)
{
    fft_size = init_fft_size;
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

void dsp_process(const complex double *samples, double *results)
{
    // Prepare the samples for processing by FFTW:
    for (unsigned i = 0; i < fft_size; i++)
    {
        complex double sample = samples[i];

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
        unsigned idx = (i + fft_size / 2) % fft_size;

        // Calculate the magnitude squared of the complex frequency bin
        // There's no point performing an expensive  square root as it's
        // just a factor of two after the upcoming log().
        double mag_sq = SQUARED(creal(fft_out[idx])) + SQUARED(cimag(fft_out[idx]));

        // Calculate the logarithm of the magnitude. The log base doesn't
        // matter as it changes the result by a constant factor.
        double log_mag = log(mag_sq);

        // Shift and scale the result using the configured upper and lower reference
        // values:
        log_mag -= config.refl;
        log_mag /= (config.refh - config.refl);

        // Clip out any negative results at this stage to simplify downstream
        // processing:
        results[i] = MAX(log_mag, 0);
    }
}
