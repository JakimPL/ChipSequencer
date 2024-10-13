; Envelope
    %define ENVELOPE_SIZE 12
    %define ENVELOPE_BASE_VOLUME 0
    %define ENVELOPE_SUSTAIN_LEVEL 2
    %define ENVELOPE_ATTACK 4

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
