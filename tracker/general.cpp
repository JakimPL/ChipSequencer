#include "general.hpp"
#include "song/core.hpp"

ScaleComposer scale_composer = ScaleComposer();
FrequencyTable frequency_table = FrequencyTable(scale_composer, DEFAULT_A4_FREQUENCY);
LinkManager link_manager;
FileDriver file_driver;
HistoryManager history_manager;
ShortcutManager shortcut_manager;
ResourceManager resource_manager;
Buffers buffers;

Song song;
GUI gui;
std::filesystem::path current_path;

void terminate() {
    gui.terminate();
    SDL_Quit();
    exit(0);
}
