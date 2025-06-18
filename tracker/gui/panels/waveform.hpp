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
    void draw_channel_waveform(const int output_channel_index, const ImVec2 &size, const ImVec2 &position);
    void draw_channel_spectrogram(const int output_channel_index, const ImVec2 &size, const ImVec2 &position);
    std::string get_frequency_name(const int frequency) const;

    void draw() override;

  public:
    GUIWaveformPanel(const bool visible = true, const bool windowed = true);
    GUIElement get_element() const override;

    void update() override;
};
