#include "main.hpp"

snd_pcm_t *handle;

void sound_driver_initialize() {
    snd_pcm_open(&handle, "default", SND_PCM_STREAM_PLAYBACK, 0);

    snd_pcm_set_params(
        handle,
        SND_PCM_FORMAT_FLOAT_LE,
        SND_PCM_ACCESS_RW_INTERLEAVED,
        output_channels,
        sample_rate,
        1,
        500000
    );
}

void sound_driver_terminate() {
    snd_pcm_drain(handle);
    snd_pcm_close(handle);
}

int main() {
    initialize();
    sound_driver_initialize();
    float buffer[buffer_size * output_channels];
    while (true) {
        for (int i = 0; i < buffer_size; ++i) {
            mix();
            for (int ch = 0; ch < output_channels; ++ch) {
                buffer[i * output_channels + ch] = output[ch];
            }
        }

        snd_pcm_writei(handle, buffer, buffer_size);
    }

    sound_driver_terminate();
    return 0;
}
