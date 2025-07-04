#pragma once

#include <vector>
#include <deque>

#include "../../audio/fft.hpp"
#include "../constants.hpp"
#include "panel.hpp"

class GUIWaveformPanel : public GUIPanel {
  private:
    struct FFTParameters {
        int fft_size = 4096;
        float min_db = -20.0f;
        bool wide_bins = true;
    } fft_parameters;

    size_t history_size = GUI_MIN_WAVEFORM_HISTORY_SIZE;
    std::vector<std::string> fft_sizes;
    std::vector<const char *> fft_sizes_labels;
    int fft_size_index = 4;

    const std::vector<int> frequencies = {50, 100, 200, 500, 1000, 2000, 5000, 10000, 20000};

    void update_fft_sizes();
    void draw_waveform();
    void draw_spectrogram();
    void draw_channel_waveform(int output_channel_index, const ImVec2 &size, const ImVec2 &position) const;
    void draw_channel_spectrogram(int output_channel_index, const ImVec2 &size, const ImVec2 &position);

    static std::string get_frequency_name(int frequency);
    static std::string find_nearest_note(double frequency);
    static void draw_frequency_tooltip(const ImVec2 &position, const ImVec2 &size, float log_min, float log_range);

    void draw() override;

  public:
    explicit GUIWaveformPanel(bool visible = true, bool windowed = true);
    GUIElement get_element() const override;

    void update() override;
};
