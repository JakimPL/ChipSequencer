#pragma once

#define APPLICATION_TITLE "ChipSequencer"

constexpr int GUI_WINDOW_WIDTH = 1280;
constexpr int GUI_WINDOW_HEIGHT = 720;
constexpr float GUI_BUTTON_WIDTH = 60.0f;

constexpr int GUI_MAX_HISTORY_SIZE = 1000;

constexpr int GUI_DEFAULT_CURRENT_OCTAVE = 3;
constexpr int GUI_DEFAULT_JUMP_STEP = 1;
constexpr int GUI_DEFAULT_PAGE_SIZE = 32;

constexpr int GUI_MAX_STRING_LENGTH = 64;
constexpr int GUI_MAX_HISTORY_ITEM_LENGTH = 256;
constexpr int GUI_COMMAND_COMMAND_SIZE = 1;
constexpr int GUI_COMMAND_VALUE_SIZE = 8;

constexpr float GUI_MINIMAL_CHANNEL_COLUMN_WIDTH = 150.0f;
constexpr int GUI_MIN_PAGE_SIZE = 1;
constexpr int GUI_MAX_PAGE_SIZE = 256;

constexpr float GUI_MIN_TRANSPOSE = -48.0f;
constexpr float GUI_MAX_TRANSPOSE = 48.0f;
constexpr float GUI_MIN_PITCH = 0.0002f;
constexpr float GUI_MAX_PITCH = 65535.0f;

constexpr int GUI_MAX_JUMP_STEP = 32;

constexpr int GUI_MIN_SAMPLE_RATE = 8000;
constexpr int GUI_MAX_SAMPLE_RATE = 96000;
constexpr int GUI_MIN_BPM = 32;
constexpr int GUI_MAX_BPM = 4096;
constexpr int GUI_MIN_DIVISION = 1;
constexpr int GUI_MAX_DIVISION = 64;

constexpr float GUI_ENVELOPE_MAX_TIMER = 10.0f;
constexpr float GUI_ENVELOPE_MAX_TICK_DURATION_COUNT = 32.0f;

constexpr float GUI_ROUTING_NODE_WIDTH = 150.0f;
constexpr float GUI_ROUTING_NODE_VERTICAL_PADDING = 30.0f;
constexpr float GUI_ROUTING_NODE_HORIZONTAL_PADDING = 50.0f;
constexpr float GUI_ROUTING_LINK_THICKNESS = 1.5f;
constexpr float GUI_ROUTING_PIN_RADIUS = 4.0f;
constexpr int GUI_ROUTING_CURVE_SEGMENTS = 12;

constexpr int GUI_MIN_WAVEFORM_HISTORY_SIZE = 256;
constexpr int GUI_MAX_WAVEFORM_HISTORY_SIZE = 4096;
constexpr int GUI_MIN_FFT_SIZE = 256;
constexpr int GUI_MAX_FFT_SIZE = 65536;
constexpr float GUI_MIN_FFT_DB = -100.0f;
constexpr float GUI_MAX_FFT_DB = -20.0f;

#define GUI_ROW_COLOR_PLAYING IM_COL32(128, 255, 128, 64)
#define GUI_ROW_COLOR_SELECTED IM_COL32(128, 128, 255, 64)
#define GUI_ROW_COLOR_SECONDARY_SELECTED IM_COL32(128, 128, 255, 24)
#define GUI_ROW_COLOR_CURRENT_COMMAND IM_COL32(255, 128, 128, 64)
#define GUI_ROW_COLOR_CURRENT_VALUE IM_COL32(128, 255, 128, 64)
#define GUI_ROW_TEXT_CURRENT ImVec4(1.0f, 0.2f, 1.0f, 1.0f)

#define GUI_WAVEFORM_COLOR_1 IM_COL32(0, 200, 0, 255)
#define GUI_WAVEFORM_COLOR_2 IM_COL32(0, 200, 200, 255)
#define GUI_WAVEFORM_COLOR_3 IM_COL32(200, 200, 0, 255)
#define GUI_WAVEFORM_COLOR_4 IM_COL32(200, 0, 200, 255)

#define GUI_ERROR_COLOR ImVec4(1.0f, 0.2f, 0.2f, 1.0f)
#define GUI_TEXT_COLOR_UNAVAILABLE ImVec4(0.7f, 0.7f, 0.7f, 1.0f)

#define GUI_SECONDARY_COLOR ImVec4(0.2f, 0.4f, 0.2f, 1.0f)
#define GUI_SECONDARY_COLOR_LIGHT ImVec4(0.3f, 0.6f, 0.3f, 1.0f)
#define GUI_SECONDARY_COLOR_BRIGHT ImVec4(0.4f, 0.7f, 0.4f, 1.0f)
#define GUI_SECONDARY_COLOR_DARK ImVec4(0.1f, 0.2f, 0.1f, 1.0f)

#define GUI_TERTIARY_COLOR ImVec4(0.4f, 0.3f, 0.4f, 1.0f)
#define GUI_TERTIARY_COLOR_LIGHT ImVec4(0.3f, 0.2f, 0.3f, 1.0f)
#define GUI_TERTIARY_COLOR_BRIGHT ImVec4(0.4f, 0.3f, 0.4f, 1.0f)
#define GUI_TERTIARY_COLOR_DARK ImVec4(0.2f, 0.1f, 0.2f, 1.0f)
