#pragma once

enum class GUIElement {
    All,
    Menu,
    Editor,
    General,
    Channels,
    CommandsChannels,
    CommandsSequences,
    DSPs,
    Envelopes,
    Orders,
    Oscillators,
    Patterns,
    Routings,
    Sequences,
    Wavetables,
    Summary,
    Waveform,
    Count,
};

enum class GUIScale {
    Linear,
    Logarithmic,
    Quadratic,
};

enum class RenderingBackend {
    OpenGL,
    Software
};
