; BIOS
    %define BIOS_KEYBOARD_INTERRUPT 0x16
    %define BIOS_KEYBOARD_CHECK 0x01
    %define BIOS_KEYBOARD_READ 0x00
    %define ESC_KEY 0x1B

; DOS
    %define DOS_INTERRUPT 0x21
    %define DOS_TERMINATE 0x4C
    %define DOS_PRINT_STRING 0x09

; Defaults
    %define DEFAULT_BPM 120
    %define DEFAULT_NORMALIZER 0.5
    %define DEFAULT_OUTPUT_CHANNELS 1

    %define DEFAULT_EDO 12
    %define DEFAULT_A4_FREQUENCY 440.0
    %define DEFAULT_NOTE_DIVISOR 1.0594630943592953

    %define DEFAULT_ENVELOPE_BASE_VOLUME 0x7FFF
    %define DEFAULT_ENVELOPE_SUSTAIN_LEVEL 0x7FFF
    %define DEFAULT_ENVELOPE_BIAS 0x0000
    %define DEFAULT_ENVELOPE_ATTACK 125
    %define DEFAULT_ENVELOPE_DECAY 125
    %define DEFAULT_ENVELOPE_HOLD 250
    %define DEFAULT_ENVELOPE_RELEASE 250

    %define DEFAULT_CHANNEL_PITCH 0x02000000

    %define DEFAULT_OSCILLATOR_DUTY_CYCLE 0x80

; General
    %define CONSTANT_PITCH 0xFF

; Notes
    %define NOTES 250
    %define NOTE_REST 254
    %define NOTE_OFF 255

; Limits
    %define MAX_OUTPUT_CHANNELS 4

    %define MAX_ENVELOPES 32
    %define MAX_SEQUENCES 64
    %define MAX_ORDERS 64
    %define MAX_OSCILLATORS 32
    %define MAX_WAVETABLES 16
    %define MAX_CHANNELS 32
    %define MAX_DSPS 16

    %define MAX_WAVETABLE_SIZE 512
    %define MAX_DSP_BUFFER_SIZE 256

; Phases
    %define ATTACK 0
    %define DECAY 1
    %define HOLD 2
    %define RELEASE 3
    %define NOTE_CUT 4

; Envelope
    %define ENVELOPE_BASE_VOLUME 0
    %define ENVELOPE_SUSTAIN_LEVEL 2
    %define ENVELOPE_BIAS 4
    %define ENVELOPE_ATTACK 6
    %define ENVELOPE_DECAY 8
    %define ENVELOPE_HOLD 10
    %define ENVELOPE_RELEASE 12
    %define SIZE_ENVELOPE 14

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
    %define CHANNEL_OUTPUT_FLAG 7
    %define CHANNEL_OUTPUT 8
    %define SIZE_CHANNEL 12
    %else
    %define CHANNEL_ENVELOPE_INDEX 0
    %define CHANNEL_ORDER_INDEX 1
    %define CHANNEL_OSCILLATOR_INDEX 2
    %define CHANNEL_PITCH 3
    %define CHANNEL_OUTPUT_FLAG 7
    %define CHANNEL_OUTPUT 8
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
    %define DSP_OUTPUT_FLAG 2
    %define DSP_OUTPUT 4

; Gainer
    %define EFFECT_GAINER 0
    %define DSP_GAINER_VOLUME 8
    %define SIZE_DSP_GAINER 16

; Delay
    %define EFFECT_DELAY 1
    %define DSP_DELAY_DRY 8
    %define DSP_DELAY_WET 10
    %define DSP_DELAY_FEEDBACK 12
    %define DSP_DELAY_TIME 14
    %define SIZE_DSP_DELAY 16

; Filter
    %define EFFECT_FILTER 2
    %define DSP_FILTER_FREQUENCY 8
    %define SIZE_DSP_FILTER 16

    %else
; DSP
    %define DSP_EFFECT_INDEX 1
    %define DSP_OUTPUT_FLAG 2
    %define DSP_OUTPUT 3

; Gainer
    %define EFFECT_GAINER 0
    %define DSP_GAINER_VOLUME 5
    %define SIZE_DSP_GAINER 16

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
