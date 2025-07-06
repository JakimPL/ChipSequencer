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
        {ThemeItem::TertiaryDark, ThemeColor(GUI_TERTIARY_COLOR_DARK)}
    };
}

ImU32 Theme::get_u32_color(const ThemeItem item) const {
    auto it = colors.find(item);
    if (it != colors.end()) {
        return it->second.to_u32();
    }

    return default_color.to_u32();
}

ImVec4 Theme::get_vec4_color(const ThemeItem item) const {
    auto it = colors.find(item);
    if (it != colors.end()) {
        return it->second.to_vec4();
    }

    return default_color.to_vec4();
}
