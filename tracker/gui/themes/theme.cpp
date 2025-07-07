#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include "../../utils/paths.hpp"
#include "../../utils/file.hpp"
#include "../constants.hpp"
#include "../names.hpp"
#include "theme.hpp"

Theme::Theme()
    : themes_path(get_themes_path()),
      current_theme_path(themes_path / "current.json"),
      default_theme_path(themes_path / "default.json") {
}

Theme::~Theme() {
    save_current_theme();
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

    if (static_cast<size_t>(item) < static_cast<size_t>(ThemeItem::RowPlaying)) {
        to_imgui();
    }
}

std::string Theme::get_item_name(const ThemeItem item) {
    switch (item) {
    case ThemeItem::ImGuiText:
        return "Text";
    case ThemeItem::ImGuiTextDisabled:
        return "TextDisabled";
    case ThemeItem::ImGuiWindowBackground:
        return "WindowBackground";
    case ThemeItem::ImGuiChildBackground:
        return "ChildBackground";
    case ThemeItem::ImGuiPopupBackground:
        return "PopupBackground";
    case ThemeItem::ImGuiBorder:
        return "Border";
    case ThemeItem::ImGuiBorderShadow:
        return "BorderShadow";
    case ThemeItem::ImGuiFrameBackground:
        return "FrameBackground";
    case ThemeItem::ImGuiFrameBackgroundHovered:
        return "FrameBackgroundHovered";
    case ThemeItem::ImGuiFrameBackgroundActive:
        return "FrameBackgroundActive";
    case ThemeItem::ImGuiTitleBackground:
        return "TitleBackground";
    case ThemeItem::ImGuiTitleBackgroundActive:
        return "TitleBackgroundActive";
    case ThemeItem::ImGuiTitleBackgroundCollapsed:
        return "TitleBackgroundCollapsed";
    case ThemeItem::ImGuiMenuBarBackground:
        return "MenuBarBackground";
    case ThemeItem::ImGuiScrollbarBackground:
        return "ScrollbarBackground";
    case ThemeItem::ImGuiScrollbarGrab:
        return "ScrollbarGrab";
    case ThemeItem::ImGuiScrollbarGrabHovered:
        return "ScrollbarGrabHovered";
    case ThemeItem::ImGuiScrollbarGrabActive:
        return "ScrollbarGrabActive";
    case ThemeItem::ImGuiCheckMark:
        return "CheckMark";
    case ThemeItem::ImGuiSliderGrab:
        return "SliderGrab";
    case ThemeItem::ImGuiSliderGrabActive:
        return "SliderGrabActive";
    case ThemeItem::ImGuiButton:
        return "Button";
    case ThemeItem::ImGuiButtonHovered:
        return "ButtonHovered";
    case ThemeItem::ImGuiButtonActive:
        return "ButtonActive";
    case ThemeItem::ImGuiHeader:
        return "Header";
    case ThemeItem::ImGuiHeaderHovered:
        return "HeaderHovered";
    case ThemeItem::ImGuiHeaderActive:
        return "HeaderActive";
    case ThemeItem::ImGuiSeparator:
        return "Separator";
    case ThemeItem::ImGuiSeparatorHovered:
        return "SeparatorHovered";
    case ThemeItem::ImGuiSeparatorActive:
        return "SeparatorActive";
    case ThemeItem::ImGuiResizeGrip:
        return "ResizeGrip";
    case ThemeItem::ImGuiResizeGripHovered:
        return "ResizeGripHovered";
    case ThemeItem::ImGuiResizeGripActive:
        return "ResizeGripActive";
    case ThemeItem::ImGuiTab:
        return "Tab";
    case ThemeItem::ImGuiTabHovered:
        return "TabHovered";
    case ThemeItem::ImGuiTabActive:
        return "TabActive";
    case ThemeItem::ImGuiTabUnfocused:
        return "TabUnfocused";
    case ThemeItem::ImGuiTabUnfocusedActive:
        return "TabUnfocusedActive";
    case ThemeItem::ImGuiPlotLines:
        return "PlotLines";
    case ThemeItem::ImGuiPlotLinesHovered:
        return "PlotLinesHovered";
    case ThemeItem::ImGuiPlotHistogram:
        return "PlotHistogram";
    case ThemeItem::ImGuiPlotHistogramHovered:
        return "PlotHistogramHovered";
    case ThemeItem::ImGuiTableHeaderBackground:
        return "TableHeaderBackground";
    case ThemeItem::ImGuiTableBorderStrong:
        return "TableBorderStrong";
    case ThemeItem::ImGuiTableBorderLight:
        return "TableBorderLight";
    case ThemeItem::ImGuiTableRowBackground:
        return "TableRowBackground";
    case ThemeItem::ImGuiTableRowBackgroundAlt:
        return "TableRowBackgroundAlt";
    case ThemeItem::ImGuiTextSelectedBackground:
        return "TextSelectedBackground";
    case ThemeItem::ImGuiDragDropTarget:
        return "DragDropTarget";
    case ThemeItem::ImGuiNavHighlight:
        return "NavHighlight";
    case ThemeItem::ImGuiNavWindowingHighlight:
        return "NavWindowingHighlight";
    case ThemeItem::ImGuiNavWindowingDimBackground:
        return "NavWindowingDimBackground";
    case ThemeItem::ImGuiModalWindowDimBackground:
        return "ModalWindowDimBackground";
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
    if (str == "Text") return ThemeItem::ImGuiText;
    if (str == "TextDisabled") return ThemeItem::ImGuiTextDisabled;
    if (str == "WindowBackground") return ThemeItem::ImGuiWindowBackground;
    if (str == "ChildBackground") return ThemeItem::ImGuiChildBackground;
    if (str == "PopupBackground") return ThemeItem::ImGuiPopupBackground;
    if (str == "Border") return ThemeItem::ImGuiBorder;
    if (str == "BorderShadow") return ThemeItem::ImGuiBorderShadow;
    if (str == "FrameBackground") return ThemeItem::ImGuiFrameBackground;
    if (str == "FrameBackgroundHovered") return ThemeItem::ImGuiFrameBackgroundHovered;
    if (str == "FrameBackgroundActive") return ThemeItem::ImGuiFrameBackgroundActive;
    if (str == "TitleBackground") return ThemeItem::ImGuiTitleBackground;
    if (str == "TitleBackgroundActive") return ThemeItem::ImGuiTitleBackgroundActive;
    if (str == "TitleBackgroundCollapsed") return ThemeItem::ImGuiTitleBackgroundCollapsed;
    if (str == "MenuBarBackground") return ThemeItem::ImGuiMenuBarBackground;
    if (str == "ScrollbarBackground") return ThemeItem::ImGuiScrollbarBackground;
    if (str == "ScrollbarGrab") return ThemeItem::ImGuiScrollbarGrab;
    if (str == "ScrollbarGrabHovered") return ThemeItem::ImGuiScrollbarGrabHovered;
    if (str == "ScrollbarGrabActive") return ThemeItem::ImGuiScrollbarGrabActive;
    if (str == "CheckMark") return ThemeItem::ImGuiCheckMark;
    if (str == "SliderGrab") return ThemeItem::ImGuiSliderGrab;
    if (str == "SliderGrabActive") return ThemeItem::ImGuiSliderGrabActive;
    if (str == "Button") return ThemeItem::ImGuiButton;
    if (str == "ButtonHovered") return ThemeItem::ImGuiButtonHovered;
    if (str == "ButtonActive") return ThemeItem::ImGuiButtonActive;
    if (str == "Header") return ThemeItem::ImGuiHeader;
    if (str == "HeaderHovered") return ThemeItem::ImGuiHeaderHovered;
    if (str == "HeaderActive") return ThemeItem::ImGuiHeaderActive;
    if (str == "Separator") return ThemeItem::ImGuiSeparator;
    if (str == "SeparatorHovered") return ThemeItem::ImGuiSeparatorHovered;
    if (str == "SeparatorActive") return ThemeItem::ImGuiSeparatorActive;
    if (str == "ResizeGrip") return ThemeItem::ImGuiResizeGrip;
    if (str == "ResizeGripHovered") return ThemeItem::ImGuiResizeGripHovered;
    if (str == "ResizeGripActive") return ThemeItem::ImGuiResizeGripActive;
    if (str == "Tab") return ThemeItem::ImGuiTab;
    if (str == "TabHovered") return ThemeItem::ImGuiTabHovered;
    if (str == "TabActive") return ThemeItem::ImGuiTabActive;
    if (str == "TabUnfocused") return ThemeItem::ImGuiTabUnfocused;
    if (str == "TabUnfocusedActive") return ThemeItem::ImGuiTabUnfocusedActive;
    if (str == "PlotLines") return ThemeItem::ImGuiPlotLines;
    if (str == "PlotLinesHovered") return ThemeItem::ImGuiPlotLinesHovered;
    if (str == "PlotHistogram") return ThemeItem::ImGuiPlotHistogram;
    if (str == "PlotHistogramHovered") return ThemeItem::ImGuiPlotHistogramHovered;
    if (str == "TableHeaderBackground") return ThemeItem::ImGuiTableHeaderBackground;
    if (str == "TableBorderStrong") return ThemeItem::ImGuiTableBorderStrong;
    if (str == "TableBorderLight") return ThemeItem::ImGuiTableBorderLight;
    if (str == "TableRowBackground") return ThemeItem::ImGuiTableRowBackground;
    if (str == "TableRowBackgroundAlt") return ThemeItem::ImGuiTableRowBackgroundAlt;
    if (str == "TextSelectedBackground") return ThemeItem::ImGuiTextSelectedBackground;
    if (str == "DragDropTarget") return ThemeItem::ImGuiDragDropTarget;
    if (str == "NavHighlight") return ThemeItem::ImGuiNavHighlight;
    if (str == "NavWindowingHighlight") return ThemeItem::ImGuiNavWindowingHighlight;
    if (str == "NavWindowingDimBackground") return ThemeItem::ImGuiNavWindowingDimBackground;
    if (str == "ModalWindowDimBackground") return ThemeItem::ImGuiModalWindowDimBackground;

    // non-ImGui colors
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

std::string Theme::color_to_hex(const ThemeColor &color) const {
    const uint8_t r = static_cast<uint8_t>(color.r * 255.0 + 0.5);
    const uint8_t g = static_cast<uint8_t>(color.g * 255.0 + 0.5);
    const uint8_t b = static_cast<uint8_t>(color.b * 255.0 + 0.5);
    const uint8_t a = static_cast<uint8_t>(color.a * 255.0 + 0.5);

    const ImU32 u32_color = (r << 24) | (g << 16) | (b << 8) | a;

    std::stringstream ss;
    ss << "#" << std::hex << std::setfill('0') << std::setw(8) << std::uppercase << u32_color;
    return ss.str();
}

ThemeColor Theme::hex_to_color(const std::string &hex) {
    if (hex.length() != 9 || hex[0] != '#') {
        return default_color;
    }

    try {
        const std::string hex_digits = hex.substr(1);
        const ImU32 u32_color = std::stoul(hex_digits, nullptr, 16);

        const uint8_t r = (u32_color >> 24) & 0xFF;
        const uint8_t g = (u32_color >> 16) & 0xFF;
        const uint8_t b = (u32_color >> 8) & 0xFF;
        const uint8_t a = u32_color & 0xFF;

        return ThemeColor(r, g, b, a);
    } catch (const std::exception &e) {
        return default_color;
    }
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

    to_imgui();
}

void Theme::to_imgui() const {
    ImGuiStyle &style = ImGui::GetStyle();
    for (const auto &mapping : imgui_mapping) {
        style.Colors[mapping.second] = get_vec4_color(mapping.first);
    }
}

void Theme::from_imgui() {
    ImGuiStyle &style = ImGui::GetStyle();
    for (const auto &mapping : imgui_mapping) {
        colors[mapping.first] = ThemeColor(style.Colors[mapping.second]);
    }
}

void Theme::load_current_theme() {
    try {
        if (std::filesystem::exists(current_theme_path)) {
            load_theme(current_theme_path);
        } else if (std::filesystem::exists(default_theme_path)) {
            load_theme(default_theme_path);
        } else {
            throw std::runtime_error("No theme files found. Expected default.json at: " + default_theme_path.string());
        }
    } catch (const std::exception &exception) {
        throw std::runtime_error("Failed to load theme: " + std::string(exception.what()));
    }
}

void Theme::load_theme(const std::filesystem::path &filepath) {
    const nlohmann::json theme_json = read_json(filepath);
    from_json(theme_json);
}

void Theme::save_current_theme() const {
    try {
        save_theme(current_theme_path);
    } catch (const std::exception &exception) {
        std::cerr << "Error saving theme: " << exception.what() << std::endl;
    }
}

void Theme::save_theme(const std::filesystem::path &filepath) const {
    std::filesystem::create_directories(filepath.parent_path());
    const nlohmann::json theme_json = to_json();
    save_json(filepath, theme_json);
}