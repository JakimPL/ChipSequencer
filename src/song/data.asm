    SEGMENT_DATA
bpm:
    dw 300
normalizer:
    dw 6

envelopes:
.envelope_0:
    dw 0x3F00                ; base_volume
    dw 0x1F00                ; sustain_level
    dw 0x4000                ; bias
    dw 25                    ; attack
    dw 250                   ; decay
    dw 250                   ; hold
    dw 0                     ; release
.envelope_1:
    dw 0x7FFF                ; base_volume
    dw 0x5FFF                ; sustain_level
    dw 0x0000                ; bias
    dw 125                   ; attack
    dw 250                   ; decay
    dw 250                   ; hold
    dw 250                   ; release
.envelope_2:
    dw 0x2FFF                ; base_volume
    dw 0x1FFF                ; sustain_level
    dw 0x0000                ; bias
    dw 1500                  ; attack
    dw 1500                  ; decay
    dw 1250                  ; hold
    dw 1250                  ; release
.envelope_3:
    dw 0x0180                ; base_volume
    dw 0x0180                ; sustain_level
    dw 0x0000                ; bias
    dw 500                   ; attack
    dw 1                     ; decay
    dw 1                     ; hold
    dw 0                     ; release
.envelope_4:
    dw 0x5FFF                ; base_volume
    dw 0x5FFF                ; sustain_level
    dw 0x0014                ; bias
    dw 1                     ; attack
    dw 1                     ; decay
    dw 1                     ; hold
    dw 0                     ; release

sequences:
.sequence_0:
    db 16                    ; data_size = 2 * note_count
; notes
    db 60, 3                 ; C-5
    db 53, 3                 ; F-4
    db 51, 3                 ; D#4
    db 50, 3                 ; D-4
    db 48, 1                 ; C-4
    db -1, 1                 ; note off
    db 48, 1                 ; C-4
    db 46, 1                 ; A#3
.sequence_1:
    db 6                     ; data_size = 2 * note_count
; notes
    db 45, 2                 ; A-3
    db 55, 1                 ; G-4
    db 53, 13                ; F-4
.sequence_2:
    db 4                     ; data_size = 2 * note_count
; notes
    db 24, 1                 ; C-2
    db -1, 1                 ; note off
.sequence_3:
    db 4                     ; data_size = 2 * note_count
; notes
    db 17, 1                 ; F-1
    db -1, 1                 ; note off
.sequence_4:
    db 4                     ; data_size = 2 * note_count
; notes
    db 48, 16                ; C-4
    db 48, 16                ; C-4
.sequence_5:
    db 4                     ; data_size = 2 * note_count
; notes
    db 51, 16                ; D#4
    db 51, 16                ; D#4
.sequence_6:
    db 4                     ; data_size = 2 * note_count
; notes
    db 55, 16                ; G-4
    db 53, 16                ; F-4
.sequence_7:
    db 4                     ; data_size = 2 * note_count
; notes
    db 58, 16                ; A#4
    db 57, 16                ; A-4

orders:
.order_0:
    db 4                     ; order_length
; sequences
    db 0, 1, 0, 1
.order_1:
    db 16                    ; order_length
; sequences
    db 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3
.order_2:
    db 1                     ; order_length
; sequences
    db 4
.order_3:
    db 1                     ; order_length
; sequences
    db 5
.order_4:
    db 1                     ; order_length
; sequences
    db 6
.order_5:
    db 1                     ; order_length
; sequences
    db 7

oscillators:
.oscillator_0:
    db 2                     ; oscillator_size
    db OSCILLATOR_SQUARE     ; generator_index
    db 0x20                  ; duty_cycle
.oscillator_1:
    db 1                     ; oscillator_size
    db OSCILLATOR_SAW        ; generator_index
.oscillator_2:
    db 1                     ; oscillator_size
    db OSCILLATOR_SINE       ; generator_index
.oscillator_3:
    db 2                     ; oscillator_size
    db OSCILLATOR_WAVETABLE  ; generator_index
    db 1                     ; wavetable_index

wavetables:
.wavetable_0:
    db WAVETABLE_0_SIZE      ; wavetable_size
    db 0x00, 0x1F, 0x3F, 0x5F, 0x7F, 0x9F, 0xBF, 0xDF, 0xFF, 0xDF, 0xBF, 0x9F, 0x7F, 0x5F, 0x3F, 0x1F
.wavetable_1:
    db WAVETABLE_1_SIZE      ; wavetable_size
    db 0x00, 0x20, 0xE0, 0x40, 0xC0, 0x60, 0xA0, 0x80

dsps:
.dsp_0:
    db 12                    ; dsp_size
    db EFFECT_DELAY          ; effect_index
    dw output                ; output
    db 0b00000000            ; output_flag
    dw 0x7FFF                ; dry
    dw 0x6FFF                ; wet
    dw 0x5FFF                ; feedback
    dw DSP_0_BUFFER_SIZE     ; delay_time
.dsp_1:
    db 6                     ; dsp_size
    db EFFECT_GAINER         ; effect_index
    dw output                ; output
    db 0b00000000            ; output_flag
    dw 0x9FFF                ; volume
.dsp_2:
    db 6                     ; dsp_size
    db EFFECT_FILTER         ; effect_index
    dw output                ; output
    db 0b00000000            ; output_flag
    dw 336                   ; frequency

channels:
.channel_0:
    db 1                     ; envelope_index
    db 0                     ; order_index
    db 3                     ; oscillator_index
    dd 0x02000000            ; pitch
    dw dsp_input + 8         ; output
    db 0b00000000            ; output_flag
.channel_1:
    db 1                     ; envelope_index
    db 1                     ; order_index
    db 1                     ; oscillator_index
    dd 0x02000000            ; pitch
    dw dsp_input + 8         ; output
    db 0b00000000            ; output_flag
.channel_2:
    db 2                     ; envelope_index
    db 2                     ; order_index
    db 0                     ; oscillator_index
    dd 0x02000000            ; pitch
    dw dsp_input + 4         ; output
    db 0b00000000            ; output_flag
.channel_3:
    db 2                     ; envelope_index
    db 3                     ; order_index
    db 0                     ; oscillator_index
    dd 0x02000000            ; pitch
    dw dsp_input + 4         ; output
    db 0b00000000            ; output_flag
.channel_4:
    db 2                     ; envelope_index
    db 4                     ; order_index
    db 0                     ; oscillator_index
    dd 0x02000000            ; pitch
    dw dsp_input + 4         ; output
    db 0b00000000            ; output_flag
.channel_5:
    db 2                     ; envelope_index
    db 5                     ; order_index
    db 0                     ; oscillator_index
    dd 0x02000000            ; pitch
    dw dsp_input + 4         ; output
    db 0b00000000            ; output_flag
.channel_6:
    db 4                     ; envelope_index
    db -1                    ; order_index
    db 1                     ; oscillator_index
    dd 0x00005000            ; pitch
    dw dsps.dsp_2 + DSP_FILTER_FREQUENCY ; output

    db 0b01010111            ; output_flag
.channel_7:
    db 3                     ; envelope_index
    db -1                    ; order_index
    db 2                     ; oscillator_index
    dd 0x00080000            ; pitch
    dw channels.channel_0 + CHANNEL_PITCH ; output

    db 0b01110110            ; output_flag
.channel_8:
    db 0                     ; envelope_index
    db -1                    ; order_index
    db 2                     ; oscillator_index
    dd 0x00014800            ; pitch
    dw oscillators.oscillator_0 + OSCILLATOR_SQUARE_DUTY_CYCLE ; output

    db 0b01101000            ; output_flag

buffer_offsets:
    dw 0                     ; dsp_0_buffer_offset
    dw 256                   ; dsp_1_buffer_offset
    dw 256                   ; dsp_2_buffer_offset

wavetable_offsets:
    dw 0                     ; wavetable_0_offset
    dw 16                    ; wavetable_1_offset
