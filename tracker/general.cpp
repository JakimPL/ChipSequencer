#include "general.hpp"
#include "song/data.hpp"

ScaleComposer scale_composer = ScaleComposer();
FrequencyTable frequency_table = FrequencyTable(scale_composer, DEFAULT_A4_FREQUENCY);
LinkManager link_manager;
FileDriver file_driver;

Song song;
GUI gui;
std::filesystem::path current_path;

void terminate() {
    gui.terminate();
    SDL_Quit();
    exit(0);
}
