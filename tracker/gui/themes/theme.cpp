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
        {ThemeItem::Waveform1, ThemeColor(GUI_WAVEFORM_COLOR_1)},
        {ThemeItem::Waveform2, ThemeColor(GUI_WAVEFORM_COLOR_2)},
        {ThemeItem::Waveform3, ThemeColor(GUI_WAVEFORM_COLOR_3)},
        {ThemeItem::Waveform4, ThemeColor(GUI_WAVEFORM_COLOR_4)},
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
        {ThemeItem::RoutingAudioLink, ThemeColor(GUI_ROUTING_AUDIO_LINK_COLOR)},
        {ThemeItem::RoutingParameterLink, ThemeColor(GUI_ROUTING_PARAMETER_LINK_COLOR)},
        {ThemeItem::RoutingDraggingLink, ThemeColor(GUI_ROUTING_DRAGGING_LINK_COLOR)},
        {ThemeItem::RoutingNodeBackground, ThemeColor(GUI_ROUTING_NODE_BACKGROUND_COLOR)},
        {ThemeItem::RoutingNodeBackgroundHover, ThemeColor(GUI_ROUTING_NODE_BACKGROUND_HOVER_COLOR)},
        {ThemeItem::RoutingNodeBorder, ThemeColor(GUI_ROUTING_NODE_BORDER_COLOR)},
        {ThemeItem::RoutingPinMain, ThemeColor(GUI_ROUTING_PIN_MAIN_COLOR)},
        {ThemeItem::RoutingPinParameter, ThemeColor(GUI_ROUTING_PIN_PARAMETER_COLOR)},
        {ThemeItem::RoutingPinAvailable, ThemeColor(GUI_ROUTING_PIN_AVAILABLE_COLOR)},
        {ThemeItem::WaveformGridLine, ThemeColor(GUI_WAVEFORM_GRID_LINE_COLOR)},
        {ThemeItem::WaveformBorderLine, ThemeColor(GUI_WAVEFORM_BORDER_LINE_COLOR)},
        {ThemeItem::WaveformText, ThemeColor(GUI_WAVEFORM_TEXT_COLOR)},
        {ThemeItem::WaveformSpectrogramLow, ThemeColor(GUI_WAVEFORM_SPECTROGRAM_LOW_COLOR)},
        {ThemeItem::WaveformSpectrogramMedium, ThemeColor(GUI_WAVEFORM_SPECTROGRAM_MEDIUM_COLOR)},
        {ThemeItem::WaveformSpectrogramHigh, ThemeColor(GUI_WAVEFORM_SPECTROGRAM_HIGH_COLOR)},
        {ThemeItem::WaveformSpectrogramOverdrive, ThemeColor(GUI_WAVEFORM_SPECTROGRAM_OVERDRIVE_COLOR)},
        {ThemeItem::WaveformFrequencyMark, ThemeColor(GUI_WAVEFORM_FREQUENCY_MARK_COLOR)},
        {ThemeItem::WaveformFrequencyMarkText, ThemeColor(GUI_WAVEFORM_FREQUENCY_MARK_TEXT_COLOR)},
        {ThemeItem::SummaryTableHighlight, ThemeColor(GUI_SUMMARY_TABLE_HIGHLIGHT_COLOR)},
        {ThemeItem::EnvelopeCanvasBackground, ThemeColor(GUI_ENVELOPE_CANVAS_BACKGROUND_COLOR)},
        {ThemeItem::EnvelopeCanvasBorder, ThemeColor(GUI_ENVELOPE_CANVAS_BORDER_COLOR)},
        {ThemeItem::EnvelopeGridLine, ThemeColor(GUI_ENVELOPE_GRID_LINE_COLOR)},
        {ThemeItem::EnvelopeGridText, ThemeColor(GUI_ENVELOPE_GRID_TEXT_COLOR)},
        {ThemeItem::EnvelopePlaybackLine, ThemeColor(GUI_ENVELOPE_PLAYBACK_LINE_COLOR)},
        {ThemeItem::EnvelopeCurrentLine, ThemeColor(GUI_ENVELOPE_CURRENT_LINE_COLOR)},
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
