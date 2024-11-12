#include <iostream>

extern "C" {
void sound_driver_initialize() __attribute__((used));
void sound_driver_terminate() __attribute__((used));
void sound_driver_step() __attribute__((used));
void initialize();
void mix();
}

void sound_driver_initialize() {
    return;
}

void sound_driver_terminate() {
    return;
}

void sound_driver_step() {
    return;
}

int main() {
    std::cout << "Starting the program..." << std::endl;
    initialize();
    std::cout << "ChipSequencer initialized!" << std::endl;
    // mix();
    return 0;
}
