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
    %define ENVELOPE_BASE_VOLUME 0
    %define ENVELOPE_SUSTAIN_LEVEL 2
    %define ENVELOPE_BIAS 4
    %define ENVELOPE_ATTACK 6
    %define SIZE_ENVELOPE 14

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
    %ifdef ELF
    %define CHANNEL_ENVELOPE_INDEX 0
    %define CHANNEL_ORDER_INDEX 1
    %define CHANNEL_OSCILLATOR_INDEX 2
    %define CHANNEL_PITCH 3
    %define CHANNEL_OUTPUT 7
    %define CHANNEL_OUTPUT_FLAG 11
    %define SIZE_CHANNEL 12
    %else
    %define CHANNEL_ENVELOPE_INDEX 0
    %define CHANNEL_ORDER_INDEX 1
    %define CHANNEL_OSCILLATOR_INDEX 2
    %define CHANNEL_PITCH 3
    %define CHANNEL_OUTPUT 7
    %define CHANNEL_OUTPUT_FLAG 9
    %define SIZE_CHANNEL 10
    %endif

; Channel output masks
    %define MASK_ADDITIVE 0b01000000
    %define MASK_VARIABLE_TYPE 0b00110000
    %define MASK_SHIFT 0b00001111

; Oscillators
    %define BASE_VOLUME 0x8000
    %define OSCILLATOR_INDEX 1

; Square wave
    %define GENERATOR_SQUARE 0
    %define OSCILLATOR_SQUARE_DUTY_CYCLE 2
    %define SIZE_OSCILLATOR_SQUARE 2

; Saw wave
    %define GENERATOR_SAW 1
    %define SIZE_OSCILLATOR_SAW 1

; Sine wave
    %define GENERATOR_SINE 2
    %define SIZE_OSCILLATOR_SINE 1

; Wavetable
    %define GENERATOR_WAVETABLE 3
    %define OSCILLATOR_WAVETABLE_WAVETABLE_INDEX 2
    %define SIZE_OSCILLATOR_WAVETABLE 2

    %ifdef ELF
; DSP
    %define DSP_EFFECT_INDEX 1
    %define DSP_OUTPUT 2
    %define DSP_OUTPUT_FLAG 6

; Gainer
    %define EFFECT_GAINER 0
    %define DSP_GAINER_VOLUME 7
    %define SIZE_DSP_GAINER 11

; Delay
    %define EFFECT_DELAY 1
    %define DSP_DELAY_DRY 7
    %define DSP_DELAY_WET 9
    %define DSP_DELAY_FEEDBACK 11
    %define DSP_DELAY_TIME 13
    %define SIZE_DSP_DELAY 15

; Filter
    %define EFFECT_FILTER 2
    %define DSP_FILTER_FREQUENCY 7
    %define SIZE_DSP_FILTER 11

    %else
; DSP
    %define DSP_EFFECT_INDEX 1
    %define DSP_OUTPUT 2
    %define DSP_OUTPUT_FLAG 4

; Gainer
    %define EFFECT_GAINER 0
    %define DSP_GAINER_VOLUME 5
    %define SIZE_DSP_GAINER 7

; Delay
    %define EFFECT_DELAY 1
    %define DSP_DELAY_DRY 5
    %define DSP_DELAY_WET 7
    %define DSP_DELAY_FEEDBACK 9
    %define DSP_DELAY_TIME 11
    %define SIZE_DSP_DELAY 13

; Filter
    %define EFFECT_FILTER 2
    %define DSP_FILTER_FREQUENCY 5
    %define SIZE_DSP_FILTER 7
    %endif
