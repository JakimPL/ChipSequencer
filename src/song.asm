    %define INSTRUMENTS 2
    %define ENVELOPES 2
    %define SEQUENCES 2

    section .data
bpm:
    dw 360

envelopes:
; Envelope 1
    dw 0x3FFF                ; base_volume
    dw 0x1FFF                ; sustain_level
    dw 125                   ; attack
    dw 25                    ; decay
    dw 500                   ; hold
    dw 1000                  ; release

; Envelope 2
    dw 0x3FFF                ; base_volume
    dw 0x2FFF                ; sustain_level
    dw 125                   ; attack
    dw 250                   ; decay
    dw 250                   ; hold
    dw 250                   ; release

instruments:
; Instrument 2
    db 0                     ; envelope_index
    db 0                     ; envelope_mode
    db 0                     ; sequence_index
    db 0                     ; sequence_current_note
    db 0                     ; oscillator_index
; Instrument 1
    db 0                     ; envelope_index
    db 0                     ; envelope_mode
    db 1                     ; sequence_index
    db 0                     ; sequence_current_note
    db 0                     ; oscillator_index

sequences:
; Sequence 1
    db 6                     ; note_count
; notes
    db 0x28, 1
    db 0x2A, 1
    db 0x2F, 2
    db 0x2B, 1
    db 0x2A, 1
    db 0x28, 2
; Sequence 2
    db 8                     ; note_count
; notes
    db 0x10, 1
    db 0x10, 1
    db 0x1C, 1
    db 0x10, 1
    db 0x1C, 1
    db 0x1F, 1
    db 0x10, 1
    db 0x1C, 1

    section .bss
    envelope_timer resd INSTRUMENTS
    sequence_timer resd INSTRUMENTS
    oscillator_timer resd INSTRUMENTS
    pitch resb INSTRUMENTS
