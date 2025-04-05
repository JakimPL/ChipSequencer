#ifndef SONG_FUNCTIONS_HPP
#define SONG_FUNCTIONS_HPP

extern "C" {
void sound_driver_initialize() __attribute__((used));
void sound_driver_terminate() __attribute__((used));
void sound_driver_step() __attribute__((used));

void calculate_ticks_per_beat();
void initialize();
void mix();
}

inline void sound_driver_initialize() {
}

inline void sound_driver_terminate() {
}

inline void sound_driver_step() {
}

#endif // SONG_FUNCTIONS_HPP
