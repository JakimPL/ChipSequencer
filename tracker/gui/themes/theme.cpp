#include <iomanip>
#include <sstream>
#include <stdexcept>

#include "../constants.hpp"
#include "theme.hpp"

Theme::Theme() {
    /* default colors */
    colors = {
        {ThemeItem::RowPlaying, ThemeColor(GUI_ROW_COLOR_PLAYING)},
        {ThemeItem::RowEdited, ThemeColor(GUI_ROW_COLOR_EDITED)},
        {ThemeItem::RowSelected, ThemeColor(GUI_ROW_COLOR_SELECTED)},
        {ThemeItem::RowSecondarySelected, ThemeColor(GUI_ROW_COLOR_SECONDARY_SELECTED)},
        {ThemeItem::RowCurrentCommand, ThemeColor(GUI_ROW_COLOR_CURRENT_COMMAND)},
        {ThemeItem::RowCurrentValue, ThemeColor(GUI_ROW_COLOR_CURRENT_VALUE)},
        {ThemeItem::RowTextCurrent, ThemeColor(GUI_ROW_COLOR_TEXT_CURRENT)},
        {ThemeItem::LockButton, ThemeColor(GUI_LOCK_BUTTON_COLOR)},
        {ThemeItem::LockButtonHover, ThemeColor(GUI_LOCK_BUTTON_HOVER_COLOR)},
        {ThemeItem::LockButtonActive, ThemeColor(GUI_LOCK_BUTTON_ACTIVE_COLOR)},
        {ThemeItem::UnlockButton, ThemeColor(GUI_UNLOCK_BUTTON_COLOR)},
        {ThemeItem::UnlockButtonHover, ThemeColor(GUI_UNLOCK_BUTTON_HOVER_COLOR)},
        {ThemeItem::UnlockButtonActive, ThemeColor(GUI_UNLOCK_BUTTON_ACTIVE_COLOR)},
        {ThemeItem::EnvelopeAttack, ThemeColor(GUI_ENVELOPE_ATTACK_COLOR)},
        {ThemeItem::EnvelopeDecay, ThemeColor(GUI_ENVELOPE_DECAY_COLOR)},
        {ThemeItem::EnvelopeSustain, ThemeColor(GUI_ENVELOPE_SUSTAIN_COLOR)},
        {ThemeItem::EnvelopeRelease, ThemeColor(GUI_ENVELOPE_RELEASE_COLOR)},
        {ThemeItem::EnvelopeCanvasBackground, ThemeColor(GUI_ENVELOPE_CANVAS_BACKGROUND_COLOR)},
        {ThemeItem::EnvelopeCanvasBorder, ThemeColor(GUI_ENVELOPE_CANVAS_BORDER_COLOR)},
        {ThemeItem::EnvelopeGridLine, ThemeColor(GUI_ENVELOPE_GRID_LINE_COLOR)},
        {ThemeItem::EnvelopeGridText, ThemeColor(GUI_ENVELOPE_GRID_TEXT_COLOR)},
        {ThemeItem::EnvelopePlaybackLine, ThemeColor(GUI_ENVELOPE_PLAYBACK_LINE_COLOR)},
        {ThemeItem::EnvelopeCurrentLine, ThemeColor(GUI_ENVELOPE_CURRENT_LINE_COLOR)},
        {ThemeItem::Waveform1, ThemeColor(GUI_WAVEFORM_COLOR_1)},
        {ThemeItem::Waveform2, ThemeColor(GUI_WAVEFORM_COLOR_2)},
        {ThemeItem::Waveform3, ThemeColor(GUI_WAVEFORM_COLOR_3)},
        {ThemeItem::Waveform4, ThemeColor(GUI_WAVEFORM_COLOR_4)},
        {ThemeItem::WaveformGridLine, ThemeColor(GUI_WAVEFORM_GRID_LINE_COLOR)},
        {ThemeItem::WaveformBorderLine, ThemeColor(GUI_WAVEFORM_BORDER_LINE_COLOR)},
        {ThemeItem::WaveformText, ThemeColor(GUI_WAVEFORM_TEXT_COLOR)},
        {ThemeItem::WaveformSpectrogramLow, ThemeColor(GUI_WAVEFORM_SPECTROGRAM_LOW_COLOR)},
        {ThemeItem::WaveformSpectrogramMedium, ThemeColor(GUI_WAVEFORM_SPECTROGRAM_MEDIUM_COLOR)},
        {ThemeItem::WaveformSpectrogramHigh, ThemeColor(GUI_WAVEFORM_SPECTROGRAM_HIGH_COLOR)},
        {ThemeItem::WaveformSpectrogramOverdrive, ThemeColor(GUI_WAVEFORM_SPECTROGRAM_OVERDRIVE_COLOR)},
        {ThemeItem::WaveformFrequencyMark, ThemeColor(GUI_WAVEFORM_FREQUENCY_MARK_COLOR)},
        {ThemeItem::WaveformFrequencyMarkText, ThemeColor(GUI_WAVEFORM_FREQUENCY_MARK_TEXT_COLOR)},
        {ThemeItem::RoutingAudioLink, ThemeColor(GUI_ROUTING_AUDIO_LINK_COLOR)},
        {ThemeItem::RoutingParameterLink, ThemeColor(GUI_ROUTING_PARAMETER_LINK_COLOR)},
        {ThemeItem::RoutingDraggingLink, ThemeColor(GUI_ROUTING_DRAGGING_LINK_COLOR)},
        {ThemeItem::RoutingNodeBackground, ThemeColor(GUI_ROUTING_NODE_BACKGROUND_COLOR)},
        {ThemeItem::RoutingNodeBackgroundHover, ThemeColor(GUI_ROUTING_NODE_BACKGROUND_HOVER_COLOR)},
        {ThemeItem::RoutingNodeBorder, ThemeColor(GUI_ROUTING_NODE_BORDER_COLOR)},
        {ThemeItem::RoutingPinMain, ThemeColor(GUI_ROUTING_PIN_MAIN_COLOR)},
        {ThemeItem::RoutingPinParameter, ThemeColor(GUI_ROUTING_PIN_PARAMETER_COLOR)},
        {ThemeItem::RoutingPinAvailable, ThemeColor(GUI_ROUTING_PIN_AVAILABLE_COLOR)},
        {ThemeItem::ClipboardRecent, ThemeColor(GUI_CLIPBOARD_TEXT_COLOR_RECENT)},
        {ThemeItem::ClipboardOlder, ThemeColor(GUI_CLIPBOARD_TEXT_COLOR_OLDER)},
        {ThemeItem::HistoryCurrentAction, ThemeColor(GUI_HISTORY_CURRENT_ACTION_COLOR)},
        {ThemeItem::HistoryUnappliedAction, ThemeColor(GUI_HISTORY_UNAPPLIED_ACTION_COLOR)},
        {ThemeItem::ButtonPlay, ThemeColor(GUI_BUTTON_COLOR_PLAY)},
        {ThemeItem::ButtonPlayFromCurrentPosition, ThemeColor(GUI_BUTTON_COLOR_PLAY_FROM_CURRENT_POSITION)},
        {ThemeItem::ButtonStop, ThemeColor(GUI_BUTTON_COLOR_STOP)},
        {ThemeItem::Error, ThemeColor(GUI_ERROR_COLOR)},
        {ThemeItem::Secondary, ThemeColor(GUI_SECONDARY_COLOR)},
        {ThemeItem::SecondaryLight, ThemeColor(GUI_SECONDARY_COLOR_LIGHT)},
        {ThemeItem::SecondaryBright, ThemeColor(GUI_SECONDARY_COLOR_BRIGHT)},
        {ThemeItem::SecondaryDark, ThemeColor(GUI_SECONDARY_COLOR_DARK)},
        {ThemeItem::Tertiary, ThemeColor(GUI_TERTIARY_COLOR)},
        {ThemeItem::TertiaryLight, ThemeColor(GUI_TERTIARY_COLOR_LIGHT)},
        {ThemeItem::TertiaryBright, ThemeColor(GUI_TERTIARY_COLOR_BRIGHT)},
        {ThemeItem::TertiaryDark, ThemeColor(GUI_TERTIARY_COLOR_DARK)},
        {ThemeItem::TextUnavailable, ThemeColor(GUI_TEXT_COLOR_UNAVAILABLE)},
        {ThemeItem::PanelBackground, ThemeColor(GUI_PANEL_BACKGROUND_COLOR)},
        {ThemeItem::PanelBorder, ThemeColor(GUI_PANEL_BORDER_COLOR)},
        {ThemeItem::SummaryTableHighlight, ThemeColor(GUI_SUMMARY_TABLE_HIGHLIGHT_COLOR)},
        {ThemeItem::WavetableCanvasBackground, ThemeColor(GUI_WAVETABLE_CANVAS_BACKGROUND_COLOR)},
        {ThemeItem::WavetableCanvasBorder, ThemeColor(GUI_WAVETABLE_CANVAS_BORDER_COLOR)},
        {ThemeItem::WavetableGridLine, ThemeColor(GUI_WAVETABLE_GRID_LINE_COLOR)},
        {ThemeItem::WavetableGridText, ThemeColor(GUI_WAVETABLE_GRID_TEXT_COLOR)},
        {ThemeItem::WavetableWaveform, ThemeColor(GUI_WAVETABLE_WAVEFORM_COLOR)}
    };
}

ThemeColor Theme::get_color(const ThemeItem item) const {
    auto it = colors.find(item);
    if (it != colors.end()) {
        return it->second;
    }

    return default_color;
}

ImU32 Theme::get_u32_color(const ThemeItem item) const {
    return get_color(item).to_u32();
}

ImVec4 Theme::get_vec4_color(const ThemeItem item) const {
    return get_color(item).to_vec4();
}

void Theme::set_color(const ThemeItem item, const ThemeColor &color) {
    colors[item] = color;
}

std::string Theme::get_item_name(const ThemeItem item) {
    switch (item) {
    case ThemeItem::RowPlaying:
        return "RowPlaying";
    case ThemeItem::RowEdited:
        return "RowEdited";
    case ThemeItem::RowSelected:
        return "RowSelected";
    case ThemeItem::RowSecondarySelected:
        return "RowSecondarySelected";
    case ThemeItem::RowCurrentCommand:
        return "RowCurrentCommand";
    case ThemeItem::RowCurrentValue:
        return "RowCurrentValue";
    case ThemeItem::RowTextCurrent:
        return "RowTextCurrent";
    case ThemeItem::LockButton:
        return "LockButton";
    case ThemeItem::LockButtonHover:
        return "LockButtonHover";
    case ThemeItem::LockButtonActive:
        return "LockButtonActive";
    case ThemeItem::UnlockButton:
        return "UnlockButton";
    case ThemeItem::UnlockButtonHover:
        return "UnlockButtonHover";
    case ThemeItem::UnlockButtonActive:
        return "UnlockButtonActive";
    case ThemeItem::EnvelopeAttack:
        return "EnvelopeAttack";
    case ThemeItem::EnvelopeDecay:
        return "EnvelopeDecay";
    case ThemeItem::EnvelopeSustain:
        return "EnvelopeSustain";
    case ThemeItem::EnvelopeRelease:
        return "EnvelopeRelease";
    case ThemeItem::Wavetable:
        return "Wavetable";
    case ThemeItem::Waveform1:
        return "Waveform1";
    case ThemeItem::Waveform2:
        return "Waveform2";
    case ThemeItem::Waveform3:
        return "Waveform3";
    case ThemeItem::Waveform4:
        return "Waveform4";
    case ThemeItem::ClipboardRecent:
        return "ClipboardRecent";
    case ThemeItem::ClipboardOlder:
        return "ClipboardOlder";
    case ThemeItem::HistoryCurrentAction:
        return "HistoryCurrentAction";
    case ThemeItem::HistoryUnappliedAction:
        return "HistoryUnappliedAction";
    case ThemeItem::ButtonPlay:
        return "ButtonPlay";
    case ThemeItem::ButtonPlayFromCurrentPosition:
        return "ButtonPlayFromCurrentPosition";
    case ThemeItem::ButtonStop:
        return "ButtonStop";
    case ThemeItem::Error:
        return "Error";
    case ThemeItem::Secondary:
        return "Secondary";
    case ThemeItem::SecondaryLight:
        return "SecondaryLight";
    case ThemeItem::SecondaryBright:
        return "SecondaryBright";
    case ThemeItem::SecondaryDark:
        return "SecondaryDark";
    case ThemeItem::Tertiary:
        return "Tertiary";
    case ThemeItem::TertiaryLight:
        return "TertiaryLight";
    case ThemeItem::TertiaryBright:
        return "TertiaryBright";
    case ThemeItem::TertiaryDark:
        return "TertiaryDark";
    case ThemeItem::TextUnavailable:
        return "TextUnavailable";
    case ThemeItem::PanelBackground:
        return "PanelBackground";
    case ThemeItem::PanelBorder:
        return "PanelBorder";
    case ThemeItem::RoutingAudioLink:
        return "RoutingAudioLink";
    case ThemeItem::RoutingParameterLink:
        return "RoutingParameterLink";
    case ThemeItem::RoutingDraggingLink:
        return "RoutingDraggingLink";
    case ThemeItem::RoutingNodeBackground:
        return "RoutingNodeBackground";
    case ThemeItem::RoutingNodeBackgroundHover:
        return "RoutingNodeBackgroundHover";
    case ThemeItem::RoutingNodeBorder:
        return "RoutingNodeBorder";
    case ThemeItem::RoutingPinMain:
        return "RoutingPinMain";
    case ThemeItem::RoutingPinParameter:
        return "RoutingPinParameter";
    case ThemeItem::RoutingPinAvailable:
        return "RoutingPinAvailable";
    case ThemeItem::WaveformGridLine:
        return "WaveformGridLine";
    case ThemeItem::WaveformBorderLine:
        return "WaveformBorderLine";
    case ThemeItem::WaveformText:
        return "WaveformText";
    case ThemeItem::WaveformSpectrogramLow:
        return "WaveformSpectrogramLow";
    case ThemeItem::WaveformSpectrogramMedium:
        return "WaveformSpectrogramMedium";
    case ThemeItem::WaveformSpectrogramHigh:
        return "WaveformSpectrogramHigh";
    case ThemeItem::WaveformSpectrogramOverdrive:
        return "WaveformSpectrogramOverdrive";
    case ThemeItem::WaveformFrequencyMark:
        return "WaveformFrequencyMark";
    case ThemeItem::WaveformFrequencyMarkText:
        return "WaveformFrequencyMarkText";
    case ThemeItem::SummaryTableHighlight:
        return "SummaryTableHighlight";
    case ThemeItem::EnvelopeCanvasBackground:
        return "EnvelopeCanvasBackground";
    case ThemeItem::EnvelopeCanvasBorder:
        return "EnvelopeCanvasBorder";
    case ThemeItem::EnvelopeGridLine:
        return "EnvelopeGridLine";
    case ThemeItem::EnvelopeGridText:
        return "EnvelopeGridText";
    case ThemeItem::EnvelopePlaybackLine:
        return "EnvelopePlaybackLine";
    case ThemeItem::EnvelopeCurrentLine:
        return "EnvelopeCurrentLine";
    case ThemeItem::WavetableCanvasBackground:
        return "WavetableCanvasBackground";
    case ThemeItem::WavetableCanvasBorder:
        return "WavetableCanvasBorder";
    case ThemeItem::WavetableGridLine:
        return "WavetableGridLine";
    case ThemeItem::WavetableGridText:
        return "WavetableGridText";
    case ThemeItem::WavetableWaveform:
        return "WavetableWaveform";
    case ThemeItem::Count:
        return "Count";
    }
    return "Unknown";
}

ThemeItem Theme::get_item_from_name(const std::string &str) {
    if (str == "RowPlaying") return ThemeItem::RowPlaying;
    if (str == "RowEdited") return ThemeItem::RowEdited;
    if (str == "RowSelected") return ThemeItem::RowSelected;
    if (str == "RowSecondarySelected") return ThemeItem::RowSecondarySelected;
    if (str == "RowCurrentCommand") return ThemeItem::RowCurrentCommand;
    if (str == "RowCurrentValue") return ThemeItem::RowCurrentValue;
    if (str == "RowTextCurrent") return ThemeItem::RowTextCurrent;
    if (str == "LockButton") return ThemeItem::LockButton;
    if (str == "LockButtonHover") return ThemeItem::LockButtonHover;
    if (str == "LockButtonActive") return ThemeItem::LockButtonActive;
    if (str == "UnlockButton") return ThemeItem::UnlockButton;
    if (str == "UnlockButtonHover") return ThemeItem::UnlockButtonHover;
    if (str == "UnlockButtonActive") return ThemeItem::UnlockButtonActive;
    if (str == "EnvelopeAttack") return ThemeItem::EnvelopeAttack;
    if (str == "EnvelopeDecay") return ThemeItem::EnvelopeDecay;
    if (str == "EnvelopeSustain") return ThemeItem::EnvelopeSustain;
    if (str == "EnvelopeRelease") return ThemeItem::EnvelopeRelease;
    if (str == "Wavetable") return ThemeItem::Wavetable;
    if (str == "Waveform1") return ThemeItem::Waveform1;
    if (str == "Waveform2") return ThemeItem::Waveform2;
    if (str == "Waveform3") return ThemeItem::Waveform3;
    if (str == "Waveform4") return ThemeItem::Waveform4;
    if (str == "ClipboardRecent") return ThemeItem::ClipboardRecent;
    if (str == "ClipboardOlder") return ThemeItem::ClipboardOlder;
    if (str == "HistoryCurrentAction") return ThemeItem::HistoryCurrentAction;
    if (str == "HistoryUnappliedAction") return ThemeItem::HistoryUnappliedAction;
    if (str == "ButtonPlay") return ThemeItem::ButtonPlay;
    if (str == "ButtonPlayFromCurrentPosition") return ThemeItem::ButtonPlayFromCurrentPosition;
    if (str == "ButtonStop") return ThemeItem::ButtonStop;
    if (str == "Error") return ThemeItem::Error;
    if (str == "Secondary") return ThemeItem::Secondary;
    if (str == "SecondaryLight") return ThemeItem::SecondaryLight;
    if (str == "SecondaryBright") return ThemeItem::SecondaryBright;
    if (str == "SecondaryDark") return ThemeItem::SecondaryDark;
    if (str == "Tertiary") return ThemeItem::Tertiary;
    if (str == "TertiaryLight") return ThemeItem::TertiaryLight;
    if (str == "TertiaryBright") return ThemeItem::TertiaryBright;
    if (str == "TertiaryDark") return ThemeItem::TertiaryDark;
    if (str == "TextUnavailable") return ThemeItem::TextUnavailable;
    if (str == "PanelBackground") return ThemeItem::PanelBackground;
    if (str == "PanelBorder") return ThemeItem::PanelBorder;
    if (str == "RoutingAudioLink") return ThemeItem::RoutingAudioLink;
    if (str == "RoutingParameterLink") return ThemeItem::RoutingParameterLink;
    if (str == "RoutingDraggingLink") return ThemeItem::RoutingDraggingLink;
    if (str == "RoutingNodeBackground") return ThemeItem::RoutingNodeBackground;
    if (str == "RoutingNodeBackgroundHover") return ThemeItem::RoutingNodeBackgroundHover;
    if (str == "RoutingNodeBorder") return ThemeItem::RoutingNodeBorder;
    if (str == "RoutingPinMain") return ThemeItem::RoutingPinMain;
    if (str == "RoutingPinParameter") return ThemeItem::RoutingPinParameter;
    if (str == "RoutingPinAvailable") return ThemeItem::RoutingPinAvailable;
    if (str == "WaveformGridLine") return ThemeItem::WaveformGridLine;
    if (str == "WaveformBorderLine") return ThemeItem::WaveformBorderLine;
    if (str == "WaveformText") return ThemeItem::WaveformText;
    if (str == "WaveformSpectrogramLow") return ThemeItem::WaveformSpectrogramLow;
    if (str == "WaveformSpectrogramMedium") return ThemeItem::WaveformSpectrogramMedium;
    if (str == "WaveformSpectrogramHigh") return ThemeItem::WaveformSpectrogramHigh;
    if (str == "WaveformSpectrogramOverdrive") return ThemeItem::WaveformSpectrogramOverdrive;
    if (str == "WaveformFrequencyMark") return ThemeItem::WaveformFrequencyMark;
    if (str == "WaveformFrequencyMarkText") return ThemeItem::WaveformFrequencyMarkText;
    if (str == "SummaryTableHighlight") return ThemeItem::SummaryTableHighlight;
    if (str == "EnvelopeCanvasBackground") return ThemeItem::EnvelopeCanvasBackground;
    if (str == "EnvelopeCanvasBorder") return ThemeItem::EnvelopeCanvasBorder;
    if (str == "EnvelopeGridLine") return ThemeItem::EnvelopeGridLine;
    if (str == "EnvelopeGridText") return ThemeItem::EnvelopeGridText;
    if (str == "EnvelopePlaybackLine") return ThemeItem::EnvelopePlaybackLine;
    if (str == "EnvelopeCurrentLine") return ThemeItem::EnvelopeCurrentLine;
    if (str == "WavetableCanvasBackground") return ThemeItem::WavetableCanvasBackground;
    if (str == "WavetableCanvasBorder") return ThemeItem::WavetableCanvasBorder;
    if (str == "WavetableGridLine") return ThemeItem::WavetableGridLine;
    if (str == "WavetableGridText") return ThemeItem::WavetableGridText;
    if (str == "WavetableWaveform") return ThemeItem::WavetableWaveform;
    return ThemeItem::Count;
}

std::string Theme::color_to_hex(const ThemeColor &color) {
    const ImU32 u32_color = color.to_u32();
    std::stringstream ss;
    ss << "#" << std::hex << std::setfill('0') << std::setw(8) << u32_color;
    return ss.str();
}

ThemeColor Theme::hex_to_color(const std::string &hex) {
    if (hex.length() != 9 || hex[0] != '#') {
        return ThemeColor(0.0, 0.0, 0.0, 1.0);
    }

    const std::string hex_digits = hex.substr(1);
    const ImU32 u32_color = std::stoul(hex_digits, nullptr, 16);
    return ThemeColor(u32_color);
}

nlohmann::json Theme::to_json() const {
    nlohmann::json json;

    for (const auto &[item, color] : colors) {
        const std::string key = get_item_name(item);
        json[key] = color_to_hex(color);
    }

    return json;
}

void Theme::from_json(const nlohmann::json &json) {
    std::map<ThemeItem, ThemeColor> backup_colors = colors;

    try {
        colors.clear();

        for (auto it = json.begin(); it != json.end(); ++it) {
            const std::string key = it.key();
            const std::string hex_value = it.value();

            const ThemeItem item = get_item_from_name(key);
            if (item != ThemeItem::Count) {
                colors[item] = hex_to_color(hex_value);
            }
        }
    } catch (const std::exception &e) {
        colors = backup_colors;
        throw;
    }
}
