/* =================================== PanPI ===================================
 * IQ sample capture module
 * (C) 2022 Ryan Suchocki
 * http://suchocki.co.uk/
 */

#include "capture.h"

#include "common.h"
#include "config.h"

#include <alsa/asoundlib.h>

/*******************************************************************************
 * Definitions
 ******************************************************************************/

#define SAMPLE_FORMAT SND_PCM_FORMAT_S16

#define SAMPLE_ACCESS SND_PCM_ACCESS_RW_INTERLEAVED

#define N_CHANNELS 2

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

static const char *get_audio_device(void);
static int capture_soundcard_open(unsigned sample_rate);
static int capture_soundcard_close(void);
static int capture_soundcard_get(complex double *buffer, unsigned n);

/*******************************************************************************
 * Variables
 ******************************************************************************/

static snd_pcm_t *capture_handle;

capture_t capture_soundcard = {
    .open = &capture_soundcard_open,
    .close = &capture_soundcard_close,
    .get = &capture_soundcard_get,
};

/*******************************************************************************
 * Code
 ******************************************************************************/

static const char *get_audio_device(void)
{
    if (strcmp(config.device, "auto"))
    {
        return strdup(config.device);
    }

    const char *result = NULL;

    snd_ctl_card_info_t *info;
    snd_ctl_card_info_alloca(&info);

    int card_idx = -1;
    while (snd_card_next(&card_idx) >= 0 && card_idx >= 0)
    {
        int err;
        char *card_name;
        if ((err = snd_card_get_name(card_idx, &card_name)) < 0)
        {
            eprintf("snd_card_get_name (%s)\n", snd_strerror(err));
            continue;
        }

        if (strstr(card_name, "USB"))
        {
            static char identifier[32];
            sprintf(identifier, "hw:%d", card_idx);
            result = strdup(identifier);
        }

        free(card_name);
    }

    if (result == NULL)
    {
        eprintf("Cannot find USB card\n");
        exit(1);
    }

    printf("Using audio device '%s'\n", result);

    return result;
}

static int capture_soundcard_open(unsigned sample_rate)
{
    int err;

    const char *device_str = get_audio_device();

    if ((err = snd_pcm_open(&capture_handle, device_str, SND_PCM_STREAM_CAPTURE, 0)) <
        0)
    {
        eprintf("Cannot open audio device '%s' (%s)\n", device_str, snd_strerror(err));
        free((void *)device_str);
        exit(1);
    }
    free((void *)device_str);

    snd_pcm_hw_params_t *hw_params;
    snd_pcm_hw_params_alloca(&hw_params);

    if ((err = snd_pcm_hw_params_any(capture_handle, hw_params)) < 0)
    {
        eprintf("Cannot initialize hardware parameter structure (%s)\n", snd_strerror(err));
        exit(1);
    }

    if ((err = snd_pcm_hw_params_set_access(capture_handle, hw_params, SAMPLE_ACCESS)) < 0)
    {
        eprintf("Cannot set access type (%s)\n", snd_strerror(err));
        exit(1);
    }

    if ((err = snd_pcm_hw_params_set_format(capture_handle, hw_params, SAMPLE_FORMAT)) < 0)
    {
        eprintf("Cannot set sample format (%s)\n", snd_strerror(err));
        exit(1);
    }

    if ((err = snd_pcm_hw_params_set_rate(capture_handle, hw_params, sample_rate, 0)) < 0)
    {
        eprintf("Cannot set sample rate (%s)\n", snd_strerror(err));
        exit(1);
    }

    if ((err = snd_pcm_hw_params_set_channels(capture_handle, hw_params, N_CHANNELS)) < 0)
    {
        eprintf("Cannot set channel count (%s)\n", snd_strerror(err));
        exit(1);
    }

    if ((err = snd_pcm_hw_params(capture_handle, hw_params)) < 0)
    {
        eprintf("Cannot set parameters (%s)\n", snd_strerror(err));
        exit(1);
    }

    if ((err = snd_pcm_prepare(capture_handle)) < 0)
    {
        eprintf("Cannot prepare audio interface for use (%s)\n", snd_strerror(err));
        exit(1);
    }

    if ((err = snd_pcm_start(capture_handle)) < 0)
    {
        eprintf("Start error: %s\n", snd_strerror(err));
        exit(1);
    }

    return 0;
}

static int capture_soundcard_close(void)
{
    snd_pcm_close(capture_handle);

    return 0;
}

static int capture_soundcard_get(complex double *buffer, unsigned n)
{
    int err = 0;

    int16_t interleaved[n * 2];

    if ((err = (int)snd_pcm_readi(capture_handle, interleaved, n)) != (int)n)
    {
        eprintf("Snd_pcm_readi(): %i (%s)\n", err, snd_strerror(err));

        if (err == -19)
        {
            exit(-19);
        }
        return 1;
    }

    // Convert interleaved integer samples to complex float:
    for (unsigned i = 0; i < n; i++)
    {
        complex double sample = CMPLX(interleaved[2 * i], interleaved[2 * i + 1]);
        buffer[i] = sample;
    }

    return err;
}
