#pragma once

#include <vector>
#include <deque>

#include "panel.hpp"

class GUIWaveformPanel : public GUIPanel {
  private:
    static constexpr size_t DEFAULT_HISTORY_SIZE = 1024;
    size_t history_size = DEFAULT_HISTORY_SIZE;
    std::vector<std::deque<float>> channel_histories;

    void draw() override;

    void update_histories();
    void draw_channel_waveform(int channel_index, const ImVec2 &size, const ImVec2 &pos);

  public:
    GUIWaveformPanel(const bool visible = true, const bool windowed = true);
    GUIElement get_element() const override;

    void from() override;
    void update() override;
};
