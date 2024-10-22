    %define CHANNELS 1
    %define DSPS 2
    %define WAVETABLES 1

    %define SONG_LENGTH 186253 ; 6.4 seconds * 29102 Hz

; Wavetables
    %define WAVETABLE_0_SIZE 16
    %define WAVETABLE_SIZE 16

; Buffers
    %define DSP_0_DELAY_TIME 0x0100
    %define DSP_BUFFER_SIZE 0x0100

    SEGMENT_DATA
bpm:
    dw 300
normalizer:
    db 6

envelopes:
; Envelope 0
    dw 0x7F00                ; base_volume
    dw 0x1F00                ; sustain_level
    dw 25                    ; attack
    dw 250                   ; decay
    dw 500                   ; hold
    dw 0                     ; release
; Envelope 1
    dw 0x7FFF                ; base_volume
    dw 0x5FFF                ; sustain_level
    dw 125                   ; attack
    dw 250                   ; decay
    dw 250                   ; hold
    dw 250                   ; release
; Envelope 2
    dw 0x2FFF                ; base_volume
    dw 0x1FFF                ; sustain_level
    dw 1500                  ; attack
    dw 1500                  ; decay
    dw 1250                  ; hold
    dw 1250                  ; release

sequences:
; Sequence 0
    db 16                    ; data_size = 2 * note_count
; notes
    db 0x3C, 3
    db 0x35, 3
    db 0x33, 3
    db 0x32, 3
    db 0x30, 1
    db 0xFF, 1
    db 0x30, 1
    db 0x2E, 1
; Sequence 1
    db 6                     ; data_size = 2 * note_count
; notes
    db 0x2D, 2
    db 0x37, 1
    db 0x35, 13
; Sequence 2
    db 4                     ; data_size = 2 * note_count
; notes
    db 0x18, 1
    db 0xFF, 1
; Sequence 3
    db 4                     ; data_size = 2 * note_count
; notes
    db 0x11, 1
    db 0xFF, 1
; Sequence 4
    db 4                     ; data_size = 2 * note_count
; notes
    db 0x30, 16
    db 0x30, 16
; Sequence 5
    db 4                     ; data_size = 2 * note_count
; notes
    db 0x33, 16
    db 0x33, 16
; Sequence 6
    db 4                     ; data_size = 2 * note_count
; notes
    db 0x37, 16
    db 0x35, 16
; Sequence 7
    db 4                     ; data_size = 2 * note_count
; notes
    db 0x3A, 16
    db 0x39, 16

orders:
; Order 0
    db 4                     ; order_length
    db 0, 1, 0, 1
; Order 1
    db 16                    ; order_length
    db 2, 2, 2, 2, 2, 2, 2, 2
    db 3, 3, 3, 3, 3, 3, 3, 3
; Order 2
    db 1                     ; order_length
    db 4
; Order 3
    db 1                     ; order_length
    db 5
; Order 4
    db 1                     ; order_length
    db 6
; Order 5
    db 1                     ; order_length
    db 7

oscillators:
; Oscillator 0
    db 2                     ; oscillator_size
    db OSCILLATOR_SQUARE     ; generator_index
    db 0x20                  ; duty_cycle
; Oscillator 1
    db 1                     ; oscillator_size
    db OSCILLATOR_SAW        ; generator_index
; Oscillator 2
    db 1                     ; oscillator_size
    db OSCILLATOR_SINE       ; generator_index
; Oscillator 3
    db 2                     ; oscillator_size
    db OSCILLATOR_WAVETABLE  ; generator_index
    db 0                     ; wavetable_index

wavetables:
; Wavetable 0
    db WAVETABLE_0_SIZE      ; wavetable_size
    db 0x00, 0x1F, 0x3F, 0x5F, 0x7F, 0x9F, 0xBF, 0xDF
    db 0xFF, 0xDF, 0xBF, 0x9F, 0x7F, 0x5F, 0x3F, 0x1F

dsps:
; DSP 0
    db 12                    ; dsp_size
    db EFFECT_DELAY          ; effect_index
    dw output                ; output
    db 0                     ; output_flag
    dw 0x7FFF                ; dry
    dw 0x0FFF                ; wet
    dw 0x6FFF                ; feedback
    dw DSP_0_DELAY_TIME      ; delay_time
; DSP 1
    db 6                     ; dsp_size
    db EFFECT_GAINER         ; effect_index
    dw output                ; output
    db 0                     ; output_flag
    dw 0x9FFF                ; volume

channels:
; Channel 0
    db 1                     ; envelope_index
    db 0                     ; order_index
    db 3                     ; oscillator_index
    dd 0                     ; transpose
    dw dsp_input             ; output
    db 0                     ; output flag
; Channel 1
    db 0                     ; envelope_index
    db -1                    ; order_index
    db 2                     ; oscillator_index
    dd 0x14800               ; pitch
    dw oscillators + 2       ; output
    db 0b01101000            ; output flag
; Channel 2
    db 1                     ; envelope_index
    db 1                     ; order_index
    db 1                     ; oscillator_index
    dd 0                     ; transpose
    dw dsp_input + 4         ; output
    db 0                     ; output flag
; Channel 3
    db 2                     ; envelope_index
    db 2                     ; order_index
    db 0                     ; oscillator_index
    dd 0                     ; transpose
    dw dsp_input + 4         ; output
    db 0                     ; output flag
; Channel 4
    db 2                     ; envelope_index
    db 3                     ; order_index
    db 0                     ; oscillator_index
    dd 0                     ; transpose
    dw dsp_input + 4         ; output
    db 0                     ; output flag
; Channel 5
    db 2                     ; envelope_index
    db 4                     ; order_index
    db 0                     ; oscillator_index
    dd 0                     ; transpose
    dw dsp_input + 4         ; output
    db 0                     ; output flag
; Channel 6
    db 2                     ; envelope_index
    db 5                     ; order_index
    db 0                     ; oscillator_index
    dd 0                     ; transpose
    dw dsp_input + 4         ; output
    db 0                     ; output flag

; Buffer offsets
buffer_offsets:
    dw 0
    dw DSP_0_DELAY_TIME

; Wavetable offsets
wavetable_offsets:
    db 0
    db WAVETABLE_0_SIZE

    SEGMENT_BSS
    envelope_timer resd CHANNELS
    sequence_timer resd CHANNELS
    oscillator_timer resd CHANNELS
    pitch resb CHANNELS

    envelope_mode resb CHANNELS
    current_order resb CHANNELS
    sequence_current_note resb CHANNELS

    dsp_input resd DSPS
    dsp_timer resw DSPS

    wavetable_samples resw WAVETABLE_SIZE
