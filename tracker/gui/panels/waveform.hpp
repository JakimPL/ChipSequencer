#pragma once

#include <vector>
#include <deque>

#include "../../audio/fft.hpp"
#include "../constants.hpp"
#include "panel.hpp"

class GUIWaveformPanel : public GUIPanel {
  private:
    struct FFTParameters {
        int fft_size = 1024;
        float min_db = -100.0f;
    } fft_parameters;

    FFT fft;
    size_t history_size = GUI_MIN_WAVEFORM_HISTORY_SIZE;

    void draw_waveform();
    void draw_spectrogram();
    void draw_channel_waveform(const int output_channel_index, const ImVec2 &size, const ImVec2 &position);
    void draw_channel_spectrogram(const int output_channel_index, const ImVec2 &size, const ImVec2 &position);

    void draw() override;

  public:
    GUIWaveformPanel(const bool visible = true, const bool windowed = true);
    GUIElement get_element() const override;
};
