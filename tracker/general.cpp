#include "driver/file.hpp"
#include "gui/gui.hpp"
#include "gui/clipboard/clipboard.hpp"
#include "gui/history/manager.hpp"
#include "gui/shortcuts/manager.hpp"
#include "gui/themes/theme.hpp"
#include "song/buffers.hpp"
#include "song/core.hpp"
#include "song/song.hpp"
#include "song/links/manager.hpp"
#include "song/lock/registry.hpp"
#include "structures/resources/manager.hpp"
#include "tuning/frequencies.hpp"
#include "tuning/scale.hpp"
#include "general.hpp"

ScaleComposer scale_composer = ScaleComposer();
FrequencyTable frequency_table = FrequencyTable(scale_composer, DEFAULT_A4_FREQUENCY);
LockRegistry lock_registry;
LinkManager link_manager;
FileDriver file_driver;
Clipboard clipboard;
HistoryManager history_manager;
ShortcutManager shortcut_manager;
ResourceManager resource_manager;
Theme theme;
Buffers buffers;

std::filesystem::path current_path;
GUI gui;
Song song;

void terminate() {
    gui.terminate();
    SDL_Quit();
    exit(0);
}
