#include <alsa/asoundlib.h>

extern "C" {
void initialize();
void mix();
extern _Float32 output[4];
extern uint32_t sample_rate;
void sound_driver_initialize();
void sound_driver_terminate();
}

const int output_channels = 2;
const int buffer_size = 64;
