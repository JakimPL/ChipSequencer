#define TRACKER

// BIOS
#define BIOS_KEYBOARD_INTERRUPT 0x16
#define BIOS_KEYBOARD_CHECK 0x01
#define BIOS_KEYBOARD_READ 0x00
#define ESC_KEY 0x1B

// DOS
#define DOS_INTERRUPT 0x21
#define DOS_TERMINATE 0x4C
#define DOS_PRINT_STRING 0x09

// Defaults
#define DEFAULT_BPM 120
#define DEFAULT_UNIT 60
#define DEFAULT_DIVISION 4
#define DEFAULT_SAMPLE_RATE 44100
#define DEFAULT_NORMALIZER 0.5
#define DEFAULT_OUTPUT_CHANNELS 2
#define DEFAULT_OUTPUT_FLAG 0

#define DEFAULT_TITLE "Untitled"
#define DEFAULT_AUTHOR "Unknown"
#define DEFAULT_MESSAGE "Chip Sequencer by Jakim, 2025"

#define DEFAULT_EDO 12
#define DEFAULT_A4_FREQUENCY 440.0
#define DEFAULT_NOTE_DIVISOR 1.0594630943592953

#define DEFAULT_ENVELOPE_BASE_VOLUME 0x7FFF
#define DEFAULT_ENVELOPE_SUSTAIN_LEVEL 0x7FFF
#define DEFAULT_ENVELOPE_BIAS 0x0000
#define DEFAULT_ENVELOPE_ATTACK 0x333
#define DEFAULT_ENVELOPE_DECAY 0x333
#define DEFAULT_ENVELOPE_HOLD 0x666
#define DEFAULT_ENVELOPE_RELEASE 0x666

#define DEFAULT_CHANNEL_PITCH 0x02000000

#define DEFAULT_OSCILLATOR_DUTY_CYCLE 0x80

#define DEFAULT_GAINER_VOLUME 0x8000

// Notes
#define NOTES 250
#define NOTE_REST 254
#define NOTE_OFF 255

// Limits
#define MAX_OUTPUT_CHANNELS 4
#define MAX_STRING_LENGTH 64

#define MAX_ENVELOPES 32
#define MAX_SEQUENCES 64
#define MAX_ORDERS 64
#define MAX_OSCILLATORS 32
#define MAX_WAVETABLES 16
#define MAX_CHANNELS 32
#define MAX_DSPS 32

#define MAX_WAVETABLE_SIZE 4096
#define MAX_DSP_BUFFER_SIZE 1048576 // 10 s

#define MAX_ENVELOPE_TIMER_LENGTH 10

#define MAX_ORDER_ITEMS 64
#define MAX_STEPS 64
#define MAX_WAVETABLE_POINTS 255

// Let C = magic_constant. Each frame, an envelope timer increases by C / v.
// where v is proportional to time t belonging to the range [0, MAX_ENVELOPE_TIMER_LENGTH].
// v is 16-bit, so 0xFFFF corresponds to MAX_ENVELOPE_TIMER_LENGTH (10 s by default):
// v = 0xFFFF * t / MAX_ENVELOPE_TIMER_LENGTH
// The counter is being reset when it exceeds dividend := sample_rate << 14.
// Since there are t * sample_rate frames, the equation is:
// C / v * sample_rate * t = dividend
// which simplifies to:
#define MAGIC_CONSTANT (0xFFFF << 14) / MAX_ENVELOPE_TIMER_LENGTH

// Phases
#define PHASE_ATTACK 0
#define PHASE_DECAY 1
#define PHASE_HOLD 2
#define PHASE_RELEASE 3
#define PHASE_NOTE_CUT 4

// Channel output masks
#define MASK_OPERATION 0b11000000
#define MASK_VARIABLE_TYPE 0b00110000
#define MASK_SHIFT 0b00001111

// Channel/DSP flags
#define FLAG_BYPASS 0b00000001
#define FLAG_SPLITTER 0b00000010
#define FLAG_SYNC 0b00000100
#define FLAG_CONSTANT_PITCH 0b00001000

// Generators
#define GENERATOR_SQUARE 0
#define GENERATOR_SAW 1
#define GENERATOR_SINE 2
#define GENERATOR_WAVETABLE 3
#define GENERATOR_NOISE 4

// Effects
#define EFFECT_GAINER 0
#define EFFECT_DISTORTION 1
#define EFFECT_FILTER 2
#define EFFECT_DELAY 3

// Oscillators
#define BASE_VOLUME 0x8000

// Noise
#define NOISE_SEED 0xACE1
#define NOISE_GALOIS_GENERATOR 0xB400
