; BIOS
    %define BIOS_KEYBOARD_INTERRUPT 0x16
    %define BIOS_KEYBOARD_CHECK 0x01
    %define BIOS_KEYBOARD_READ 0x00
    %define ESC_KEY 0x1B

; DOS
    %define DOS_INTERRUPT 0x21
    %define DOS_TERMINATE 0x4C
    %define DOS_PRINT_STRING 0x09

; Envelope
    %define ENVELOPE_SIZE 14
    %define ENVELOPE_BASE_VOLUME 0
    %define ENVELOPE_SUSTAIN_LEVEL 2
    %define ENVELOPE_BIAS 4
    %define ENVELOPE_ATTACK 6

; Phases
    %define ATTACK 0
    %define DECAY 1
    %define HOLD 2
    %define RELEASE 3
    %define NOTE_CUT 4

; Sequence
    %define SEQUENCE_NOTES 1

; Order
    %define ORDER_SEQUENCES 1

; Wavetable
    %define WAVETABLE_START 1

; Channel
    %define CHANNEL_SIZE 10
    %define CHANNEL_ENVELOPE_INDEX 0
    %define CHANNEL_ORDER_INDEX 1
    %define CHANNEL_OSCILLATOR_INDEX 2
    %define CHANNEL_TRANSPOSE 3
    %define CHANNEL_OUTPUT 7
    %define CHANNEL_SHIFT 9

; Oscillators
    %define BASE_VOLUME 0x8000
    %define OSCILLATOR_INDEX 1

; Square wave
    %define OSCILLATOR_SQUARE 0
    %define OSCILLATOR_SQUARE_PARAMETERS 1
    %define OSCILLATOR_SQUARE_DUTY_CYCLE 2

; Saw wave
    %define OSCILLATOR_SAW 1
    %define OSCILLATOR_SAW_PARAMETERS 0

; Sine wave
    %define OSCILLATOR_SINE 2
    %define OSCILLATOR_SINE_PARAMETERS 0

; Wavetable
    %define OSCILLATOR_WAVETABLE 3
    %define OSCILLATOR_WAVETABLE_PARAMETERS 1
    %define OSCILLATOR_WAVETABLE_WAVETABLE_INDEX 2

; DSP
    %define DSP_EFFECT_INDEX 1
    %define DSP_OUTPUT 2
    %define DSP_SHIFT 4

; Gainer
    %define EFFECT_GAINER 0
    %define DSP_GAINER_PARAMETERS 1
    %define DSP_GAINER_VOLUME 5

; Delay
    %define EFFECT_DELAY 1
    %define DSP_DELAY_PARAMETERS 4
    %define DSP_DELAY_DRY 5
    %define DSP_DELAY_WET 7
    %define DSP_DELAY_FEEDBACK 9
    %define DSP_DELAY_TIME 11

; Delay
    %define EFFECT_FILTER 2
    %define DSP_FILTER_PARAMETERS 1
    %define DSP_FILTER_VALUE 5
