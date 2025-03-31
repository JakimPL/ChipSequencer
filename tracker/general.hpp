#ifndef GENERAL_HPP
#define GENERAL_HPP

#include <filesystem>

class GUI;

#include "gui/gui.hpp"
#include "song/song.hpp"

extern Song song;
extern GUI gui;
extern std::filesystem::path current_path;

void terminate();

#endif // GENERAL_HPP
