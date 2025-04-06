#include "../../general.hpp"
#include "../names.hpp"
#include "../utils.hpp"
#include "dsps.hpp"

GUIDSPsPanel::GUIDSPsPanel() {
    from();
    update();
}

void GUIDSPsPanel::draw() {
    ImGui::Begin("DSP Editor");
    ImGui::Columns(1, "dsp_columns");

    draw_add_or_remove();
    prepare_combo(dsp_names, "##DSPCombo", dsp_index);
    ImGui::Separator();

    from();
    draw_dsp();
    check_keyboard_input();
    to();

    ImGui::Columns(1);
    ImGui::End();
}

bool GUIDSPsPanel::is_index_valid() const {
    return dsp_index >= 0 && dsp_index < dsps.size();
}

void GUIDSPsPanel::from() {
    if (!is_index_valid()) {
        return;
    }
}

void GUIDSPsPanel::to() const {
    if (!is_index_valid()) {
        return;
    }
}

void GUIDSPsPanel::add() {
    void *new_dsp = song.add_dsp();
    if (new_dsp == nullptr) {
        return;
    }

    dsp_index = dsps.size() - 1;
    update();
}

void GUIDSPsPanel::remove() {
    if (is_index_valid()) {
        song.remove_dsp(dsp_index);
        dsp_index = std::max(0, dsp_index - 1);
        update();
    }
}

void GUIDSPsPanel::update() {
    update_items(dsp_names, dsps.size(), "DSP ", dsp_index);
}

void GUIDSPsPanel::draw_dsp() {
    if (dsps.empty()) {
        ImGui::Text("No dsps available.");
        return;
    }
}

void GUIDSPsPanel::check_keyboard_input() {
}
