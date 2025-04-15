#ifndef GENERAL_HPP
#define GENERAL_HPP

#include <filesystem>

class GUI;

#include "gui/gui.hpp"
#include "song/song.hpp"
#include "song/links/manager.hpp"
#include "tuning/frequencies.hpp"
#include "tuning/scale.hpp"

extern ScaleComposer scale_composer;
extern FrequencyTable frequency_table;
extern LinkManager link_manager;

extern Song song;
extern GUI gui;
extern std::filesystem::path current_path;

void terminate();

#endif // GENERAL_HPP
