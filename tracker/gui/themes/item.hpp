#pragma once

#include <cstddef>
#include <map>

#include "../init.hpp"

enum class ThemeItem : size_t {
    ImGuiText,
    ImGuiTextDisabled,
    ImGuiWindowBackground,
    ImGuiChildBackground,
    ImGuiPopupBackground,
    ImGuiBorder,
    ImGuiBorderShadow,
    ImGuiFrameBackground,
    ImGuiFrameBackgroundHovered,
    ImGuiFrameBackgroundActive,
    ImGuiTitleBackground,
    ImGuiTitleBackgroundActive,
    ImGuiTitleBackgroundCollapsed,
    ImGuiMenuBarBackground,
    ImGuiScrollbarBackground,
    ImGuiScrollbarGrab,
    ImGuiScrollbarGrabHovered,
    ImGuiScrollbarGrabActive,
    ImGuiCheckMark,
    ImGuiSliderGrab,
    ImGuiSliderGrabActive,
    ImGuiButton,
    ImGuiButtonHovered,
    ImGuiButtonActive,
    ImGuiHeader,
    ImGuiHeaderHovered,
    ImGuiHeaderActive,
    ImGuiSeparator,
    ImGuiSeparatorHovered,
    ImGuiSeparatorActive,
    ImGuiResizeGrip,
    ImGuiResizeGripHovered,
    ImGuiResizeGripActive,
    ImGuiTab,
    ImGuiTabHovered,
    ImGuiTabActive,
    ImGuiTabUnfocused,
    ImGuiTabUnfocusedActive,
    ImGuiPlotLines,
    ImGuiPlotLinesHovered,
    ImGuiPlotHistogram,
    ImGuiPlotHistogramHovered,
    ImGuiTableHeaderBackground,
    ImGuiTableBorderStrong,
    ImGuiTableBorderLight,
    ImGuiTableRowBackground,
    ImGuiTableRowBackgroundAlt,
    ImGuiTextSelectedBackground,
    ImGuiDragDropTarget,
    ImGuiNavHighlight,
    ImGuiNavWindowingHighlight,
    ImGuiNavWindowingDimBackground,
    ImGuiModalWindowDimBackground,
    RowPlaying,
    RowEdited,
    RowSelected,
    RowSecondarySelected,
    RowCurrentCommand,
    RowCurrentValue,
    RowTextCurrent,
    LockButton,
    LockButtonHover,
    LockButtonActive,
    UnlockButton,
    UnlockButtonHover,
    UnlockButtonActive,
    EnvelopeAttack,
    EnvelopeDecay,
    EnvelopeSustain,
    EnvelopeRelease,
    EnvelopeCanvasBackground,
    EnvelopeCanvasBorder,
    EnvelopeGridLine,
    EnvelopeGridText,
    EnvelopePlaybackLine,
    EnvelopeCurrentLine,
    ClipboardRecent,
    ClipboardOlder,
    HistoryCurrentAction,
    HistoryUnappliedAction,
    ButtonPlay,
    ButtonPlayFromCurrentPosition,
    ButtonStop,
    Error,
    Secondary,
    SecondaryLight,
    SecondaryBright,
    SecondaryDark,
    Tertiary,
    TertiaryLight,
    TertiaryBright,
    TertiaryDark,
    TextUnavailable,
    PanelBackground,
    PanelBorder,
    RoutingAudioLink,
    RoutingParameterLink,
    RoutingDraggingLink,
    RoutingNodeBackground,
    RoutingNodeBackgroundHover,
    RoutingNodeBorder,
    RoutingPinMain,
    RoutingPinParameter,
    RoutingPinAvailable,
    Wavetable,
    Waveform1,
    Waveform2,
    Waveform3,
    Waveform4,
    WaveformGridLine,
    WaveformBorderLine,
    WaveformText,
    WaveformSpectrogramLow,
    WaveformSpectrogramMedium,
    WaveformSpectrogramHigh,
    WaveformSpectrogramOverdrive,
    WaveformFrequencyMark,
    WaveformFrequencyMarkText,
    SummaryTableHighlight,
    WavetableCanvasBackground,
    WavetableCanvasBorder,
    WavetableGridLine,
    WavetableGridText,
    WavetableWaveform,
    Count
};

const std::pair<ThemeItem, ImGuiCol> imgui_mapping[] = {
    {ThemeItem::ImGuiText, ImGuiCol_Text},
    {ThemeItem::ImGuiTextDisabled, ImGuiCol_TextDisabled},
    {ThemeItem::ImGuiWindowBackground, ImGuiCol_WindowBg},
    {ThemeItem::ImGuiChildBackground, ImGuiCol_ChildBg},
    {ThemeItem::ImGuiPopupBackground, ImGuiCol_PopupBg},
    {ThemeItem::ImGuiBorder, ImGuiCol_Border},
    {ThemeItem::ImGuiBorderShadow, ImGuiCol_BorderShadow},
    {ThemeItem::ImGuiFrameBackground, ImGuiCol_FrameBg},
    {ThemeItem::ImGuiFrameBackgroundHovered, ImGuiCol_FrameBgHovered},
    {ThemeItem::ImGuiFrameBackgroundActive, ImGuiCol_FrameBgActive},
    {ThemeItem::ImGuiTitleBackground, ImGuiCol_TitleBg},
    {ThemeItem::ImGuiTitleBackgroundActive, ImGuiCol_TitleBgActive},
    {ThemeItem::ImGuiTitleBackgroundCollapsed, ImGuiCol_TitleBgCollapsed},
    {ThemeItem::ImGuiMenuBarBackground, ImGuiCol_MenuBarBg},
    {ThemeItem::ImGuiScrollbarBackground, ImGuiCol_ScrollbarBg},
    {ThemeItem::ImGuiScrollbarGrab, ImGuiCol_ScrollbarGrab},
    {ThemeItem::ImGuiScrollbarGrabHovered, ImGuiCol_ScrollbarGrabHovered},
    {ThemeItem::ImGuiScrollbarGrabActive, ImGuiCol_ScrollbarGrabActive},
    {ThemeItem::ImGuiCheckMark, ImGuiCol_CheckMark},
    {ThemeItem::ImGuiSliderGrab, ImGuiCol_SliderGrab},
    {ThemeItem::ImGuiSliderGrabActive, ImGuiCol_SliderGrabActive},
    {ThemeItem::ImGuiButton, ImGuiCol_Button},
    {ThemeItem::ImGuiButtonHovered, ImGuiCol_ButtonHovered},
    {ThemeItem::ImGuiButtonActive, ImGuiCol_ButtonActive},
    {ThemeItem::ImGuiHeader, ImGuiCol_Header},
    {ThemeItem::ImGuiHeaderHovered, ImGuiCol_HeaderHovered},
    {ThemeItem::ImGuiHeaderActive, ImGuiCol_HeaderActive},
    {ThemeItem::ImGuiSeparator, ImGuiCol_Separator},
    {ThemeItem::ImGuiSeparatorHovered, ImGuiCol_SeparatorHovered},
    {ThemeItem::ImGuiSeparatorActive, ImGuiCol_SeparatorActive},
    {ThemeItem::ImGuiResizeGrip, ImGuiCol_ResizeGrip},
    {ThemeItem::ImGuiResizeGripHovered, ImGuiCol_ResizeGripHovered},
    {ThemeItem::ImGuiResizeGripActive, ImGuiCol_ResizeGripActive},
    {ThemeItem::ImGuiTab, ImGuiCol_Tab},
    {ThemeItem::ImGuiTabHovered, ImGuiCol_TabHovered},
    {ThemeItem::ImGuiTabActive, ImGuiCol_TabActive},
    {ThemeItem::ImGuiTabUnfocused, ImGuiCol_TabUnfocused},
    {ThemeItem::ImGuiTabUnfocusedActive, ImGuiCol_TabUnfocusedActive},
    {ThemeItem::ImGuiPlotLines, ImGuiCol_PlotLines},
    {ThemeItem::ImGuiPlotLinesHovered, ImGuiCol_PlotLinesHovered},
    {ThemeItem::ImGuiPlotHistogram, ImGuiCol_PlotHistogram},
    {ThemeItem::ImGuiPlotHistogramHovered, ImGuiCol_PlotHistogramHovered},
    {ThemeItem::ImGuiTableHeaderBackground, ImGuiCol_TableHeaderBg},
    {ThemeItem::ImGuiTableBorderStrong, ImGuiCol_TableBorderStrong},
    {ThemeItem::ImGuiTableBorderLight, ImGuiCol_TableBorderLight},
    {ThemeItem::ImGuiTableRowBackground, ImGuiCol_TableRowBg},
    {ThemeItem::ImGuiTableRowBackgroundAlt, ImGuiCol_TableRowBgAlt},
    {ThemeItem::ImGuiTextSelectedBackground, ImGuiCol_TextSelectedBg},
    {ThemeItem::ImGuiDragDropTarget, ImGuiCol_DragDropTarget},
    {ThemeItem::ImGuiNavHighlight, ImGuiCol_NavHighlight},
    {ThemeItem::ImGuiNavWindowingHighlight, ImGuiCol_NavWindowingHighlight},
    {ThemeItem::ImGuiNavWindowingDimBackground, ImGuiCol_NavWindowingDimBg},
    {ThemeItem::ImGuiModalWindowDimBackground, ImGuiCol_ModalWindowDimBg}
};
