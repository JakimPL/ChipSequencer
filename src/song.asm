    %define CHANNELS 2
    %define ENVELOPES 2
    %define SEQUENCES 2

    section .data
bpm:
    dw 360

envelopes:
; Envelope 1
    dw 0x4F00                ; base_volume
    dw 0x4F00                ; sustain_level
    dw 125                   ; attack
    dw 25                    ; decay
    dw 500                   ; hold
    dw 1000                  ; release

; Envelope 2
    dw 0x7FFF                ; base_volume
    dw 0x2FFF                ; sustain_level
    dw 125                   ; attack
    dw 250                   ; decay
    dw 250                   ; hold
    dw 250                   ; release

channels:
; Channel 1
    db 0                     ; envelope_index
    db 1                     ; sequence_index
    db 0                     ; oscillator_index
    db 0                     ; transpose
; Channel 2
    db 1                     ; envelope_index
    db 0                     ; sequence_index
    db 0                     ; oscillator_index
    db 0                     ; transpose

sequences:
; Sequence 1
    db 12                    ; note_count
; notes
    db 0x28, 1
    db 0x2A, 1
    db 0x2F, 2
    db 0x2B, 1
    db 0x2A, 1
    db 0x28, 2
; Sequence 2
    db 16                    ; note_count
; notes
    db 0x10, 1
    db 0x10, 1
    db 0x1C, 1
    db 0x10, 1
    db 0x1C, 1
    db 0x1F, 1
    db 0x10, 1
    db 0x1C, 1

orders:
; Order 1
    db 4                     ; order_length
    db 0, 0, 1, 1
; Order 2
    db 4                     ; order_length
    db 1, 1, 0, 0

    section .bss
    envelope_timer resd CHANNELS
    sequence_timer resd CHANNELS
    oscillator_timer resd CHANNELS
    pitch resb CHANNELS

    envelope_mode resb CHANNELS
    current_order resb CHANNELS
    current_sequence resb CHANNELS
    sequence_current_note resb CHANNELS
