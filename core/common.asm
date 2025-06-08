    %ifdef TRACKER
    %define USED_DSP
    %define USED_DSP_GAINER
    %define USED_DSP_DISTORTION
    %define USED_DSP_FILTER
    %define USED_DSP_DELAY

    %define USED_OSCILLATOR_SQUARE
    %define USED_OSCILLATOR_SINE
    %define USED_OSCILLATOR_SAW
    %define USED_OSCILLATOR_WAVETABLE
    %define USED_OSCILLATOR_NOISE

    %define USED_COMMAND
    %define USED_COMMAND_PORTAMENTO
    %define USED_COMMAND_PORTAMENTO_UP
    %define USED_COMMAND_PORTAMENTO_DOWN
    %define USED_COMMAND_SET_MASTER_GAINER
    %define USED_COMMAND_SET_BPM
    %define USED_COMMAND_SET_DIVISION
    %define USED_COMMAND_LOAD_TARGET
    %define USED_COMMAND_CHANGE_32BIT_VALUE
    %define USED_COMMAND_CHANGE_BYTE_VALUE
    %define USED_COMMAND_CHANGE_WORD_VALUE
    %define USED_COMMAND_CHANGE_DWORD_VALUE
    %define USED_COMMAND_CHANGE_FLOAT_VALUE
    %define USED_COMMAND_ADD_32BIT_VALUE
    %define USED_COMMAND_ADD_BYTE_VALUE
    %define USED_COMMAND_ADD_WORD_VALUE
    %define USED_COMMAND_ADD_DWORD_VALUE
    %define USED_COMMAND_ADD_FLOAT_VALUE
    %endif

    %include "core/common/config.asm"
    %include "core/common/const.asm"
    %include "core/common/offsets.asm"
    %include "core/common/sizes.asm"
    %include "core/common/macros.asm"
    %include "core/song/header.asm"
