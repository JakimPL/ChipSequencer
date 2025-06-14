#pragma once

extern "C" {
void sound_driver_initialize() __attribute__((used));
void sound_driver_terminate() __attribute__((used));
void sound_driver_step() __attribute__((used));

void calculate_ticks_per_beat();
void initialize();
void frame();
}

inline void sound_driver_initialize() {
}

inline void sound_driver_terminate() {
}

inline void sound_driver_step() {
}
