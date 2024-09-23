    %define CHANNELS 1
    %define ENVELOPES 2
    %define SEQUENCES 2

    section .data
bpm:
    dw 360

envelopes:
; Envelope 0
    dw 0x4F00                ; base_volume
    dw 0x4F00                ; sustain_level
    dw 25                    ; attack
    dw 25                    ; decay
    dw 500                   ; hold
    dw 1000                  ; release
; Envelope 1
    dw 0x7FFF                ; base_volume
    dw 0x2FFF                ; sustain_level
    dw 125                   ; attack
    dw 250                   ; decay
    dw 250                   ; hold
    dw 250                   ; release
; Envelope 2
    dw 0x4FFF                ; base_volume
    dw 0x0FFF                ; sustain_level
    dw 15                    ; attack
    dw 50                    ; decay
    dw 250                   ; hold
    dw 250                   ; release

channels:
; Channel 0
    db 0                     ; envelope_index
    db 1                     ; order_index
    db 0                     ; oscillator_index
    db 0                     ; transpose
; Channel 1
    db 1                     ; envelope_index
    db 1                     ; order_index
    db 0                     ; oscillator_index
    db 0                     ; transpose

sequences:
; Sequence 0
    db 12                    ; data_size = 2 * note_count
; notes
    db 0x28, 1
    db 0x2A, 1
    db 0x2F, 2
    db 0x2B, 1
    db 0x2A, 1
    db 0x28, 2
; Sequence 1
    db 16                    ; data_size = 2 * note_count
; notes
    db 0x1C, 1
    db 0x10, 1
    db 0x10, 1
    db 0x10, 1
    db 0x10, 1
    db 0x10, 1
    db 0x10, 1
    db 0x10, 1
; Sequence 2
    db 4                     ; data_size = 2 * note_count
; notes
    db 0x3B, 1
    db 0xFF, 7

orders:
; Order 0
    db 4                     ; order_length
    db 1, 1, 1, 1
; Order 1
    db 4                     ; order_length
    db 2, 2, 2, 2

    section .bss
    envelope_timer resd CHANNELS
    sequence_timer resd CHANNELS
    oscillator_timer resd CHANNELS
    pitch resb CHANNELS

    envelope_mode resb CHANNELS
    current_order resb CHANNELS
    sequence_current_note resb CHANNELS
