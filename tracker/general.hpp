#ifndef GENERAL_HPP
#define GENERAL_HPP

#include <filesystem>

class GUI;

#include "driver/file.hpp"
#include "gui/gui.hpp"
#include "gui/shortcuts.hpp"
#include "song/buffers.hpp"
#include "song/song.hpp"
#include "song/links/manager.hpp"
#include "structures/resources/manager.hpp"
#include "tuning/frequencies.hpp"
#include "tuning/scale.hpp"

extern ScaleComposer scale_composer;
extern FrequencyTable frequency_table;
extern LinkManager link_manager;
extern FileDriver file_driver;
extern ShortcutManager shortcut_manager;
extern ResourceManager resource_manager;
extern Buffers buffers;

extern Song song;
extern GUI gui;
extern std::filesystem::path current_path;

void terminate();

#endif // GENERAL_HPP
