#pragma once

#include <filesystem>

class GUI;
class ScaleComposer;
class FrequencyTable;
class LockRegistry;
class LinkManager;
class FileDriver;
class HistoryManager;
class ShortcutManager;
class ResourceManager;
class Buffers;
class Song;

extern ScaleComposer scale_composer;
extern FrequencyTable frequency_table;
extern LockRegistry lock_registry;
extern LinkManager link_manager;
extern FileDriver file_driver;
extern HistoryManager history_manager;
extern ShortcutManager shortcut_manager;
extern ResourceManager resource_manager;
extern Buffers buffers;

extern std::filesystem::path current_path;
extern GUI gui;
extern Song song;

void terminate();
