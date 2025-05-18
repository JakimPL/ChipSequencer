#ifndef GUI_ENUMS_HPP
#define GUI_ENUMS_HPP

enum class GUIElement {
    All,
    Menu,
    Editor,
    General,
    Channels,
    CommandsChannel,
    CommandsSequence,
    DSPs,
    Envelopes,
    Orders,
    Oscillators,
    Patterns,
    Routing,
    Sequences,
    Wavetables,
    Summary,
    Count,
};

enum class GUIScale {
    Linear,
    Logarithmic,
    Quadratic,
};

#endif // GUI_ENUMS_HPP
